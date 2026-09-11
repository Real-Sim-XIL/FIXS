"""Can a CARLA-shaped agent drive the FIXS ego, built on CARLA's own vehicle?

That is the adoption claim, and it is the kind that rots quietly: nothing in a
run says whether the agent was wired up or merely constructed.

So this drives the documented shape end to end -- `BehaviorAgent(carla.ego)`,
`session.drive(agent)`, then step it -- and asserts what must hold: the agent is
untouched, the world it sees carries FIXS's traffic and CARLA's
everything-else, and the command reaches the record.

A bare unit test has no simulator, so the backend is stubbed with the adapter's
own stand-ins. That is the only place stand-ins appear now; a run uses the real
`carla.ego`.

    python -m pytest tests/Python/unit/test_carla_agent_template.py
"""
from __future__ import annotations

import os

import pytest

from CommonLib import fixs
from CommonLib.fixs import carla as relay

realCarla = pytest.importorskip("carla", reason="needs the CARLA PythonAPI")
BehaviorAgent = pytest.importorskip(
    "agents.navigation.behavior_agent",
    reason="needs the vendored agents' dependencies").BehaviorAgent
adapter = pytest.importorskip("fixs_adapter")


class _StubBackend:
    """What a run's CarlaBackend exposes, without a simulator behind it."""

    def __init__(self):
        self.carlaEgoActor = adapter.EgoAdapter(realCarla)
        self.carlaWorld = self.carlaEgoActor.get_world()
        self.carlaClient = 'stub-client'
        self._actors = {}

    def place(self, handle, x=0.0):
        """A mirrored vehicle, where the bridge would have placed it."""
        a = adapter._OtherVehicle(realCarla, None)
        a._tf = realCarla.Transform(realCarla.Location(x=x, y=0.0, z=0.0),
                                    realCarla.Rotation(yaw=0.0))
        self._actors[handle] = a
        return a

    def actorOf(self, h):
        return self._actors.get(h)


class _StubCore:
    """VirEnvCore's wire id -> backend handle map, which is how a controller
    resolves a record to the actor mirroring it."""

    def __init__(self, mapping=None):
        self._m = dict(mapping or {})

    def mappedVehicles(self):
        return self._m


def config(tmp_path, **over):
    cfg = {
        "CarlaTimeStep": 0.05,
        "EgoTargetSpeed": 8.33,
        "EgoRouteSpacing": 2.0,
        # A straight 300 m route is enough: this is about wiring, not steering.
        "EgoRoutePoints": [(float(x), 0.0) for x in range(0, 320, 20)],
        "EgoRouteRepeat": 2,
        "EgoControllerLog": str(tmp_path / "agent.csv"),
    }
    cfg.update(over)
    return cfg


def record(**fields):
    """An ego record the way the bridge builds one, guard bypassed."""
    e = object.__new__(fixs.Vehicle)
    object.__setattr__(e, "id", fields.pop("id", "ego"))
    object.__setattr__(e, "_written", frozenset())
    values = dict(positionX=0.0, positionY=0.0, positionZ=0.0, heading=90.0,
                  speed=5.0, speedDesired=8.0, speedLimit=11.18, feedAge=0.0,
                  precedingVehicleDistance=-1.0, signalLightColor=0,
                  signalLightDistance=-1.0, length=4.5, width=1.8,
                  acceleratorPedalDesired=0.0, brakePedalDesired=0.0,
                  steerAngleDesired=0.0)
    values.update(fields)
    for k, v in values.items():
        object.__setattr__(e, k, v)
    return e


@pytest.fixture(autouse=True)
def backend():
    """Vehicle.set refuses fields absent from VehicleMessageField, which is a
    property of a connection these tests do not have; and fixs.carla needs a
    backend to answer `ego`, `world` and `map`."""
    from CommonLib.VirEnv import EgoControllerHost as host
    savedFields = fixs._declaredFields
    savedBackend, savedCore = host._backend, host._core
    fixs._declaredFields = None
    host._backend = _StubBackend()
    host._core = _StubCore()
    relay._reset()
    yield host._backend
    fixs._declaredFields = savedFields
    host._backend, host._core = savedBackend, savedCore
    relay._reset()


@pytest.fixture
def driven(tmp_path):
    """The template, verbatim: this is the shape users are given."""
    session = fixs.EgoSession(config(tmp_path), "ego")
    agent = BehaviorAgent(relay.ego, behavior="normal")
    session.drive(agent)
    yield session, agent
    session.close()


# --------------------------------------------------------------------------
# the agent stays the user's
# --------------------------------------------------------------------------

def test_the_agent_is_not_subclassed_or_rewritten(driven):
    """FIXS supplies what the agent reads and changes nothing it does. If its
    class or its methods had been swapped, this is where it would show."""
    _, agent = driven
    assert type(agent) is BehaviorAgent
    # The vehicle is the REAL ego, wrapped only to supply the speed limit an
    # imported corridor has no CARLA sign for.
    assert agent._vehicle._actor is relay.ego


def test_drive_carries_the_carla_step_into_both_PIDs(driven):
    """LocalPlanner builds its gain dicts BEFORE it reads opt_dict, so passing
    {'dt': ...} there is a no-op and both loops silently run on CARLA's 20 Hz
    default. drive() sets them through the planner's own setters instead, which
    is what lets the caller's constructor stay the one they already wrote."""
    session, agent = driven
    ctl = agent.get_local_planner()._vehicle_controller
    assert ctl._lat_controller._dt == pytest.approx(session.dt)
    assert ctl._lon_controller._dt == pytest.approx(session.dt)


# --------------------------------------------------------------------------
# the world the agent sees
# --------------------------------------------------------------------------

def test_traffic_is_the_real_actor_with_a_supplied_velocity(driven, backend):
    """Position, extent and id are the REAL mirrored actor's -- the bridge
    placed it, so there is no frame to convert and nothing to get wrong. Only
    velocity is supplied, because CARLA cannot report one for a physics-off
    actor moved by set_transform. Its own SUMO co-simulation is the same, and
    set_target_velocity on such an actor does nothing."""
    from CommonLib.VirEnv import EgoControllerHost as host
    actor = backend.place(7, x=12.0)
    host._core = _StubCore({"lead": 7})

    session, agent = driven
    session._others = lambda: [record(id="lead", positionX=99.0, speed=3.0)]
    session.update(record(), 0.05)

    seen = agent._world.get_actors().filter("*vehicle*")
    assert len(seen) == 1
    # 12.0 (the actor) not 99.0 (the record): the pose is CARLA's, not the wire's
    assert seen[0].get_transform().location.x == pytest.approx(12.0)
    v = seen[0].get_velocity()
    assert (v.x ** 2 + v.y ** 2) ** 0.5 == pytest.approx(3.0, abs=1e-6)


def test_the_ego_is_left_out_of_its_own_traffic(driven, backend):
    from CommonLib.VirEnv import EgoControllerHost as host
    backend.place(1)
    backend.place(2, x=9.0)
    host._core = _StubCore({"ego": 1, "other": 2})

    session, agent = driven
    session._others = lambda: [record(id="ego"),
                               record(id="other", positionX=9.0)]
    session.update(record(), 0.05)
    assert len(agent._world.get_actors().filter("*vehicle*")) == 1


def test_everything_that_is_not_a_vehicle_is_carlas_own(driven):
    """Traffic lights are real: the bridge keeps CARLA's in step with the
    traffic simulator, so there is nothing to substitute."""
    session, agent = driven
    session.update(record(), 0.05)
    agent._world.get_actors().filter("*traffic_light*")     # must not raise
    assert agent._world.get_map() is relay.map


# --------------------------------------------------------------------------
# the command, and the step
# --------------------------------------------------------------------------

def test_the_command_is_three_fields_the_caller_writes(driven):
    """A FIXS controller commands by WRITING to the record -- control()'s return
    value is ignored. FIXS does not do it for you, because that is the interface
    a non-CARLA controller has to use too."""
    session, agent = driven
    rec = record()
    session.update(rec, 0.05)
    c = agent.run_step()
    rec.set(acceleratorPedalDesired=c.throttle, brakePedalDesired=c.brake,
            steerAngleDesired=c.steer * fixs.MAX_STEER_RAD)
    assert rec._written == {"acceleratorPedalDesired", "brakePedalDesired",
                            "steerAngleDesired"}


def test_the_speed_limit_comes_from_the_wire(driven):
    """An imported corridor carries no CARLA speed-limit signs, so the actor
    reports a value that is not a speed -- an agent computing its look-ahead as
    int(speed_limit / 10) overflowed a deque index with it. SUMO owns the limit
    here and publishes it."""
    session, agent = driven
    session.update(record(speedLimit=11.18), 0.05)
    assert agent._vehicle.get_speed_limit() == pytest.approx(11.18 * 3.6)


def test_the_advisory_governs_the_target(driven):
    """Every run_step branch takes min(max_speed, speed_limit - speed_lim_dist),
    so the eco advisory has to arrive as max_speed with the margin zeroed -- a
    target is not a ceiling to undercut."""
    session, agent = driven
    session.update(record(speedDesired=6.0), 0.05)
    assert agent._behavior.max_speed == pytest.approx(6.0 * 3.6)
    assert agent._behavior.speed_lim_dist == 0.0


def test_the_route_is_relaid_before_the_plan_runs_out(driven):
    """The corridor is a LAP the traffic simulator drives EgoRouteRepeat times.
    The agent has no notion of that and brakes to a stop when its plan empties,
    which reads exactly like a stall."""
    session, _ = driven
    assert session.lapsLaid == 1
    session.update(record(), 0.05)
    assert session.lapsLaid == 2
    session.update(record(), 0.05)
    assert session.lapsLaid == 2


def test_a_teleport_tick_yields_no_command(driven):
    """Adoption reads the ego once before its spawn pose is applied, so the
    first step is a kilometre. Steering off a discontinuity aims at a place the
    car is not; measured, it wound the lateral PID to saturation."""
    session, _ = driven
    assert session.update(record(positionX=0.0), 0.05) is True
    assert session.update(record(positionX=0.4), 0.05) is True     # motion
    assert session.update(record(positionX=900.0), 0.05) is False  # a jump
    assert session.update(record(positionX=900.4), 0.05) is True


def test_an_empty_route_fails_at_setup_rather_than_mid_run(tmp_path):
    with pytest.raises(SystemExit):
        fixs.EgoSession(config(tmp_path, EgoRoutePoints=[]), "ego")


def test_the_log_carries_one_row_per_command(tmp_path, driven):
    session, agent = driven
    for _ in range(5):
        rec = record()
        session.update(rec, 0.05)
        session.logStep(rec, agent.run_step())
    session.close()
    with open(os.path.join(str(tmp_path), "agent.csv"), encoding="utf-8") as f:
        rows = f.read().strip().splitlines()
    assert len(rows) == 6                       # header + 5
    assert rows[0].startswith("t,feedAge,x,y")


def test_get_transform_hands_out_a_copy():
    """carla.Actor.get_transform returns a fresh copy, and agent code relies on
    it: `front = tf; front.location += ...` (basic_agent.py:367, :410). The
    stand-ins used for traffic must do the same, or every detector call walks
    the vehicle's own pose half a body forward."""
    v = adapter._OtherVehicle(realCarla, None, speed_ms=5.0)
    before = v.get_transform().location.x
    scratch = v.get_transform()
    scratch.location.x += 100.0
    assert v.get_transform().location.x == pytest.approx(before)
