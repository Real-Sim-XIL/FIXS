"""CARLA, reached through FIXS.

A user bringing a CARLA-shaped controller changes its import and keeps the rest
of its code::

    -import carla
    +import fixs.carla as carla

Every ``carla.XX`` site in their file then goes on working. What changes is WHO
EXECUTES the calls that touch the simulator: FIXS does, against the session the
bridge already owns. The user never opens a client, never ticks the world, and
cannot end up with a second connection racing the one driving the run.

Two kinds of name live here, and the split is the whole design.

VALUE TYPES -- ``Location``, ``Transform``, ``Rotation``, ``Vector3D``,
``LaneType``, ``VehicleControl`` and the rest -- are re-exported unchanged.
Constructing one is arithmetic: three numbers in a struct, no connection
involved, nothing to mediate. Relaying is about who performs calls, and a
``Location(x=2.5)`` is not a call. Measured on a real 2656-line controller: 38
of its 40 ``carla.`` sites are these, and all 38 need no FIXS involvement at all.

SESSION OBJECTS -- ``client``, ``world``, ``map``, ``ego`` -- are answered by
FIXS from the backend. ``ego`` is the REAL physics vehicle, which is what a
user's agent should be built on::

    -vehicle = world.get_actor(some_id)
    +vehicle = carla.ego

This is where the relay earns its keep: one owner of the CARLA session, and a
seam a non-CARLA backend can answer later.

WHAT IS REFUSED, and why it is refused rather than quietly allowed:

    carla.Client(host, port)   a second connection to the same server, racing
                              the bridge's. Use `carla.client`.
    world.tick()              only one client may advance a synchronous world,
                              and the bridge is it. Return from control(ego, dt)
                              instead -- that IS the tick.

Actuation is the third thing FIXS keeps, and it is not refused here because it
never reaches here: the bridge applies the FIXS record to the physics ego, so a
controller commands with ``ego.set(acceleratorPedalDesired=...)`` rather than
``actor.apply_control()``. Two writers would fight over the actuator.

See ORNL-Real-Sim/FIXS#305.
"""
from __future__ import annotations

import importlib
import math

__all__ = ['client', 'world', 'map', 'ego', 'available', 'bind', 'refresh']

_carla = None
_mapCache = None


# ---------------------------------------------------------------------------
# what CARLA cannot state here, supplied on otherwise-real actors
# ---------------------------------------------------------------------------
#
# Three values, each one property on a real CARLA object, each with a measured
# reason. Everything else an agent reads -- poses, bounding boxes, ids, the map
# -- is CARLA's own, because reconstructing those is what produced a front-vs-
# centre anchor error, a missing elevation and a hand-rolled yaw, each found
# only by a crash.

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

    #: The fastest a wire speed limit can plausibly be, in m/s (~360 km/h).
    #: Above it the value is not a speed, so it is not a reading.
    _MAX_LIMIT = 100.0

    __slots__ = ('_actor', '_limitKmh')

    def __init__(self, actor, speedLimit=0.0):
        self._actor = actor
        self._limitKmh = max(0.0, float(speedLimit)) * 3.6

    def setSpeedLimit(self, mps):
        """Take the wire's limit, but only if it IS one.

        The corridor has stretches -- the U-turn's internal lanes -- where the
        traffic simulator has no limit to publish and the field arrives as
        -815417536. Clamping that to zero is worse than ignoring it: an agent
        takes min(its target, the limit) in every branch it has, so a limit of
        zero is an order to stop, and it obeys forever. A limit does not
        vanish between two ticks of the same road, so the last real one stands.
        """
        try:
            mps = float(mps)
        except (TypeError, ValueError):
            return
        if 0.0 < mps < self._MAX_LIMIT:
            self._limitKmh = mps * 3.6

    def get_speed_limit(self):
        return self._limitKmh

    def __getattr__(self, name):
        return getattr(self._actor, name)


class _LightActor:
    """A CARLA traffic light, presented AT its stop bar.

    An agent finds the signal governing it by deriving a trigger location from
    the actor's transform and its trigger_volume, snapping that to a lane and
    comparing road_id with its own. On an imported corridor those volumes do
    not line up with the lanes: measured against the wire, an agent matched
    only 619 of 2667 red ticks (23%), drove into a junction at 3.8 m/s on a
    red, and was struck by crossing traffic.

    FIXS knows exactly where each signalised movement's stop bar is
    (tl_table.csv) and which actor shows it. Presenting the actor at that pose
    with a zero trigger volume makes the agent's OWN derivation return the stop
    bar, so its logic works unmodified -- this corrects an input, it does not
    replace a decision. `state` is the real actor's; the bridge already keeps it
    in step with the traffic simulator.
    """

    __slots__ = ('_actor', '_tf', 'trigger_volume')

    def __init__(self, actor, stopBarTf, carla_mod):
        self._actor = actor
        self._tf = stopBarTf
        zero = carla_mod.Vector3D(0.0, 0.0, 0.0)
        self.trigger_volume = carla_mod.BoundingBox(
            carla_mod.Location(0.0, 0.0, 0.0), zero)

    def get_transform(self):
        return self._tf

    def get_location(self):
        return self._tf.location

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

    def __init__(self, vehicles=(), world=None, lights=None):
        super().__init__(vehicles)
        self._world = world
        self._lights = lights

    def filter(self, pattern):
        if 'vehicle' in pattern:
            return list(self)
        if 'traffic_light' in pattern and self._lights is not None:
            return list(self._lights)
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

    def __init__(self, carlaMap, world, lights=None):
        self._map = carlaMap
        self._world = world
        self._lights = lights
        self.ego = None
        self.agent = None
        self.vehicles = _ActorListView(world=world, lights=lights)

    def get_map(self):
        return self._map

    def get_actors(self):
        return self.vehicles


_view = None
#: Laps of the scenario's corridor already given to the agent.
_lapsLaid = 0
#: Waypoints left at which the next lap is appended (~400 m at 2 m spacing).
_kPlanMargin = 200
#: Whether the vehicle feed has already reported itself unreadable.
_feedFailed = False


def bind(agent, egoId=''):
    """Give an agent FIXS's corrected view of the world, and the ego's route.

    Its class, its methods and its logic are untouched -- this swaps what the
    agent READS, never what it decides. Call once, after constructing it on
    `carla.ego`; call `refresh` each tick.

    The route is FIXS's to give. The ego is a vehicle in the traffic simulator
    with a route already assigned, and the traffic around it reacts on that
    basis, so an agent must not plan its own: `set_destination` would have it
    choose a path the rest of the simulation does not know about. What the
    agent gets instead is the scenario's own corridor, as CARLA waypoints on
    real lanes -- which is what its planner reads lane_id, road_id and
    is_junction off.
    """
    global _view, _lapsLaid
    _lapsLaid = 0
    _view = _WorldView(__getattr__('map'), __getattr__('world'), _signalHeads())
    agent._world = _view
    agent._map = _view.get_map()
    ego = _EgoActor(agent._vehicle, _configured('EgoTargetSpeed', 0.0))
    agent._vehicle = ego
    _view.ego = ego
    _view.agent = agent
    lp = getattr(agent, 'get_local_planner', lambda: None)()
    if lp is not None and getattr(lp, '_vehicle', None) is not None:
        lp._vehicle = ego
        vc = getattr(lp, '_vehicle_controller', None)
        for sub in ('_lon_controller', '_lat_controller'):
            c = getattr(vc, sub, None)
            if c is not None and hasattr(c, '_vehicle'):
                c._vehicle = ego
    _layRoute(agent, first=True)
    return agent


def refresh(record):
    """Bring the agent's view up to this tick. Call before the agent runs.

    The traffic, the ego's speed limit, and enough route left to plan on. All
    three are FIXS's to know, so none of them is asked of the caller.
    """
    if _view is None:
        return
    egoId = (getattr(record, 'id', '') or '').strip()
    _view.vehicles = _ActorListView(_trafficActors(_feedVehicles(), egoId),
                                    world=_view._world, lights=_view._lights)
    if _view.ego is not None:
        _view.ego.setSpeedLimit(float(getattr(record, 'speedLimit', 0.0) or 0.0))
    _layRoute(_view.agent)


def _feedVehicles():
    """Every vehicle record in this tick's feed.

    Read here rather than asked of the controller: the caller would only be
    fetching FIXS's own data to hand it straight back. A torn-down connection
    must not take the run down, but it is reported once -- an empty list is
    indistinguishable from an empty road, and an agent given one drives through
    traffic for a whole run without a word (ORNL-Real-Sim/FIXS#355).
    """
    global _feedFailed
    from CommonLib import fixs
    try:
        return [v for v in (fixs.vehicle.get(i)
                            for i in fixs.vehicle.getIDList()) if v is not None]
    except Exception as exc:                                    # noqa: BLE001
        if not _feedFailed:
            _feedFailed = True
            print('[fixs] cannot read the vehicle feed (%s: %s); the agent is '
                  'driving an empty world from here on.'
                  % (type(exc).__name__, exc), flush=True)
        return ()


def _layRoute(agent, first=False):
    """Keep the agent's plan topped up with the ego's route.

    The corridor is a LAP the traffic simulator drives EgoRouteRepeat times.
    An agent has no notion of that: its plan empties, it brakes to a stop, and
    that reads exactly like a stall. Re-laying is therefore FIXS's job, because
    repeating the lap is FIXS's doing.
    """
    global _lapsLaid
    if agent is None:
        return
    lp = getattr(agent, 'get_local_planner', lambda: None)()
    if not first and (lp is None or len(lp.get_plan()) >= _kPlanMargin):
        return
    if _lapsLaid >= max(1, int(_configured('EgoRouteRepeat', 1))):
        return
    plan = _routePlan()
    if not plan:
        return
    if _lapsLaid == 0:
        agent.set_global_plan(plan)
    else:
        agent.set_global_plan(plan, stop_waypoint_creation=True,
                              clean_queue=False)
    _lapsLaid += 1


def _routePlan():
    """The scenario's corridor as (waypoint, RoadOption) pairs on real lanes.

    Three conversions, all FIXS's. The y flip between FIXS's north-positive
    frame and CARLA's. The spacing: CARLA's own plans are ~2 m apart and
    LocalPlanner's purge distance is tuned for that, while a scenario route is
    decimated -- ~14 m on this corridor. And the DIRECTION.
    """
    pts = _configured('EgoRoutePoints', None) or []
    if len(pts) < 2:
        return []
    from agents.navigation.local_planner import RoadOption
    from Carla.carla_agents.fixs_adapter import densify
    real, cmap = _real(), __getattr__('map')
    dense = densify([(float(a), float(b)) for a, b in pts],
                    float(_configured('EgoRouteSpacing', 2.0) or 2.0))
    out, opposed, crossed = [], 0, 0
    for i, (x, y) in enumerate(dense):
        heading = _headingAt(dense, i)
        snapped = cmap.get_waypoint(real.Location(x=x, y=-y, z=0.0),
                                    project_to_road=True)
        if snapped is None:
            continue
        wp = _facingLane(snapped, heading, real, cmap, x, -y)
        if not _agrees(snapped, heading):
            opposed += 1
            if wp is not snapped:
                crossed += 1
        out.append((wp, RoadOption.LANEFOLLOW))
    print('[fixs] ego route: %d waypoints; %d snapped against the route, '
          '%d crossed to the lane that agrees'
          % (len(out), opposed, crossed), flush=True)
    return out


def _headingAt(points, i):
    """Which way the route is going at points[i], as a CARLA yaw in degrees."""
    j = i + 1 if i + 1 < len(points) else i
    k = j - 1 if j > 0 else 0
    (ax, ay), (bx, by) = points[k], points[j]
    return math.degrees(math.atan2(-(by - ay), bx - ax))


def _facingLane(wp, headingDeg, real, cmap=None, x=0.0, y=0.0, reach=20.0):
    """The lane at this route point that goes the way the route goes.

    get_waypoint(project_to_road=True) snaps to the NEAREST driving lane and
    says nothing about direction. On an out-and-back corridor the carriageways
    are metres apart, so the return leg snaps onto the OUTBOUND lane and the
    plan leads the ego up the wrong side of the road. Measured: the ego drove
    the wrong way along lane E3_0 until it met an oncoming vehicle 3.09 m ahead,
    both at a standstill, and the agent held an emergency stop for the rest of
    the run -- correctly, on a route it should never have been given.

    Crossing lane links is tried first and is usually enough on a divided road
    carried as one road in OpenDRIVE. It is NOT enough here: measured on this
    corridor, 184 of 2558 waypoints snapped against the route and not one of
    them could reach an agreeing lane that way, because the two directions are
    separate roads. So the second attempt probes SIDEWAYS in space -- the
    opposing carriageway is a few metres abeam -- and takes the nearest lane
    that agrees. Nothing within `reach` metres that agrees means the map has no
    such lane, and CARLA's own answer stands rather than a made-up one.
    """
    if wp is None or _agrees(wp, headingDeg):
        return wp
    for step in ('get_left_lane', 'get_right_lane'):
        cur = wp
        for _ in range(4):
            nxt = getattr(cur, step, lambda: None)()
            if nxt is None or str(nxt.lane_type) != str(real.LaneType.Driving):
                break
            if _agrees(nxt, headingDeg):
                return nxt
            cur = nxt
    if cmap is None:
        return wp
    rad = math.radians(headingDeg)
    nx, ny = -math.sin(rad), math.cos(rad)          # unit normal, CARLA frame
    best, bestOff = None, None
    off = 2.0
    while off <= reach:
        for sign in (1.0, -1.0):
            probe = cmap.get_waypoint(
                real.Location(x=x + nx * off * sign, y=y + ny * off * sign,
                              z=0.0), project_to_road=True)
            if probe is not None and _agrees(probe, headingDeg):
                if bestOff is None or off < bestOff:
                    best, bestOff = probe, off
        if best is not None:
            return best
        off += 2.0
    return wp


def _agrees(wp, headingDeg):
    """Does this lane run the way the route runs? Within a right angle."""
    d = (wp.transform.rotation.yaw - headingDeg + 180.0) % 360.0 - 180.0
    return abs(d) <= 90.0


def _configured(key, default=None):
    from CommonLib.VirEnv.EgoControllerHost import currentConfig
    cfg = currentConfig()
    if cfg is None:
        return default
    v = cfg.get(key)
    return default if v is None else v


def _signalHeads():
    """Every signal head, at the stop bar it actually governs."""
    b = _backend()
    heads = getattr(b, 'signalHeads', None)
    if heads is None:
        return None
    try:
        return [_LightActor(a, tf, _real()) for a, tf in heads()]
    except Exception:
        return None


def _trafficActors(records, egoId):
    """This tick's traffic, as the CARLA actors mirroring it.

    Resolved wire id -> core handle -> actor, the same mapping the bridge uses
    to place them. A record with no actor yet (spawned this tick) is skipped
    rather than faked.
    """
    from CommonLib.VirEnv.EgoControllerHost import currentCore
    core, backend = currentCore(), _backend()
    if core is None or backend is None:
        return []
    mapped, out = core.mappedVehicles(), []
    for r in records:
        vid = (getattr(r, 'id', '') or '').strip()
        if not vid or vid == egoId:
            continue
        h = mapped.get(vid)
        actor = backend.actorOf(h) if h is not None else None
        if actor is not None:
            out.append(_TrafficActor(actor, float(r.speed or 0.0), _real()))
    return out


def _real():
    """The installed CARLA package. Value types come from here verbatim."""
    global _carla
    if _carla is None:
        _carla = importlib.import_module('carla')
    return _carla


def _backend():
    from CommonLib.VirEnv.EgoControllerHost import currentBackend
    return currentBackend()


def available():
    """Is a FIXS backend behind this module? False under a bare unit test."""
    return _backend() is not None


class _Refused(RuntimeError):
    """A call FIXS holds rather than forwards."""


def Client(*_args, **_kwargs):                              # noqa: N802
    """Refused: FIXS owns the connection.

    Opening a second client to the same server gives a synchronous run two
    parties who each think they may advance it. The bridge is already connected;
    ask for that one.
    """
    raise _Refused(
        "carla.Client(...) is not available through FIXS -- the bridge already "
        "holds the connection, and a second client racing it is how a "
        "synchronous run loses its clock.\n"
        "    use:  world = carla.world      (or carla.client for the client)")


def __getattr__(name):
    """Session names from FIXS; everything else from CARLA itself."""
    if name == 'client':
        b = _backend()
        c = getattr(b, 'carlaClient', None) if b is not None else None
        if c is None:
            raise _Refused(
                "carla.client needs a running FIXS backend, and none is "
                "registered. This module is only live inside a controller the "
                "bridge loaded (EgoSetup.Controller).")
        return c
    if name == 'world':
        b = _backend()
        w = getattr(b, 'carlaWorld', None) if b is not None else None
        if w is None:
            raise _Refused(
                "carla.world needs a running FIXS backend, and none is "
                "registered. This module is only live inside a controller the "
                "bridge loaded (EgoSetup.Controller).")
        return w
    if name == 'ego':
        b = _backend()
        a = getattr(b, 'carlaEgoActor', None) if b is not None else None
        if a is None:
            raise _Refused(
                "carla.ego is not up yet. The traffic simulator inserts the ego "
                "and the bridge adopts it, so it exists from the tick it enters "
                "-- build your agent in setup() only if the scenario spawns it "
                "up front, otherwise on the first control() call.")
        return a
    if name == 'map':
        global _mapCache
        if _mapCache is None:
            # Cached because get_map() serialises the whole OpenDRIVE; a
            # controller asking per tick would pay that on every step.
            _mapCache = __getattr__('world').get_map()
        return _mapCache
    return getattr(_real(), name)


def _reset():
    """Drop everything cached per run -- for tests, and for a backend swapped
    mid-process."""
    global _mapCache, _view, _lapsLaid, _feedFailed
    _mapCache = None
    _view = None
    _lapsLaid = 0
    _feedFailed = False
