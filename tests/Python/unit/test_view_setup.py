"""ViewSetup.Views: the list of co-sim views, and how the legacy keys map onto it.

    python -m pytest tests/Python/unit/test_view_setup.py
"""
from __future__ import annotations

import numpy as np
import pytest
import yaml

from CommonLib.ConfigHelper import ConfigHelper
from Carla.VirEnv.CarlaViews import bgraToPpm, cameraMount, cameraViews, windowTitle

BASE = {
    "SimulationSetup": {"SelectedTrafficSimulator": "SUMO",
                        "VehicleMessageField": ["id", "speed", "speedDesired"]},
}


def parse(tmp_path, carla=None, sumo=None, views=None):
    doc = yaml.safe_load(yaml.safe_dump(BASE))
    if carla is not None:
        doc["CarlaSetup"] = carla
    if sumo is not None:
        doc["SumoSetup"] = sumo
    if views is not None:
        doc["ViewSetup"] = {"Views": views}
    p = tmp_path / "c.yaml"
    p.write_text(yaml.safe_dump(doc), encoding="utf-8")
    cfg = ConfigHelper()
    cfg.getConfig(str(p))
    return cfg


def byName(cfg):
    return {v["Name"]: v for v in cfg.View_setup["Views"]}


def test_no_views_and_no_legacy_keys_keep_todays_defaults(tmp_path):
    cfg = parse(tmp_path)
    views = cfg.View_setup["Views"]
    assert [(v["Name"], v["Simulator"], v["Type"], v["Implied"]) for v in views] == [
        ("spectator", "carla", "spectator", True), ("sumo-gui", "sumo", "follow", True)]
    spec, gui = views
    assert (spec["Enable"], spec["Target"], spec["Height"], spec["AlignYaw"]) == (True, "ego", 50.0, False)
    assert (gui["Enable"], gui["Target"], gui["Zoom"]) == (False, "ego", 600.0)
    assert cfg.Carla_setup["EnableSpectatorFollow"] is True
    assert cfg.Sumo_setup["EnableGuiFollow"] is False
    assert cameraViews(views) == []


def test_legacy_keys_become_the_implied_views(tmp_path):
    cfg = parse(tmp_path,
                carla={"EnableSpectatorFollow": False, "CenteredViewId": "veh7",
                       "SpectatorHeight": 30, "SpectatorAlignYaw": True},
                sumo={"EnableGuiFollow": True, "GuiCenteredViewId": "veh8", "GuiZoom": 900})
    v = byName(cfg)
    assert (v["spectator"]["Enable"], v["spectator"]["Target"], v["spectator"]["Height"],
            v["spectator"]["AlignYaw"]) == (False, "veh7", 30.0, True)
    assert (v["sumo-gui"]["Enable"], v["sumo-gui"]["Target"], v["sumo-gui"]["Zoom"]) == (True, "veh8", 900.0)
    # the legacy keys read back unchanged
    assert (cfg.Carla_setup["EnableSpectatorFollow"], cfg.Carla_setup["CenteredViewId"],
            cfg.Carla_setup["SpectatorHeight"], cfg.Carla_setup["SpectatorAlignYaw"]) == (False, "veh7", 30.0, True)
    assert (cfg.Sumo_setup["EnableGuiFollow"], cfg.Sumo_setup["GuiCenteredViewId"],
            cfg.Sumo_setup["GuiZoom"]) == (True, "veh8", 900.0)


def test_a_camera_view_is_added_alongside_the_legacy_spectator(tmp_path):
    cfg = parse(tmp_path, carla={"SpectatorHeight": 40},
                views=[{"Name": "pov", "Type": "chase", "Size": [1280, 720]}])
    v = byName(cfg)
    assert set(v) == {"spectator", "sumo-gui", "pov"}
    assert v["spectator"]["Implied"] and v["spectator"]["Height"] == 40.0
    pov = v["pov"]
    assert (pov["Simulator"], pov["Target"], pov["Size"], pov["Fov"], pov["SensorTick"]) == (
        "carla", "ego", [1280, 720], 90.0, 0.0)
    assert [c["Name"] for c in cameraViews(cfg.View_setup["Views"])] == ["pov"]
    assert windowTitle("pov") == "FIXS view: pov"


def test_a_declared_spectator_replaces_the_legacy_keys(tmp_path):
    cfg = parse(tmp_path, carla={"EnableSpectatorFollow": True, "SpectatorHeight": 50},
                views=[{"Name": "top", "Type": "spectator", "Target": "veh3", "Height": 80, "AlignYaw": True}])
    v = byName(cfg)
    assert "spectator" not in v and not v["top"]["Implied"]
    assert (cfg.Carla_setup["CenteredViewId"], cfg.Carla_setup["SpectatorHeight"],
            cfg.Carla_setup["SpectatorAlignYaw"]) == ("veh3", 80.0, True)


def test_the_first_sumo_view_is_sumo_guis_own(tmp_path):
    cfg = parse(tmp_path, sumo={"EnableGuiFollow": False},
                views=[{"Name": "gui", "Simulator": "sumo", "Target": "ego", "Zoom": 300},
                       {"Name": "junction", "Simulator": "sumo", "Type": "fixed",
                        "Boundary": [0, 0, 200, 100]}])
    v = byName(cfg)
    assert "sumo-gui" not in v
    assert (cfg.Sumo_setup["EnableGuiFollow"], cfg.Sumo_setup["GuiZoom"]) == (True, 300.0)
    assert v["junction"]["Boundary"] == [0.0, 0.0, 200.0, 100.0]


def test_a_disabled_view_parses_but_is_not_a_camera(tmp_path):
    cfg = parse(tmp_path, views=[{"Name": "pov", "Type": "driver", "Enable": False}])
    assert byName(cfg)["pov"]["Enable"] is False
    assert cameraViews(cfg.View_setup["Views"]) == []


@pytest.mark.parametrize("vtype,offset,pitch", [
    ("chase", [-6.5, 0.0, 2.8], -12.0),
    ("driver", [0.5, 0.0, 1.5], -5.0),
    ("bev", [0.0, 0.0, 50.0], -90.0),
])
def test_camera_mount_defaults(vtype, offset, pitch, tmp_path):
    view = byName(parse(tmp_path, views=[{"Name": "v", "Type": vtype}]))["v"]
    assert cameraMount(view) == tuple(offset) + (pitch,)


def test_bev_height_and_explicit_mount(tmp_path):
    v = byName(parse(tmp_path, views=[
        {"Name": "b", "Type": "bev", "Height": 25},
        {"Name": "c", "Type": "chase", "Offset": [-4, 0.5, 2], "Pitch": -20}]))
    assert cameraMount(v["b"]) == (0.0, 0.0, 25.0, -90.0)
    assert cameraMount(v["c"]) == (-4.0, 0.5, 2.0, -20.0)


@pytest.mark.parametrize("views,needle", [
    ([{"Simulator": "vissim"}], "Simulator must be carla|sumo"),
    ([{"Type": "follow"}], "Type for carla"),
    ([{"Simulator": "sumo", "Type": "chase"}], "Type for sumo"),
    ([{"Type": "spectator"}, {"Name": "s2", "Type": "spectator"}], "only one carla 'spectator'"),
    ([{"Name": "a"}, {"Name": "a", "Type": "driver"}], "duplicate Name 'a'"),
    ([{"Name": "spectator", "Type": "chase"}], "duplicate Name 'spectator'"),
    ([{"Size": [640]}], "Size must be a list of 2"),
    ([{"Size": [0, 480]}], "two positive pixel counts"),
    ([{"Simulator": "sumo", "Type": "fixed"}], "needs Boundary"),
])
def test_bad_views_are_refused(views, needle, tmp_path):
    with pytest.raises(SystemExit) as e:
        parse(tmp_path, views=views)
    assert needle in str(e.value)


def test_bgra_frame_becomes_an_rgb_ppm():
    w, h = 3, 2
    bgra = np.zeros((h, w, 4), np.uint8)
    bgra[..., 0], bgra[..., 1], bgra[..., 2], bgra[..., 3] = 10, 20, 200, 255
    bgra[1, 2] = (1, 2, 3, 255)
    ppm = bgraToPpm(bgra.tobytes(), w, h)
    header = b"P6 3 2 255\n"
    assert ppm.startswith(header)
    rgb = np.frombuffer(ppm[len(header):], np.uint8).reshape(h, w, 3)
    assert tuple(rgb[0, 0]) == (200, 20, 10)
    assert tuple(rgb[1, 2]) == (3, 2, 1)
