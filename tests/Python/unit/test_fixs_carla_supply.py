"""What FIXS supplies an agent, and what it leaves alone.

`carla.bind` swaps the WORLD an agent reads; it must never touch the agent
itself. Three values are supplied on otherwise-real CARLA objects, each because
CARLA cannot state it in a co-simulation, and each with a measured reason:

    traffic velocity   mirrors are physics-off and moved by set_transform, so
                       get_velocity is 0.000 -- 182 of 182 while 119 were
                       moving -- and set_target_velocity on one does nothing.
                       CARLA's own SUMO co-simulation behaves identically.
    ego speed limit    an imported corridor carries no CARLA speed-limit signs,
                       so the actor returns a value that is not a speed; an
                       agent computing int(speed_limit / 10) for its look-ahead
                       overflowed a deque index with it.
    signal stop bar    an agent locates the signal governing it from a light's
                       trigger volume, and those do not line up with the lanes
                       on an imported map.

Everything else -- poses, bounding boxes, ids, the map -- is CARLA's own.
Reconstructing those is what produced a front-vs-centre anchor error, a missing
elevation and a hand-rolled yaw, each found only by a crash.

    python -m pytest tests/Python/unit/test_fixs_carla_supply.py
"""
from __future__ import annotations

import pytest

from CommonLib.fixs import carla as relay

realCarla = pytest.importorskip("carla", reason="needs the CARLA PythonAPI")


class _Actor:
    """A CARLA actor, as far as this test is concerned."""

    def __init__(self, x=0.0, yaw=0.0, actorId=1):
        self.id = actorId
        self._tf = realCarla.Transform(realCarla.Location(x=x, y=0.0, z=205.0),
                                       realCarla.Rotation(yaw=yaw))
        self.bounding_box = realCarla.BoundingBox(
            realCarla.Location(0, 0, 0), realCarla.Vector3D(2.3, 1.0, 0.75))

    def get_transform(self):
        # A real carla.Actor hands out a FRESH copy every call, and agent code
        # relies on it. The stub does the same, so the test below exercises the
        # proxy rather than this class.
        t = self._tf
        return realCarla.Transform(
            realCarla.Location(t.location.x, t.location.y, t.location.z),
            realCarla.Rotation(t.rotation.pitch, t.rotation.yaw, t.rotation.roll))

    def get_location(self):
        return self.get_transform().location

    def get_velocity(self):
        return realCarla.Vector3D(0.0, 0.0, 0.0)     # what a mirror reports

    def get_speed_limit(self):
        return 1e18                                  # what an unsigned road gives


class _World:
    def __init__(self):
        self.lights = []

    def get_map(self):
        return 'the-map'

    def get_actors(self):
        class _L(list):
            def filter(self, _p):
                return []
        return _L()


class _Backend:
    def __init__(self):
        self.carlaWorld = _World()
        self.carlaClient = 'client'
        self.carlaEgoActor = _Actor(actorId=99)
        self._actors = {}
        self.heads = []

    def actorOf(self, h):
        return self._actors.get(h)

    def signalHeads(self):
        return self.heads


class _Core:
    def __init__(self, mapping=None):
        self._m = dict(mapping or {})

    def mappedVehicles(self):
        return self._m


class _Agent:
    """Only what bind touches."""

    def __init__(self, vehicle):
        self._vehicle = vehicle
        self._world = None
        self._map = None

    def get_local_planner(self):
        return None


class _Rec:
    def __init__(self, vid='ego', speed=0.0, limit=11.18):
        self.id, self.speed, self.speedLimit = vid, speed, limit


@pytest.fixture(autouse=True)
def wired():
    from CommonLib.VirEnv import EgoControllerHost as host
    savedB, savedC = host._backend, host._core
    host._backend, host._core = _Backend(), _Core()
    relay._reset()
    yield host
    host._backend, host._core = savedB, savedC
    relay._reset()


# --------------------------------------------------------------------------

def test_bind_leaves_the_agent_itself_alone(wired):
    agent = _Agent(relay.ego)
    before = type(agent)
    relay.bind(agent, 'ego')
    assert type(agent) is before                 # not subclassed
    assert agent._vehicle._actor is wired._backend.carlaEgoActor


def test_bind_gives_the_agent_carlas_own_map(wired):
    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego')
    assert agent._map == 'the-map'


def test_traffic_is_the_real_actor_with_a_supplied_velocity(wired):
    """The mirror reports 0.000; the wire knows the speed. Pose stays CARLA's."""
    actor = _Actor(x=12.0, actorId=7)
    wired._backend._actors[7] = actor
    wired._core = _Core({'lead': 7})
    from CommonLib.VirEnv import EgoControllerHost as host
    host._core = wired._core

    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego')
    relay.refresh(_Rec('ego'), [_Rec('lead', speed=3.0)])

    seen = agent._world.get_actors().filter('*vehicle*')
    assert len(seen) == 1
    assert seen[0].get_transform().location.x == pytest.approx(12.0)
    assert seen[0].get_transform().location.z == pytest.approx(205.0)   # not 0
    v = seen[0].get_velocity()
    assert (v.x ** 2 + v.y ** 2) ** 0.5 == pytest.approx(3.0, abs=1e-6)


def test_the_ego_is_left_out_of_its_own_traffic(wired):
    from CommonLib.VirEnv import EgoControllerHost as host
    wired._backend._actors.update({1: _Actor(actorId=1), 2: _Actor(x=9.0, actorId=2)})
    host._core = _Core({'ego': 1, 'other': 2})

    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego')
    relay.refresh(_Rec('ego'), [_Rec('ego'), _Rec('other')])
    assert len(agent._world.get_actors().filter('*vehicle*')) == 1


def test_a_record_with_no_actor_yet_is_skipped_not_faked(wired):
    """Spawned this tick: the bridge has no actor for it. Inventing one is how
    a reconstruction gets a pose wrong."""
    from CommonLib.VirEnv import EgoControllerHost as host
    host._core = _Core({})
    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego')
    relay.refresh(_Rec('ego'), [_Rec('brand-new', speed=5.0)])
    assert agent._world.get_actors().filter('*vehicle*') == []


def test_the_speed_limit_comes_from_the_wire(wired):
    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego')
    relay.refresh(_Rec('ego', limit=11.18), [])
    assert agent._vehicle.get_speed_limit() == pytest.approx(11.18 * 3.6)
    assert agent._vehicle.get_speed_limit() < 1e6      # not the actor's value


def test_a_signal_is_presented_at_its_stop_bar(wired):
    """An agent derives the light governing it from the actor's transform and
    trigger volume; a zero volume at the stop bar makes that derivation land
    where the stop bar really is."""
    light, bar = _Actor(actorId=11), realCarla.Transform(
        realCarla.Location(x=40.0, y=5.0, z=205.0))
    wired._backend.heads = [(light, bar)]

    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego')
    lights = agent._world.get_actors().filter('*traffic_light*')
    assert len(lights) == 1
    assert lights[0].get_transform().location.x == pytest.approx(40.0)
    assert lights[0].trigger_volume.extent.x == pytest.approx(0.0)
    assert lights[0].id == 11                    # still the real light


def test_get_transform_hands_out_a_copy(wired):
    """carla.Actor.get_transform returns a fresh copy and agent code relies on
    it: `front = tf; front.location += ...` (basic_agent.py:367, :410). The
    proxy must forward that call rather than cache a transform of its own --
    caching one is what walked a vehicle's pose half a body forward per
    detector call and stalled the ego outright."""
    from CommonLib.VirEnv import EgoControllerHost as host
    wired._backend._actors[7] = _Actor(x=12.0, actorId=7)
    host._core = _Core({'lead': 7})
    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego')
    relay.refresh(_Rec('ego'), [_Rec('lead', speed=3.0)])

    v = agent._world.get_actors().filter('*vehicle*')[0]
    before = v.get_transform().location.x
    scratch = v.get_transform()
    scratch.location.x += 100.0
    assert v.get_transform().location.x == pytest.approx(before)


def test_a_limit_that_is_not_a_speed_is_refused(wired):
    """The wire publishes -815417536 where the traffic simulator has no limit
    to give -- the U-turn's internal lanes. Clamping that to zero is an order to
    stop that an agent obeys forever: it takes min(target, limit) in every
    branch. Measured: 12876 consecutive ticks parked at one waypoint."""
    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego', speedLimit=8.33)
    relay.refresh(_Rec('ego', limit=11.18), [])
    relay.refresh(_Rec('ego', limit=-815417536.0), [])
    assert agent._vehicle.get_speed_limit() == pytest.approx(11.18 * 3.6)


def test_the_seed_stands_until_the_wire_publishes_a_real_one(wired):
    agent = _Agent(relay.ego)
    relay.bind(agent, 'ego', speedLimit=8.33)
    assert agent._vehicle.get_speed_limit() == pytest.approx(8.33 * 3.6)
    relay.refresh(_Rec('ego', limit=-815417536.0), [])
    assert agent._vehicle.get_speed_limit() == pytest.approx(8.33 * 3.6)
    relay.refresh(_Rec('ego', limit=13.41), [])
    assert agent._vehicle.get_speed_limit() == pytest.approx(13.41 * 3.6)
