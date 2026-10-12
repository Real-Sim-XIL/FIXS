"""The controller hook (#325): loading, both command shapes, and the guards.

Runs with no CARLA, no TrafficLayer and no SUMO -- the point of keeping the
controller contract SDK-free is that this is possible at all.

    python -m pytest tests/VirEnv/test_ego_controller.py
"""

import os
import sys
import textwrap

import pytest

_HERE = os.path.dirname(os.path.abspath(__file__))
_ROOT = os.path.dirname(os.path.dirname(_HERE))
for _p in (_ROOT, _HERE):
    if _p not in sys.path:
        sys.path.insert(0, _p)

from CommonLib import fixs                                          # noqa: E402
from CommonLib.VirEnv.EgoControllerHost import (                    # noqa: E402
    ControllerError, loadController, runController)
from CommonLib.VirEnv.IVirEnvBackend import EgoState                # noqa: E402
from MockVirEnvBackend import MockVirEnvBackend                     # noqa: E402


# --------------------------------------------------------------------------
# a stand-in for the ego record the bridge hands a controller
# --------------------------------------------------------------------------

def makeEgo(**fields):
    """A Vehicle with the read-only guard bypassed, as the bridge builds one."""
    ego = object.__new__(fixs.Vehicle)
    object.__setattr__(ego, 'id', 'ego')
    object.__setattr__(ego, '_written', frozenset())
    defaults = dict(positionX=0.0, positionY=0.0, positionZ=0.0, heading=90.0,
                    speed=6.0, speedDesired=8.0, signalLightColor=-1,
                    signalLightDistance=0.0, precedingVehicleDistance=0.0,
                    hasPrecedingVehicle=0, feedAge=0.0,
                    acceleratorPedalDesired=0.0, brakePedalDesired=0.0,
                    steerAngleDesired=0.0, accelerationDesired=0.0)
    defaults.update(fields)
    for k, v in defaults.items():
        object.__setattr__(ego, k, v)
    return ego


@pytest.fixture(autouse=True)
def noWireCheck():
    """Vehicle.set refuses a field absent from VehicleMessageField, which is a
    property of a connection these tests deliberately do not have. Clearing it
    exercises the real set() -- validation, _written bookkeeping and all --
    rather than a stand-in that could drift from it."""
    saved = fixs._declaredFields
    fixs._declaredFields = None
    yield
    fixs._declaredFields = saved


def write(ego, **fields):
    ego.set(**fields)


def controllerFile(tmp_path, body, name='ctl.py'):
    p = tmp_path / name
    p.write_text(textwrap.dedent(body), encoding='utf-8')
    return str(p)


# --------------------------------------------------------------------------
# the two command shapes
# --------------------------------------------------------------------------

def test_pedals_and_steer_is_an_actuation_command():
    ego = makeEgo()
    write(ego, acceleratorPedalDesired=0.3, brakePedalDesired=0.0,
          steerAngleDesired=0.1)
    assert fixs.commandKind(ego) == 'actuation'
    fixs._validateCommand(ego)


def test_speed_and_steer_is_a_speedsteer_command():
    """The shape that used to be unrepresentable: steerAngleDesired lived inside
    ACTUATION_FIELDS, so this raised 'missing acceleratorPedalDesired'."""
    ego = makeEgo()
    write(ego, speedDesired=8.3, steerAngleDesired=0.1)
    assert fixs.commandKind(ego) == 'speedsteer'
    fixs._validateCommand(ego)


def test_speed_alone_is_still_a_command():
    ego = makeEgo()
    write(ego, speedDesired=8.3)
    assert fixs.commandKind(ego) == 'speedsteer'
    fixs._validateCommand(ego)


def test_partial_pedals_still_rejected():
    """applyEgoActuation reads all three every tick, so a partial write would
    ship whatever arrived last for the rest."""
    ego = makeEgo()
    write(ego, acceleratorPedalDesired=0.3, brakePedalDesired=0.0)
    with pytest.raises(fixs.ProtocolError, match='steerAngleDesired'):
        fixs._validateCommand(ego)


def test_steer_alone_rejected():
    """Newly representable once steer left the pedal set, and meaningless:
    nothing downstream knows what to do with a steer angle alone."""
    ego = makeEgo()
    write(ego, steerAngleDesired=0.1)
    assert fixs.commandKind(ego) is None
    with pytest.raises(fixs.ProtocolError, match='alone is not a command'):
        fixs._validateCommand(ego)


def test_both_longitudinal_accepted():
    ego = makeEgo()
    write(ego, speedDesired=8.3, accelerationDesired=1.0)
    assert fixs.commandKind(ego) == 'speedsteer'
    fixs._validateCommand(ego)


def test_nothing_written_is_not_an_error():
    """A controller with nothing to say this step is normal: the last command
    persists in the plant. Substituting a zero would brake the car."""
    ego = makeEgo()
    assert fixs.commandKind(ego) is None
    fixs._validateCommand(ego)


# --------------------------------------------------------------------------
# loading
# --------------------------------------------------------------------------

def test_bare_function(tmp_path):
    f = controllerFile(tmp_path, '''
        def control(ego, dt):
            ego.set(speedDesired=7.5, steerAngleDesired=0.0)
    ''')
    c = loadController(f)
    c.setup({}, 'ego')
    ego = makeEgo()
    c.control(ego, 0.05)
    assert fixs.commandKind(ego) == 'speedsteer'


def test_setup_state_reaches_control(tmp_path):
    f = controllerFile(tmp_path, '''
        def setup(config, egoId):
            return {"gain": 0.5}

        def control(ego, dt, state=None):
            ego.set(speedDesired=state["gain"] * 10.0, steerAngleDesired=0.0)
    ''')
    c = loadController(f)
    c.setup({}, 'ego')
    ego = makeEgo()
    c.control(ego, 0.05)
    assert ego.speedDesired == pytest.approx(5.0)


def test_class_form(tmp_path):
    f = controllerFile(tmp_path, '''
        class Controller:
            def __init__(self, config, egoId):
                self.egoId = egoId
            def control(self, ego, dt):
                ego.set(speedDesired=3.0, steerAngleDesired=0.0)
    ''')
    c = loadController(f)
    c.setup({}, 'ego')
    ego = makeEgo()
    c.control(ego, 0.05)
    assert ego.speedDesired == pytest.approx(3.0)


def test_dt_is_given_not_guessed(tmp_path):
    """The caller is the only thing that knows the interval. A controller that
    has to assume one is how CARLA's own LocalPlanner ends up running its PIDs
    at 20 Hz inside a 10 Hz loop."""
    f = controllerFile(tmp_path, '''
        seen = []
        def control(ego, dt):
            seen.append(dt)
            ego.set(speedDesired=1.0, steerAngleDesired=0.0)
    ''')
    c = loadController(f)
    ego = makeEgo()
    c.control(ego, 0.05)
    c.control(ego, 0.05)
    assert sys.modules['ctl'].seen == [0.05, 0.05]


# --------------------------------------------------------------------------
# the guards -- each one turns a silent failure into a startup error
# --------------------------------------------------------------------------

def test_missing_file_names_the_path():
    with pytest.raises(ControllerError, match='no such file'):
        loadController('does/not/exist.py')


def test_no_entry_point_says_what_to_define(tmp_path):
    f = controllerFile(tmp_path, '''
        def drive(ego, dt):        # wrong name
            pass
    ''', name='noentry.py')
    with pytest.raises(ControllerError, match='defines none of'):
        loadController(f)


def test_named_attribute_that_is_missing(tmp_path):
    f = controllerFile(tmp_path, 'def control(ego, dt): pass\n', name='named.py')
    with pytest.raises(ControllerError, match='has no'):
        loadController(f + ':nosuch')


def test_not_callable(tmp_path):
    f = controllerFile(tmp_path, 'control = 42\n', name='notcallable.py')
    with pytest.raises(ControllerError, match='not callable'):
        loadController(f)


def test_class_without_control(tmp_path):
    f = controllerFile(tmp_path, '''
        class Controller:
            def __init__(self, config, egoId): pass
    ''', name='noctl.py')
    with pytest.raises(ControllerError, match='no control'):
        loadController(f)
# --------------------------------------------------------------------------
# runController -- the whole path against a mock backend, no simulator
#
# This is what moving the driver out of Carla/VirEnv/mainVirCarla.py bought:
# the dispatch is the new behaviour, and where it used to live nothing could
# reach it. Writing these found that it read EgoState as if it were nested
# (es.pose.x) when EgoState is flat -- it would have thrown on the first step.
# --------------------------------------------------------------------------

def _mockWithEgo(speed=6.0):
    backend = MockVirEnvBackend()
    es = EgoState()
    es.speed, es.x, es.y, es.heading = speed, 12.0, -3.0, 90.0
    backend.setMockEgo(es, available=True)
    return backend


def _run(tmp_path, body, name, dt=0.05, onFeed=True, speed=6.0):
    backend = _mockWithEgo(speed)
    ctl = loadController(controllerFile(tmp_path, body, name=name))
    ctl.setup({}, 'ego')
    ego = makeEgo()
    backend.clear()
    kind = runController(backend, ctl, ego, dt, onFeed, maxSteerRad=0.7)
    return kind, ego, [e for e in backend.events() if 'applyEgo' in e]


def test_run_dispatches_pedals_to_applyEgoActuation(tmp_path):
    body = ("def control(ego, dt):\n"
            "    ego.set(acceleratorPedalDesired=0.4, brakePedalDesired=0.0,\n"
            "            steerAngleDesired=0.35)\n")
    kind, ego, applied = _run(tmp_path, body, 'pedals.py')
    assert kind == 'actuation'
    assert any('applyEgoActuation' in e for e in applied), applied
    assert any('steer=0.5' in e for e in applied), applied      # 0.35 rad / 0.7


def test_run_dispatches_speed_to_applyEgoSpeedSteer(tmp_path):
    body = ("def control(ego, dt):\n"
            "    ego.set(speedDesired=9.0, steerAngleDesired=0.0)\n")
    kind, ego, applied = _run(tmp_path, body, 'speed.py')
    assert kind == 'speedsteer'
    assert any('applyEgoSpeedSteer' in e for e in applied), applied


def test_run_applies_nothing_when_controller_is_silent(tmp_path):
    """Not an error, and deliberately not a zero command: the last one persists
    in the plant, and a zero would brake a car that simply had nothing to say."""
    kind, ego, applied = _run(tmp_path, "def control(ego, dt):\n    pass\n", 'quiet.py')
    assert kind is None
    assert applied == []


def test_run_refreshes_pose_every_step(tmp_path):
    """The reason the hook exists: state is what the backend just measured, not
    what arrived at the last feed."""
    body = ("def control(ego, dt):\n"
            "    ego.set(speedDesired=ego.speed, steerAngleDesired=0.0)\n")
    kind, ego, _ = _run(tmp_path, body, 'echo.py', speed=7.25)
    assert ego.positionX == pytest.approx(12.0)
    assert ego.speed == pytest.approx(7.25)
    assert ego.speedDesired == pytest.approx(7.25)


def test_feedage_is_zero_on_a_feed_and_grows_between(tmp_path):
    body = ("seenAges = []\n"
            "def control(ego, dt):\n"
            "    seenAges.append(ego.feedAge)\n"
            "    ego.set(speedDesired=5.0, steerAngleDesired=0.0)\n")
    backend = _mockWithEgo()
    ctl = loadController(controllerFile(tmp_path, body, name='ages.py'))
    ego = makeEgo()
    runController(backend, ctl, ego, 0.05, True, 0.7)      # the feed
    runController(backend, ctl, ego, 0.05, False, 0.7)     # sub-step
    runController(backend, ctl, ego, 0.05, False, 0.7)     # sub-step
    assert sys.modules['ages'].seenAges == pytest.approx([0.0, 0.05, 0.10])


def test_feedspeed_holds_the_feeds_speed_while_speed_goes_live(tmp_path):
    body = ("seen = []\n"
            "def control(ego, dt):\n"
            "    seen.append((ego.feedSpeed, ego.speed))\n"
            "    ego.set(speedDesired=5.0, steerAngleDesired=0.0)\n")
    backend = _mockWithEgo(speed=7.25)
    ctl = loadController(controllerFile(tmp_path, body, name='feedspeed.py'))
    ego = makeEgo(speed=6.0)
    runController(backend, ctl, ego, 0.05, True, 0.7)      # the feed
    runController(backend, ctl, ego, 0.05, False, 0.7)     # sub-step
    assert sys.modules['feedspeed'].seen == [(6.0, 7.25), (6.0, 7.25)]


def test_written_is_cleared_between_steps(tmp_path):
    """Without this _written only grows, so a controller that commanded once
    would look like it was still commanding ten steps later."""
    body = ("step = [0]\n"
            "def control(ego, dt):\n"
            "    step[0] += 1\n"
            "    if step[0] == 1:\n"
            "        ego.set(acceleratorPedalDesired=0.5, brakePedalDesired=0.0,\n"
            "                steerAngleDesired=0.0)\n")
    backend = _mockWithEgo()
    ctl = loadController(controllerFile(tmp_path, body, name='once.py'))
    ego = makeEgo()
    assert runController(backend, ctl, ego, 0.05, True, 0.7) == 'actuation'
    assert runController(backend, ctl, ego, 0.05, False, 0.7) is None


def test_no_ego_actor_is_not_an_error(tmp_path):
    backend = MockVirEnvBackend()
    backend.setMockEgo(EgoState(), available=False)
    ctl = loadController(controllerFile(
        tmp_path, "def control(ego, dt):\n    pass\n", name='noego.py'))
    assert runController(backend, ctl, makeEgo(), 0.05, True, 0.7) is None
