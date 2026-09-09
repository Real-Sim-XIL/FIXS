"""
doctor.py - check that this machine can actually run a co-sim, before it tries.

Every check here exists because the thing it tests failed once and reported
itself as something else. That is the whole point: these are all cheap, and all
of them are questions people ended up answering by bisecting a failed run.

  pyyaml missing        every scenario-yaml setting silently read as its default,
                        including CarlaServerIP -> localhost, so a correctly
                        configured distributed run refused to start and blamed a
                        yaml that was right all along
  pandas/shapely        "could not generate TL table; TL sync off" - the run
                        works, traffic lights never change
  gh not authenticated  the Digital-Twin-Library is private, so a map fetch fails
                        several minutes into a run
  port closed           a silent multi-minute block that looks like a hang

Two rules the checks follow:

  * test the interpreter named in ~/.fixs/carla.json, NOT the ambient one.
    run_cosim re-execs into that env; checking whatever python happens to be on
    PATH is how a missing pyyaml stayed hidden while `import yaml` worked fine at
    a prompt.
  * be role-aware. A render host needs Unreal and cooked maps; a traffic host
    needs SUMO and the FIXS binaries. Reporting the other machine's requirements
    as failures teaches people to ignore the output.
"""
import json
import os
import platform
import shutil
import socket
import subprocess
import sys

OK, WARN, FAIL = "OK", "WARN", "FAIL"


class Report:
    """Collected findings, printed as sections. Exit code comes from the worst."""

    def __init__(self):
        self.rows = []          # (section, label, status, detail)

    def add(self, section, label, status, detail=""):
        self.rows.append((section, label, status, detail))
        return status

    @property
    def failed(self):
        return any(s == FAIL for _sec, _l, s, _d in self.rows)

    def show(self):
        width = max((len(l) for _s, l, _st, _d in self.rows), default=10)
        section = None
        for sec, label, status, detail in self.rows:
            if sec != section:
                print(f"\n{sec}")
                section = sec
            mark = {OK: "OK  ", WARN: "WARN", FAIL: "FAIL"}[status]
            print(f"  {label:<{width}}  {mark}  {detail}")
        worst = FAIL if self.failed else (
            WARN if any(s == WARN for _a, _b, s, _c in self.rows) else OK)
        print(f"\n[doctor] {'problems found' if worst == FAIL else 'usable' if worst == OK else 'usable, with warnings'}.")
        return 1 if worst == FAIL else 0


def _run(cmd, timeout=10):
    try:
        out = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        return out.returncode, (out.stdout or "") + (out.stderr or "")
    except Exception as e:
        return 1, str(e)


def detect_role(cfg, fixs_root):
    """Which half of a distributed co-sim this machine is, from what is ON it.

    Inferring it from the flags of the doctor invocation was wrong: nobody passes
    --serve to --doctor, so a render host reported itself as a traffic host and
    was then told off for lacking SUMO and the FIXS bridge binaries - which on
    Linux is simply correct, there being no Linux build of them yet.

    The evidence that actually distinguishes them:
      * client mode means no local CARLA at all -> traffic, definitively
      * the bridge binaries are what drive a co-sim FROM here -> traffic
      * a local CARLA and no bridge binaries -> this machine renders"""
    if (cfg or {}).get("mode") == "client":
        return "traffic", "carla.json is client mode - no CARLA on this machine"
    exe = ".exe" if platform.system() == "Windows" else ""
    has_bridge = all(os.path.isfile(os.path.join(fixs_root, n + exe))
                     for n in ("TrafficLayer", "VirCarlaEnv"))
    if has_bridge:
        return "traffic", "the FIXS bridge binaries are here"
    if (cfg or {}).get("carla_root"):
        # AMBIGUOUS, and it cannot be resolved from installed files: this is either
        # a render host for a remote traffic machine, or a self-contained
        # standalone co-sim (SUMO + standalone/run_synchronization.py + CARLA, all local).
        # The second needs SUMO; the first does not. So say so instead of guessing.
        return "render-or-local", ("a local CARLA and no FIXS bridge binaries - "
                                   "either a render host, or a self-contained "
                                   "engine=py co-sim")
    return "traffic", "no local CARLA configured"


def _check_fixs(rep, fixs_root, role):
    ver = "unknown"
    vf = os.path.join(fixs_root, "FIXS_VERSION.txt")
    if os.path.isfile(vf):
        with open(vf, encoding="utf-8-sig") as f:
            ver = f.readline().strip()
        rep.add("FIXS", "build", OK, ver)
    else:
        rep.add("FIXS", "build", FAIL, f"no {vf} - run initialize")

    exe = ".exe" if platform.system() == "Windows" else ""
    for name in ("TrafficLayer", "VirCarlaEnv"):
        p = os.path.join(fixs_root, name + exe)
        if os.path.isfile(p):
            rep.add("FIXS", name, OK, p)
        elif role in ("render", "render-or-local"):
            # The release ships Windows binaries only. On Linux their absence is
            # not a finding: such a machine either serves CARLA for a remote
            # traffic host, or runs engine=py locally - neither uses these.
            rep.add("FIXS", name, OK,
                    "no Linux build exists; not used by --serve or engine=py")
        else:
            rep.add("FIXS", name, WARN, "not present (needed for engine=cpp)")

    # TrafficLayer loads libsumo at startup; the headers ship in the build but the
    # runtime comes from a separate release asset, and its absence is only visible
    # as TrafficLayer dying immediately.
    libsumo = os.path.join(fixs_root, "CommonLib", "libsumo", "bin")
    if os.path.isdir(libsumo):
        n = len([f for f in os.listdir(libsumo)
                 if f.lower().endswith((".dll", ".so"))])
        rep.add("FIXS", "libsumo runtime", OK if n else WARN, f"{libsumo} ({n} libs)")
    else:
        rep.add("FIXS", "libsumo runtime", FAIL,
                "CommonLib/libsumo/bin missing - TrafficLayer will not start")
    return ver


def _check_python(rep, cfg, env_mod):
    py = (cfg or {}).get("python") or sys.executable
    where = "from ~/.fixs/carla.json" if (cfg or {}).get("python") else "current interpreter"
    rep.add("Python", "interpreter", OK if os.path.isfile(py) else FAIL, f"{py}  ({where})")
    if not os.path.isfile(py):
        return py
    missing = env_mod.missing_runtime(py)
    for mod in env_mod.RUNTIME_MODULES:
        hurt = {"yaml": "scenario yamls read as defaults - endpoints silently wrong",
                "pandas": "no TL table; traffic lights will not sync",
                "shapely": "no TL table; traffic lights will not sync",
                "traci": "cannot drive SUMO",
                "sumolib": "cannot read the SUMO net"}.get(mod, "")
        rep.add("Python", mod, FAIL if mod in missing else OK,
                hurt if mod in missing else "")
    # carla exposes no __version__; the wheel metadata is the reliable source.
    rc, out = _run([py, "-c",
                    "import carla, importlib.metadata as m;"
                    "print(m.version('carla'))"])
    rep.add("Python", "carla", OK if rc == 0 else FAIL,
            out.strip() if rc == 0 else "not importable - run_cosim needs it even "
                                        "with no local CARLA (load_world, spectator)")
    return py


def _check_sumo(rep, role="traffic"):
    exe = shutil.which("sumo") or shutil.which("sumo-gui")
    if not exe:
        # Only a machine that RUNS the co-sim needs SUMO. A pure render host does
        # not - but we cannot always tell which this is, so on an ambiguous machine
        # this is a warning with the condition spelled out, never a silent skip.
        if role == "render":
            rep.add("SUMO", "on PATH", OK, "not needed on a render host")
        elif role == "render-or-local":
            rep.add("SUMO", "on PATH", WARN,
                    "not found - needed only if you run the co-sim HERE "
                    "(engine=py); not needed if this machine only serves CARLA")
        else:
            rep.add("SUMO", "on PATH", FAIL, "sumo / sumo-gui not found")
        return
    rc, out = _run([exe, "--version"])
    ver = next((l for l in out.splitlines() if "SUMO" in l), out.strip().splitlines()[0] if out.strip() else "?")
    rep.add("SUMO", "on PATH", OK, f"{exe}")
    rep.add("SUMO", "version", OK, ver.strip())


def _check_carla(rep, cfg, py, host, port, timeout=5.0, who_has_port=None):
    mode = (cfg or {}).get("mode") or "not configured"
    root = (cfg or {}).get("carla_root")
    rep.add("CARLA", "mode", OK if cfg else FAIL,
            f"{mode}" + (f"  {root}" if root else "  (no local install)"))
    if mode == "source":
        ue4 = (cfg or {}).get("ue4_root")
        ok = bool(ue4) and os.path.isdir(ue4)
        rep.add("CARLA", "UE4_ROOT", OK if ok else FAIL, ue4 or "unset")
    rep.add("CARLA", "endpoint", OK, f"{host}:{port}")
    # Reachability, then a real RPC so "port open" is not mistaken for "CARLA is
    # there" - something else listening on 2000 answers the TCP connect happily.
    try:
        with socket.create_connection((host, port), timeout=timeout):
            pass
    except OSError as e:
        rep.add("CARLA", "reachable", WARN,
                f"{e} - start it there, or ignore if this run will launch it")
        return
    rep.add("CARLA", "reachable", OK, "tcp connect ok")
    rc, out = _run([py, "-c",
                    "import carla,sys;c=carla.Client(sys.argv[1],int(sys.argv[2]));"
                    "c.set_timeout(8.0);"
                    "print(c.get_client_version(),'|',c.get_server_version())",
                    str(host), str(port)], timeout=20)
    if rc != 0:
        # Name the culprit. "port open but no CARLA answered" is true and useless;
        # the usual cause is an ORPHANED CARLA from a run whose parent was killed
        # hard (terminal closed), so its cleanup never ran. Knowing the PID turns
        # this from a symptom into an instruction.
        who = who_has_port(port) if who_has_port else None
        if who:
            pid, name = who
            rep.add("CARLA", "rpc", FAIL,
                    f"port held by PID {pid} ({name}) which does not answer as "
                    f"CARLA - likely an orphan from an earlier run. run_cosim will "
                    f"reclaim it on the next launch, or: kill {pid}")
        else:
            rep.add("CARLA", "rpc", FAIL, "port open but no CARLA answered")
        return
    line = out.strip().splitlines()[-1]
    client, server = [s.strip() for s in line.split("|", 1)]
    same = client.split("-")[0] == server.split("-")[0]
    rep.add("CARLA", "versions", OK if same else WARN,
            f"client {client} / server {server}"
            + ("" if same else "  - mismatched client and server"))


def _check_peer(rep, host, port, fixs_version):
    import peer
    try:
        sock = peer.connect(host, port, wait=0, quiet=True)
    except Exception as e:
        rep.add("Peer", "control channel", WARN,
                f"nothing listening on {host}:{port} - fine unless you expect "
                f"'run_cosim --serve' to be running there")
        return
    try:
        _r, welcome = peer.hello(sock, fixs_version, timeout=5.0)
        theirs = welcome.get("fixs_version", "?")
        rep.add("Peer", "control channel", OK, f"{host}:{port}")
        rep.add("Peer", "FIXS version", OK if theirs == fixs_version else WARN,
                f"peer {theirs}" + ("" if theirs == fixs_version else
                                    f" vs local {fixs_version} - mismatched builds"))
    except Exception as e:
        rep.add("Peer", "control channel", FAIL, str(e))
    finally:
        try:
            sock.close()
        except OSError:
            pass


def _check_maps(rep, maps_root):
    if not os.path.isdir(maps_root):
        rep.add("Maps", "cache", WARN, f"{maps_root} does not exist yet")
        return
    names = sorted(d for d in os.listdir(maps_root)
                   if os.path.isdir(os.path.join(maps_root, d)))
    if not names:
        rep.add("Maps", "cache", WARN, "no maps cached yet")
        return
    for n in names:
        d = os.path.join(maps_root, n)
        bits = []
        bits.append("sumo/" if os.path.isdir(os.path.join(d, "sumo")) else "no sumo/")
        bits.append("tl_table.csv" if os.path.isfile(os.path.join(d, "tl_table.csv"))
                    else "no tl_table.csv")
        ok = "no sumo/" not in bits
        rep.add("Maps", n, OK if ok else WARN, ", ".join(bits))


def _check_dtl(rep):
    if not shutil.which("gh"):
        rep.add("Digital-Twin-Library", "gh cli", WARN,
                "not installed - needed to fetch map bundles (the library is private)")
        return
    rc, _out = _run(["gh", "auth", "status"])
    rep.add("Digital-Twin-Library", "gh auth", OK if rc == 0 else FAIL,
            "authenticated" if rc == 0
            else "not authenticated - map fetches will fail (`gh auth login`)")


def _check_scenario(rep, scenario):
    """Is the SELECTED SCENARIO coherent? Today: is its ego route drivable?

    Every other check here answers "can this host run a co-sim". None of them can
    answer "will THIS scenario behave", and a scenario can be broken in a way that
    still produces a full, clean-looking run of meaningless numbers.
    mlk_eco_driving's ego route was generated by applying one lane index to every
    edge, which put the ego in lanes unreachable from its own route: SUMO could
    not place it coherently, the odometer froze and then rewound 17 m, the cached
    next-signal list drifted with it, and the controller stopped recognising the
    movement and released the ego to free-flow speed into the car ahead -- for
    650 s, with no error raised anywhere.

    `scenario` is (config_path, net_path), or None when nothing is selected. That
    case is reported as a WARN rather than passed over quietly: a check silently
    not running is exactly how the last one of these went unnoticed.
    """
    if scenario is None:
        rep.add("Scenario", "selection", WARN,
                "skipped - no app/config selected (pass --profile)")
        return
    config_path, net_path = scenario
    if not net_path:
        rep.add("Scenario", "net", WARN, "no .net.xml found for this scenario")
        return
    try:
        import check_ego_route
    except ImportError as exc:
        rep.add("Scenario", "ego route", WARN, f"checker unavailable: {exc}")
        return
    try:
        violations, count = check_ego_route.check_config(config_path, net_path)
    except Exception as exc:            # a bad net must not take --doctor down
        rep.add("Scenario", "ego route", WARN, f"could not check: {exc}")
        return
    if violations is None:
        rep.add("Scenario", "ego route", OK, "no EgoRoutePoints in this config")
    elif violations:
        rep.add("Scenario", "ego route", FAIL,
                f"{len(violations)} illegal step(s) in {count} points - "
                f"first {violations[0].from_lane} -> {violations[0].to_lane}")
        for v in violations[:3]:
            rep.add("Scenario", "", FAIL, str(v).replace("\n", "\n         "))
    else:
        rep.add("Scenario", "ego route", OK,
                f"{count} points, every step is a real connection")


def run(cfg, env_mod, fixs_root, maps_root, host, port, fixs_version,
        peer_port=None, role=None, why=None, who_has_port=None, scenario=None):
    """Run every applicable check. Returns a shell exit code."""
    # Say WHY. A misread role quietly skews which checks run, so the inference has
    # to be visible rather than something to discover from a confusing report.
    print(f"[doctor] {socket.gethostname()} ({platform.system()}) - role: {role}"
          f"{'  (' + why + ')' if why else ''}"
          f"{'' if why is None else '  [--role to override]'}")
    rep = Report()
    _check_fixs(rep, fixs_root, role)
    py = _check_python(rep, cfg, env_mod)
    _check_sumo(rep, role)
    _check_carla(rep, cfg, py, host, port, who_has_port=who_has_port)
    if peer_port and role != "render":  # a render host dials nobody
        _check_peer(rep, host, peer_port, fixs_version)
    _check_maps(rep, maps_root)
    _check_dtl(rep)
    _check_scenario(rep, scenario)
    return rep.show()
