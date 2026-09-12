"""EgoSetup: one description of the ego, for every backend (#305).

A vehicle whose motion something outside the traffic simulator computes is the
same situation whether that something is CarMaker, an XIL plant or Carla. #305
replaced every duplicate description of it with one EgoSetup section.

Two things are worth testing and neither is visible in a run: that the new
vocabulary derives what the bridges act on, and that a v0.9.0 config still
derives the same thing it always did. A config that quietly derives something
else does not fail -- the ego is simply driven by something other than what the
scenario asked for.

    python -m pytest tests/Python/unit/test_ego_setup_config.py
"""
from __future__ import annotations

import pytest
import yaml

from CommonLib.ConfigHelper import ConfigHelper

# The smallest config that reaches the EgoSetup section. VehicleMessageField
# carries the three actuation fields because a 'user' control law served over
# the feed is refused without them.
BASE = {
    "SimulationSetup": {
        "EnableRealSim": True,
        "SimulationEndTime": 100,
        "SelectedTrafficSimulator": "SUMO",
        "VehicleMessageField": ["id", "speed", "positionX", "positionY",
                                "steerAngleDesired", "acceleratorPedalDesired",
                                "brakePedalDesired"],
    },
    "ApplicationSetup": {
        "EnableApplicationLayer": True,
        "VehicleSubscription": [{"type": "ego",
                                 "attribute": {"all": ["true"]},
                                 "ip": ["127.0.0.1"], "port": [430]}],
    },
    # The C++ bridge, so these tests are about the EgoSetup vocabulary and not
    # about the Python backend's in-process rule -- which has its own test at
    # the bottom of this file.
    "CarlaSetup": {"EnableCosimulation": True, "InterestedIds": ["ego"],
                   "EnablePythonBackend": False},
}

EGO = ("Id", "Type", "Dynamics", "ActuationSource", "Controller")


def parse(tmp_path, ego=None, carla=None, name="c.yaml"):
    doc = yaml.safe_load(yaml.safe_dump(BASE))     # deep copy
    if ego is not None:
        doc["EgoSetup"] = dict(ego)
    if carla:
        doc["CarlaSetup"].update(carla)
    p = tmp_path / name
    p.write_text(yaml.safe_dump(doc), encoding="utf-8")
    cfg = ConfigHelper()
    cfg.getConfig(str(p))
    return cfg


def ego(cfg):
    return {k: cfg.Ego_setup[k] for k in EGO}


# --------------------------------------------------------------------------
# the vocabulary
# --------------------------------------------------------------------------

@pytest.mark.parametrize("source,controller,inProcess", [
    ("simulator", None, False),
    ("fixs", None, False),
    ("user", None, False),          # no Controller -> over the 0.1 s feed
    ("user", "ctl.py", True),       # Controller named -> in-process
])
def test_user_code_placement_follows_the_Controller_key(source, controller, inProcess, tmp_path):
    """"user" is ONE value. Where the code runs is decided by whether a
    Controller file is named, not by a fourth vocabulary word."""
    e = {"Dynamics": "virenv", "ActuationSource": source}
    if controller:
        e["Controller"] = controller
    cfg = parse(tmp_path, ego=e)
    assert cfg.Ego_setup["ActuationSource"] == source
    assert bool(cfg.Ego_setup["Controller"]) is inProcess


def test_traffic_dynamics_leaves_the_ego_with_the_traffic_simulator(tmp_path):
    assert parse(tmp_path, ego={"Dynamics": "traffic"}).Ego_setup["Dynamics"] == "traffic"


def test_values_are_canonical_whatever_case_they_are_written_in(tmp_path):
    """Consumers compare strings, so the parser owns the folding."""
    cfg = parse(tmp_path, ego={"Dynamics": "VirEnv", "ActuationSource": "FIXS"})
    assert cfg.Ego_setup["Dynamics"] == "virenv"
    assert cfg.Ego_setup["ActuationSource"] == "fixs"


def test_an_unknown_value_is_refused_rather_than_ignored(tmp_path):
    """Silently ignoring one is how a scenario ran as L0 on one side and L2 on
    the other: nothing failed, the ego just was not taken over."""
    with pytest.raises(SystemExit) as exc:
        parse(tmp_path, ego={"Dynamics": "physx"})
    assert "traffic|virenv|xil" in str(exc.value)

    with pytest.raises(SystemExit) as exc:
        parse(tmp_path, ego={"Dynamics": "virenv", "ActuationSource": "autopilot"})
    assert "simulator|fixs|user" in str(exc.value)


def test_xil_says_where_that_case_actually_lives(tmp_path):
    """It is not unimplemented -- it is the CarMaker path, switched on elsewhere."""
    with pytest.raises(SystemExit) as exc:
        parse(tmp_path, ego={"Dynamics": "xil"})
    assert "CarMakerSetup.EnableCosimulation" in str(exc.value)


def test_the_id_is_inferred_from_the_lone_subscription(tmp_path):
    """Better than the per-backend defaults it replaces, which named a vehicle
    that did not exist whenever the subscription was called something else."""
    assert parse(tmp_path, ego={"Dynamics": "virenv"}).Ego_setup["Id"] == "ego"


# --------------------------------------------------------------------------
# EgoSetup wins over the per-backend keys it replaced
# --------------------------------------------------------------------------

def test_EgoSetup_overrides_the_key_it_replaces(tmp_path):
    """A half-migrated config must resolve to ONE ego, and it is EgoSetup's."""
    cfg = parse(tmp_path, ego={"Id": "ego"}, carla={"EgoId": "stale_ego"})
    assert cfg.Ego_setup["Id"] == "ego"


def test_the_replaced_keys_still_supply_id_and_type(tmp_path):
    cfg = parse(tmp_path, ego={"Dynamics": "virenv"},
                carla={"EgoId": "veh0", "EgoSumoType": "EGO_TYPE_EXTERNAL"})
    assert cfg.Ego_setup["Id"] == "veh0"
    assert cfg.Ego_setup["Type"] == "EGO_TYPE_EXTERNAL"


# --------------------------------------------------------------------------
# back-compat for the keys that shipped in v0.9.0
# --------------------------------------------------------------------------

@pytest.mark.parametrize("mode,ext,dynamics", [
    (2, True, "virenv"),
    (0, False, "traffic"),
    (2, False, "traffic"),      # the pair could disagree; BOTH were required
    (0, True, "traffic"),
])
def test_EgoMode_and_EnableExternalControl_still_say_Dynamics(mode, ext, dynamics, tmp_path):
    """v0.9.0 needed BOTH keys to mean "the virtual environment owns the ego",
    and nothing stopped them disagreeing -- which is the bug EgoSetup replaced."""
    cfg = parse(tmp_path, carla={"EgoMode": mode, "EnableExternalControl": ext})
    assert cfg.Ego_setup["Dynamics"] == dynamics


@pytest.mark.parametrize("l0,source", [
    ("TM", "simulator"), ("Pursuit", "fixs"), ("EgoDriver", "fixs"),
    ("Actuation", "user"), ("Embedded", "user")])
def test_EgoL0Driver_still_says_ActuationSource(l0, source, tmp_path):
    cfg = parse(tmp_path, carla={"EgoMode": 2, "EnableExternalControl": True,
                                 "EgoL0Driver": l0})
    assert cfg.Ego_setup["ActuationSource"] == source


def test_a_config_with_no_ego_keys_at_all_is_not_taken_over(tmp_path):
    assert parse(tmp_path).Ego_setup["Dynamics"] == ""


# --------------------------------------------------------------------------
# the Python backend serves a user control law in-process only
# --------------------------------------------------------------------------

def test_the_python_backend_refuses_user_control_over_the_feed(tmp_path):
    """On the feed, `speed` carries whatever the traffic simulator left -- the
    eco advisory under L2 -- so a speed loop reads back its own setpoint. The
    run does not fail; the controller's own log shows textbook tracking while
    the car does something else. Refused at load instead."""
    with pytest.raises(SystemExit) as exc:
        parse(tmp_path, ego={"Dynamics": "virenv", "ActuationSource": "user"},
              carla={"EnablePythonBackend": True})
    assert "needs a Controller on the Python" in str(exc.value)


def test_naming_a_controller_satisfies_it(tmp_path):
    cfg = parse(tmp_path, ego={"Dynamics": "virenv", "ActuationSource": "user",
                               "Controller": "ctl.py"},
                carla={"EnablePythonBackend": True})
    assert cfg.Ego_setup["Controller"] == "ctl.py"


def test_the_legacy_spelling_is_refused_the_same_way(tmp_path):
    """EgoL0Driver: Actuation is the v0.9.0 way of asking for the feed path, and
    it is the same broken loop -- so it is refused, not quietly honoured."""
    with pytest.raises(SystemExit) as exc:
        parse(tmp_path, carla={"EgoMode": 2, "EnableExternalControl": True,
                               "EgoL0Driver": "Actuation",
                               "EnablePythonBackend": True})
    assert "needs a Controller on the Python" in str(exc.value)
