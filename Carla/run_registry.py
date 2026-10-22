"""run_registry.py - the co-sim runs on this machine and what each one owns.

Each run has a dir holding lease.json and lease.lock; the run is alive exactly while
its OS lock on lease.lock is held. Processes are identified by pid + start time, and
only children named in a dead run's lease are ever stopped.
"""
import atexit
import datetime
import getpass
import json
import os
import platform
import re
import secrets
import shutil
import signal
import socket
import subprocess
import sys
import time

WINDOWS = platform.system() == "Windows"
LINUX = platform.system() == "Linux"

RUNS_DIR_ENV = "FIXS_RUNS_DIR"
LEASE_FILE = "lease.json"
LOCK_FILE = "lease.lock"
SCHEMA = 1
# A run dir with no lease.json yet is still being created, unless it is older than this.
_UNBORN_GRACE_S = 30.0


def _now_iso():
    return datetime.datetime.now().astimezone().isoformat(timespec="seconds")


# --------------------------------------------------------------------------- #
# where the registry lives
# --------------------------------------------------------------------------- #
def machine_runs_dir():
    """The machine-wide registry: one place every user and every app repo sees."""
    if WINDOWS:
        return os.path.join(os.environ.get("ProgramData") or r"C:\ProgramData",
                            "FIXS", "runs")
    return "/var/tmp/fixs/runs"


def user_runs_dir():
    """Where a run registers when the machine-wide dir cannot be written."""
    return os.path.join(os.path.expanduser("~"), ".fixs", "runs")


def registry_dirs():
    """Every registry dir to read, in order. FIXS_RUNS_DIR replaces them all."""
    override = os.environ.get(RUNS_DIR_ENV)
    if override:
        return [override]
    return [machine_runs_dir(), user_runs_dir()]


def _makedirs_shared(path):
    """mkdir -p, leaving dirs this call creates writable by every user on POSIX."""
    missing = []
    p = os.path.abspath(path)
    while not os.path.isdir(p):
        missing.append(p)
        parent = os.path.dirname(p)
        if parent == p:
            break
        p = parent
    os.makedirs(path, exist_ok=True)
    if not WINDOWS and path.startswith("/var/tmp"):
        for d in missing:
            try:
                os.chmod(d, 0o1777)          # sticky + world-writable, like /tmp
            except OSError:
                pass


def _writable_registry():
    """(dir, fell_back) for a new run, or (None, False) if nothing can be written."""
    dirs = registry_dirs()
    for i, d in enumerate(dirs):
        try:
            _makedirs_shared(d)
            probe = os.path.join(d, f".probe-{os.getpid()}-{secrets.token_hex(2)}")
            with open(probe, "w"):
                pass
            os.remove(probe)
            return d, i > 0
        except OSError:
            continue
    return None, False


def new_run_id(app_id=None, now=None):
    """<app>-<yyyymmdd-hhmmss>-<4hex>."""
    slug = re.sub(r"[^A-Za-z0-9_.-]+", "_", (app_id or "cosim")).strip("_.") or "cosim"
    stamp = (now or datetime.datetime.now()).strftime("%Y%m%d-%H%M%S")
    return f"{slug}-{stamp}-{secrets.token_hex(2)}"


# --------------------------------------------------------------------------- #
# Win32 bindings
# --------------------------------------------------------------------------- #
if WINDOWS:
    import ctypes
    import msvcrt
    from ctypes import wintypes

    _k32 = ctypes.WinDLL("kernel32", use_last_error=True)
    _ntdll = ctypes.WinDLL("ntdll")

    _PROCESS_TERMINATE = 0x0001
    _PROCESS_QUERY_LIMITED = 0x1000
    _SYNCHRONIZE = 0x00100000
    _STILL_ACTIVE = 259
    _WAIT_OBJECT_0 = 0
    _WAIT_TIMEOUT = 0x102
    _CREATE_SUSPENDED = 0x00000004
    _JOB_KILL_ON_CLOSE = 0x00002000
    _JobObjectExtendedLimitInformation = 9
    _TH32CS_SNAPPROCESS = 0x00000002
    _INVALID_HANDLE = ctypes.c_void_p(-1).value

    class _IO_COUNTERS(ctypes.Structure):
        _fields_ = [(n, ctypes.c_ulonglong) for n in (
            "ReadOperationCount", "WriteOperationCount", "OtherOperationCount",
            "ReadTransferCount", "WriteTransferCount", "OtherTransferCount")]

    class _JOB_BASIC_LIMITS(ctypes.Structure):
        _fields_ = [("PerProcessUserTimeLimit", ctypes.c_int64),
                    ("PerJobUserTimeLimit", ctypes.c_int64),
                    ("LimitFlags", wintypes.DWORD),
                    ("MinimumWorkingSetSize", ctypes.c_size_t),
                    ("MaximumWorkingSetSize", ctypes.c_size_t),
                    ("ActiveProcessLimit", wintypes.DWORD),
                    ("Affinity", ctypes.c_size_t),
                    ("PriorityClass", wintypes.DWORD),
                    ("SchedulingClass", wintypes.DWORD)]

    class _JOB_EXTENDED_LIMITS(ctypes.Structure):
        _fields_ = [("BasicLimitInformation", _JOB_BASIC_LIMITS),
                    ("IoInfo", _IO_COUNTERS),
                    ("ProcessMemoryLimit", ctypes.c_size_t),
                    ("JobMemoryLimit", ctypes.c_size_t),
                    ("PeakProcessMemoryUsed", ctypes.c_size_t),
                    ("PeakJobMemoryUsed", ctypes.c_size_t)]

    class _PROCESSENTRY32W(ctypes.Structure):
        _fields_ = [("dwSize", wintypes.DWORD),
                    ("cntUsage", wintypes.DWORD),
                    ("th32ProcessID", wintypes.DWORD),
                    ("th32DefaultHeapID", ctypes.c_size_t),
                    ("th32ModuleID", wintypes.DWORD),
                    ("cntThreads", wintypes.DWORD),
                    ("th32ParentProcessID", wintypes.DWORD),
                    ("pcPriClassBase", ctypes.c_long),
                    ("dwFlags", wintypes.DWORD),
                    ("szExeFile", ctypes.c_wchar * 260)]

    _k32.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
    _k32.OpenProcess.restype = wintypes.HANDLE
    _k32.CloseHandle.argtypes = [wintypes.HANDLE]
    _k32.CloseHandle.restype = wintypes.BOOL
    _k32.GetProcessTimes.argtypes = [wintypes.HANDLE] + [ctypes.POINTER(ctypes.c_ulonglong)] * 4
    _k32.GetProcessTimes.restype = wintypes.BOOL
    _k32.GetExitCodeProcess.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
    _k32.GetExitCodeProcess.restype = wintypes.BOOL
    _k32.WaitForSingleObject.argtypes = [wintypes.HANDLE, wintypes.DWORD]
    _k32.WaitForSingleObject.restype = wintypes.DWORD
    _k32.TerminateProcess.argtypes = [wintypes.HANDLE, wintypes.UINT]
    _k32.TerminateProcess.restype = wintypes.BOOL
    _k32.CreateJobObjectW.argtypes = [ctypes.c_void_p, wintypes.LPCWSTR]
    _k32.CreateJobObjectW.restype = wintypes.HANDLE
    _k32.SetInformationJobObject.argtypes = [wintypes.HANDLE, ctypes.c_int,
                                             ctypes.c_void_p, wintypes.DWORD]
    _k32.SetInformationJobObject.restype = wintypes.BOOL
    _k32.AssignProcessToJobObject.argtypes = [wintypes.HANDLE, wintypes.HANDLE]
    _k32.AssignProcessToJobObject.restype = wintypes.BOOL
    _k32.CreateToolhelp32Snapshot.argtypes = [wintypes.DWORD, wintypes.DWORD]
    _k32.CreateToolhelp32Snapshot.restype = wintypes.HANDLE
    _k32.Process32FirstW.argtypes = [wintypes.HANDLE, ctypes.POINTER(_PROCESSENTRY32W)]
    _k32.Process32FirstW.restype = wintypes.BOOL
    _k32.Process32NextW.argtypes = [wintypes.HANDLE, ctypes.POINTER(_PROCESSENTRY32W)]
    _k32.Process32NextW.restype = wintypes.BOOL
    _ntdll.NtResumeProcess.argtypes = [wintypes.HANDLE]
    _ntdll.NtResumeProcess.restype = ctypes.c_long

    def _open_process(pid, access):
        """(handle, can_wait) for a pid, or (None, False)."""
        if not pid or pid <= 4:
            return None, False
        h = _k32.OpenProcess(access | _SYNCHRONIZE, False, int(pid))
        if h:
            return h, True
        h = _k32.OpenProcess(access, False, int(pid))
        return (h, False) if h else (None, False)

    def _handle_running(h, can_wait):
        if can_wait:
            return _k32.WaitForSingleObject(h, 0) == _WAIT_TIMEOUT
        code = wintypes.DWORD()
        return bool(_k32.GetExitCodeProcess(h, ctypes.byref(code))) \
            and code.value == _STILL_ACTIVE

    def _handle_start(h):
        times = [ctypes.c_ulonglong() for _ in range(4)]
        if not _k32.GetProcessTimes(h, *[ctypes.byref(t) for t in times]):
            return None
        return str(times[0].value)

    def _win_snapshot():
        """{pid: (ppid, exe name)} for every process, from one Toolhelp snapshot."""
        out = {}
        snap = _k32.CreateToolhelp32Snapshot(_TH32CS_SNAPPROCESS, 0)
        if not snap or snap == _INVALID_HANDLE:
            return out
        try:
            entry = _PROCESSENTRY32W()
            entry.dwSize = ctypes.sizeof(_PROCESSENTRY32W)
            ok = _k32.Process32FirstW(snap, ctypes.byref(entry))
            while ok:
                out[int(entry.th32ProcessID)] = (int(entry.th32ParentProcessID),
                                                 entry.szExeFile)
                ok = _k32.Process32NextW(snap, ctypes.byref(entry))
        finally:
            _k32.CloseHandle(snap)
        return out

elif LINUX:
    import ctypes
    import fcntl
    try:
        _prctl = ctypes.CDLL(None, use_errno=True).prctl
    except (OSError, AttributeError):
        _prctl = None
    _PR_SET_PDEATHSIG = 1
else:                                                   # pragma: no cover - macOS
    import fcntl
    _prctl = None


# --------------------------------------------------------------------------- #
# process identity: pid + start time, always
# --------------------------------------------------------------------------- #
_BOOT_ID = None


def _boot_id():
    global _BOOT_ID
    if _BOOT_ID is None:
        try:
            with open("/proc/sys/kernel/random/boot_id", encoding="ascii") as f:
                _BOOT_ID = f.read().strip()
        except OSError:
            _BOOT_ID = ""
    return _BOOT_ID


def _proc_stat(pid):
    """/proc/<pid>/stat fields after the command name, or None."""
    try:
        with open(f"/proc/{int(pid)}/stat", "rb") as f:
            data = f.read().decode("utf-8", "replace")
    except (OSError, ValueError):
        return None
    return data[data.rfind(")") + 2:].split()


def process_start(pid):
    """An opaque start-time token for a RUNNING pid, or None (gone, zombie, unreadable).

    Two tokens are equal only for the same process: a reused pid gets a new one."""
    if not pid:
        return None
    if WINDOWS:
        h, can_wait = _open_process(pid, _PROCESS_QUERY_LIMITED)
        if not h:
            return None
        try:
            return _handle_start(h) if _handle_running(h, can_wait) else None
        finally:
            _k32.CloseHandle(h)
    if LINUX:
        st = _proc_stat(pid)
        if not st or len(st) < 20 or st[0] in ("Z", "X"):
            return None
        return f"{_boot_id()}:{st[19]}"
    try:                                                # pragma: no cover - macOS
        out = subprocess.check_output(["ps", "-o", "lstart=", "-p", str(int(pid))],
                                      text=True, stderr=subprocess.DEVNULL).strip()
        return out or None
    except Exception:                                   # noqa: BLE001
        return None


def _start_order(token):
    """A sortable number from a start token, or None (for ancestry checks only)."""
    try:
        return int(str(token).rsplit(":", 1)[-1])
    except (TypeError, ValueError):
        return None


def same_process(pid, start):
    """True only if `pid` is running AND is the process that had `start`."""
    return start is not None and process_start(pid) == start


def process_name(pid, _snap=None):
    if WINDOWS:
        snap = _snap if _snap is not None else _win_snapshot()
        return (snap.get(int(pid)) or (None, ""))[1]
    try:
        with open(f"/proc/{int(pid)}/comm", encoding="utf-8", errors="replace") as f:
            return f.read().strip()
    except (OSError, ValueError):
        return ""


def _parent_of(pid, snap=None):
    if WINDOWS:
        return (snap or {}).get(int(pid), (None, ""))[0]
    st = _proc_stat(pid)
    return int(st[1]) if st and len(st) > 1 else None


def _pgid_of(pid):
    if WINDOWS:
        return None
    try:
        return os.getpgid(int(pid))
    except (OSError, ValueError):
        return None


def ancestry(pid, snap=None, depth=32):
    """[pid, parent, grandparent, ...] while each parent provably predates its child.

    A parent pid that started AFTER its child is a reused pid, not the parent."""
    if WINDOWS and snap is None:
        snap = _win_snapshot()
    chain = [int(pid)]
    cur_start = _start_order(process_start(pid))
    while len(chain) < depth:
        ppid = _parent_of(chain[-1], snap)
        if not ppid or ppid in chain:
            break
        p_start = _start_order(process_start(ppid))
        if p_start is None or cur_start is None or p_start > cur_start:
            break
        chain.append(ppid)
        cur_start = p_start
    return chain


def _descendants(pid, start, snap=None):
    """[(pid, start)] of every live descendant of a verified process, deepest last."""
    if WINDOWS:
        snap = snap if snap is not None else _win_snapshot()
        children = {}
        for p, (pp, _n) in snap.items():
            children.setdefault(pp, []).append(p)
    else:
        children = {}
        for name in os.listdir("/proc") if os.path.isdir("/proc") else ():
            if name.isdigit():
                st = _proc_stat(name)
                if st and len(st) > 1:
                    children.setdefault(int(st[1]), []).append(int(name))
    out, frontier = [], [(int(pid), _start_order(start))]
    seen = {int(pid)}
    while frontier:
        parent, p_order = frontier.pop()
        for c in children.get(parent, ()):
            if c in seen:
                continue
            c_start = process_start(c)
            c_order = _start_order(c_start)
            if c_start is None or p_order is None or c_order is None or c_order < p_order:
                continue
            seen.add(c)
            out.append((c, c_start))
            frontier.append((c, c_order))
    return out


# --------------------------------------------------------------------------- #
# stopping a verified process
# --------------------------------------------------------------------------- #
def _wait_gone(pid, start, timeout):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if not same_process(pid, start):
            return True
        time.sleep(0.1)
    return not same_process(pid, start)


def _terminate_one(pid, start, grace=5.0):
    """Stop `pid` only if it is still the process that had `start`. True if it stopped."""
    if WINDOWS:
        h, can_wait = _open_process(pid, _PROCESS_TERMINATE | _PROCESS_QUERY_LIMITED)
        if not h:
            return False
        try:
            # The open handle pins the process object, so the pid cannot be reused
            # between this identity check and the terminate.
            if not _handle_running(h, can_wait) or _handle_start(h) != start:
                return False
            _k32.TerminateProcess(h, 1)
            if can_wait:
                _k32.WaitForSingleObject(h, int(grace * 1000))
            return True
        finally:
            _k32.CloseHandle(h)
    if not same_process(pid, start):
        return False
    try:
        os.kill(int(pid), signal.SIGTERM)
    except OSError:
        return True
    if not _wait_gone(pid, start, grace):
        try:
            os.kill(int(pid), signal.SIGKILL)
        except OSError:
            pass
        _wait_gone(pid, start, 2.0)
    return True


def _group_is_ours(pgid, leader_pid, leader_start):
    """A recorded process group may be signalled only if it cannot be someone else's."""
    if not pgid or pgid != leader_pid:
        return False
    if pgid in (os.getpgrp(), 0, 1):
        return False
    now = process_start(leader_pid)
    # A pid is not reused while its group exists, so a gone leader leaves only our group.
    return now is None or now == leader_start


def stop_tree(pid, start, pgid=None, grace=5.0):
    """Stop a recorded process and what it started. Never touches a reused pid."""
    if not WINDOWS and pgid and _group_is_ours(pgid, pid, start):
        try:
            os.killpg(int(pgid), signal.SIGTERM)
        except OSError:
            return False
        deadline = time.time() + grace
        while time.time() < deadline:
            try:
                os.killpg(int(pgid), 0)
            except OSError:
                return True
            time.sleep(0.1)
        try:
            os.killpg(int(pgid), signal.SIGKILL)
        except OSError:
            pass
        return True
    if not same_process(pid, start):
        return False
    kids = _descendants(pid, start)
    stopped = _terminate_one(pid, start, grace)
    for c, c_start in reversed(kids):
        _terminate_one(c, c_start, grace=1.0)
    return stopped


# --------------------------------------------------------------------------- #
# the OS-held lock
# --------------------------------------------------------------------------- #
class FileLock:
    """An exclusive, non-blocking OS lock on byte 0 of a file, held via an fd.

    The fd is non-inheritable (PEP 446), so no child can keep a dead run alive."""

    def __init__(self, path):
        self.path = path
        self.fd = None

    def acquire(self, retries=0, wait=0.05):
        for attempt in range(retries + 1):
            try:
                fd = os.open(self.path, os.O_RDWR | os.O_CREAT, 0o666)
            except OSError:
                try:
                    fd = os.open(self.path, os.O_RDONLY)
                except OSError:
                    return False
            try:
                if not WINDOWS:
                    try:
                        os.fchmod(fd, 0o666)
                    except OSError:
                        pass
                _lock_fd(fd)
            except OSError:
                os.close(fd)
                if attempt < retries:
                    time.sleep(wait)
                    continue
                return False
            self.fd = fd
            return True
        return False

    def release(self):
        if self.fd is None:
            return
        try:
            _unlock_fd(self.fd)
        except OSError:
            pass
        os.close(self.fd)
        self.fd = None


def _lock_fd(fd):
    if WINDOWS:
        os.lseek(fd, 0, os.SEEK_SET)
        msvcrt.locking(fd, msvcrt.LK_NBLCK, 1)
    else:
        fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)


def _unlock_fd(fd):
    if WINDOWS:
        os.lseek(fd, 0, os.SEEK_SET)
        msvcrt.locking(fd, msvcrt.LK_UNLCK, 1)
    else:
        fcntl.flock(fd, fcntl.LOCK_UN)


def lock_state(path):
    """'held' (its run is alive), 'free' (its run is dead) or 'unknown' (cannot tell)."""
    if not os.path.exists(path):
        return "free"
    probe = FileLock(path)
    try:
        fd = os.open(path, os.O_RDONLY)
    except OSError:
        return "unknown"
    try:
        _lock_fd(fd)
    except OSError:
        os.close(fd)
        return "held"
    probe.fd = fd
    probe.release()
    return "free"


# --------------------------------------------------------------------------- #
# tying children to this process
# --------------------------------------------------------------------------- #
class Leash:
    """Starts children that die with this process: a kill-on-close Job Object on
    Windows (children are assigned while suspended), PR_SET_PDEATHSIG on Linux."""

    def __init__(self, warn=None):
        self._job = None
        self._job_failed = False
        self._warned = False
        self._warn = warn or (lambda msg: print(msg, flush=True))

    def _job_handle(self):
        # Never closed: the kernel closes it when this process dies, which kills the job.
        if self._job is not None or self._job_failed:
            return self._job
        h = _k32.CreateJobObjectW(None, None)
        if h:
            info = _JOB_EXTENDED_LIMITS()
            info.BasicLimitInformation.LimitFlags = _JOB_KILL_ON_CLOSE
            if _k32.SetInformationJobObject(h, _JobObjectExtendedLimitInformation,
                                            ctypes.byref(info), ctypes.sizeof(info)):
                self._job = h
                return h
            _k32.CloseHandle(h)
        self._job_failed = True
        self._say(f"[fixs] could not create a Job Object (error {ctypes.get_last_error()}); "
                  f"children may outlive this run if it is killed.")
        return None

    def _say(self, msg):
        if not self._warned:
            self._warned = True
            self._warn(msg)

    def popen(self, cmd, new_session=False, **kw):
        if WINDOWS:
            job = self._job_handle()
            if job is None:
                return subprocess.Popen(cmd, **kw)
            kw["creationflags"] = kw.get("creationflags", 0) | _CREATE_SUSPENDED
            proc = subprocess.Popen(cmd, **kw)
            h = int(proc._handle)
            try:
                if not _k32.AssignProcessToJobObject(job, h):
                    self._say(f"[fixs] could not tie pid {proc.pid} to this run (error "
                              f"{ctypes.get_last_error()}); it may outlive this run if "
                              f"the run is killed.")
            finally:
                if _ntdll.NtResumeProcess(h) != 0:
                    proc.kill()
                    raise OSError(f"could not resume {cmd[0]!r} after creating it")
            return proc
        parent = os.getpid()
        user_preexec = kw.pop("preexec_fn", None)

        def _preexec():
            if new_session:
                os.setsid()
            if _prctl is not None:
                _prctl(_PR_SET_PDEATHSIG, signal.SIGTERM, 0, 0, 0)
            if os.getppid() != parent:          # the parent died before prctl landed
                os.kill(os.getpid(), signal.SIGTERM)
            if user_preexec is not None:
                user_preexec()
        return subprocess.Popen(cmd, preexec_fn=_preexec, **kw)

    def call(self, cmd, **kw):
        """subprocess.call, leashed."""
        with self.popen(cmd, **kw) as p:
            try:
                return p.wait()
            except BaseException:
                p.kill()
                raise


LEASH = Leash()


# --------------------------------------------------------------------------- #
# a run and its lease
# --------------------------------------------------------------------------- #
def _write_json_atomic(path, data):
    tmp = f"{path}.{os.getpid()}.tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2)
    for _ in range(20):
        try:
            os.replace(tmp, path)
            return
        except PermissionError:
            time.sleep(0.05)                    # a reader has it open (Windows)
    os.replace(tmp, path)


def _read_json(path):
    try:
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    except (OSError, ValueError):
        return None


def _user():
    try:
        return getpass.getuser()
    except Exception:                                   # noqa: BLE001
        return os.environ.get("USERNAME") or os.environ.get("USER") or "?"


class Run:
    """This process's run: its dir, its held lock and its lease."""

    def __init__(self, run_id, path, lock, lease):
        self.id, self.dir, self._lock, self.lease = run_id, path, lock, lease
        self.fell_back = False

    @classmethod
    def start(cls, app_id=None, config=None, ports=None, root=None):
        """Register a new run and hold its lock. None if no registry is writable."""
        fell_back = False
        if root is None:
            root, fell_back = _writable_registry()
            if root is None:
                return None
        else:
            _makedirs_shared(root)
        for _ in range(8):
            run_id = new_run_id(app_id)
            path = os.path.join(root, run_id)
            try:
                os.makedirs(path)
            except FileExistsError:
                continue
            lock = FileLock(os.path.join(path, LOCK_FILE))
            if lock.acquire(retries=40):
                break
            shutil.rmtree(path, ignore_errors=True)
        else:
            return None
        me = os.getpid()
        lease = {
            "schema": SCHEMA,
            "id": run_id,
            "host": socket.gethostname(),
            "user": _user(),
            "owner": {"pid": me, "start": process_start(me), "pgid": _pgid_of(me),
                      "argv": list(sys.argv)},
            "app": app_id,
            "config": config,
            "ports": dict(ports or {}),
            "children": [],
            "created": _now_iso(),
            "last_progress": {"at": _now_iso(), "what": "starting"},
        }
        run = cls(run_id, path, lock, lease)
        run.fell_back = fell_back
        run._write()
        atexit.register(run.close)
        return run

    def _write(self):
        if self._lock is None:
            return
        try:
            _write_json_atomic(os.path.join(self.dir, LEASE_FILE), self.lease)
        except OSError:
            pass

    def add_child(self, label, proc_or_pid):
        pid = getattr(proc_or_pid, "pid", proc_or_pid)
        self.lease["children"] = [c for c in self.lease["children"]
                                  if same_process(c["pid"], c.get("start"))]
        self.lease["children"].append({"label": label, "pid": pid,
                                       "start": process_start(pid),
                                       "pgid": _pgid_of(pid),
                                       "since": _now_iso()})
        self._write()

    def set_ports(self, **ports):
        self.lease["ports"].update({k: v for k, v in ports.items() if v is not None})
        self._write()

    def update(self, **fields):
        self.lease.update({k: v for k, v in fields.items() if v is not None})
        self._write()

    def progress(self, what):
        self.lease["last_progress"] = {"at": _now_iso(), "what": what}
        self._write()

    def close(self):
        """Unregister. Children are the caller's to have stopped by now."""
        if self._lock is None:
            return
        try:
            os.remove(os.path.join(self.dir, LEASE_FILE))
        except OSError:
            pass
        self._lock.release()
        self._lock = None
        shutil.rmtree(self.dir, ignore_errors=True)


class RunInfo:
    """A run dir as seen from outside: its lease (may be None) and whether it lives."""

    def __init__(self, path, lease, state):
        self.dir = path
        self.id = os.path.basename(path)
        self.lease = lease or {}
        self.state = state

    @property
    def alive(self):
        return self.state != "free"

    def age_s(self):
        try:
            return max(0.0, time.time() - os.path.getmtime(self.dir))
        except OSError:
            return 0.0

    def created_age_s(self):
        try:
            created = datetime.datetime.fromisoformat(self.lease["created"])
            return max(0.0, (datetime.datetime.now().astimezone() - created).total_seconds())
        except (KeyError, TypeError, ValueError):
            return self.age_s()


def scan():
    """[RunInfo] for every run dir in every registry dir."""
    out = []
    for root in registry_dirs():
        try:
            names = sorted(os.listdir(root))
        except OSError:
            continue
        for name in names:
            path = os.path.join(root, name)
            if name.startswith(".") or not os.path.isdir(path):
                continue
            out.append(RunInfo(path, _read_json(os.path.join(path, LEASE_FILE)),
                               lock_state(os.path.join(path, LOCK_FILE))))
    return out


def find(run_id):
    """The run with this id, or the only one it is a prefix of, or None."""
    runs = scan()
    exact = [r for r in runs if r.id == run_id]
    if exact:
        return exact[0]
    pref = [r for r in runs if r.id.startswith(run_id)]
    return pref[0] if len(pref) == 1 else None


def _reap_one(info, log):
    """Stop what a dead run recorded, then remove its dir. [(label, pid)] stopped."""
    lock = FileLock(os.path.join(info.dir, LOCK_FILE))
    if not lock.acquire():
        return None                             # alive after all, or another reaper has it
    stopped = []
    try:
        if not info.lease and info.age_s() < _UNBORN_GRACE_S:
            return None                         # still being created
        for child in info.lease.get("children", []):
            pid, start = child.get("pid"), child.get("start")
            if pid and stop_tree(pid, start, child.get("pgid")):
                stopped.append((child.get("label") or "?", pid))
    finally:
        lock.release()
    shutil.rmtree(info.dir, ignore_errors=True)
    if log is not None:
        what = ", ".join(f"{l} (pid {p})" for l, p in stopped) or "nothing left running"
        log(f"[cosim] run {info.id} ended without cleaning up; stopped: {what}.")
    return stopped


def reap_stale(log=print, exclude=()):
    """Reap every run whose lock is free. Returns {run_id: [(label, pid)]}."""
    out = {}
    for info in scan():
        if info.alive or info.id in exclude:
            continue
        stopped = _reap_one(info, log)
        if stopped is not None:
            out[info.id] = stopped
    return out


class Ownership:
    """Which run, if any, owns a pid: the pid, or one of its ancestors, is a lease child."""

    def __init__(self, runs=None):
        self.runs = scan() if runs is None else runs
        self._snap = _win_snapshot() if WINDOWS else None

    def of(self, pid):
        """(RunInfo, child dict) of the run owning `pid`, or None."""
        if not pid:
            return None
        chain = ancestry(pid, self._snap)
        starts = {p: process_start(p) for p in chain}
        pgid = _pgid_of(pid)
        for info in self.runs:
            for child in info.lease.get("children", []):
                cpid, cstart = child.get("pid"), child.get("start")
                if cpid in starts and cstart is not None and starts[cpid] == cstart:
                    return info, child
                if pgid and child.get("pgid") == pgid and _group_is_ours(pgid, cpid, cstart):
                    return info, child
        return None

    def name(self, pid):
        return process_name(pid, self._snap)


def stop_run(run_id, log=print, grace=15.0):
    """Stop one run by its lease: its run_cosim first, then whatever it recorded."""
    info = find(run_id)
    if info is None:
        log(f"[cosim] no run '{run_id}' on this machine (run_cosim --list shows them).")
        return 1
    if info.alive:
        owner = info.lease.get("owner") or {}
        opid, ostart = owner.get("pid"), owner.get("start")
        if opid and same_process(opid, ostart):
            log(f"[cosim] stopping run {info.id} (run_cosim pid {opid}) ...")
            stop_tree(opid, ostart, grace=grace)
        deadline = time.time() + grace
        while time.time() < deadline and lock_state(os.path.join(info.dir, LOCK_FILE)) != "free":
            time.sleep(0.2)
        if lock_state(os.path.join(info.dir, LOCK_FILE)) != "free":
            log(f"[cosim] run {info.id} is still running; could not stop it.")
            return 1
    _reap_one(RunInfo(info.dir, info.lease, "free"), log=None)
    log(f"[cosim] run {info.id} stopped.")
    return 0
