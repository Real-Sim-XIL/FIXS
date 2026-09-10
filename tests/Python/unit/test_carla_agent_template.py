"""Can a CARLA-shaped agent drive the FIXS ego with no CARLA-specific glue?

That is the whole claim of `fixs.EgoVehicle`, and it is the kind of claim that
rots quietly: the adapter it delegates to is the only thing standing between
CARLA's agent code and a FIXS record, and nothing in a run says whether the
agent was wired up or merely constructed.

So this drives the documented shape end to end -- build the agent on the
handle, hand it `route()`, `attach()`, then step it -- and asserts the two
things that must be true: the agent's own class is still underneath, and a
command reaches the record.

    python -m pytest tests/Python/unit/test_carla_agent_template.py
"""
from __future__ import annotations

import os

import pytest

from CommonLib import fixs

carla = pytest.importorskip("carla", reason="needs the CARLA PythonAPI")
BehaviorAgent = pytest.importorskip(
    "agents.navigation.behavior_agent",
    reason="needs the vendored agents' dependencies").BehaviorAgent


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
    object.__setattr__(e, "id", "ego")
    object.__setattr__(e, "_written", frozenset())
    values = dict(positionX=0.0, positionY=0.0, positionZ=0.0, heading=90.0,
                  speed=5.0, speedDesired=8.0, speedLimit=11.18, feedAge=0.0,
                  precedingVehicleDistance=-1.0, signalLightColor=0,
                  signalLightDistance=-1.0, acceleratorPedalDesired=0.0,
                  brakePedalDesired=0.0, steerAngleDesired=0.0)
    values.update(fields)
    for k, v in values.items():
        object.__setattr__(e, k, v)
    return e


@pytest.fixture(autouse=True)
def noWireCheck():
    """Vehicle.set refuses fields absent from VehicleMessageField, which is a
    property of a connection these tests do not have."""
    saved = fixs._declaredFields
    fixs._declaredFields = None
    yield
    fixs._declaredFields = saved


@pytest.fixture
def driven(tmp_path):
    """The template, verbatim: this is the shape users are given."""
    ego = fixs.EgoVehicle(config(tmp_path), "ego")
    agent = BehaviorAgent(ego, behavior="normal", opt_dict=ego.plannerOptions())
    agent.set_global_plan(ego.route())
    ego.attach(agent)
    yield ego, agent
    ego.close()


def test_the_agents_own_class_is_still_underneath(driven):
    """FIXS answers the two detectors from the wire and changes nothing else.
    If the user's agent stopped being their agent, this is where it shows."""
    _, agent = driven
    assert isinstance(agent, BehaviorAgent)
    assert type(agent).__mro__[1] is BehaviorAgent


def test_a_command_reaches_the_record(driven):
    ego, agent = driven
    rec = record()
    ego.update(rec, 0.05)
    ego.apply(rec, agent.run_step())
    assert rec._written == {"acceleratorPedalDesired", "brakePedalDesired",
                            "steerAngleDesired"}
    assert 0.0 <= rec.acceleratorPedalDesired <= 1.0
    assert 0.0 <= rec.brakePedalDesired <= 1.0


def test_the_advisory_governs_the_target_and_the_road_limit_does_not(driven):
    """Every run_step branch takes min(max_speed, speed_limit - speed_lim_dist).
    The advisory has to arrive as max_speed and the ROAD limit as speed_limit:
    swap them and the obstacle search range collapses to its 10 m floor."""
    ego, agent = driven
    ego.update(record(speedDesired=6.0, speedLimit=11.18), 0.05)
    assert agent._behavior.max_speed == pytest.approx(6.0 * 3.6)
    assert ego.get_speed_limit() == pytest.approx(11.18 * 3.6)
    assert agent._behavior.speed_lim_dist == 0.0


def test_the_route_is_relaid_before_the_plan_runs_out(driven):
    """The corridor is a LAP the traffic simulator drives EgoRouteRepeat times.
    The agent has no notion of that and brakes to a stop when its plan empties,
    which reads exactly like a stall."""
    ego, agent = driven
    assert ego.lapsLaid == 1
    ego.update(record(), 0.05)          # plan is short, so this tops it up
    assert ego.lapsLaid == 2
    ego.update(record(), 0.05)          # and stops at EgoRouteRepeat
    assert ego.lapsLaid == 2


def test_the_step_the_agent_is_given_is_the_one_it_is_called_at(driven):
    """dt is the CARLA step, not the feed period -- the PID gains are per-step,
    so the feed period would detune them by the sub-step ratio."""
    ego, _ = driven
    ego.update(record(), 0.025)
    assert ego.dt == pytest.approx(0.025)


def test_an_empty_route_fails_at_setup_rather_than_mid_run(tmp_path):
    with pytest.raises(SystemExit):
        fixs.EgoVehicle(config(tmp_path, EgoRoutePoints=[]), "ego")


def test_the_log_carries_one_row_per_command(tmp_path):
    ego = fixs.EgoVehicle(config(tmp_path), "ego")
    agent = BehaviorAgent(ego, behavior="normal", opt_dict=ego.plannerOptions())
    agent.set_global_plan(ego.route())
    ego.attach(agent)
    for _ in range(5):
        rec = record()
        ego.update(rec, 0.05)
        ego.apply(rec, agent.run_step())
    ego.close()
    with open(os.path.join(str(tmp_path), "agent.csv"), encoding="utf-8") as f:
        rows = f.read().strip().splitlines()
    assert len(rows) == 6                       # header + 5
    assert rows[0].startswith("t,feedAge,x,y")
