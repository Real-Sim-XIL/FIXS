"""ConfigHelper.cpp and ConfigHelper.py resolve ViewSetup.Views identically.

TrafficLayer and VirCarlaEnv read the C++ parse, the Python bridge reads the
Python one, and both back-fill the legacy CarlaSetup/SumoSetup keys from it.
This runs view_dump.exe (tests/VirEnvCore/view_dump.cpp) on each case and holds
its lines to the same lines built from ConfigHelper.py.

    python -m pytest tests/VirEnv/test_view_setup_parity.py
"""

import os
import subprocess
import sys

import pytest
import yaml

HERE = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, REPO_ROOT)
sys.path.insert(0, HERE)
import cpp_build                                                    # noqa: E402
from CommonLib.ConfigHelper import ConfigHelper                     # noqa: E402

BASE = {"SimulationSetup": {"SelectedTrafficSimulator": "SUMO",
                            "VehicleMessageField": ["id", "speed", "speedDesired"]}}

CASES = {
    "defaults": {},
    "legacy": {
        "CarlaSetup": {"EnableSpectatorFollow": False, "CenteredViewId": "veh7",
                       "SpectatorHeight": 30.5, "SpectatorAlignYaw": True},
        "SumoSetup": {"EnableGuiFollow": True, "GuiCenteredViewId": "veh8", "GuiZoom": 900},
    },
    "pov_alongside_legacy": {
        "CarlaSetup": {"SpectatorHeight": 40},
        "SumoSetup": {"EnableGuiFollow": True},
        "ViewSetup": {"Views": [
            {"Name": "pov", "Type": "chase", "Size": [1280, 720], "Fov": 100, "SensorTick": 0.05},
            {"Name": "eye", "Type": "driver", "Enable": False},
            {"Name": "rec", "Type": "chase", "Display": False, "Record": "_video/chase.mp4"},
            {"Name": "top", "Type": "bev", "Height": 25, "Target": "veh3"},
        ]},
    },
    "everything_declared": {
        "ViewSetup": {"Views": [
            {"Name": "main", "Simulator": "carla", "Type": "spectator", "Target": "veh1",
             "Height": 70, "AlignYaw": True},
            {"Name": "cam", "Type": "chase", "Offset": [-4, 0.5, 2], "Pitch": -20},
            {"Name": "gui", "Simulator": "sumo", "Zoom": 300},
            {"Name": "junction", "Simulator": "sumo", "Type": "fixed", "Boundary": [0, 0, 200, 100.5]},
            {"Name": "second", "Simulator": "sumo", "Type": "follow", "Target": "veh9"},
        ]},
    },
}


def _g(x):
    return '%.6g' % x


def _pythonLines(path):
    c = ConfigHelper()
    c.getConfig(path)
    out = []
    for v in c.View_setup["Views"]:
        out.append('VIEW|%s|%s|%s|%s|%d|%d|%dx%d|%s|%s|%s|%s|%s|%d|%s|%s|%d|%s' % (
            v["Name"], v["Simulator"], v["Type"], v["Target"], v["Enable"], v["Implied"],
            v["Size"][0], v["Size"][1], _g(v["Fov"]), _g(v["SensorTick"]),
            ','.join(_g(x) for x in v["Offset"]), _g(v["Pitch"]), _g(v["Height"]),
            v["AlignYaw"], _g(v["Zoom"]), ','.join(_g(x) for x in v["Boundary"]),
            v["Display"], v["Record"]))
    cs, ss = c.Carla_setup, c.Sumo_setup
    out.append('VIEW|legacy-carla|%d|%s|%s|%d' % (cs["EnableSpectatorFollow"], cs["CenteredViewId"],
                                                   _g(cs["SpectatorHeight"]), cs["SpectatorAlignYaw"]))
    out.append('VIEW|legacy-sumo|%d|%s|%s' % (ss["EnableGuiFollow"], ss["GuiCenteredViewId"], _g(ss["GuiZoom"])))
    return out


@pytest.mark.parametrize("case", sorted(CASES))
def test_cpp_and_python_resolve_the_same_views(case, tmp_path):
    exe = cpp_build.ensureBuilt()['view_dump']
    doc = dict(BASE)
    doc.update(CASES[case])
    p = tmp_path / ('%s.yaml' % case)
    p.write_text(yaml.safe_dump(doc), encoding='utf-8')
    proc = subprocess.run([exe, str(p)], capture_output=True, text=True, timeout=30)
    assert proc.returncode == 0, proc.stdout + proc.stderr
    cpp = [ln for ln in proc.stdout.splitlines() if ln.startswith('VIEW|')]
    assert cpp == _pythonLines(str(p))


@pytest.mark.parametrize("views", [
    [{"Simulator": "vissim"}],
    [{"Type": "spectator"}, {"Name": "s2", "Type": "spectator"}],
    [{"Name": "a"}, {"Name": "a"}],
    [{"Simulator": "sumo", "Type": "fixed"}],
    [{"Offset": [1, 2]}],
    [{"Record": "chase.mkv"}],
])
def test_cpp_refuses_what_python_refuses(views, tmp_path):
    exe = cpp_build.ensureBuilt()['view_dump']
    doc = dict(BASE)
    doc["ViewSetup"] = {"Views": views}
    p = tmp_path / 'bad.yaml'
    p.write_text(yaml.safe_dump(doc), encoding='utf-8')
    with pytest.raises(SystemExit):
        _pythonLines(str(p))
    proc = subprocess.run([exe, str(p)], capture_output=True, text=True, timeout=30)
    assert proc.returncode != 0 and 'ERROR: ViewSetup' in proc.stdout
