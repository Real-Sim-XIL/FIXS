"""Invariants for the dyno / CARLA speed-sync model.

These are not accuracy tests against measured data -- there is none yet. They
pin the properties the study's conclusions rest on, so that if someone changes
the model the conclusions cannot silently stop following from it.

Run:  pytest tests/Xil/Probes/dyno_speed_sync/ -v
"""

import math
import os
import sys

import pytest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import dyno_sync_sim as sim  # noqa: E402


# ------------------------------------------------------------------ actuator

def test_actuator_settles_to_its_command():
    """A second-order lag must reach the commanded force, not something else."""
    act = sim.SecondOrderActuator(2 * math.pi * 5.0, 0.7)
    for _ in range(20000):                     # 20 s at 1 ms, many time constants
        f = act.step(1000.0, 0.001)
    assert f == pytest.approx(1000.0, rel=1e-3)


def test_actuator_underdamped_overshoots_and_overdamped_does_not():
    """Guards the damping term's sign, which is easy to flip unnoticed."""
    peaks = {}
    for zeta in (0.2, 2.0):
        act = sim.SecondOrderActuator(2 * math.pi * 5.0, zeta)
        peaks[zeta] = max(act.step(1000.0, 0.001) for _ in range(4000))
    assert peaks[0.2] > 1050.0                 # clear overshoot
    assert peaks[2.0] <= 1000.0 + 1e-6         # none


# ------------------------------------------------------------------ resistance

def test_road_load_and_carla_resistance_oppose_motion():
    d, c = sim.DynoParams(), sim.CarlaParams()
    assert sim.road_load_N(d, 10.0) > 0 and sim.road_load_N(d, -10.0) < 0
    assert sim.carla_resistance_N(c, 10.0) > 0 and sim.carla_resistance_N(c, -10.0) < 0


def test_steady_state_gap_changes_sign():
    """The central physical claim: the mismatch is not a constant offset.

    CARLA's rolling term dominates at low speed and the dyno's C dominates at
    high speed, so the disagreement crosses zero. If it stopped crossing, a
    single bias correction would fix the coupling and the study's conclusion
    would be wrong.
    """
    gap = sim.steady_state_gap_N(sim.DynoParams(), sim.CarlaParams(),
                                 [float(v) for v in range(0, 41)])
    lo = gap[0][1]
    hi = gap[-1][1]
    assert lo < 0.0 < hi
    crossings = [v for (v, g), (_, gn) in zip(gap, gap[1:]) if g * gn < 0]
    assert len(crossings) == 1, "expected exactly one sign change, got %s" % crossings


def _matched_pair():
    """A dyno and a CARLA whose resistance models are algebraically identical.

    CARLA's aero is 0.5*rho*CdA*v^2 with rho hardcoded to 1.25, so setting the
    dyno's C to 0.5*1.25*0.377 and zeroing A, B and CARLA's rolling makes the
    two resistance curves the same function.
    """
    c = sim.CarlaParams(roll_coeff=0.0)
    C_equiv = 0.5 * c.air_density * c.aero_CdA_m2
    d = sim.DynoParams(mass_kg=c.mass_kg, road_A_N=0.0, road_B_Npms=0.0,
                       road_C_Npms2=C_equiv)
    return d, c


def test_matching_the_resistance_models_cuts_the_sync_force():
    """Sanity anchor: remove the resistance mismatch and F_sync must drop hard."""
    cycle = sim.synthetic_cycle(40.0, 0.001)
    run = sim.RunParams()
    d, c = _matched_pair()
    f_matched = sim.simulate(cycle, d, c, run,
                             "forced_driven")["_summary"]["F_sync_rms_N"]
    f_mismatched = sim.simulate(cycle, sim.DynoParams(), sim.CarlaParams(), run,
                                "forced_driven")["_summary"]["F_sync_rms_N"]
    assert f_matched < f_mismatched / 5.0


def test_actuator_lag_is_a_second_independent_source_of_sync_force():
    """Matching the resistance does not drive F_sync to zero, and that matters.

    Even with identical resistance curves and equal masses the dyno delivers its
    tractive effort through a second-order actuator while CARLA's driver applies
    force immediately. The plants therefore differ dynamically, and the sync has
    to supply that difference. Widening the actuator bandwidth shrinks it, which
    is what identifies the lag as the cause rather than the resistance.
    """
    cycle = sim.synthetic_cycle(40.0, 0.001)
    run = sim.RunParams()
    d, c = _matched_pair()

    slow = sim.simulate(cycle, d, c, run, "forced_driven")["_summary"]["F_sync_rms_N"]
    d_fast = sim.DynoParams(**{**d.__dict__,
                               "act_wn_radps": 2 * math.pi * 50.0})
    fast = sim.simulate(cycle, d_fast, c, run,
                        "forced_driven")["_summary"]["F_sync_rms_N"]

    assert slow > 1.0, "matched resistance should still leave a lag residual"
    assert fast < slow / 3.0, "a 10x faster actuator should shrink it"


# ------------------------------------------------------------------ the point

def test_sync_force_is_independent_of_sync_rate_but_error_is_not():
    """The result the whole study exists to establish.

    F_sync = m*dv/T and dv grows roughly linearly with T, so the injected FORCE
    is nearly invariant while the speed ERROR scales with T. Syncing faster buys
    a tighter speed match and does nothing about the disturbance.
    """
    cycle = sim.synthetic_cycle(60.0, 0.001)
    dyno, carla = sim.DynoParams(), sim.CarlaParams()
    out = {}
    for T in (0.005, 0.05):
        run = sim.RunParams(sync_dt_s=T)
        out[T] = sim.simulate(cycle, dyno, carla, run, "forced_driven")["_summary"]

    force_ratio = out[0.05]["F_sync_rms_N"] / out[0.005]["F_sync_rms_N"]
    error_ratio = out[0.05]["speed_err_rms_mps"] / out[0.005]["speed_err_rms_mps"]

    assert 0.9 < force_ratio < 1.1, "force should be ~invariant, got %.3f" % force_ratio
    assert error_ratio > 5.0, "error should scale with T, got %.3f" % error_ratio


def test_forced_driven_beats_forced_coast_by_a_wide_margin():
    """Coasting CARLA makes the sync supply the whole tractive effort.

    That is the pessimistic bound, not the architecture, and the gap between the
    two is why the distinction matters.
    """
    cycle = sim.synthetic_cycle(60.0, 0.001)
    args = (cycle, sim.DynoParams(), sim.CarlaParams(), sim.RunParams())
    coast = sim.simulate(*args, "forced_coast")["_summary"]
    driven = sim.simulate(*args, "forced_driven")["_summary"]
    assert driven["F_sync_rms_N"] < coast["F_sync_rms_N"] / 3.0


def test_forced_tracks_speed_far_better_than_a_closed_loop_driver():
    """Forced sync is a hard overwrite, so it must dominate PI tracking."""
    cycle = sim.synthetic_cycle(60.0, 0.001)
    args = (cycle, sim.DynoParams(), sim.CarlaParams(), sim.RunParams())
    track = sim.simulate(*args, "track")["_summary"]
    forced = sim.simulate(*args, "forced_driven")["_summary"]
    assert forced["speed_err_rms_mps"] < track["speed_err_rms_mps"] / 100.0


def test_distance_error_stays_small_under_forced_sync():
    cycle = sim.synthetic_cycle(60.0, 0.001)
    s = sim.simulate(cycle, sim.DynoParams(), sim.CarlaParams(),
                     sim.RunParams(), "forced_driven")["_summary"]
    assert abs(s["distance_err_pct"]) < 0.05


# ------------------------------------------------------------------ plumbing

def test_unknown_coupling_is_rejected():
    with pytest.raises(ValueError):
        sim.simulate([0.0], sim.DynoParams(), sim.CarlaParams(),
                     sim.RunParams(), "nonsense")


def test_cycle_csv_roundtrip(tmp_path):
    p = tmp_path / "cycle.csv"
    p.write_text("time,speed\n0,0\n1,10\n2,10\n")
    out = sim.load_cycle_csv(str(p), 0.01)
    assert len(out) == 200
    assert out[0] == pytest.approx(0.0, abs=1e-9)
    assert out[100] == pytest.approx(10.0, abs=0.2)


def test_main_writes_its_outputs(tmp_path):
    rc = sim.main(["--duration", "5", "--no-plot", "--out", str(tmp_path)])
    assert rc == 0
    assert (tmp_path / "summary.json").is_file()
    for coupling in sim.COUPLINGS:
        assert (tmp_path / ("trace_%s.csv" % coupling)).is_file()
