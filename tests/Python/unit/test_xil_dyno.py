"""Invariants for the simulated dyno sim (CommonLib/xil).

Physics checks, not regression snapshots. Each pins a property something
downstream relies on, so that changing the model cannot quietly change what a
coupling built on it is entitled to assume.

Run:  pytest tests/Python/unit/test_xil_dyno.py -v
"""

import math
import os
import sys

import pytest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__),
                                                '..', '..', '..')))

from CommonLib.xil.driver import RobotDriver  # noqa: E402
from CommonLib.xil.dynosim import Dyno, DynoSim  # noqa: E402
from CommonLib.xil.vehicle import Vehicle  # noqa: E402

DT = 0.001
RL = 2


def run(sim, throttle, brake, secs, dt=DT):
    state = None
    for _ in range(int(secs / dt)):
        state = sim.step_pedals(throttle, brake, dt)
    return state


# --------------------------------------------------------------------- setup

def test_bad_configuration_is_rejected():
    with pytest.raises(ValueError):
        Dyno(mode='rollers')
    with pytest.raises(ValueError):
        Vehicle(wheel_radius_m=0.0)
    with pytest.raises(ValueError):
        Vehicle(front_share=1.5)
    with pytest.raises(ValueError):
        DynoSim().step_pedals(0.0, 0.0, 0.0)


# -------------------------------------------------------------- sub-stepping

def test_a_large_dt_is_chopped_into_steps_it_can_integrate():
    """A caller sets the outer rate and should not need the sim's time
    constants. Integrated whole, 0.1 s diverged to 1e81 and the clamps then read
    it back as a stopped vehicle."""
    sim = DynoSim()
    assert sim.step_pedals(0.5, 0.0, 0.001).substeps == 1
    assert sim.step_pedals(0.5, 0.0, 0.100).substeps > 1


def test_the_answer_does_not_depend_on_the_callers_rate():
    reference = None
    for dt in (0.001, 0.02, 0.05, 0.1, 0.2):
        sim = DynoSim()
        for _ in range(int(40.0 / dt)):
            state = sim.step(15.0, dt)
        if reference is None:
            reference = state.speed
        assert state.speed == pytest.approx(reference, abs=1e-3), \
            'dt=%g gave %.6f against %.6f at 1 ms' % (dt, state.speed, reference)


def test_the_inner_step_follows_the_fastest_dynamic():
    slow = DynoSim(Vehicle(torque_bandwidth_Hz=1.0))
    fast = DynoSim(Vehicle(torque_bandwidth_Hz=20.0))
    assert fast.inner_dt < slow.inner_dt
    assert slow.inner_dt == pytest.approx(0.1)


# ------------------------------------------------------------------- chassis

def test_a_parked_vehicle_stays_parked():
    """Road resistance must not accelerate a stopped car. It is a reaction
    force, and summing it as a signed term makes one roll backwards."""
    state = run(DynoSim(), 0.0, 0.0, 5.0)
    assert state.speed == pytest.approx(0.0, abs=1e-9)
    assert state.standstill
    assert state.force == pytest.approx(0.0, abs=1e-9)


def test_brake_brings_it_to_rest_and_holds():
    sim = DynoSim()
    run(sim, 1.0, 0.0, 6.0)
    assert sim.speed > 10.0
    state = run(sim, 0.0, 1.0, 8.0)
    assert state.speed == pytest.approx(0.0, abs=1e-9)
    assert state.standstill


def test_it_settles_where_tractive_effort_meets_road_resistance():
    sim = DynoSim()
    state = run(sim, 1.0, 0.0, 400.0, dt=0.01)
    assert abs(state.force) < 25.0, 'not settled: %.1f N' % state.force
    assert state.resistance == pytest.approx(
        sum(state.drive_torque) / sim.vehicle.wheel_radius_m, rel=0.02)


def test_the_speed_limiter_holds_the_top_speed():
    """Real EVs are limited below what their power would reach. Without one this
    sim settled at 229 km/h against an EV6's actual 185."""
    sim = DynoSim()
    state = run(sim, 1.0, 0.0, 400.0, dt=0.01)
    limit = sim.vehicle.powertrain.max_speed_mps
    assert state.speed <= limit
    assert state.speed > limit - sim.vehicle.powertrain.limiter_taper_mps - 0.5


def test_road_resistance_matches_its_coefficients():
    dyno = Dyno(road_A_N=100.0, road_B_Npms=2.0, road_C_Npms2=0.5)
    assert dyno.resistance(10.0) == pytest.approx(170.0)
    assert dyno.resistance(-10.0) == pytest.approx(-170.0)


def test_roller_inertia_slows_the_acceleration():
    """Referred to the road through r^2, so a heavier roller must accelerate
    more slowly on identical torque."""
    light = DynoSim(dyno=Dyno(roller_inertia_kgm2=0.0))
    heavy = DynoSim(dyno=Dyno(roller_inertia_kgm2=200.0))
    assert run(heavy, 1.0, 0.0, 3.0).speed < run(light, 1.0, 0.0, 3.0).speed


def test_wheels_are_rigidly_coupled_to_the_roller():
    sim = DynoSim()
    state = run(sim, 0.6, 0.0, 4.0)
    for w in state.omega:
        assert w == pytest.approx(state.speed / sim.vehicle.wheel_radius_m)


def test_a_hill_rolls_it_back_with_the_brake_off():
    sim = DynoSim(dyno=Dyno(grade_rad=math.radians(15.0)))
    assert run(sim, 0.0, 0.0, 4.0).speed < -0.5


def test_axle_torque_is_the_measured_one():
    """It is drive minus brake minus what spun the wheel up, so during a
    transient it must differ from drive minus brake, and at steady state it
    must not."""
    sim = DynoSim()
    early = sim.step_pedals(1.0, 0.0, 0.01)
    settled = run(sim, 1.0, 0.0, 400.0, dt=0.01)
    produced = sum(early.drive_torque) - sum(early.brake_torque)
    assert sum(early.axle_torque) != pytest.approx(produced, abs=1.0)
    produced = sum(settled.drive_torque) - sum(settled.brake_torque)
    assert sum(settled.axle_torque) == pytest.approx(produced, abs=1.0)


# ---------------------------------------------------------------------- axle

def test_an_axle_bench_spins_up_on_its_own():
    state = run(DynoSim(dyno=Dyno(mode='axle')), 0.5, 0.0, 6.0)
    assert state.omega[RL] > 10.0
    assert state.speed > 3.0
    assert state.resistance > 0.0


def test_the_hubs_have_to_accelerate_the_body_too():
    """There is no body on an axle dyno, so its inertia is added electrically.
    A heavier vehicle must spin up more slowly."""
    light = DynoSim(Vehicle(mass_kg=500.0), Dyno(mode='axle'))
    heavy = DynoSim(Vehicle(mass_kg=4000.0), Dyno(mode='axle'))
    assert heavy.axle_inertia_kgm2 > light.axle_inertia_kgm2
    assert run(heavy, 0.5, 0.0, 3.0).omega[RL] < run(light, 0.5, 0.0, 3.0).omega[RL]


def test_an_axle_bench_holds_still_with_no_pedal():
    state = run(DynoSim(dyno=Dyno(mode='axle')), 0.0, 0.0, 5.0)
    assert state.standstill
    for w in state.omega:
        assert w == pytest.approx(0.0, abs=1e-9)


def test_brake_never_spins_the_wheel_backwards():
    sim = DynoSim(dyno=Dyno(mode='axle'))
    run(sim, 0.4, 0.0, 3.0)
    for w in run(sim, 0.0, 1.0, 6.0).omega:
        assert w > -1e-3, 'brake drove the wheel backwards to %.4f rad/s' % w


# ---------------------------------------------------------------- powertrain

def test_full_throttle_delivers_the_whole_vehicle():
    """front_share splits the demand; it must not scale each axle's capability.
    Doing that gave 51 % of the car -- 3280 of 6400 Nm."""
    v = Vehicle()
    t_f, t_r = v.axle_torque(1.0, 0.0, 0.0)
    assert (t_f + t_r) > 0.99 * (v.powertrain.front_peak_torque_Nm
                                 + v.powertrain.rear_peak_torque_Nm)


def test_the_share_is_honoured_below_the_caps():
    v = Vehicle()
    t_f, t_r = v.axle_torque(0.5, 0.0, 0.0)
    assert t_f / (t_f + t_r) == pytest.approx(v.powertrain.front_share,
                                              abs=1e-6)


def test_a_zero_share_means_that_axle_is_not_driven():
    """Rear-drive-only is a real vehicle, and a spill-over rule sent it torque
    anyway."""
    for share, driven, idle in ((0.0, 1, 0), (1.0, 0, 1)):
        t = Vehicle(front_share=share).axle_torque(1.0, 0.0, 0.0)
        assert t[idle] == pytest.approx(0.0)
        assert t[driven] > 0.0


def test_constant_torque_then_constant_power():
    """Base speed is where peak power meets peak torque: 10 kW / 1000 Nm."""
    v = Vehicle(rear_peak_torque_Nm=1000.0, rear_peak_power_W=10.0e3,
                front_share=0.0)
    assert v.axle_torque(1.0, 5.0, 5.0)[1] == pytest.approx(1000.0, rel=0.02)
    assert v.axle_torque(1.0, 40.0, 40.0)[1] == pytest.approx(250.0, rel=0.02)


def test_the_powertrain_can_be_replaced_wholesale():
    """The hook for making the sim agree with another simulator's torque map
    instead of guessing at it."""

    class Flat(Vehicle):
        def axle_torque(self, throttle, omega_front, omega_rear):
            return 0.0, 800.0

    state = run(DynoSim(Flat()), 0.5, 0.0, 3.0)
    assert state.drive_torque[RL] == pytest.approx(400.0, rel=1e-3)
    assert state.drive_torque[0] == pytest.approx(0.0, abs=1e-6)


def test_torque_delivery_lags_the_command():
    sim = DynoSim(Vehicle(torque_bandwidth_Hz=2.0))
    first = sim.step_pedals(1.0, 0.0, DT).drive_torque[RL]
    settled = run(sim, 1.0, 0.0, 4.0).drive_torque[RL]
    assert abs(first) < 0.05 * abs(settled)


# -------------------------------------------------------------------- driver

def test_the_driver_drives_the_speed_error_to_zero():
    """The whole specification. A proportional-only law would sit permanently
    below the reference by however much error makes the pedal it needs."""
    sim = DynoSim()
    for _ in range(int(60.0 / DT)):
        state = sim.step(15.0, DT)
    assert state.speed == pytest.approx(15.0, abs=1e-3)


def test_the_pedal_is_pinned_by_physics_not_by_history():
    """Under road resistance a steady speed needs exactly the pedal whose
    tractive force balances it, so the same setpoint reached three ways must
    settle on the same pedal."""
    parked = []
    for ramp in (None, 5.0, 20.0):
        sim = DynoSim()
        for k in range(int(60.0 / DT)):
            ref = 15.0 if ramp is None else min(15.0, 15.0 * k * DT / ramp)
            sim.step(ref, DT)
        parked.append(sim.throttle)
    assert max(parked) - min(parked) < 1e-4, 'history-dependent: %s' % parked


def test_throttle_and_brake_are_never_both_applied():
    driver = RobotDriver()
    for ref, v in ((20.0, 0.0), (0.0, 20.0), (10.0, 10.0), (5.0, 5.2)):
        thr, brk = driver.step(ref, v, DT)
        assert thr == 0.0 or brk == 0.0
        assert 0.0 <= thr <= 1.0 and 0.0 <= brk <= 1.0


def test_the_integral_does_not_wind_up_against_a_rail():
    driver = RobotDriver()
    for _ in range(20000):
        driver.step(100.0, 0.0, DT)             # unreachable: pedal saturated
    assert abs(driver.integral) < 10.0, 'wound to %.1f' % driver.integral


def test_a_standstill_reference_is_held_on_the_brake():
    """A PI chasing zero from zero dithers; a driver stops."""
    sim = DynoSim()
    for _ in range(int(5.0 / DT)):
        state = sim.step(0.0, DT)
    assert state.speed == pytest.approx(0.0, abs=1e-9)
    assert sim.brake > 0.0 and sim.throttle == 0.0


def test_separate_throttle_and_brake_ceilings():
    driver = RobotDriver(max_throttle=0.25)
    for _ in range(2000):
        thr, _ = driver.step(50.0, 0.0, DT)
    assert thr == pytest.approx(0.25)


def test_a_delay_applies_the_command_of_that_long_ago():
    plain, late = RobotDriver(), RobotDriver(delay_s=0.1)
    sent = [plain.step(20.0, 0.0, 0.05) for _ in range(6)]
    got = [late.step(20.0, 0.0, 0.05) for _ in range(6)]
    assert got[:2] == [(0.0, 0.0)] * 2
    assert got[2:] == sent[:4]
    assert late.pedal == plain.pedal


def test_reset_empties_the_delay_line():
    driver = RobotDriver(delay_s=0.1)
    for _ in range(5):
        driver.step(20.0, 0.0, 0.05)
    driver.reset()
    assert driver.step(20.0, 0.0, 0.05) == (0.0, 0.0)


def test_the_actuator_gets_the_delayed_command_and_has_the_last_word():
    seen = []

    def half(thr, brk, dt):
        seen.append((thr, brk, dt))
        return 0.5 * thr, brk

    plain = RobotDriver()
    late = RobotDriver(delay_s=0.05, actuator=half)
    sent = [plain.step(20.0, 0.0, 0.05) for _ in range(3)]
    got = [late.step(20.0, 0.0, 0.05) for _ in range(3)]
    assert seen == [(0.0, 0.0, 0.05)] + [c + (0.05,) for c in sent[:2]]
    assert got == [(0.0, 0.0)] + [(0.5 * t, b) for t, b in sent[:2]]


def test_reset_resets_the_actuator_if_it_can():
    class Lag(object):
        resets = 0

        def __call__(self, thr, brk, dt):
            return thr, brk

        def reset(self):
            self.resets += 1

    lag = Lag()
    DynoSim(driver=RobotDriver(actuator=lag)).reset()
    assert lag.resets == 1


def test_a_delayed_driver_still_settles_on_the_reference():
    sim = DynoSim(driver=RobotDriver(delay_s=0.2))
    for _ in range(int(60.0 / 0.05)):
        state = sim.step(15.0, 0.05)
    assert state.speed == pytest.approx(15.0, abs=0.05)


def test_an_unknown_mode_is_rejected():
    with pytest.raises(ValueError):
        RobotDriver(mode='torque')


def test_accel_mode_needs_an_acceleration_command():
    with pytest.raises(ValueError):
        RobotDriver(mode='accel').step(5.0, 0.0, 0.05)


def test_accel_mode_tracks_a_constant_acceleration():
    sim = DynoSim(driver=RobotDriver(mode='accel'))
    speeds = [sim.step(0.0, 0.05, a_ref=1.0).speed for _ in range(160)]
    assert speeds[-1] == pytest.approx(8.0, abs=0.3)
    assert (speeds[-1] - speeds[-81]) / 4.0 == pytest.approx(1.0, abs=0.02)


def test_accel_mode_ignores_the_speed_reference():
    a, b = RobotDriver(mode='accel'), RobotDriver(mode='accel')
    for k in range(20):
        v = 0.1 * k
        assert a.step(0.0, v, 0.05, a_ref=0.5) == b.step(30.0, v, 0.05, a_ref=0.5)


def test_accel_mode_caps_the_command_at_the_envelope():
    sim = DynoSim(driver=RobotDriver(mode='accel', max_accel_mps2=1.8))
    speeds = [sim.step(0.0, 0.05, a_ref=5.0).speed for _ in range(160)]
    assert (speeds[-1] - speeds[-81]) / 4.0 == pytest.approx(1.8, abs=0.05)


def test_accel_mode_holds_the_brake_at_rest_when_not_asked_to_move():
    driver = RobotDriver(mode='accel')
    for _ in range(20):
        thr, brk = driver.step(0.0, 0.0, 0.05, a_ref=0.0)
    assert thr == 0.0 and brk == pytest.approx(0.3)
    assert driver.integral == 0.0


def test_the_bench_can_fail_to_reach_the_reference():
    """The gap between asked and achieved is the point of having a plant."""
    sim = DynoSim()
    for _ in range(int(3.0 / DT)):
        state = sim.step(60.0, DT)
    assert state.speed < 40.0
    assert sim.throttle == pytest.approx(1.0)


def test_reset_restores_a_known_state():
    sim = DynoSim()
    for _ in range(int(5.0 / DT)):
        sim.step(15.0, DT)
    sim.reset(speed=7.0)
    assert sim.speed == pytest.approx(7.0)
    assert sim.driver.integral == 0.0
    assert sim.time == 0.0
