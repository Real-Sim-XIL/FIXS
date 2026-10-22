"""ViewSetup.Views: the list of co-sim views, and how the legacy keys map onto it.

    python -m pytest tests/Python/unit/test_view_setup.py
"""
from __future__ import annotations

import random

import pytest
import yaml

from CommonLib.ConfigHelper import ConfigHelper
from Carla.VirEnv.CarlaViews import FrameSequencer, cameraMount, cameraViews, windowTitle

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
    assert (pov["Simulator"], pov["Target"], pov["Size"], pov["Fov"], pov["SensorTick"],
            pov["Display"], pov["Record"]) == ("carla", "ego", [1280, 720], 90.0, 0.0, True, "")
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


def test_a_camera_needs_a_window_or_a_recording(tmp_path):
    cfg = parse(tmp_path, views=[
        {"Name": "rec", "Type": "chase", "Display": False, "Record": "_video/chase.mp4"},
        {"Name": "nothing", "Type": "driver", "Display": False}])
    assert [v["Name"] for v in cameraViews(cfg.View_setup["Views"])] == ["rec"]
    assert byName(cfg)["rec"]["Record"] == "_video/chase.mp4"


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
    ([{"Record": "chase.mkv"}], "Record must be a .mp4 or .avi"),
])
def test_bad_views_are_refused(views, needle, tmp_path):
    with pytest.raises(SystemExit) as e:
        parse(tmp_path, views=views)
    assert needle in str(e.value)


def sequence(frames, step=0.05, **kw):
    """World frame of each video frame the sequencer writes, for frames arriving in this order."""
    sq = FrameSequencer(step, **kw)
    out = []
    for f in frames:
        out += [img for img, n in sq.add(f, f) for _ in range(n)]
    out += [img for img, n in sq.finish() for _ in range(n)]
    return sq, out


def test_frames_in_order_give_one_video_frame_per_tick():
    sq, out = sequence(range(100, 6100))
    assert out == list(range(100, 6100))
    assert (sq.written, sq.filled, sq.dropped) == (6000, 0, 0)


def test_late_and_out_of_order_frames_give_one_video_frame_per_tick():
    """The live bug: arrival order is not frame order. No frame is missing, so no frame repeats."""
    rng = random.Random(371)
    # Each frame arrives up to 8 ticks late, so neighbours swap and some are overtaken by several.
    frames = sorted(range(1000, 4000), key=lambda f: f + rng.uniform(0, 8))
    assert frames != sorted(frames)
    sq, out = sequence(frames)
    assert out == list(range(1000, 4000))
    assert (sq.written, sq.filled, sq.skipped, sq.dropped) == (3000, 0, 0, 0)


def test_a_missing_frame_holds_the_previous_one():
    sq, out = sequence([10, 11, 13, 14, 15])
    assert out == [10, 11, 11, 13, 14, 15] and sq.filled == 1


def test_sensor_tick_coarser_than_the_world_repeats_each_frame():
    sq, out = sequence(range(0, 40, 2))              # SensorTick 0.1 on a 0.05 s world
    assert out == [f - (f % 2) for f in range(0, 39)]
    assert sq.written == 39


def test_a_long_gap_is_skipped_not_filled():
    sq, out = sequence(list(range(0, 5)) + list(range(100, 105)))   # 4.75 s gap at 0.05 s
    assert out == list(range(0, 5)) + list(range(100, 105))
    assert (sq.skipped, sq.filled) == (95, 0)


def test_never_more_video_frames_than_world_ticks():
    frames = [5, 3, 4, 3, 9, 8, 7, 6, 4, 10]          # duplicates, late, reversed
    sq, out = sequence(frames)
    assert out == list(range(3, 11)) and sq.written == 10 - 3 + 1



def bursty(first, n, rng, maxLate):
    """Arrival order of frames first..first+n-1 when CARLA stalls a run of them and delivers it in one burst.

    A stalled run of 3-12 consecutive frames arrives together, 11 to maxLate ticks after
    its first frame, with the later frames overtaking it. No frame is lost.
    """
    arrival = {}
    f = first
    while f < first + n:
        if rng.random() < 0.012:
            run = rng.randint(3, 12)
            when = f + rng.randint(max(11, run), maxLate)
            for k in range(f, min(first + n, f + run)):
                arrival[k] = when
            f += run
        else:
            arrival[f] = f
            f += 1
    return sorted(arrival, key=lambda k: (arrival[k], k))


def test_bursts_up_to_30_ticks_late_are_kept_not_repeated():
    """The live run: about 9% of frames came more than 10 ticks late, in bursts. None missing."""
    rng = random.Random(4321)
    frames = bursty(1791, 6000, rng, 30)
    assert sorted(frames) == list(range(1791, 7791))
    sq, out = sequence(frames)
    assert 20 <= sq.maxLateness <= 30
    assert out == list(range(1791, 7791))
    assert (sq.written, sq.filled, sq.skipped, sq.dropped, sq.capHits) == (6000, 0, 0, 0, 0)


def test_the_window_is_two_seconds_of_sim_time():
    assert FrameSequencer(0.05).window == 40
    assert FrameSequencer(0.1).window == 20


def test_the_window_widens_to_the_worst_lateness_seen():
    # Frame 100 arrives after 160 and frame 200 after 260: both 60 ticks late, past the 40-tick window.
    frames = [f for f in range(300) if f not in (100, 200)]
    frames.insert(frames.index(160) + 1, 100)
    frames.insert(frames.index(260) + 1, 200)
    sq, out = sequence(frames)
    assert sq.maxLateness == 60 and sq.window == 61
    assert (sq.dropped, sq.filled) == (1, 1)          # the first one was lost, the second kept
    assert out[200] == 200 and out[100] == 99 and len(out) == 300


def test_buffered_frames_are_capped():
    frames = list(range(1, 200)) + [0]                # frame 0 never comes in time
    sq, out = sequence(frames, maxBuffered=16)
    assert sq.peakBuffered <= 16 and sq.capHits >= 1
    assert len(out) <= 200

