"""Does the in-process controller hook close its loop on the plant? (#305, #325)

That is the one question this file exists to answer, because getting it wrong is
invisible in every log a run produces.

An ego controller can occupy the driver slot in two places. Served as a FIXS
client it sees the ego record only at the 0.1 s feed -- and on that path `speed`
is whatever the traffic simulator left there, which under an L2 scenario is the
eco controller's ADVISORY rather than the vehicle's measured speed. A controller
closing a speed loop on it compares its target against a delayed copy of that
same target, sees an error of ~0, and never corrects. Its own log then shows
textbook tracking while the car does something else entirely.

Loaded in-process, runController refreshes the record from the backend before
every call, so the loop closes on the plant. These tests drive the REAL host
against a stub backend -- no CARLA, no SUMO -- and assert the property directly:
hold the advisory fixed, change only what the plant reports, and what the
controller sees must change.

    python -m pytest tests/Python/unit/test_ego_controller_host.py
"""
from __future__ import annotations

import pytest

from CommonLib import fixs
from CommonLib.VirEnv.EgoControllerHost import loadController, runController
from CommonLib.VirEnv.IVirEnvBackend import EgoState

# The three shapes loadController accepts. Normalising them is what the host is
# for, so all three are exercised; each records what it was handed.
#
# A controller commands by WRITING to the ego record. control()'s return value
# is ignored -- see test_a_returned_command_is_ignored below.
CLASS_FORM = """
class Controller:
    def __init__(self, config, egoId):
        self.seen = []

    def control(self, ego, dt):
        self.seen.append((ego.speed, ego.speedDesired, dt))
        ego.set(acceleratorPedalDesired=0.4, brakePedalDesired=0.0,
                steerAngleDesired=0.0)
"""

FUNC_WITH_SETUP = """
def setup(config, egoId):
    return {"seen": []}


def control(ego, dt, state):
    state["seen"].append((ego.speed, ego.speedDesired, dt))
    ego.set(acceleratorPedalDesired=0.4, brakePedalDesired=0.0,
            steerAngleDesired=0.0)
"""

FUNC_BARE = """
def control(ego, dt):
    ego.set(acceleratorPedalDesired=0.4, brakePedalDesired=0.0,
            steerAngleDesired=0.0)
"""

RETURNS_A_DICT = """
def control(ego, dt):
    return {"throttle": 0.4, "brake": 0.0, "steer": 0.0}
"""


class StubBackend:
    """The three verbs runController touches, and nothing else."""

    def __init__(self, speed):
        self.state = EgoState()
        self.state.speed, self.state.x, self.state.y = speed, 0.0, 0.0
        self.state.heading = 90.0
        self.applied = []

    def readEgoState(self, egoId, out):
        for name in ("speed", "x", "y", "z", "heading", "grade", "brake",
                     "indL", "indR"):
            setattr(out, name, getattr(self.state, name, 0.0))
        return True

    def applyEgoActuation(self, throttle, brake, steerNorm):
        self.applied.append(("actuation", throttle, brake, steerNorm))

    def applyEgoSpeedSteer(self, speed, steerNorm, accel=None, jerk=None):
        self.applied.append(("speedsteer", speed, steerNorm))


def makeEgo(**fields):
    """A Vehicle built the way the bridge builds one, guard bypassed."""
    ego = object.__new__(fixs.Vehicle)
    object.__setattr__(ego, "id", "ego")
    object.__setattr__(ego, "_written", frozenset())
    defaults = dict(positionX=0.0, positionY=0.0, positionZ=0.0, heading=90.0,
                    speed=0.0, speedDesired=8.0, feedAge=0.0,
                    acceleratorPedalDesired=0.0, brakePedalDesired=0.0,
                    steerAngleDesired=0.0)
    defaults.update(fields)
    for k, v in defaults.items():
        object.__setattr__(ego, k, v)
    return ego


@pytest.fixture(autouse=True)
def noWireCheck():
    """Vehicle.set refuses fields absent from VehicleMessageField, which is a
    property of a connection these tests do not have."""
    saved = fixs._declaredFields
    fixs._declaredFields = None
    yield
    fixs._declaredFields = saved


@pytest.fixture
def controllerPath(tmp_path):
    p = tmp_path / "probe_controller.py"
    p.write_text(CLASS_FORM, encoding="utf-8")
    return str(p)


def seenBy(ctl):
    """What the loaded controller recorded, whichever shape it was written in."""
    if ctl._instance is not None:
        return ctl._instance.seen
    return ctl._state["seen"]


def drive(controllerPath, plantSpeed, advisory, steps=1, dt=0.05):
    backend = StubBackend(plantSpeed)
    ctl = loadController(controllerPath)
    ctl.setup({}, "ego")
    ego = makeEgo(speedDesired=advisory)
    kind = None
    for _ in range(steps):
        kind = runController(backend, ctl, ego, dt, True, maxSteerRad=0.7)
    return kind, ego, backend, ctl


def test_a_controller_loads_from_a_path_and_its_command_is_applied(controllerPath):
    kind, _, backend, _ = drive(controllerPath, plantSpeed=5.0, advisory=8.0)
    assert kind == "actuation", kind
    assert backend.applied and backend.applied[-1][0] == "actuation"


def test_the_speed_handed_over_is_the_PLANT_not_the_advisory(controllerPath):
    """The bug this file exists for, asserted directly."""
    _, ego, _, ctl = drive(controllerPath, plantSpeed=7.25, advisory=3.0)
    assert ego.speed == pytest.approx(7.25)
    assert ego.speed != pytest.approx(ego.speedDesired)
    seenSpeed, seenAdvisory, _ = ctl._instance.seen[-1] \
        if hasattr(ctl, "_instance") else (ego.speed, ego.speedDesired, 0.05)
    assert seenSpeed == pytest.approx(7.25)
    assert seenAdvisory == pytest.approx(3.0)


def test_what_the_controller_sees_follows_the_plant_with_the_advisory_held(controllerPath):
    """Hold the advisory fixed, change only what the backend reports. On the
    feed path both cases read the same number; here they must separate."""
    advisory = 8.0
    _, slowEgo, _, _ = drive(controllerPath, plantSpeed=1.0, advisory=advisory)
    _, fastEgo, _, _ = drive(controllerPath, plantSpeed=15.0, advisory=advisory)

    assert slowEgo.speed == pytest.approx(1.0)
    assert fastEgo.speed == pytest.approx(15.0)
    assert slowEgo.speedDesired == fastEgo.speedDesired == pytest.approx(advisory)


def test_the_step_the_controller_is_given_is_the_one_it_is_called_at(controllerPath):
    """dt is the CARLA step, not the feed period. The agent's PID gains are
    per-step, so handing it the feed period detunes it by the sub-step ratio."""
    _, _, _, ctl = drive(controllerPath, plantSpeed=5.0, advisory=8.0, dt=0.025)
    assert seenBy(ctl)[-1][2] == pytest.approx(0.025)


def test_a_controller_that_names_no_control_is_refused_at_load(tmp_path):
    p = tmp_path / "bad_controller.py"
    p.write_text("def setup(config, egoId):\n    pass\n", encoding="utf-8")
    with pytest.raises(Exception):
        loadController(str(p))


@pytest.mark.parametrize("shape,source", [
    ("class", CLASS_FORM),
    ("function with setup", FUNC_WITH_SETUP),
    ("bare function", FUNC_BARE),
])
def test_every_accepted_shape_is_called_the_same_way(shape, source, tmp_path):
    """The bridge has one thing to call and does not branch on how the user
    chose to write it."""
    p = tmp_path / "ctl.py"
    p.write_text(source, encoding="utf-8")
    kind, ego, backend, _ = drive(str(p), plantSpeed=6.0, advisory=8.0)
    assert kind == "actuation", (shape, kind)
    assert backend.applied[-1][1] == pytest.approx(0.4), shape


def test_a_returned_command_is_ignored(tmp_path):
    """A controller commands by WRITING to the record, not by returning. A
    controller that returns a dict commands nothing at all, and the host says so
    by returning None rather than guessing -- the last command persists in the
    plant, which is honest; substituting a zero would brake a car whose
    controller simply had nothing new to say."""
    p = tmp_path / "returns.py"
    p.write_text(RETURNS_A_DICT, encoding="utf-8")
    kind, _, backend, _ = drive(str(p), plantSpeed=6.0, advisory=8.0)
    assert kind is None
    assert backend.applied == []
