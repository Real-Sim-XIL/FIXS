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
        self.vehicles = _ActorListView(world=world, lights=lights)

    def get_map(self):
        return self._map

    def get_actors(self):
        return self.vehicles


_view = None


def bind(agent, egoId='', speedLimit=0.0):
    """Give an agent FIXS's corrected view of the world, and nothing else.

    ``speedLimit`` (m/s) is what the ego's limit reads until the wire publishes
    a real one -- the caller's own default, since what to drive at where the
    road says nothing is the application's policy, not FIXS's.

    Its class, its methods and its logic are untouched -- this swaps the WORLD
    it reads, not the decisions it makes. Call once, after constructing it on
    `carla.ego`; call `refresh` each tick.
    """
    global _view
    _view = _WorldView(__getattr__('map'), __getattr__('world'), _signalHeads())
    agent._world = _view
    agent._map = _view.get_map()
    ego = _EgoActor(agent._vehicle, speedLimit)
    agent._vehicle = ego
    _view.ego = ego
    lp = getattr(agent, 'get_local_planner', lambda: None)()
    if lp is not None and getattr(lp, '_vehicle', None) is not None:
        lp._vehicle = ego
        vc = getattr(lp, '_vehicle_controller', None)
        for sub in ('_lon_controller', '_lat_controller'):
            c = getattr(vc, sub, None)
            if c is not None and hasattr(c, '_vehicle'):
                c._vehicle = ego
    return agent


def refresh(record, others=()):
    """This tick's traffic and speed limit. Call before the agent runs."""
    if _view is None:
        return
    egoId = (getattr(record, 'id', '') or '').strip()
    _view.vehicles = _ActorListView(_trafficActors(others, egoId),
                                    world=_view._world, lights=_view._lights)
    if _view.ego is not None:
        _view.ego.setSpeedLimit(float(getattr(record, 'speedLimit', 0.0) or 0.0))


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
    """Drop the cached map -- for tests, and for a backend swapped mid-process."""
    global _mapCache
    _mapCache = None
