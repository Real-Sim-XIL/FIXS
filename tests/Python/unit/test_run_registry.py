"""run_registry: a run is alive exactly while its OS lock is held, processes are
identified by pid + start time, and only what a dead run's lease names is reaped.

These spawn real (tiny, sleeping) python processes and kill them hard, because
the property under test is what the OS does when a process dies.
"""
import json
import os
import platform
import re
import signal
import subprocess
import sys
import time

import pytest

CARLA = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "Carla"))
sys.path.insert(0, CARLA)
import run_registry as rr  # noqa: E402

WINDOWS = platform.system() == "Windows"
PY = sys.executable
SLEEP = "import time; time.sleep(120)"


def _kill_hard(pid):
    if WINDOWS:
        subprocess.call(["taskkill", "/F", "/PID", str(pid)],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    else:
        os.kill(pid, signal.SIGKILL)


def _gone_within(pid, start, seconds=10.0):
    deadline = time.time() + seconds
    while time.time() < deadline:
        if not rr.same_process(pid, start):
            return True
        time.sleep(0.1)
    return False


@pytest.fixture
def procs():
    """Popen objects to kill at teardown, whatever the test did."""
    started = []
    yield started
    for p in started:
        if p.poll() is None:
            p.kill()
        try:
            p.wait(timeout=10)
        except Exception:
            pass


@pytest.fixture
def registry(tmp_path, monkeypatch):
    root = tmp_path / "runs"
    monkeypatch.setenv(rr.RUNS_DIR_ENV, str(root))
    return str(root)


def _sleeper(procs):
    p = subprocess.Popen([PY, "-c", SLEEP])
    procs.append(p)
    return p


def _owner(procs, root, body=""):
    """A python process that registers a run in `root`, runs `body`, prints the
    run dir and whatever `body` adds to `out`, then sleeps."""
    code = (f"import sys, time, subprocess\n"
            f"sys.path.insert(0, {CARLA!r})\n"
            f"import run_registry as r\n"
            f"run = r.Run.start(app_id='t', root={root!r})\n"
            f"out = [run.dir]\n"
            f"{body}\n"
            f"print(' '.join(str(x) for x in out), flush=True)\n"
            f"time.sleep(120)\n")
    p = subprocess.Popen([PY, "-c", code], stdout=subprocess.PIPE, text=True)
    procs.append(p)
    fields = p.stdout.readline().split()
    assert fields, "owner process did not start"
    return p, fields


def _stale_lease(root, run_id, children):
    path = os.path.join(root, run_id)
    os.makedirs(path)
    lease = {"schema": 1, "id": run_id, "owner": {"pid": 0, "start": None},
             "app": "t", "children": children, "created": rr._now_iso()}
    with open(os.path.join(path, rr.LEASE_FILE), "w", encoding="utf-8") as f:
        json.dump(lease, f)
    open(os.path.join(path, rr.LOCK_FILE), "w").close()
    return path


# --------------------------------------------------------------------------- #
# the lock is the liveness signal
# --------------------------------------------------------------------------- #
def test_run_id_names_the_app_the_time_and_a_nonce():
    rid = rr.new_run_id("mlk eco/driving")
    assert re.fullmatch(r"mlk_eco_driving-\d{8}-\d{6}-[0-9a-f]{4}", rid)
    assert rr.new_run_id(None).startswith("cosim-")


def test_lock_is_free_the_moment_its_owner_is_killed(procs, registry):
    owner, (run_dir,) = _owner(procs, registry)
    lock = os.path.join(run_dir, rr.LOCK_FILE)
    assert rr.lock_state(lock) == "held"
    assert [i.alive for i in rr.scan()] == [True]

    _kill_hard(owner.pid)
    owner.wait(timeout=10)
    # No sleep, no retry: the OS releases the lock as the process dies.
    assert rr.lock_state(lock) == "free"
    taker = rr.FileLock(lock)
    assert taker.acquire()
    taker.release()
    assert [i.alive for i in rr.scan()] == [False]


def test_a_child_cannot_keep_a_dead_run_alive(procs, registry):
    body = ("kid = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(120)'])\n"
            "out.append(kid.pid)")
    owner, (run_dir, kid) = _owner(procs, registry, body)
    kid = int(kid)
    kid_start = rr.process_start(kid)
    try:
        _kill_hard(owner.pid)
        owner.wait(timeout=10)
        assert rr.same_process(kid, kid_start), "the unleashed child should survive"
        assert rr.lock_state(os.path.join(run_dir, rr.LOCK_FILE)) == "free"
    finally:
        _kill_hard(kid)


def test_lock_fd_is_not_inheritable(registry):
    run = rr.Run.start(app_id="t")
    try:
        assert os.get_inheritable(run._lock.fd) is False
    finally:
        run.close()
    assert not os.path.exists(run.dir)


def test_lease_records_owner_ports_and_children(procs, registry):
    run = rr.Run.start(app_id="t", config="c.yaml", ports={"traci": 1337})
    try:
        kid = _sleeper(procs)
        run.add_child("SUMO", kid)
        run.set_ports(bridge=4440)
        run.progress("stack up")
        with open(os.path.join(run.dir, rr.LEASE_FILE), encoding="utf-8") as f:
            lease = json.load(f)
        assert lease["id"] == run.id and lease["app"] == "t"
        assert lease["owner"]["pid"] == os.getpid()
        assert lease["owner"]["start"] == rr.process_start(os.getpid())
        assert lease["ports"] == {"traci": 1337, "bridge": 4440}
        assert lease["children"][0]["pid"] == kid.pid
        assert lease["children"][0]["start"] == rr.process_start(kid.pid)
        assert lease["last_progress"]["what"] == "stack up"
    finally:
        run.close()


# --------------------------------------------------------------------------- #
# identity is pid + start time
# --------------------------------------------------------------------------- #
def test_a_reused_pid_is_not_the_same_process(procs):
    p = _sleeper(procs)
    start = rr.process_start(p.pid)
    assert start is not None
    assert rr.same_process(p.pid, start)
    assert not rr.same_process(p.pid, "0")          # same pid, different process
    assert not rr.same_process(p.pid, None)
    p.kill()
    p.wait(timeout=10)
    assert rr.process_start(p.pid) is None
    assert not rr.same_process(p.pid, start)


def test_stop_tree_refuses_a_pid_whose_start_time_does_not_match(procs):
    p = _sleeper(procs)
    assert rr.stop_tree(p.pid, "0") is False
    time.sleep(0.3)
    assert p.poll() is None


# --------------------------------------------------------------------------- #
# reaping
# --------------------------------------------------------------------------- #
def test_reaper_stops_only_what_a_dead_lease_names(procs, registry):
    leased = _sleeper(procs)
    unleased = _sleeper(procs)
    reused = _sleeper(procs)            # its pid is in the lease, its start time is not
    leased_start = rr.process_start(leased.pid)
    path = _stale_lease(registry, "t-20260101-000000-abcd", [
        {"label": "SUMO", "pid": leased.pid, "start": leased_start},
        {"label": "TrafficLayer", "pid": reused.pid, "start": "0"},
    ])

    reaped = rr.reap_stale(log=None)

    assert reaped == {"t-20260101-000000-abcd": [("SUMO", leased.pid)]}
    assert _gone_within(leased.pid, leased_start)
    assert unleased.poll() is None
    assert reused.poll() is None
    assert not os.path.exists(path)


def test_reaper_leaves_a_live_run_alone(procs, registry):
    run = rr.Run.start(app_id="t")
    try:
        kid = _sleeper(procs)
        run.add_child("SUMO", kid)
        assert rr.reap_stale(log=None) == {}
        assert kid.poll() is None
        assert os.path.isdir(run.dir)
    finally:
        run.close()


def test_reaper_never_touches_a_process_no_lease_names(procs, registry):
    foreign = _sleeper(procs)
    _stale_lease(registry, "t-20260101-000000-0000", [])
    rr.reap_stale(log=None)
    assert foreign.poll() is None
    assert rr.Ownership().of(foreign.pid) is None


def test_ownership_follows_a_leased_child_to_its_descendants(procs, registry):
    run = rr.Run.start(app_id="t")
    try:
        mid = rr.LEASH.popen(
            [PY, "-c", "import subprocess, sys, time\n"
                       "k = subprocess.Popen([sys.executable, '-c', 'import time; time.sleep(120)'])\n"
                       "print(k.pid, flush=True)\n"
                       "time.sleep(120)"],
            stdout=subprocess.PIPE, text=True)
        procs.append(mid)
        run.add_child("app t", mid)
        grand = int(mid.stdout.readline())
        try:
            hit = rr.Ownership().of(grand)
            assert hit is not None and hit[0].id == run.id
            assert hit[1]["pid"] == mid.pid
            assert rr.Ownership().of(_sleeper(procs).pid) is None
        finally:
            _kill_hard(grand)
    finally:
        run.close()


def test_stop_run_stops_the_owner_and_what_it_started(procs, registry):
    body = ("kid = r.LEASH.popen([sys.executable, '-c', 'import time; time.sleep(120)'])\n"
            "run.add_child('SUMO', kid)\n"
            "out.append(kid.pid)")
    owner, (run_dir, kid) = _owner(procs, registry, body)
    kid = int(kid)
    kid_start = rr.process_start(kid)
    rc = rr.stop_run(os.path.basename(run_dir), log=lambda *_: None, grace=10.0)
    assert rc == 0
    owner.wait(timeout=10)
    assert _gone_within(kid, kid_start)
    assert not os.path.exists(run_dir)


def test_stop_run_names_an_unknown_run(registry):
    said = []
    assert rr.stop_run("nope", log=said.append) == 1
    assert "no run 'nope'" in said[0]


# --------------------------------------------------------------------------- #
# the leash
# --------------------------------------------------------------------------- #
def test_leash_call_returns_the_exit_code():
    assert rr.Leash().call([PY, "-c", "import sys; sys.exit(3)"]) == 3


@pytest.mark.skipif(not WINDOWS, reason="Job Objects are Windows")
def test_job_kills_the_whole_tree_when_its_owner_dies(procs, registry):
    body = ("mid = r.LEASH.popen([sys.executable, '-c', "
            "'import subprocess, sys, time\\n"
            "k = subprocess.Popen([sys.executable, \"-c\", \"import time; time.sleep(120)\"])\\n"
            "print(k.pid, flush=True)\\n"
            "time.sleep(120)'], stdout=subprocess.PIPE, text=True)\n"
            "out.append(mid.pid)\n"
            "out.append(int(mid.stdout.readline()))")
    owner, (_run_dir, mid, grand) = _owner(procs, registry, body)
    mid, grand = int(mid), int(grand)
    mid_start, grand_start = rr.process_start(mid), rr.process_start(grand)
    assert mid_start and grand_start
    try:
        # /F without /T: only the owner is killed; the job must take the rest.
        _kill_hard(owner.pid)
        owner.wait(timeout=10)
        assert _gone_within(mid, mid_start, 5.0)
        assert _gone_within(grand, grand_start, 5.0)
    finally:
        rr.stop_tree(grand, grand_start)
        rr.stop_tree(mid, mid_start)


@pytest.mark.skipif(not WINDOWS, reason="Job Objects are Windows")
@pytest.mark.parametrize("victim", ["outer", "inner"])
def test_a_leashed_reexec_dies_with_either_process(victim):
    """The configured-python re-exec: outer leashes inner, inner leashes the stack."""
    inner = (f"import sys, time\nsys.path.insert(0, {CARLA!r})\nimport run_registry as r\n"
             "k = r.LEASH.popen([sys.executable, '-c', 'import time; time.sleep(120)'])\n"
             "print(k.pid, flush=True)\ntime.sleep(120)")
    outer = (f"import sys\nsys.path.insert(0, {CARLA!r})\nimport run_registry as r\n"
             f"sys.exit(r.LEASH.call([sys.executable, '-c', {inner!r}]))")
    o = subprocess.Popen([PY, "-c", outer], stdout=subprocess.PIPE, text=True)
    try:
        kid = int(o.stdout.readline())
        kid_start = rr.process_start(kid)
        inner_pid = rr._win_snapshot()[kid][0]
        inner_start = rr.process_start(inner_pid)
        _kill_hard(o.pid if victim == "outer" else inner_pid)
        assert _gone_within(inner_pid, inner_start, 5.0)
        assert _gone_within(kid, kid_start, 5.0)
        o.wait(timeout=10)
    finally:
        if o.poll() is None:
            o.kill()


@pytest.mark.skipif(platform.system() != "Linux", reason="PR_SET_PDEATHSIG is Linux")
def test_parent_death_signal_stops_a_child_when_its_owner_dies(procs, registry):
    body = ("kid = r.LEASH.popen([sys.executable, '-c', 'import time; time.sleep(120)'])\n"
            "out.append(kid.pid)")
    owner, (_run_dir, kid) = _owner(procs, registry, body)
    kid = int(kid)
    kid_start = rr.process_start(kid)
    try:
        _kill_hard(owner.pid)
        owner.wait(timeout=10)
        assert _gone_within(kid, kid_start, 5.0)
    finally:
        rr.stop_tree(kid, kid_start)


def test_leashed_child_stays_alive_while_its_owner_does(procs):
    p = rr.Leash().popen([PY, "-c", SLEEP])
    procs.append(p)
    time.sleep(0.5)
    assert p.poll() is None
