#!/usr/bin/env python
"""A carla.Vehicle-shaped handle over the FIXS ego record.

A user who already has a CARLA-agent-shaped controller should be able to run it
against FIXS by swapping the vehicle it was built on, and changing nothing else.
That is what this is. `EgoVehicle` answers everything an agent asks of
`world.player`, out of the ego's FIXS record, and adds three verbs on the FIXS
side: drive, update, logStep.

    a CARLA script                          the same script on FIXS
    ------------------------------------    -----------------------------------
    client = carla.Client(host, port)       (FIXS holds the connection)
    vehicle = world.get_actor(id)           ego = fixs.EgoVehicle(config, egoId)
    agent = BehaviorAgent(vehicle)          IDENTICAL
    agent.set_destination(loc)              ego.drive(agent)
    while True: world.tick()                (FIXS calls control(ego, dt))
    agent.run_step()                        IDENTICAL
    vehicle.apply_control(control)          ego.apply(record, control)

The CARLA-side impersonation lives one directory over, in
Carla/carla_agents/fixs_adapter.py, and is delegated to rather than duplicated.
See ORNL-Real-Sim/FIXS#305 for why the controller talks to FIXS and not to CARLA.
"""
from __future__ import annotations

import importlib
import math
import os
import sys

from CommonLib import fixs
from CommonLib.VirEnv.EgoControllerHost import currentBackend

#: A pose step beyond this is a teleport, not motion. Mirrors the adapter's
#: own threshold: 5 m in one CARLA tick is 100 m/s.
_TELEPORT_STEP = 5.0

_ADAPTER_DIR = os.path.join(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
    'Carla', 'carla_agents')


def _adapter():
    """The CARLA-side impersonation, imported from where FIXS ships it."""
    if _ADAPTER_DIR not in sys.path:
        sys.path.append(_ADAPTER_DIR)
    return importlib.import_module('fixs_adapter')


class _ActorListView(list):
    """`world.get_actors()`, carrying FIXS's traffic."""

    def filter(self, pattern):
        if 'vehicle' in pattern:
            return list(self)
        return []                      # no pedestrians or lights on this wire


class _WorldView:
    """The world as a CARLA agent sees it: CARLA's real map, FIXS's traffic.

    The map is forwarded untouched. The traffic is NOT, and the reason is
    measured: CARLA's mirrored vehicles are spawned physics-off and moved with
    set_transform, so `get_velocity()` returns exactly 0.000 for every one of
    them -- 182 of 182 on this corridor, while 119 of them were moving, several
    above 24 m/s. A following model fed those reads every leader as stationary.
    The wire carries the truthful speed, so the agent is handed records.
    """

    def __init__(self, carlaMap):
        self._map = carlaMap
        self.vehicles = _ActorListView()

    def get_map(self):
        return self._map

    def get_actors(self):
        return self.vehicles


class EgoVehicle:
    """`world.player`, backed by the FIXS record.

    Construct it, build your agent on it exactly as you would on a
    carla.Vehicle, then `drive()` the agent -- that is the whole binding. After
    that the step is `update()`, your agent, then the three actuation fields
    written onto the record -- which is the FIXS command contract, not
    something this class does for you.
    """

    #: Waypoints left at which the next lap is appended (~400 m at 2 m spacing).
    _LAP_MARGIN = 200

    def __init__(self, config, egoId=None):
        a = _adapter()
        self._a = a
        self.egoId = egoId or ''

        # dt is the rate the controller is actually called at -- CarlaTimeStep,
        # not the feed. The agent's PID gains are per-step, so handing it the
        # feed period would detune it by the sub-step ratio.
        self.dt = float(config.get('CarlaTimeStep') or 0.1)
        self.fallbackSpeed = float(config.get('EgoTargetSpeed') or 8.33)
        self.sampling = float(config.get('EgoRouteSpacing') or 2.0)

        route = config.get('EgoRoutePoints') or []
        if len(route) < 2:
            raise SystemExit(
                "[agent] CarlaSetup.EgoRoutePoints is empty: this controller "
                "steers a path and has none. Generate it from the map bundle "
                "with FIXS/Carla/utils/sumo_route_points.py.")

        carla, _BasicAgent, RoadOption, _BehaviorAgent = a.import_carla_agents(
            config.get('CarlaRoot') or None)
        self._carla = carla
        self._RoadOption = RoadOption
        self._actor = a.EgoAdapter(carla)

        # A CARLA agent asks its map road questions -- get_waypoint, lane ids,
        # is_junction -- inside its own constructor. FIXS holds the backend, so
        # those are FORWARDED to the real map. Falling back to the adapter's
        # stand-in is for a controller driven with no backend behind it, which
        # is how the tests run; a run has one.
        backend = currentBackend()
        self._world = getattr(backend, 'carlaWorld', None) if backend else None
        if self._world is None and backend is not None:
            print('[agent] backend exposes no CARLA world; road questions will '
                  'be answered from the route polyline', flush=True)

        # Which obstacle detector drives. FIXS's override exists only because
        # there was no road network to filter on; with a real map, stock's own
        # can run. Under measurement -- one of the two collapses (FIXS#305).
        self.stockObstacles = bool(config.get('EgoStockObstacles'))
        if self.stockObstacles and self._world is None:
            raise SystemExit(
                "[agent] EgoStockObstacles needs the real CARLA map, and no "
                "backend exposed one. Stock's filter is road_id/lane_id; "
                "against the route polyline every vehicle matches.")
        self._view = _WorldView(self._world.get_map()) if self.stockObstacles else None

        # One lap, densified, kept so it can be laid down again. The corridor
        # route is a LAP and the traffic simulator drives it EgoRouteRepeat
        # times; the agent has no notion of that and brakes to a stop when its
        # plan runs out, which reads exactly like a stall (FIXS#305).
        self._lap = a.densify([(float(x), float(y)) for x, y in route],
                              self.sampling)
        self.lapsLaid = 0
        self.lapsAllowed = max(1, int(config.get('EgoRouteRepeat') or 1))

        self.agent = None
        self._lastXY = None
        self._config = config
        self.steps = 0
        # Seconds since the first control step: the record carries no clock, and
        # without one this log can only be joined to the datalog by position.
        self.elapsed = 0.0
        self._yaw = 0.0
        self._nSeen = 0
        self._hazardGap = None
        self.log = None
        logPath = config.get('EgoControllerLog', '_datalog/agent_embedded.csv')
        if logPath:
            os.makedirs(os.path.dirname(logPath) or '.', exist_ok=True)
            self.log = open(logPath, 'w', encoding='utf-8', buffering=1)
            self.log.write('t,feedAge,x,y,carlaYaw,speed,advisory,target,'
                           'throttle,brake,steer,wpLeft,leaderGap,signalColor,'
                           'signalDist,nSeen,hazardGap\n')
        print('[agent] BehaviorAgent embedded: %d route points -> %d after densify, '
              'lap repeats up to %d, dt=%.3f s, fallback %.2f m/s'
              % (len(route), len(self._lap), self.lapsAllowed, self.dt,
                 self.fallbackSpeed), flush=True)
        # Said out loud because both are silent when wrong: a stand-in map
        # answers every road question plausibly, and an unparsed switch reads
        # as off.
        print('[agent] map: %s | obstacles: %s'
              % ('CARLA (forwarded)' if self._world is not None else 'route polyline',
                 'stock' if self.stockObstacles else 'FIXS override'), flush=True)

    # ---- the carla.Vehicle interface, forwarded --------------------------
    def get_world(self):
        if self._view is not None:
            return self._view
        return self._world if self._world is not None else self._actor.get_world()

    def get_control(self):    return self._actor.get_control()
    def get_transform(self):  return self._actor.get_transform()
    def get_location(self):   return self._actor.get_location()
    def get_velocity(self):   return self._actor.get_velocity()
    def get_speed_limit(self):return self._actor.get_speed_limit()

    @property
    def bounding_box(self):   return self._actor.bounding_box

    @property
    def id(self):             return self._actor.id

    # ---- the FIXS side ---------------------------------------------------
    def route(self):
        """One lap of the ego's corridor, as (waypoint, RoadOption) pairs."""
        self.lapsLaid += 1
        return [(self._a.Waypoint(self._carla, x, y),
                 self._RoadOption.LANEFOLLOW) for x, y in self._lap]

    def drive(self, agent):
        """Hand an agent over to FIXS: give it the route, and wire FIXS in.

        One call rather than three, because it is one act. Construct your agent
        on this vehicle exactly as you would on a carla.Vehicle, then hand it
        here; from that point FIXS calls you per step.

        What it does, none of which the caller should have to know: carries the
        CARLA step into the PIDs (LocalPlanner builds its gain dicts before it
        reads opt_dict, so passing {'dt': ...} there is a no-op and the loop
        silently runs on CARLA's 20 Hz default), lays the first lap of the
        route, and answers the agent's two detectors from the wire instead of
        from a CARLA world sweep.

        The agent itself is untouched -- its class is still underneath, and
        nothing in agents/ is modified.
        """
        cfg = self._config
        self.agent = agent

        # CARLA's own gains, with the step this controller is actually called
        # at. Applied through the planner's own setters rather than opt_dict,
        # so the caller's constructor stays the one they already wrote.
        planner = agent.get_local_planner()
        planner._dt = self.dt
        planner._sampling_radius = self.sampling
        ctl = planner._vehicle_controller
        ctl.change_lateral_PID({'K_P': 1.95, 'K_I': 0.05, 'K_D': 0.2, 'dt': self.dt})
        ctl.change_longitudinal_PID({'K_P': 1.0, 'K_I': 0.05, 'K_D': 0.0, 'dt': self.dt})

        agent.set_global_plan(self.route())

        agent.__class__ = self._a.make_agent_class(
            agent.__class__, stockObstacles=self.stockObstacles)
        # Every run_step branch takes min(max_speed, speed_limit -
        # speed_lim_dist), so the advisory has to reach it through those two
        # knobs; a target is not a ceiling to undercut.
        if getattr(agent, '_behavior', None) is not None:
            agent._behavior.speed_lim_dist = 0.0
        agent.ignore_vehicles(bool(cfg.get('EgoIgnoreVehicles')))
        agent.ignore_traffic_lights(bool(cfg.get('EgoIgnoreTrafficLights')))
        return agent

    def update(self, ego, dt):
        """Become this tick's record, and hand the agent what it reads.

        Returns False when this tick's pose is a JUMP rather than motion, and
        the caller should not run its agent: adoption (the ego actor is read
        once before its spawn pose is applied, reporting a pose near the CARLA
        origin), a lap wrap, a SUMO re-insertion. Commanding off a
        discontinuity steers for a place the car is not -- with the wire-based
        detector that was survivable, because it emergency-stops on SUMO's
        leader distance whatever the pose; with stock's, the ego wound its
        steering up in those two ticks and never recovered (FIXS#305).
        """
        agent = self.agent
        prev = self._lastXY
        self._lastXY = (ego.positionX, ego.positionY)
        if prev is not None and math.hypot(ego.positionX - prev[0],
                                           ego.positionY - prev[1]) > _TELEPORT_STEP:
            self._actor.update_from_record(ego)     # keep the pose current
            return False
        # dt is taken every step rather than assumed: the host is entitled to
        # call at a different rate than CarlaTimeStep advertised.
        if dt > 0 and abs(dt - self.dt) > 1e-9:
            self.dt = dt
        self.elapsed += dt
        self._yaw = self._actor.update_from_record(ego)

        # The eco controller's advisory, overlaid onto this record because it is
        # served on a lower port. Held since the last feed; ego.feedAge says how
        # long ago.
        advisory = ego.speedDesired if (ego.speedDesired or 0) > 0.01 else None
        self._target = advisory if advisory is not None else self.fallbackSpeed
        self._advisory = advisory
        if getattr(agent, '_behavior', None) is not None:
            agent._behavior.max_speed = max(0.0, self._target) * 3.6
        # Advisory -> max_speed, ROAD limit -> speed_limit. Both are needed: the
        # obstacle search range scales off the speed limit and collapses to its
        # 10 m floor if the advisory is used there.
        self._actor.set_speed_limit(
            float(getattr(ego, 'speedLimit', 0.0) or 0.0) or self.fallbackSpeed)

        agent.ego_record = ego               # what the detectors read
        # Every vehicle on the wire. SUMO's leader answers "what is ahead on my
        # route", not "is anything in my way".
        agent.fixs_vehicles = self._others()
        self._nSeen = len(agent.fixs_vehicles)
        if self._view is not None:
            # Stock sweeps world.get_actors() itself, so this tick's records go
            # there, wearing the shape it expects. The ego is left out rather
            # than filtered by id: on the wire that id is a string, and stock
            # compares it against a CARLA actor's int.
            egoId = (ego.id or '').strip()
            self._view.vehicles = _ActorListView(
                self._a._OtherVehicle(self._carla, r)
                for r in agent.fixs_vehicles
                if (r.id or '').strip() != egoId)

        # Top up before the queue empties: run it to zero and the agent has
        # already braked by the time the next lap lands.
        if len(agent.get_local_planner().get_plan()) < self._LAP_MARGIN:
            self._layLap()

        # What the detectors concluded this step, recorded so a run can answer
        # "did it see the thing it hit" instead of leaving it to inference.
        seen, _, gap = agent._vehicle_obstacle_detected(
            max_distance=agent._base_vehicle_threshold
            + agent._speed_ratio * (ego.speed * 3.6))
        self._hazardGap = gap if seen else None
        return True

    def logStep(self, ego, cmd):
        """FIXS's own per-tick record of what it fed the agent and what came
        back. Bookkeeping, not the interface -- the COMMAND is the `ego.set`
        the controller writes itself, and a controller that wants no CSV can
        leave this out entirely.
        """
        self.steps += 1
        wpLeft = None
        if self.log is not None or self.steps % 500 == 0:
            wpLeft = len(self.agent.get_local_planner().get_plan())
        if self.log is not None:
            self.log.write('%s,%.3f,%.3f,%.3f,%.2f,%.3f,%s,%.3f,%.4f,%.4f,%.4f,'
                           '%d,%s,%s,%s,%d,%s\n'
                           % ('%.3f' % self.elapsed, getattr(ego, 'feedAge', 0.0),
                              ego.positionX, ego.positionY, self._yaw, ego.speed,
                              '' if self._advisory is None else '%.3f' % self._advisory,
                              self._target, cmd.throttle, cmd.brake, cmd.steer,
                              wpLeft, ego.precedingVehicleDistance,
                              ego.signalLightColor, ego.signalLightDistance,
                              self._nSeen,
                              '' if self._hazardGap is None else '%.2f' % self._hazardGap))
        if self.steps % 500 == 0:
            print('[agent] step %d spd=%5.2f tgt=%5.2f (age %.2fs) thr=%.2f '
                  'brk=%.2f steer=%+.2f wp=%d'
                  % (self.steps, ego.speed, self._target,
                     getattr(ego, 'feedAge', 0.0), cmd.throttle, cmd.brake,
                     cmd.steer, wpLeft), flush=True)

    def close(self):
        if self.log is not None:
            self.log.close()
            self.log = None
        print('[agent] %d control steps' % self.steps, flush=True)

    # ---- internals -------------------------------------------------------
    def _layLap(self):
        """Append one more lap to the local planner's queue, BEFORE the current
        one runs out, so the seam carries no stop."""
        if self.lapsLaid >= self.lapsAllowed:
            return False
        plan = [(self._a.Waypoint(self._carla, x, y),
                 self._RoadOption.LANEFOLLOW) for x, y in self._lap]
        self.agent.set_global_plan(plan, stop_waypoint_creation=True,
                                   clean_queue=False)
        self.lapsLaid += 1
        return True

    def _others(self):
        """Every vehicle record this tick. Empty rather than raising if the
        connection has no tick to report -- the host guards that, but a
        controller that hard-failed here would take the run down with it."""
        try:
            return [v for v in (fixs.vehicle.get(i)
                                for i in fixs.vehicle.getIDList())
                    if v is not None]
        except Exception:
            return ()
