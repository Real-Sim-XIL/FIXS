"""run_cosim stops only what it owns: a port held by a process no run owns, or by
another live run, is refused rather than killed, and --cleanup without --all
leaves unowned processes alone.

The holders are real sleeping processes; the port table and the name-based
leftover scan are stubbed so nothing listens on a real port.
"""
import os
import subprocess
import sys

import pytest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "..", "Carla"))
import run_cosim  # noqa: E402
import run_registry as rr  # noqa: E402


@pytest.fixture
def registry(tmp_path, monkeypatch):
    monkeypatch.setenv(rr.RUNS_DIR_ENV, str(tmp_path / "runs"))
    monkeypatch.setattr(run_cosim, "_RUN", None)
    return str(tmp_path / "runs")


@pytest.fixture
def sleeper():
    p = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(120)"])
    yield p
    p.kill()
    p.wait(timeout=10)


@pytest.fixture
def kills(monkeypatch):
    seen = []
    monkeypatch.setattr(run_cosim, "_kill_pid_tree", seen.append)
    return seen


def test_a_port_held_by_no_run_is_refused_not_killed(registry, sleeper, kills, monkeypatch):
    monkeypatch.setattr(run_cosim, "_pid_on_port", lambda port: sleeper.pid)
    with pytest.raises(SystemExit) as exc:
        run_cosim._free_port_or_exit(1337, "SUMO (TraCI)")
    msg = str(exc.value.code)
    assert "no FIXS run owns" in msg and str(sleeper.pid) in msg
    assert "--cleanup --all" in msg
    assert kills == []
    assert sleeper.poll() is None


def test_a_port_held_by_another_live_run_names_that_run(registry, sleeper, kills, monkeypatch):
    other = rr.Run.start(app_id="other")
    try:
        other.add_child("SUMO", sleeper)
        monkeypatch.setattr(run_cosim, "_RUN", rr.Run.start(app_id="mine"))
        monkeypatch.setattr(run_cosim, "_pid_on_port", lambda port: sleeper.pid)
        with pytest.raises(SystemExit) as exc:
            run_cosim._free_port_or_exit(1337, "SUMO (TraCI)")
        msg = str(exc.value.code)
        assert other.id in msg and "still running" in msg
        assert f"--stop {other.id}" in msg
        assert kills == []
    finally:
        run_cosim._RUN.close()
        other.close()


def test_a_port_held_by_this_run_is_freed(registry, sleeper, kills, monkeypatch):
    mine = rr.Run.start(app_id="mine")
    try:
        mine.add_child("CARLA", sleeper)
        monkeypatch.setattr(run_cosim, "_RUN", mine)
        monkeypatch.setattr(run_cosim, "_pid_on_port", lambda port: sleeper.pid)
        monkeypatch.setattr(run_cosim, "_port_listening", lambda port: False)
        run_cosim._free_port_or_exit(2000, "CARLA RPC", carla=True)
        assert kills == [sleeper.pid]
    finally:
        mine.close()


def test_a_free_port_needs_nothing(registry, kills, monkeypatch):
    monkeypatch.setattr(run_cosim, "_pid_on_port", lambda port: None)
    run_cosim._free_port_or_exit(4440, "TrafficLayer (bridge)")
    assert kills == []


def test_cleanup_leaves_unowned_processes_alone(registry, sleeper, kills, monkeypatch, capsys):
    monkeypatch.setattr(run_cosim, "_cosim_leftovers",
                        lambda app_id=None, include_carla=False:
                        [(sleeper.pid, "sumo.exe", "sumo -c x.sumocfg")])
    assert run_cosim.run_cleanup() == 0
    assert kills == []
    out = capsys.readouterr().out
    assert "not owned by any FIXS run" in out and "--cleanup --all" in out


def test_cleanup_all_stops_everything_by_name(registry, sleeper, kills, monkeypatch):
    calls = []

    def leftovers(app_id=None, include_carla=False):
        calls.append(include_carla)
        return [(sleeper.pid, "sumo.exe", "sumo")] if len(calls) == 1 else []
    monkeypatch.setattr(run_cosim, "_cosim_leftovers", leftovers)
    monkeypatch.setattr(run_cosim.time, "sleep", lambda s: None)
    assert run_cosim.run_cleanup(everything=True) == 0
    assert kills == [sleeper.pid]
    assert calls[0] is True                 # CARLA included, as before


def test_cleanup_reaps_a_dead_runs_children(registry, sleeper, monkeypatch):
    monkeypatch.setattr(run_cosim, "_cosim_leftovers", lambda *a, **k: [])
    start = rr.process_start(sleeper.pid)
    path = os.path.join(registry, "t-20260101-000000-abcd")
    os.makedirs(path)
    with open(os.path.join(path, rr.LEASE_FILE), "w", encoding="utf-8") as f:
        f.write('{"id": "t-20260101-000000-abcd", "children": [{"label": "SUMO", '
                f'"pid": {sleeper.pid}, "start": "{start}"}}]}}')
    assert run_cosim.run_cleanup() == 0
    sleeper.wait(timeout=10)
    assert not os.path.exists(path)


def test_begin_run_refuses_a_leftover_of_this_app(registry, sleeper, monkeypatch):
    monkeypatch.setattr(run_cosim, "_cosim_leftovers",
                        lambda app_id=None, include_carla=False:
                        [(sleeper.pid, "python.exe", "python apps/eco/controller.py")])
    try:
        with pytest.raises(SystemExit) as exc:
            run_cosim._begin_run({"id": "eco"}, None, None)
        assert "outside any FIXS run" in str(exc.value.code)
        assert sleeper.poll() is None
    finally:
        if run_cosim._RUN is not None:
            run_cosim._RUN.close()


def test_begin_run_only_notes_other_unowned_processes(registry, sleeper, monkeypatch, capsys):
    monkeypatch.setattr(run_cosim, "_cosim_leftovers",
                        lambda app_id=None, include_carla=False:
                        [(sleeper.pid, "sumo.exe", "sumo -c other.sumocfg")])
    try:
        run_cosim._begin_run({"id": "eco"}, "c.yaml", 2000)
        assert run_cosim._RUN is not None
        assert run_cosim._RUN.lease["ports"] == {"carla": 2000}
        assert "left alone" in capsys.readouterr().out
        assert sleeper.poll() is None
    finally:
        run_cosim._RUN.close()


def test_list_shows_runs_and_who_holds_the_ports(registry, sleeper, monkeypatch, capsys):
    run = rr.Run.start(app_id="eco", ports={"traci": 1337})
    try:
        run.add_child("SUMO", sleeper)
        monkeypatch.setattr(run_cosim, "_pid_on_port",
                            lambda port: sleeper.pid if port == 1337 else None)
        assert run_cosim.run_list() == 0
        out = capsys.readouterr().out
        assert f"LIVE  {run.id}" in out
        assert f"-> run {run.id}" in out
    finally:
        run.close()
