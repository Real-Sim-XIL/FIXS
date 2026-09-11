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
from CommonLib.fixs import carla as fixsCarla
from CommonLib.VirEnv.EgoControllerHost import currentBackend, currentCore

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


class _EgoActor:
    """The real physics ego, with the one value CARLA cannot state here.

    Everything -- pose, velocity, bounding box, id -- is the actor's own and
    measured truthful (get_velocity read 9.839 m/s against the wire's 9.83768
    at the same instant). Only the SPEED LIMIT is supplied: an imported
    corridor carries no CARLA speed-limit signs, so the actor reports a value
    that is not a speed, and an agent doing `int(speed_limit / 10)` for its
    look-ahead overflowed a deque index with it. SUMO owns the limit in this
    co-simulation and publishes it on the wire, so that is what the agent gets.
    """

    __slots__ = ('_actor', '_limitKmh')

    def __init__(self, actor):
        self._actor = actor
        self._limitKmh = 0.0

    def setSpeedLimit(self, mps):
        self._limitKmh = max(0.0, float(mps)) * 3.6

    def get_speed_limit(self):
        return self._limitKmh

    def __getattr__(self, name):
        return getattr(self._actor, name)


class _TrafficActor:
    """A mirrored vehicle: CARLA's own actor, with the one thing it lacks.

    Position, heading, bounding box and id are the REAL actor's -- the bridge
    placed it, so there is no frame to convert and nothing to get wrong. Only
    the velocity is supplied, because CARLA cannot report one: mirrors are
    spawned physics-off and moved by set_transform (its own SUMO co-simulation
    does the same), and set_target_velocity on such an actor does nothing.
    Measured: 182 of 182 read exactly 0.000 while 119 were moving.
    """

    __slots__ = ('_actor', '_vel')

    def __init__(self, actor, speedMs, carla_mod):
        self._actor = actor
        yaw = math.radians(actor.get_transform().rotation.yaw)
        self._vel = carla_mod.Vector3D(x=speedMs * math.cos(yaw),
                                       y=speedMs * math.sin(yaw), z=0.0)

    def get_velocity(self):
        return self._vel

    def __getattr__(self, name):
        return getattr(self._actor, name)


class _ActorListView(list):
    """`world.get_actors()`: FIXS's traffic, CARLA's everything else."""

    def __init__(self, vehicles=(), world=None):
        super().__init__(vehicles)
        self._world = world

    def filter(self, pattern):
        if 'vehicle' in pattern:
            return list(self)
        # Traffic lights and the rest are REAL -- the bridge keeps CARLA's
        # lights in step with the traffic simulator, so they are truthful and
        # there is nothing to substitute.
        return self._world.get_actors().filter(pattern) if self._world else []


class _WorldView:
    """The world an agent sees: CARLA's real map, FIXS's traffic.

    The map is forwarded untouched. The traffic is SUPPLIED, and that is not a
    FIXS quirk -- CARLA's own SUMO co-simulation spawns mirrored vehicles with
    SetSimulatePhysics(False) and moves them with set_transform
    (Co-Simulation/Sumo/sumo_integration/carla_simulation.py:113,:144), so it
    never gives them a velocity either. Measured here: 182 of 182 report
    exactly 0.000 while 119 of them were moving, several above 24 m/s. Any
    following model reading get_velocity off those sees every leader as
    stationary.

    The wire carries the truthful speed, so that is what the agent is handed.
    This is FIXS supplying data, not deciding -- the agent's own logic runs on
    it unchanged.
    """

    def __init__(self, carlaMap, world):
        self._map = carlaMap
        self._world = world
        self.vehicles = _ActorListView(world=world)

    def get_map(self):
        return self._map

    def get_actors(self):
        return self.vehicles


class EgoSession:
    """The FIXS side of one controlled ego: route, world view, per-tick record.

    NOT a vehicle. The agent is built on `fixs.carla.ego`, which is CARLA's own
    physics vehicle -- measured truthful, its get_velocity reading 9.839 m/s
    against the wire's 9.83768 at the same instant -- so there is nothing about
    the ego worth standing in for.

    What this owns is the part CARLA cannot answer: the route the traffic
    simulator holds, the traffic states CARLA's mirrors cannot report a velocity
    for, and the per-tick log. `drive()` hands those to an agent; `update()`
    refreshes them; the COMMAND is the caller's `ego.set`.
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
        # Through fixs.carla, which is the one place FIXS reaches the
        # simulator -- the same module a user's own code imports.
        self._world = fixsCarla.world if fixsCarla.available() else None

        # One lap, densified, kept so it can be laid down again. The corridor
        # route is a LAP and the traffic simulator drives it EgoRouteRepeat
        # times; the agent has no notion of that and brakes to a stop when its
        # plan runs out, which reads exactly like a stall (FIXS#305).
        # What the agent's own sensing reads: CARLA's map, FIXS's traffic.
        self._view = (_WorldView(fixsCarla.map, self._world)
                      if self._world is not None else None)

        self._lap = a.densify([(float(x), float(y)) for x, y in route],
                              self.sampling)
        self.lapsLaid = 0
        self.lapsAllowed = max(1, int(config.get('EgoRouteRepeat') or 1))

        self.agent = None
        self._ego = None
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
        print('[agent] map: %s'
              % ('CARLA (forwarded)' if self._world is not None else 'route polyline'),
              flush=True)

    # ---- the FIXS side -------------------------------------------------
    def route(self):
        """One lap of the ego's corridor, as (waypoint, RoadOption) pairs.

        REAL carla.Waypoints when a map is behind us, snapped from the corridor
        the traffic simulator holds. That matters beyond tidiness: an agent
        reads lane_id, road_id and is_junction off its own plan, and a stand-in
        answers 0/0/False for every point -- so its lane reasoning compares the
        real waypoint under the car against a plan that claims to be nowhere.
        """
        self.lapsLaid += 1
        RO = self._RoadOption.LANEFOLLOW
        if self._world is not None:
            cmap, L = fixsCarla.map, self._carla.Location
            plan = []
            for x, y in self._lap:
                # FIXS y is north-positive; CARLA's is flipped.
                wp = cmap.get_waypoint(L(x=x, y=-y, z=0.0),
                                       project_to_road=True)
                if wp is not None:
                    plan.append((wp, RO))
            if plan:
                return plan
        return [(self._a.Waypoint(self._carla, x, y), RO)
                for x, y in self._lap]

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

        # The ONE thing FIXS substitutes, and it substitutes DATA, not a
        # decision: the agent's view of the world. CARLA's mirrored traffic has
        # no velocity -- its own SUMO co-simulation spawns physics-off and moves
        # by set_transform, and set_target_velocity on such an actor does
        # nothing -- so a following model reading get_velocity off them sees
        # every leader as stationary. Everything else in the view, the map and
        # the traffic lights included, is CARLA's own.
        if self._view is not None:
            agent._world = self._view
            agent._map = self._view.get_map()
        # The ego it was built on, with the speed limit CARLA cannot state on an
        # imported corridor. Everything else about it is the real actor's.
        self._ego = _EgoActor(agent._vehicle)
        agent._vehicle = self._ego
        lp = agent.get_local_planner()
        if getattr(lp, '_vehicle', None) is not None:
            lp._vehicle = self._ego
            vc = getattr(lp, '_vehicle_controller', None)
            for sub in ('_lon_controller', '_lat_controller'):
                c = getattr(vc, sub, None)
                if c is not None and hasattr(c, '_vehicle'):
                    c._vehicle = self._ego
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
        if agent is not None and getattr(agent, '_behavior', None) is not None:
            agent._behavior.max_speed = max(0.0, self._target) * 3.6
        # Advisory -> max_speed, ROAD limit -> speed_limit. Both are needed: the
        # obstacle search range scales off the speed limit and collapses to its
        # 10 m floor if the advisory is used there.
        if self._ego is not None:
            self._ego.setSpeedLimit(
                float(getattr(ego, 'speedLimit', 0.0) or 0.0) or self.fallbackSpeed)

        if self._view is not None:
            # This tick's records, wearing the shape an agent sweeping
            # world.get_actors() expects. The ego is left out by wire id: an
            # agent compares against a CARLA actor's int, which never matches.
            egoId = (ego.id or '').strip()
            self._view.vehicles = _ActorListView(
                self._trafficActors(egoId), world=self._world)
            self._nSeen = len(self._view.vehicles)

        # What the agent's OWN detector concludes this tick, recorded so a run
        # can answer "did it see the thing it hit" instead of leaving it to
        # inference. Read-only: run_step calls it again for real.
        self._hazardGap = None
        if agent is not None:
            try:
                seen, _, gap = agent._vehicle_obstacle_detected(
                    max_distance=agent._base_vehicle_threshold
                    + agent._speed_ratio * (ego.speed * 3.6))
                self._hazardGap = gap if seen else None
            except Exception:
                pass

        # Top up before the queue empties: run it to zero and the agent has
        # already braked by the time the next lap lands.
        # Nothing to top up before the caller has built its agent -- which it
        # does on the first controlled tick, because carla.ego does not exist
        # until the traffic simulator inserts the ego and the bridge adopts it.
        if agent is not None and                 len(agent.get_local_planner().get_plan()) < self._LAP_MARGIN:
            self._layLap()

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

    def _trafficActors(self, egoId):
        """This tick's traffic, as the CARLA actors mirroring it.

        Resolved wire id -> backend handle -> actor, which is the mapping the
        bridge itself uses to place them. A record with no actor yet (spawned
        this tick) is skipped rather than faked.
        """
        core, backend = currentCore(), currentBackend()
        if core is None or backend is None:
            return []
        mapped = core.mappedVehicles()
        out = []
        for r in self._others():
            vid = (r.id or '').strip()
            if not vid or vid == egoId:
                continue
            h = mapped.get(vid)
            actor = backend.actorOf(h) if h is not None else None
            if actor is None:
                continue
            out.append(_TrafficActor(actor, float(r.speed or 0.0), self._carla))
        return out

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
