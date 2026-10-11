"""fixs.driver -- the ready-made controller, and the one seam a rig owner uses.

The seam is a function: a speed in, the speed the cell reached out. These tests
hold it to the three cases the driver has to answer -- your function, the
simulated cell, or nobody -- and to the rule that a tick the cell could not
answer passes the reference through untouched rather than inventing motion.
"""
import io
import os
import sys
import tempfile

import pytest

sys.path.insert(0, os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..')))

import CommonLib.fixs as fixs                                    # noqa: E402
from CommonLib.fixs._driver import (Controller, Limits, Tuning,  # noqa: E402
                                    driver, _options)  # noqa: E402


_SCENARIO = """
SimulationSetup: {EnableRealSim: true, SelectedTrafficSimulator: SUMO}
EgoSetup: {Dynamics: virenv, ActuationSource: user, Controller: x.py}
XilSetup: {EnableXil: %s, Transport: inprocess}
CarlaSetup: {EnableCosimulation: true, EnablePythonBackend: true}
"""


@pytest.fixture
def scenario_off(tmp_path):
    """A scenario on disk with no bench, for loading a template."""
    p = tmp_path / 's.yaml'
    p.write_text(_SCENARIO % 'false')
    old = os.environ.get('FIXS_CONFIG_YAML')
    os.environ['FIXS_CONFIG_YAML'] = str(p)
    yield str(p)
    os.environ.pop('FIXS_CONFIG_YAML', None)
    if old is not None:
        os.environ['FIXS_CONFIG_YAML'] = old


@pytest.fixture
def scenario(request):
    """A yaml on disk, with or without the simulated dyno declared."""
    with tempfile.NamedTemporaryFile('w', suffix='.yaml', delete=False) as f:
        f.write(_SCENARIO % ('true' if request.param else 'false'))
        path = f.name
    old = os.environ.get('FIXS_CONFIG_YAML')
    os.environ['FIXS_CONFIG_YAML'] = path
    yield path
    os.environ.pop('FIXS_CONFIG_YAML', None)
    if old is not None:
        os.environ['FIXS_CONFIG_YAML'] = old
    os.unlink(path)


def _build(cls):
    """The driver without its CARLA half: __init__ up to the agent, which is
    built on the first controlled tick and needs a live simulator."""
    obj = cls.__new__(cls)
    Controller.__init__(obj, {'EgoControllerLog': ''}, 'ego')
    return obj


# -- what fixs.driver() hands back -----------------------------------------

def test_it_returns_a_class_because_that_is_what_the_loader_expects():
    cls = driver()
    assert isinstance(cls, type)
    assert callable(getattr(cls, 'control', None))


def test_a_non_callable_exchange_is_refused_at_the_call_not_at_the_tick():
    with pytest.raises(TypeError):
        driver(42)


# -- the three cases the seam has to answer ---------------------------------

@pytest.mark.parametrize('scenario', [False], indirect=True)
def test_no_exchange_means_nothing_in_the_loop(scenario):
    d = _build(driver())
    assert d.benchInLoop is False


@pytest.mark.parametrize('scenario', [True], indirect=True)
def test_the_scenario_alone_brings_nothing(scenario):
    """EnableXil declares the SIMULATED cell exists; it does not put one in
    this driver's loop. Only passing a function does that."""
    d = _build(driver())
    assert d.benchInLoop is False


@pytest.mark.parametrize('scenario', [False], indirect=True)
def test_your_function_needs_no_scenario_flag(scenario):
    """Passing one IS the declaration."""
    d = _build(driver(lambda v, dt: v))
    assert d.benchInLoop is True


@pytest.mark.parametrize('scenario', [True], indirect=True)
def test_the_simulated_dyno_is_passed_in_like_any_other(scenario):
    """The bench is built with its parameters, and the exchange is a function
    the caller writes -- the same two lines a rig owner replaces."""
    from CommonLib.fixs import xil
    bench = xil.dynosim(vehicle={'mass_kg': 900.0})

    def exchange(vref, dt):
        return bench.exchange(vref, 0.0, dt)

    d = _build(driver(exchange))
    assert d.benchInLoop is True
    assert d.exchange(10.0) > 0.0        # it answered
    assert bench.sim.vehicle.mass_kg == 900.0   # stated in code, not the yaml


def test_the_driver_does_not_import_xil():
    """The coupling this PR removes: a driver that reaches for a bench is
    deciding something that belongs to whoever built it."""
    src = io.open(os.path.join(os.path.dirname(__file__), '..', '..', '..',
                               'CommonLib', 'fixs', '_driver.py'),
                  encoding='utf-8').read()
    code = [ln for ln in src.splitlines()
            if ln.startswith(('import ', 'from ')) and 'xil' in ln]
    assert code == [], code


# -- the rule that matters when hardware misbehaves -------------------------

@pytest.mark.parametrize('scenario', [False], indirect=True)
def test_a_tick_the_dyno_cannot_answer_passes_the_reference_through(scenario):
    """None back must not become 0 m/s: that would invent a stop."""
    answers = [7.5, None, 7.6]
    d = _build(driver(lambda v, dt: answers.pop(0)))
    assert d.exchange(9.0) == 7.5           # the cell answered
    assert d.exchange(9.0) == 9.0           # it did not -- the reference stands
    assert d.misses == 1
    assert d.exchange(9.0) == 7.6
    assert d.misses == 1


# -- options, nearest the run wins ------------------------------------------

def test_the_scenario_beats_the_factory_beats_the_default():
    assert _options({}).command_shape == 'speed'
    assert _options({}, {'shape': 'pedals'}).command_shape == 'pedals'
    assert _options({'EgoControllerArgs': ['--command-shape', 'speed']},
                    {'shape': 'pedals'}).command_shape == 'speed'


def test_an_unknown_option_fails_at_construction():
    with pytest.raises(TypeError):
        _options({}, {'nonsense': 1})


# -- the gains, as one value ------------------------------------------------

def test_a_gain_ladder_is_one_token_per_rung():
    assert str(Tuning()) == 'kv=0.25,ki=0.5,kp=5,k=0.01,maxAccel=1.5,fullStop=0.1'
    assert Tuning.parse('kv=0.3,ki=0.6').kv == 0.3
    assert Tuning.parse('kv=0.3,ki=0.6').ki == 0.6
    assert Tuning.parse('kv=0.3').ki == 0.5          # the rest stand


def test_a_typo_in_a_gain_name_is_refused_not_ignored():
    """The failure that matters: a run that silently used the defaults while
    its command line claimed otherwise."""
    with pytest.raises(TypeError):
        Tuning().replace(nope=1)
    with pytest.raises(TypeError):
        Tuning.parse('nope=1')
    with pytest.raises(ValueError):
        Tuning.parse('kv')
    with pytest.raises(ValueError):
        Tuning.parse('kv=fast')


def test_the_scenario_beats_the_factory_for_gains_too():
    assert _options({}, {'tuning': Tuning(kv=0.9)}).tuning.kv == 0.9
    assert _options({'EgoControllerArgs': ['--tune', 'kv=0.1']},
                    {'tuning': Tuning(kv=0.9)}).tuning.kv == 0.1


@pytest.mark.parametrize('scenario', [False], indirect=True)
def test_the_run_records_what_drove_it(scenario, tmp_path):
    """A batch once reported gains it had not used. The log says."""
    path = tmp_path / 'agent.csv'
    cls = driver(lambda v, dt: v, shape='pedals', tuning=Tuning(kv=0.33))
    obj = cls.__new__(cls)
    Controller.__init__(obj, {'EgoControllerLog': str(path)}, 'ego')
    obj.shutdown()
    first = path.read_text().splitlines()[0]
    assert first.startswith('#')
    assert 'shape=pedals' in first
    assert 'kv=0.33' in first
    assert 'exchange=<lambda>' in first
    assert 'comfortDecel=2' in first   # the limits are recorded too


# -- the law itself, which is pure arithmetic and needs no simulator ---------

def _pedalLaw(**tune):
    cls = driver(shape='pedals', tuning=Tuning(**tune))
    obj = cls.__new__(cls)
    Controller.__init__(obj, {'EgoControllerLog': '', 'CarlaTimeStep': 0.05}, 'ego')
    return obj


def test_a_stopped_car_asked_for_nothing_holds_the_brake():
    """The full-stop rule -- and the line that broke rung 4 in review, because
    it reads a gain before the law had bound it."""
    d = _pedalLaw()
    assert d._speedToPedal(0.0, 0.0) == (0.0, 1.0)


def test_zero_speed_error_still_holds_a_pedal():
    """The whole reason the integral exists: road load has to be paid."""
    d = _pedalLaw()
    for _ in range(40):
        thr, brk = d._speedToPedal(8.0, 8.0 - 0.2)     # a standing shortfall
    assert thr > 0.0
    thr, brk = d._speedToPedal(8.0, 8.0)               # error now zero
    assert thr > 0.0, 'the pedal collapsed when the error did'


def test_the_gains_reach_the_law():
    """A tuning that cannot move the pedal is a tuning nobody is applying."""
    slow, fast = _pedalLaw(kv=0.01), _pedalLaw(kv=0.9)
    assert fast._speedToPedal(9.0, 4.0)[0] > slow._speedToPedal(9.0, 4.0)[0]


# -- the limits, and that they reach the ceilings ---------------------------

def test_limits_reach_the_stopping_ceiling():
    """A limit that cannot change how early the ego slows is a limit nobody is
    applying -- the same failure the gains had, caught the same way."""
    from CommonLib.fixs._driver import Limits, _stopBy
    gentle, firm = Limits(comfortDecel=1.0), Limits(comfortDecel=4.0)
    assert _stopBy(50.0, firm.comfortDecel) > _stopBy(50.0, gentle.comfortDecel)


def test_limits_are_carried_and_recorded():
    cls = driver(limits=Limits(comfortDecel=1.5, stopMargin=3.0))
    obj = cls.__new__(cls)
    Controller.__init__(obj, {'EgoControllerLog': ''}, 'ego')
    assert obj.limits.comfortDecel == 1.5
    assert obj.limits.stopMargin == 3.0
    assert 'comfortDecel=1.5' in str(obj.limits)


def test_a_wrong_type_for_limits_is_refused():
    with pytest.raises(TypeError):
        _options({}, {'limits': {'comfortDecel': 1.5}})


def test_ideal_speed_tracking_is_settable_like_the_shape_it_pairs_with():
    """It only bites under shape='speed'; leaving it a bare global while the
    shape was settable was the inconsistency."""
    assert _options({}).idealSpeedTracking is True
    assert _options({}, {'idealSpeedTracking': False}).idealSpeedTracking is False


# -- the name is the user's ------------------------------------------------

_ANY_NAME = '''
import sys
sys.path.insert(0, %r)
import CommonLib.fixs as fixs
%s
'''


def _controllerFile(tmp_path, body, name='c.py'):
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
    p = tmp_path / name
    p.write_text(_ANY_NAME % (root, body))
    return str(p)


def test_the_name_you_give_the_driver_is_yours(tmp_path):
    """No `Controller =` anywhere: fixs.driver() says what it built."""
    from CommonLib.VirEnv.EgoControllerHost import loadController
    for body in ('Driver = fixs.driver()',       # any name
                 'fixs.driver()',                # or none
                 'WhateverIWant = fixs.driver()'):
        lc = loadController(_controllerFile(tmp_path, body, 'c%d.py' % hash(body)))
        assert callable(getattr(lc._obj, 'control', None))


def test_a_hand_written_controller_still_works_by_name(tmp_path):
    """The name scan stays, for anyone not using fixs.driver()."""
    from CommonLib.VirEnv.EgoControllerHost import loadController
    p = _controllerFile(tmp_path, '''
class Controller:
    def __init__(self, config, egoId): pass
    def control(self, ego, dt): pass
''', 'hand.py')
    assert loadController(p)._obj.__name__ == 'Controller'


def test_two_drivers_in_one_file_is_refused(tmp_path):
    """Last-wins would run one of two, chosen by statement order."""
    from CommonLib.VirEnv.EgoControllerHost import loadController, ControllerError
    p = _controllerFile(tmp_path, 'A = fixs.driver()\nB = fixs.driver()', 'two.py')
    with pytest.raises(ControllerError) as e:
        loadController(p)
    assert 'twice' in str(e.value) or '2 times' in str(e.value)


# -- the shipped template ---------------------------------------------------

def _template():
    return os.path.join(os.path.dirname(__file__), '..', '..', '..',
                        'Carla', 'templates', 'driver_template.py')


def test_the_template_is_a_working_driver(scenario_off):
    """It is a TEMPLATE: copy it, point a scenario at it, and it drives. The
    version this replaces was 86 lines of docstring and one statement -- it
    would have failed at load with 'defines none of Controller, control', and
    a test that only grepped its prose passed anyway."""
    from CommonLib.VirEnv.EgoControllerHost import loadController
    lc = loadController(_template())
    assert callable(getattr(lc._obj, 'control', None))


def test_the_template_names_keys_and_files_that_exist():
    """How the last one rotted: pre-EgoSetup yaml keys, a pointer to a file
    that does not exist, and a config key that reads back empty."""
    src = io.open(_template(), encoding='utf-8').read()
    for wrong in ('EgoActuationSource', 'EgoController:', 'IEgoController',
                  'EgoRoutePoints'):
        assert wrong not in src, wrong
    assert 'EgoSetup' in src


def test_the_template_shows_the_other_two_forms():
    src = io.open(_template(), encoding='utf-8').read()
    assert 'exchange' in src and 'usercontrol' in src


# -- your own driving, through the same factory ------------------------------

class _FakeEgo:
    speed = 4.0
    speedDesired = 0.0
    feedAge = 0.0

    def set(self, **kw):
        self.cmd = kw


@pytest.mark.parametrize('scenario', [False], indirect=True)
def test_usercontrol_replaces_the_driving_entirely(scenario):
    """Your logic, called with the same (ego, dt), writing the same way --
    and none of the driver's own runs."""
    seen = []

    def mine(ego, dt):
        seen.append(dt)
        ego.set(speedDesired=9.0)

    d = _build(driver(usercontrol=mine))
    ego = _FakeEgo()
    d.control(ego, 0.05)
    assert seen == [0.05]
    assert ego.cmd == {'speedDesired': 9.0}
    assert d.agent is None, 'the CARLA agent was built for logic that is not ours'


@pytest.mark.parametrize('scenario', [False], indirect=True)
def test_usercontrol_needs_no_name_and_no_method(scenario):
    """The point of it: you write a function, not a class with control()."""
    from CommonLib.VirEnv.EgoControllerHost import loadController
    cls = driver(usercontrol=lambda ego, dt: None)
    assert callable(getattr(cls, 'control', None))


def test_usercontrol_and_exchange_together_is_refused():
    """Your control decides when to ask a cell; ours would never call it."""
    with pytest.raises(TypeError):
        driver(exchange=lambda v, dt: v, usercontrol=lambda ego, dt: None)


def test_a_non_callable_usercontrol_is_refused_at_the_call():
    with pytest.raises(TypeError):
        driver(usercontrol=42)


def test_the_template_makes_exactly_one_driver():
    """Following its own instructions must not break the run.

    The version this replaced had `Driver = fixs.driver(usercontrol=...)`
    inside a commented block AND one at the end. Uncommenting the block, as
    the file told you to, gave two drivers -- and the loader refuses to guess
    which was meant. So: one live call, and no commented-out one waiting to
    become a second.
    """
    import ast
    src = io.open(_template(), encoding='utf-8').read()

    live = [n for n in ast.walk(ast.parse(src))
            if isinstance(n, ast.Call) and isinstance(n.func, ast.Attribute)
            and n.func.attr == 'driver']
    assert len(live) == 1, 'live fixs.driver() calls'

    # The alternatives are allowed, but only BESIDE the live one, where
    # swapping which line carries the '#' is obvious. The bug was one buried
    # in a block far above: uncommenting that block left two.
    lines = src.splitlines()
    live = [i for i, ln in enumerate(lines) if ln.startswith('Driver =')]
    commented = [i for i, ln in enumerate(lines)
                 if ln.startswith('#') and ln.lstrip('#').strip().startswith('Driver =')]
    assert len(live) == 1, live
    assert all(abs(i - live[0]) <= 3 for i in commented), (live, commented)


# -- passive: the ego the TRAFFIC SIMULATOR owns (#24) -----------------------
#
# These drive the real Controller.control(), which is the half the stub-based
# host tests do not reach. The first version of this feature passed every test
# in the suite and then died on the first controlled tick of a real run, inside
# _build, because control() reached for carla.ego on a rung that never spawns
# one. A test that stops short of control() cannot see that.


def _passiveEgo(**fields):
    """An ego record shaped the way the bridge hands one over."""
    ego = object.__new__(fixs.Vehicle)
    object.__setattr__(ego, 'id', 'ego')
    object.__setattr__(ego, '_written', frozenset())
    defaults = dict(positionX=0.0, positionY=0.0, positionZ=0.0, heading=90.0,
                    speed=4.0, speedDesired=9.0, feedAge=0.0,
                    acceleratorPedalDesired=0.0, brakePedalDesired=0.0,
                    steerAngleDesired=0.0, speedLimit=13.4,
                    signalLightColor='', signalLightDistance=0.0,
                    hasPrecedingVehicle=0, precedingVehicleDistance=0.0,
                    precedingVehicleSpeed=0.0)
    defaults.update(fields)
    for k, v in defaults.items():
        object.__setattr__(ego, k, v)
    return ego


@pytest.fixture
def passive(scenario_off):
    """EgoSetup.Dynamics: traffic, as the host publishes it, and no wire-field
    guard (that is a property of a connection these tests do not have)."""
    from CommonLib.VirEnv import EgoControllerHost as host
    saved, host._dynamics = host._dynamics, 'traffic'
    savedFields = fixs._declaredFields
    fixs._declaredFields = None
    yield
    host._dynamics = saved
    fixs._declaredFields = savedFields


def test_the_cell_drives_a_traffic_owned_ego(passive):
    """No agent is built and no simulator is touched: the cell's answer is
    written straight onto the record for the traffic simulator to integrate."""
    d = _build(driver(lambda v, dt: v * 0.5))
    assert d.passive is True

    ego = _passiveEgo(speedDesired=9.0)
    d.control(ego, 0.1)

    assert d.agent is None, 'passive must not reach for a CARLA agent'
    assert 'speedDesired' in ego._written
    assert ego.speedDesired == pytest.approx(4.5)
    assert fixs.commandKind(ego) == 'speedsteer'


def test_passive_leaves_the_lateral_alone(passive):
    """Steering belongs to whoever owns the lateral, and on this rung that is
    the traffic simulator. Writing one would be a command nothing asked for."""
    d = _build(driver(lambda v, dt: v))
    ego = _passiveEgo()
    d.control(ego, 0.1)
    assert 'steerAngleDesired' not in ego._written


def test_a_null_cell_reproduces_the_rung_below(passive):
    """THE INVARIANT. A cell that hands back its reference unchanged must leave
    the advisory exactly as the eco controller wrote it -- because that, with no
    cell at all, IS the rung below. If this drifts, a dyno comparison is not
    measuring the dyno."""
    d = _build(driver(lambda v, dt: v))
    for advisory in (0.25, 4.0, 9.75, 13.4):
        ego = _passiveEgo(speedDesired=advisory)
        d.control(ego, 0.1)
        assert ego.speedDesired == pytest.approx(advisory, abs=1e-12), advisory


def test_no_cell_at_all_reproduces_the_rung_below(passive):
    """Same invariant with nothing in the loop: fixs.driver() without an
    exchange is a pass-through on this rung."""
    d = _build(driver())
    ego = _passiveEgo(speedDesired=9.75)
    d.control(ego, 0.1)
    assert ego.speedDesired == pytest.approx(9.75, abs=1e-12)


def test_an_almost_massless_cell_tracks_the_advisory(passive):
    """A bench with almost no dynamics must be almost invisible. This is the
    cheap check that a dyno run can be trusted: make the cell nearly ideal and
    the two rungs should sit on top of each other."""
    d = _build(driver(lambda v, dt: v * 0.999))
    ego = _passiveEgo(speedDesired=12.0)
    d.control(ego, 0.1)
    assert ego.speedDesired == pytest.approx(12.0, rel=2e-3)


def test_the_stop_bar_is_not_planned_twice(passive):
    """A red ahead must NOT pull the command down here.

    The eco controller has already planned its approach to that bar and written
    the result into speedDesired; this rung hands that to the traffic simulator.
    Re-applying _signalCeiling on top commanded a 2 m/s^2 stop from as far as
    200 m out, and the ego crawled toward a bar it was never going to overrun.
    Safety is not lost by leaving it out -- SumoSetup.SpeedMode keeps the
    traffic simulator's own red-light and safe-speed checks on."""
    d = _build(driver(lambda v, dt: v))
    ego = _passiveEgo(speedDesired=11.0, signalLightColor=1,   # _RED
                      signalLightDistance=150.0, speed=11.0)
    d.control(ego, 0.1)
    assert ego.speedDesired == pytest.approx(11.0, abs=1e-12)


def test_a_leader_is_not_planned_twice(passive):
    """Same, for the vehicle ahead: the traffic simulator's car-following owns
    the gap on this rung, and it still runs under SpeedMode."""
    d = _build(driver(lambda v, dt: v))
    ego = _passiveEgo(speedDesired=11.0, hasPrecedingVehicle=1,
                      precedingVehicleDistance=6.0, precedingVehicleSpeed=0.0)
    d.control(ego, 0.1)
    assert ego.speedDesired == pytest.approx(11.0, abs=1e-12)


def test_the_command_shape_is_inert_when_nothing_integrates_pedals(passive):
    """--command-shape picks who closes the loop against a plant. There is no
    plant here, so both shapes arrive at the same place and a scenario carries
    the same Controller line as its virenv sibling."""
    for shape in ('speed', 'pedals'):
        d = _build(driver(lambda v, dt: v * 0.5, shape=shape))
        ego = _passiveEgo(speedDesired=8.0)
        d.control(ego, 0.1)
        assert ego.speedDesired == pytest.approx(4.0), shape
        assert 'acceleratorPedalDesired' not in ego._written, shape


def test_a_zero_advisory_is_a_command_not_an_absence(passive):
    """Zero is what the eco controller writes AT a stop bar.

    _advisoryOf drops anything <= 0.01 as 'no advisory', which on the virenv
    path means 'hold the last target'. Taken literally here it meant the
    fallback speed, so the ego was told to accelerate away from the bar it had
    just been asked to stop at. The cell must see the zero, and the zero must
    reach the traffic simulator."""
    seen = []
    d = _build(driver(lambda v, dt: (seen.append(v), v)[1]))
    ego = _passiveEgo(speedDesired=0.0, speed=0.0)
    d.control(ego, 0.1)
    assert seen == [pytest.approx(0.0)], 'the cell must be asked for the stop'
    assert ego.speedDesired == pytest.approx(0.0, abs=1e-12)


def test_nothing_invents_a_speed_the_eco_controller_did_not_ask_for(passive):
    """No fallback on this rung. Whatever came off the wire is what the cell is
    handed, across the range -- there is no floor below which this driver
    substitutes an opinion of its own."""
    d = _build(driver())
    for advisory in (0.0, 0.005, 0.01, 0.5, 8.33, 20.0):
        ego = _passiveEgo(speedDesired=advisory)
        d.control(ego, 0.1)
        assert ego.speedDesired == pytest.approx(advisory, abs=1e-12), advisory


@pytest.mark.parametrize('scenario', [False], indirect=True)
def test_start_in_gear_is_opt_in_and_set_before_building(scenario, monkeypatch):
    """CARLA's autobox leaves a spawned car in neutral below ~15% throttle."""
    import CommonLib.fixs._driver as drv
    monkeypatch.setattr(drv, '_DEFAULTS', {})
    assert _build(driver(lambda v, dt: v)).startInGear is False
    fixs.driver.set(startInGear=True)
    assert _build(driver(lambda v, dt: v)).startInGear is True
    assert _build(driver(lambda v, dt: v, startInGear=False)).startInGear is False


def test_driver_set_refuses_an_unknown_option():
    with pytest.raises(TypeError):
        fixs.driver.set(startInGera=True)


# -- referenceMode: what the driver reads from the eco controller ------------

def test_the_acceleration_rebuilds_the_exact_float32_speed():
    """Exact at a stop and above 0.25 m/s; in between, within float32 of the acceleration."""
    import random
    rng = random.Random(0)
    for _ in range(50000):
        v = fixs._f32(rng.uniform(0.0, 30.0))
        target = fixs._f32(max(0.0, v + rng.uniform(-3.0, 2.0) * 0.1))
        a = fixs.accelerationFromSpeed(v, target, 0.1)
        assert a == fixs._f32(a)
        got = fixs.speedFromAcceleration(v, a, 0.1)
        if target == 0.0 or target > 0.25:
            assert got == target, (v, target)
        else:
            assert abs(got - target) < 2e-8, (v, target)


def test_the_reference_mode_comes_from_the_scenario():
    assert _options({}).reference_mode == 'speed'
    assert _options({'EgoControllerArgs': ['--reference-mode', 'accel']}).reference_mode == 'accel'
    assert _options({}, {'referenceMode': 'both'}).reference_mode == 'both'


@pytest.mark.parametrize('mode', ['speed', 'accel', 'both'])
def test_every_reference_mode_drives_the_same_speed(passive, mode):
    v, target = fixs._f32(12.3), fixs._f32(12.41)
    d = _build(driver(lambda vref, dt: vref, referenceMode=mode))
    ego = _passiveEgo(speed=v, feedSpeed=v, speedDesired=target,
                      accelerationDesired=fixs.accelerationFromSpeed(v, target, 0.1))
    d.control(ego, 0.1)
    assert ego.speedDesired == target


@pytest.mark.parametrize('sent', [dict(speedDesired=12.41, accelerationDesired=0.0),
                                  dict(speedDesired=12.3, accelerationDesired=1.1)])
def test_both_refuses_a_controller_that_sent_only_one(passive, sent):
    """The field left unset holds TrafficLayer's default: 0 acceleration, or the speed echoed back."""
    d = _build(driver(lambda vref, dt: vref, referenceMode='both'))
    with pytest.raises(fixs.FixsError, match='disagrees'):
        d.control(_passiveEgo(speed=12.3, feedSpeed=12.3, **sent), 0.1)


def test_accel_mode_refuses_a_wire_without_the_field(passive):
    fixs._declaredFields = frozenset({'id', 'speed', 'speedDesired'})
    d = _build(driver(lambda vref, dt: vref, referenceMode='accel'))
    with pytest.raises(fixs.FixsError, match='accelerationDesired'):
        d.control(_passiveEgo(feedSpeed=4.0), 0.1)


def test_a_three_argument_exchange_gets_the_acceleration(passive):
    got = []

    def exchange(vref, aref, dt):
        got.append((vref, aref))
        return vref

    d = _build(driver(exchange, referenceMode='accel'))
    d.control(_passiveEgo(speed=10.0, feedSpeed=10.0, speedDesired=0.0,
                          accelerationDesired=1.5), 0.1)
    assert got[0][0] == pytest.approx(10.15, abs=1e-5)
    assert got[0][1] == pytest.approx(1.5)


def test_the_acceleration_follows_a_speed_the_driver_changed():
    d = _build(driver(lambda vref, aref, dt: vref))
    d.feedVref, d.feedAref, d.feedStep = 10.0, 1.0, 0.1
    assert d.arefFor(10.0) == pytest.approx(1.0)
    assert d.arefFor(9.8) == pytest.approx(-1.0)


def test_the_exchange_says_what_it_takes_by_its_arguments():
    from CommonLib.fixs import xil
    assert driver(lambda vref, dt: vref)._EXCHANGE_TAKES_AREF is False
    assert driver(lambda vref, aref, dt: vref)._EXCHANGE_TAKES_AREF is True
    assert driver(xil.dynosim().exchange)._EXCHANGE_TAKES_AREF is True
    with pytest.raises(TypeError):
        driver(lambda vref: vref)
