"""
run_cosim.py - cross-platform SUMO <-> CARLA co-simulation launcher.

Reads the per-machine CARLA env saved by carla_env_setup.py (~/.fixs/carla.json),
launches that CARLA (packaged build or source editor, OS-aware), waits for the RPC
port, loads the target map, and runs the SUMO <-> CARLA synchronization. Works on
Windows and Linux.

The very first time this runs on a fresh clone there is no saved config, so it
auto-invokes carla_env_setup.run_setup() to ask which CARLA to use and remember
it; every run afterwards is seamless. To switch CARLA later, run
`python carla_env_setup.py`, or pass --reconfigure here.

Examples:
  # first run prompts for CARLA, then launches and runs; later runs are seamless:
  python run_cosim.py --sumocfg fixtures/grid_tls.sumocfg --map Town01

  # CARLA already running (e.g. an editor in Play, or a RoadRunner map):
  python run_cosim.py --no-launch --no-net-offset \
      --map RP_Ver0529 --sumocfg <cfg> --tl-table <csv> --sumo-gui

  # two machines: CARLA on one, SUMO + the bridge on the other. Either order.
  python run_cosim.py --serve                    # on the CARLA machine
  python run_cosim.py --peer 192.168.140.56      # on the traffic machine
"""
import argparse
import json
import re
import os
import platform
import shutil
import signal
import socket
import subprocess
import sys
import time
import traceback
import urllib.request

import app_catalog
import carla_env_setup as env
import run_profile

# Flush every line as it is printed. Redirected to a file or a pipe - which is how
# the .bat/.sh wrappers get run from a shortcut, and how anyone captures a log -
# python block-buffers stdout, so nothing appears until the process exits: a run that
# is working looks like a hang, and a run that was killed loses the record of what it
# launched. VirCarlaEnv sets std::unitbuf for the same reason.
try:
    sys.stdout.reconfigure(line_buffering=True)      # py3.7+
except AttributeError:                               # pragma: no cover - old python
    pass

HERE = os.path.dirname(os.path.abspath(__file__))
# CARLA's own standalone bridge. It lives OUTSIDE Carla/ (#330) because
# 8_create_zip.ps1 packs Carla/ wholesale and this speaks no FIXS: it drives SUMO
# over its own TraCI connection and takes no controller, XIL component or
# TrafficLayer subscription. Reachable only via --standalone-bridge.
SYNC = os.path.join(os.path.dirname(HERE), "standalone", "run_synchronization",
                    "run_synchronization.py")
PY_BRIDGE = os.path.join(HERE, "VirEnv", "mainVirCarla.py")
FIXS_ROOT = os.path.dirname(HERE)          # FIXS/Carla -> FIXS (the fetched bundle root)
APP_ROOT = os.path.dirname(FIXS_ROOT)      # FIXS -> the app dir (holds initialize.{sh,ps1})

# Defaults for the native stack's two ports, used ONLY to seed a freshly generated
# scenario yaml and as the fallback when one cannot be read. The yaml is the single
# source of truth at run time - read_stack_ports() takes them from
# SimulationSetup.TrafficSimulatorPort and CarlaSetup.CarlaClientPort, so editing
# the yaml moves the ports for every component instead of just some of them.
DEFAULT_TRACI_PORT = 1337    # SUMO TraCI server (TrafficLayer connects as its client)
DEFAULT_BRIDGE_PORT = 440    # TrafficLayer serves VirCarlaEnv here
DEFAULT_CARLA_HOST = "localhost"   # CARLA RPC; CarlaSetup.CarlaServerIP overrides
DEFAULT_CARLA_PORT = 2000

# How many runs may fail back-to-back before --serve gives up and stops listening.
# Not unlimited: a host whose CARLA cannot start would otherwise accept peer after
# peer and fail each one, which on the traffic side is indistinguishable from a
# service that is simply busy. Any success resets it.
SERVE_MAX_CONSECUTIVE_FAILURES = 5

# The FIXS exchange period, in seconds - the python mirror of fixs::kFeedPeriodS
# (CommonLib/FixsProtocol.h). This is a property of the protocol, NOT a knob:
# every VirEnvCore host trades messages with TrafficLayer only on this grid, and
# TrafficLayer steps the traffic simulator exactly once per exchange. So SUMO's
# --step-length IS this number, and run_cosim passes it rather than offering it -
# a 0.05 s SUMO step against the 0.1 s exchange makes CARLA advance two ticks per
# SUMO step, which renders every sample twice and plays the scene at half speed.
#
# Both bridges get it, so a py run and a cpp run drive SUMO identically.
FIXS_FEED_S = 0.1
# Ticks the CARLA world may use: the feed itself (1:1) or an exact divisor of it,
# because the exchange boundary is tested on the feed grid - a tick that does not
# divide the feed (0.03) never lands on a boundary, so no exchange ever happens.
CARLA_TICK_CHOICES = (0.1, 0.05, 0.025, 0.02, 0.01)


# --------------------------------------------------------------------------- #
# FIXS bundle freshness. FIXS/ is a gitignored, fetched artifact; the rolling
# v0.9.0-alpha release republishes in place, so its TAG NAME never changes -
# compare the BUILD COMMIT (BUILD_INFO.txt) against the tag's current commit.
# Best-effort: any hiccup (offline, rate-limited, parse) is swallowed; a stale
# bundle only ever prompts, never blocks.
# --------------------------------------------------------------------------- #
def _first_group(path, pattern):
    import re
    try:
        # utf-8-sig: these are the same PowerShell-written files as above. A leading
        # BOM would not break the searches used here, but reading one member of a
        # pair BOM-tolerantly and the other not is exactly how that bug survived.
        with open(path, encoding="utf-8-sig", errors="ignore") as f:
            m = re.search(pattern, f.read())
        return m.group(1) if m else None
    except OSError:
        return None


def _local_fixs_commit():
    return (_first_group(os.path.join(FIXS_ROOT, "BUILD_INFO.txt"),
                         r"Git Commit:\s*([0-9a-fA-F]{7,40})") or "").lower() or None


def _fixs_tag_repo():
    """(tag, repo) from FIXS_VERSION.txt: line 1 is the tag; a 'Source:' line the repo."""
    tag, repo = None, "ORNL-Real-Sim/FIXS"
    try:
        # utf-8-SIG: this file is written by scripts/update_fixs.ps1, and under
        # PowerShell 5.1 `Out-File -Encoding UTF8` means UTF-8 *with* a BOM. Read as
        # plain utf-8 the BOM survives as ﻿ on line 1 - and str.strip() does not
        # remove it, because it is not whitespace - so the tag came out as
        # '﻿v0.9.0-alpha', the releases API call built from it failed, and the
        # freshness check below disabled itself silently on every Windows install.
        with open(os.path.join(FIXS_ROOT, "FIXS_VERSION.txt"), encoding="utf-8-sig") as f:
            lines = [ln.strip() for ln in f if ln.strip()]
        if lines:
            # Line 1 is the tag, but a rolling release is stamped as
            # "<tag> (<published_at>)" while a pinned one writes the bare tag. Take
            # the first field so either form resolves; passing the stamped form to the
            # releases API 404s and silently disables this check.
            tag = lines[0].split()[0]
        for ln in lines:
            if ln.lower().startswith("source:"):
                repo = ln.split(":", 1)[1].strip() or repo
    except OSError:
        pass
    return tag, repo


def _remote_fixs_commit(repo, tag, timeout=4.0):
    """The commit the remote <tag> release points at now (target_commitish), or None
    if it cannot be told (offline, or the field is a branch name, not a sha)."""
    url = "https://api.github.com/repos/%s/releases/tags/%s" % (repo, tag)
    req = urllib.request.Request(url, headers={"User-Agent": "fixs-fetch",
                                               "Accept": "application/vnd.github+json"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        sha = (json.load(r).get("target_commitish") or "").lower()
    is_sha = len(sha) >= 7 and all(c in "0123456789abcdef" for c in sha)
    return sha if is_sha else None


def _run_initialize(tag):
    """Re-fetch the FIXS bundle for <tag> through the app's front door (the tag is
    passed as an argument, so it runs non-interactively). True on success.

    Goes through run_cosim.bat/.sh rather than reaching for an update script by
    name. This used to probe a list of filenames per platform, because the app
    repo had renamed the script once (FIXS_Applications#13) and a bundle this new
    can sit in a clone laid out either way - so a self-update reported
    "initialize.sh not found" on a repo that had simply renamed it. That list
    could only grow: since #272 the updater itself lives in FIXS and is fetched
    per release, so the ONE thing an app repo is guaranteed to expose is its
    documented entry point. Naming that instead means the engine no longer tracks
    any downstream repo's internal layout."""
    if platform.system() == "Windows":
        front_door = os.path.join(APP_ROOT, "run_cosim.bat")
        cmd = ["cmd", "/c", front_door]
    else:
        front_door = os.path.join(APP_ROOT, "run_cosim.sh")
        cmd = ["bash", front_door]
    if not os.path.isfile(front_door):
        print(f"[cosim] cannot self-update: {front_door} not found.")
        return False
    print(f"[cosim] updating FIXS -> {tag} via {os.path.basename(front_door)} ...")
    # --update-fixs exits before the front door's bootstrap gate and never re-runs
    # python, so this cannot recurse back into run_cosim.py.
    return subprocess.call(cmd + ["--update-fixs", tag]) == 0


def maybe_update_fixs(no_check=False, interactive=None):
    """Detect a local FIXS bundle that diverges from the published rolling release
    and, when interactive, offer to update + relaunch. Advisory only: every failure
    is swallowed so a run is never blocked by the check.

    `interactive` is the caller's answer, not this function's guess. serve_forever
    re-enters main() for every run, so on a --serve host this is asked again
    between runs - with the operator long gone and the peer port not yet
    listening. An advisory prompt must never be what stops a service serving."""
    # FIXS_REEXEC: env.reexec_under_configured() relaunches us under the configured
    # interpreter, and this runs before that - so without the guard the child asked the
    # same question
    # the parent had just answered, and a wrapper that starts a different python (any
    # of them: run_cosim.bat finds its own) prompted twice on every run. One bundle,
    # one question.
    if (no_check or os.environ.get("FIXS_NO_FRESHNESS")
            or os.environ.get("FIXS_REEXEC") == "1"):
        return
    try:
        local = _local_fixs_commit()
        tag, repo = _fixs_tag_repo()
        if not local or not tag:
            return
        remote = _remote_fixs_commit(repo, tag)
        if not remote:
            return
        n = min(len(local), len(remote))
        if local[:n] == remote[:n]:
            return  # up to date
        print(f"[cosim] local FIXS bundle {local[:8]} diverges from the published "
              f"{tag} ({remote[:8]}).")
        if not (sys.stdin.isatty() if interactive is None else interactive):
            print("[cosim] non-interactive; run initialize to update. Continuing.")
            return
        ans = input("[cosim] update the local FIXS bundle now? [y/N]: ").strip().lower()
        if ans not in ("y", "yes"):
            print("[cosim] keeping the current bundle.")
            return
        if _run_initialize(tag):
            print("[cosim] FIXS updated; relaunching run_cosim ...")
            os.environ["FIXS_NO_FRESHNESS"] = "1"   # the relaunch is already current
            # subprocess, not os.execv. Windows has no exec, so the CRT emulates it
            # by joining argv into ONE command line WITHOUT quoting - so an
            # interpreter at 'C:\Program Files\Python39\python.exe' relaunched as
            #   C:\Program: can't open file '...\main\Files\Python39\python.exe'
            # CreateProcess still found the .exe by its space-fallback search, but
            # argv was already split, so python took the tail of its own path as the
            # script to run. subprocess quotes the vector properly on both OSes, and
            # is what env.reexec_under_configured already uses for the same reason.
            cmd = [sys.executable, os.path.join(HERE, "run_cosim.py")] + sys.argv[1:]
            sys.exit(subprocess.call(cmd))
        print("[cosim] update did not complete; continuing with the current bundle.")
    except Exception as e:
        if os.environ.get("FIXS_DEBUG"):
            print(f"[cosim] freshness check skipped: {e}")


# --------------------------------------------------------------------------- #
# Co-sim engine selection. 'py' (default) = the standalone run_synchronization.py
# bridge; 'cpp' = the FIXS-native stack (SUMO + TrafficLayer + VirCarlaEnv), driven
# by a scenario yaml. The bridge is chosen by CarlaSetup.Backend in that yaml, read
# through the shipped CommonLib/ConfigHelper.py; --engine overrides per run.
# --------------------------------------------------------------------------- #
def _native_binaries():
    exe = ".exe" if platform.system() == "Windows" else ""
    return (os.path.join(FIXS_ROOT, "TrafficLayer" + exe),
            os.path.join(FIXS_ROOT, "VirCarlaEnv" + exe))


def _read_scenario_config(config_yaml):
    """The parsed scenario yaml via CommonLib/ConfigHelper.py (the canonical parser
    the C++ engine mirrors), or None if it cannot be read."""
    if not config_yaml or not os.path.isfile(config_yaml):
        return None
    try:
        sys.path.insert(0, os.path.join(FIXS_ROOT, "CommonLib"))
        import ConfigHelper
        ch = ConfigHelper.ConfigHelper()
        ch.getConfig(config_yaml)
        return ch
    except ImportError as e:
        # A MISSING PARSER is not a missing file. Falling back to defaults here
        # invents an answer and then drives real decisions with it: without
        # pyyaml, CarlaSetup.CarlaServerIP reads as localhost, which is enough to
        # make a correctly configured distributed run refuse to start while
        # blaming a yaml that was right all along. Fail where the cause is.
        sys.exit(
            f"[cosim] cannot parse scenario yamls: {e}\n"
            f"        CommonLib/ConfigHelper.py needs PyYAML. Install it into the "
            f"interpreter run_cosim uses (the 'python' entry in {env.CONFIG_PATH}):\n"
            f"            \"{sys.executable}\" -m pip install pyyaml\n"
            f"        Continuing without it would silently substitute defaults for "
            f"every setting in {os.path.basename(config_yaml)}.")
    except Exception as e:
        print(f"[cosim] could not read {config_yaml} ({e}); using defaults.")
        return None


def read_backend(config_yaml):
    """'py' or 'cpp' from CarlaSetup.EnablePythonBackend -- which VirEnvCore runs.

    BOTH are the FIXS-native stack (SUMO + TrafficLayer + a VirEnvCore bridge) and
    both read the scenario yaml in full; they differ only in the implementation of
    the bridge:

      'py'  -> Carla/VirEnv/mainVirCarla.py  (CommonLib/VirEnv, #325)
      'cpp' -> VirCarlaEnv.exe               (CommonLib/VirEnvCore.cpp, #174)

    That is what makes a py run and a cpp run of one scenario directly comparable,
    which is the point of having two (tests/VirEnv/test_core_parity.py).

    A bool rather than a free-text engine name, so a typo cannot silently pick the
    wrong bridge. 'py' on absence or any parse problem.

    The standalone run_synchronization.py bridge is NOT one of these: it speaks no
    FIXS, drives SUMO over its own TraCI connection, and is selected explicitly with
    --standalone-bridge rather than by a scenario key."""
    ch = _read_scenario_config(config_yaml)
    if ch is None:
        return "py"
    return "py" if ch.Carla_setup["EnablePythonBackend"] else "cpp"


def read_stack_ports(config_yaml):
    """(traci_port, bridge_port) from the scenario yaml - SUMO's TraCI server
    (SimulationSetup.TrafficSimulatorPort) and the TrafficLayer<->VirCarlaEnv bridge
    (CarlaSetup.CarlaClientPort). Read rather than hard-coded so the yaml stays the
    single source of truth: the exes get these values from the same file."""
    ch = _read_scenario_config(config_yaml)
    if ch is None:
        return DEFAULT_TRACI_PORT, DEFAULT_BRIDGE_PORT
    try:
        traci_port = int(ch.simulation_setup["TrafficSimulatorPort"] or DEFAULT_TRACI_PORT)
        bridge_port = int(ch.Carla_setup["CarlaClientPort"] or DEFAULT_BRIDGE_PORT)
        return traci_port, bridge_port
    except (TypeError, ValueError):
        return DEFAULT_TRACI_PORT, DEFAULT_BRIDGE_PORT


def read_carla_endpoint(config_yaml):
    """(host, port) of the CARLA RPC server from CarlaSetup.CarlaServerIP/Port,
    or (None, None) if unreadable.

    VirCarlaEnv takes its CARLA endpoint from this yaml, so run_cosim has to
    launch/probe/connect to the SAME one or the two disagree silently: run_cosim
    reports a healthy CARLA on localhost:2000 while VirCarlaEnv dials whatever
    the yaml says and times out. Note this is a DIFFERENT link from
    CarlaSetup.CarlaClientIP/Port, which despite the name is not CARLA at all -
    it is TrafficLayer's bridge endpoint (mainVirCarla: trafficLayerIP_)."""
    ch = _read_scenario_config(config_yaml)
    if ch is None:
        return None, None
    try:
        host = (ch.Carla_setup["CarlaServerIP"] or "").strip() or None
        port = int(ch.Carla_setup["CarlaServerPort"] or 0) or None
        return host, port
    except (TypeError, ValueError, AttributeError):
        return None, None


def read_sumo_num_clients(config_yaml):
    """SumoSetup.NumClients - how many TraCI clients SUMO must wait for before it
    starts stepping. 1 (the SUMO default) unless the yaml says otherwise."""
    ch = _read_scenario_config(config_yaml)
    if ch is None:
        return 1
    try:
        return max(1, int(ch.Sumo_setup["NumClients"] or 1))
    except (TypeError, ValueError, KeyError):
        return 1


def read_sumo_autostart(config_yaml):
    """SumoSetup.AutoStart: whether sumo-gui gets --start. True (default) = it steps
    as soon as it loads; False = it opens loaded but PAUSED so you press Play.
    run_cosim launches SUMO either way. --sumo-no-start overrides."""
    ch = _read_scenario_config(config_yaml)
    if ch is None:
        return True
    return bool(ch.Sumo_setup["AutoStart"])


def get_yaml_scalar(path, section, key):
    """The raw text of `section.key` in a scenario yaml, or None when absent.

    The read counterpart of set_yaml_scalar, and here for the same reason: these
    files are read by hand as much as by the engine. It also does not depend on the
    shipped CommonLib/ConfigHelper.py mirroring every key the C++ ConfigHelper
    reads - CarlaTimeStep and RealtimePacing, for instance, are read by the engine
    but absent from the python mirror, so _read_scenario_config cannot see them."""
    import re
    # utf-8-sig, not utf-8: a scenario yaml is meant to be hand-edited, and a Windows
    # editor (Notepad, PowerShell's Set-Content) leaves a UTF-8 BOM. Read as plain
    # utf-8 the BOM sticks to line 1, so a section declared there stops matching and
    # every key under it reads as absent - a default silently taking over.
    try:
        with open(path, encoding="utf-8-sig") as f:
            lines = f.readlines()
    except OSError:
        return None
    in_section = False
    for line in lines:
        if re.match(rf"^{re.escape(section)}\s*:", line):
            in_section = True
            continue
        if in_section:
            m = re.match(rf"^\s+{re.escape(key)}\s*:\s*(.*?)(\s+#.*)?$", line)
            if m:
                return m.group(1).strip() or None
            if line.strip() and not line[0].isspace():
                return None                 # left the section without finding it
    return None


def _yaml_float(config_yaml, section, key, default):
    """`section.key` as a float, or `default` when absent/blank/unparseable.

    Omitted keys are normal - a hand-written app yaml carries only what it cares
    about - so every cadence read goes through here and the resolved value is
    printed in the cadence banner. That way a default is visible rather than
    silently assumed."""
    raw = get_yaml_scalar(config_yaml, section, key)
    if raw is None:
        return default
    try:
        return float(raw)
    except ValueError:
        print(f"[cosim] {os.path.basename(config_yaml)}: {section}.{key} = '{raw}' is "
              f"not a number; using {default}.")
        return default


def read_cadence(config_yaml):
    """(carla_tick, pose_refresh) in seconds, resolved and validated.

    The three cadences are separate things and none falls back to another:
      feed          FIXS_FEED_S - the exchange period AND the SUMO step. Constant.
      carla_tick    CarlaSetup.CarlaTimeStep - the CARLA world step
                    (fixed_delta_seconds), the analogue of CarMaker's solver dt.
                    Absent/0 -> the feed, i.e. tick 1:1 and do not interpolate.
      pose_refresh  CarlaSetup.TrafficRefreshRate - how often the bridge re-applies
                    (interpolated) traffic poses. The SAME meaning it has on the
                    CarMaker side (VirEnvHelper -> core_.trafficRefreshRate_): a
                    visual / RPC-cost knob. Absent/0 -> every tick.

    Exits with the fix spelled out rather than running a cadence the engine will
    silently reinterpret - which is exactly how the 0.05 s half-speed bug hid."""
    tick = _yaml_float(config_yaml, "CarlaSetup", "CarlaTimeStep", 0.0)
    tick = tick if tick > 1e-9 else FIXS_FEED_S
    refresh = _yaml_float(config_yaml, "CarlaSetup", "TrafficRefreshRate", 0.0)
    refresh = refresh if refresh > 1e-9 else tick

    slots = FIXS_FEED_S / tick
    if tick > FIXS_FEED_S + 1e-9 or abs(slots - round(slots)) > 1e-6:
        sys.exit(f"[cosim] {os.path.basename(config_yaml)}: CarlaSetup.CarlaTimeStep "
                 f"= {tick} s must be the FIXS feed period ({FIXS_FEED_S} s) or an "
                 f"exact divisor of it "
                 f"({', '.join(str(c) for c in CARLA_TICK_CHOICES)}). The FIXS "
                 f"exchange boundary is tested on the feed grid, so a tick that does "
                 f"not divide it never lands on a boundary and the bridge would "
                 f"exchange nothing at all.")
    if refresh < tick - 1e-9 or abs(1.0 / refresh - round(1.0 / refresh)) > 1e-6:
        sys.exit(f"[cosim] {os.path.basename(config_yaml)}: "
                 f"CarlaSetup.TrafficRefreshRate = {refresh} s must be >= "
                 f"CarlaTimeStep ({tick} s) and have a whole reciprocal "
                 f"({', '.join(str(c) for c in CARLA_TICK_CHOICES)}). It is the pose "
                 f"re-apply cadence, not the feed period - to tick CARLA faster set "
                 f"CarlaTimeStep. (Before #219 this key doubled as the feed period, "
                 f"so an older yaml may still carry a value meant for that.)")
    return tick, refresh


def read_realtime_pacing(config_yaml):
    """CarlaSetup.RealtimePacing - pace the co-sim to wall-clock time, or run as
    fast as the hardware allows. False when absent, matching the C++ ConfigHelper
    default. Honoured by BOTH bridges: run_cosim turns it into the python bridge's
    --no-realtime, and VirCarlaEnv reads it itself."""
    raw = get_yaml_scalar(config_yaml, "CarlaSetup", "RealtimePacing")
    return str(raw).strip().lower() in ("true", "1", "yes", "on")


# --------------------------------------------------------------------------- #
# The SUMO co-sim convention.
#
# A DT-Library map ships ONE app-independent .sumocfg: the net, the demand, the
# seed, the time window - what the map and its calibration own. It says nothing
# about co-simulation, and it must not have to: a co-sim requirement does not belong
# in a shared artifact, and otherwise every new map would need the same edit before
# it worked. So run_cosim injects the co-sim convention on the command line, where it
# applies to whatever map any app runs against, and the map file stays untouched.
#
# Injected is not hidden. What was wrong before was that ONE bridge (CARLA's
# sumo_integration, inside the python path) injected "--lateral-resolution 0.25" and
# "--collision.check-junctions" where nobody could see them, while the native stack
# passed neither - so the two ran different traffic and nobody had chosen that. The
# fix is not to stop injecting; it is to inject from ONE table, in git, for BOTH
# bridges, and print every flag with where it came from.
#
# Tiers, highest first:
#   contract    FIXS requires it; an app may not override it.
#   convention  the default co-sim behaviour; an app may override it via apps.json.
#   (the map's own cfg is the base, and any flag here wins over it)
# --------------------------------------------------------------------------- #
SUMO_CONTRACT = {
    # One SUMO step per FIXS exchange - see FIXS_FEED_S. Not negotiable: a different
    # step makes SUMO time and host time run at different rates.
    "--step-length": (lambda: f"{FIXS_FEED_S:g}", "one SUMO step per FIXS exchange"),
}
SUMO_CONVENTION = {
    # Sublane model. Without it a lane change is an instantaneous one-lane-width jump,
    # which a co-sim viewer draws as the car teleporting sideways with its heading
    # unchanged. 0.25 m is what CARLA's sumo_integration has always passed, so this
    # keeps the shipped behaviour - now for both bridges. It is not only cosmetic:
    # sublane is a different lane-change model, so an app whose calibration assumes
    # SUMO's default should override it to 0.
    "--lateral-resolution": ("0.25", "sublane: lane changes slide instead of jumping"),
    # Also inherited from CARLA's sumo_integration; kept so the two bridges match.
    "--collision.check-junctions": ("true", "check collisions inside junctions"),
}


def resolve_sumo_args(app, app_owns_scenario=False):
    """[(flag, value, origin)] for this run: contract over app over convention.

    An app deviates through apps.json `sumo_args` - tracked, reviewed next to the app
    declaration, and valid on every map that app runs against. A null value drops a
    convention flag. A contract flag cannot be overridden: an app that tries is told
    so rather than silently getting a broken clock.

    `app_owns_scenario` drops the CONVENTION entirely. The convention exists because a
    Digital-Twin-Library .sumocfg is a SHARED artifact that must not carry one
    consumer's co-sim requirement - so run_cosim adds it on the command line instead.
    An app running its own declared sumocfg has no shared artifact and no such
    problem, and injecting sublane lane-changing into a scenario its author
    calibrated without it changes results silently. The CONTRACT still applies:
    --step-length is the FIXS feed, and a scenario cannot opt out of the protocol."""
    args = {}
    if not app_owns_scenario:
        for flag, (value, why) in SUMO_CONVENTION.items():
            args[flag] = (value, f"convention: {why}")
    for flag, value in ((app or {}).get("sumo_args") or {}).items():
        if flag in SUMO_CONTRACT:
            print(f"[cosim] app '{app.get('id')}' sets {flag} in sumo_args; ignoring "
                  f"it - that one is the FIXS contract, not an app choice.")
            continue
        if value is None:
            args.pop(flag, None)
            print(f"[cosim] app '{app.get('id')}': {flag} dropped (sumo_args: null)")
            continue
        args[flag] = (str(value), f"app {app.get('id')}")
    for flag, (value, why) in SUMO_CONTRACT.items():
        args[flag] = (value() if callable(value) else value, f"contract: {why}")
    return [(f, v, o) for f, (v, o) in sorted(args.items())]


def print_sumo_args(origins):
    """One line per injected flag, with where it came from."""
    for flag, value, origin in origins:
        print(f"[cosim]   ->   {flag} {value}".ljust(52) + f"[{origin}]")


def sumo_launch_cmd(sumocfg, traci_port, num_clients, gui, autostart, app=None,
                    app_owns_scenario=False):
    """The SUMO command line - ONE builder, used by BOTH bridges.

    Three kinds of argument, and that is the whole story:
      * the map's .sumocfg, used as it ships (never edited by us);
      * co-sim plumbing - where the TraCI server listens, how many clients must
        attach, whether a GUI window starts stepping;
      * the convention above, so both bridges drive SUMO identically.

    --start is passed EXPLICITLY as true/false rather than omitted when off. Omitting
    it does not turn it off: a cfg that declares <start value="t"/> (roosevelt's does)
    then starts stepping anyway, which made SumoSetup.AutoStart=false and
    --sumo-no-start quietly do nothing on that map.

    Returns (cmd, origins) - origins is [(flag, value, where-from)] for the caller to
    print, so a run always says what it gave SUMO and why."""
    cmd = [("sumo-gui" if gui else "sumo"), "-c", sumocfg,
           "--remote-port", str(traci_port),
           "--num-clients", str(num_clients)]
    origins = []
    for flag, value, origin in resolve_sumo_args(app, app_owns_scenario):
        cmd += [flag, value]
        origins.append((flag, value, origin))
    if gui:
        cmd += ["--start", "true" if autostart else "false"]
        origins.append(("--start", "true" if autostart else "false",
                        "SumoSetup.AutoStart"))
    return cmd, origins


def cadence_banner(engine, carla_tick, pose_refresh, realtime):
    """One line stating the whole resolved cadence, printed by every bridge.

    `engine` is which one is about to run -- "py", "cpp" or "standalone" -- so the
    banner and the engine line above it always agree.

    So that a run never again has to be reverse engineered from three keys and a
    hardcoded grid: it names the feed, the tick, the interpolation factor and the
    pacing, including values that came from a default."""
    interp = int(round(FIXS_FEED_S / carla_tick))
    how = f"interpolated {interp}x" if interp > 1 else "1:1 with the feed"
    return (f"[cosim] cadence ({engine}): FIXS feed {FIXS_FEED_S:g} s "
            f"(= SUMO --step-length) | CARLA tick {carla_tick:g} s ({how}) "
            f"| pose refresh {pose_refresh:g} s "
            f"| pacing {'realtime' if realtime else 'as fast as possible'}")


def _tl_junction_ids(tl_table):
    """Unique junction ids in the TL table, for the VirCarlaEnv SignalSubscription."""
    import csv
    ids = []
    try:
        with open(tl_table, newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                j = (row.get("junction_id") or "").strip()
                if j and j not in ids:
                    ids.append(j)
    except (OSError, KeyError):
        pass
    return ids


def generate_config_yaml(path, tl_table, carla_host, carla_port, realtime=True,
                         carla_tick=FIXS_FEED_S, backend="cpp",
                         traci_port=DEFAULT_TRACI_PORT, bridge_port=DEFAULT_BRIDGE_PORT):
    """Write a probe-shaped VirCarlaEnv scenario config: mirror EVERY SUMO vehicle
    (#176 all:['true']) into CARLA and sync the tl_table's junctions. Visualization-
    only (#77): no XIL, no ego. Machine bits (CARLA host/port) come from the resolved
    env; Backend=cpp records the engine choice. Modelled on
    tests/Sumo/Probes/TrafficLayer_SUMO_Carla/config.yaml. Regenerated on --reimport.

    Carries the CARLA tick (CarlaTimeStep) but NOT a feed period: the feed is
    FIXS_FEED_S, fixed by the protocol. It also carries no SUMO behaviour settings -
    those live in the .sumocfg (see sumo_launch_cmd)."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    junctions = _tl_junction_ids(tl_table) if tl_table else []
    rt = "true" if realtime else "false"
    use_py = "true" if backend == "py" else "false"
    tick = carla_tick if carla_tick and carla_tick > 1e-9 else FIXS_FEED_S
    interp = int(round(FIXS_FEED_S / tick))
    # The C++ side takes a literal address, not a resolvable hostname: normalise the
    # python client's 'localhost' default so both readers agree on 127.0.0.1.
    if carla_host in ("localhost", ""):
        carla_host = "127.0.0.1"
    sig = ""
    if junctions:
        names = ", ".join(f"'{j}'" for j in junctions)
        sig = ("  SignalSubscription:\n"
               "  - type: intersection\n"
               f"    attribute: {{ name: [{names}] }}\n"
               "    ip: ['127.0.0.1']\n"
               f"    port: [{bridge_port}]\n")
    text = f"""\
# Auto-generated by run_cosim: the per-map co-sim scenario config.
# Visualization-only: CARLA mirrors every SUMO vehicle; SUMO owns the traffic.
#
# CarlaSetup.EnablePythonBackend selects WHICH VirEnvCore drives the stack:
#   true  -> Carla/VirEnv/mainVirCarla.py  (the Python core, #325)
#   false -> VirCarlaEnv.exe               (the C++ core, #174)
# Both run SUMO + TrafficLayer + a bridge and read this whole file; they exist to
# be compared. Edit it here, or override per run with: run_cosim --engine cpp
# CARLA's own run_synchronization.py is not selectable here - it speaks no FIXS;
# ask for it explicitly with: run_cosim --standalone-bridge
# Regenerate from scratch with --reimport.
#
# What is NOT in this file, on purpose:
#   * the FIXS feed period / SUMO --step-length. Fixed at {FIXS_FEED_S} s by the
#     protocol (CommonLib/FixsProtocol.h): every VirEnvCore host exchanges with
#     TrafficLayer on that grid and TrafficLayer steps SUMO once per exchange.
#     run_cosim passes it to both bridges; it is not a knob.
#   * how the TRAFFIC behaves - lane-change model (--lateral-resolution / sublane),
#     collision checks, seed, demand, begin/end. Those belong in the .sumocfg,
#     SUMO's own format, so both bridges get identical traffic. To make a variant:
#       sumo -c base.sumocfg --lateral-resolution 0.25 \\
#            --save-configuration base_sublane.sumocfg
SimulationSetup:
  EnableRealSim: true
  EnableVerboseLog: false
  SelectedTrafficSimulator: 'SUMO'
  VehicleMessageField: [id, type, vehicleClass, speed, speedDesired, positionX, positionY, positionZ, heading, grade, length, width, height, color, linkId, laneId]
  TrafficSimulatorIP: '127.0.0.1'
  TrafficSimulatorPort: {traci_port}     # SUMO TraCI; run_cosim launches SUMO on this
SumoSetup:
  SpeedMode: 32
  # sumo-gui's --start. true -> the simulation runs as soon as it loads.
  # false -> the window opens loaded but PAUSED and you press Play, so you control
  #          when stepping begins (--sumo-no-start overrides for one run).
  AutoStart: true
ApplicationSetup:
  EnableApplicationLayer: true
  VehicleSubscription:
  - type: ego
    attribute: {{ all: ['true'] }}     # #176: mirror ALL vehicles (no ego anchor)
    ip: ['127.0.0.1']
    port: [{bridge_port}]
{sig}XilSetup:
  EnableXil: false
CarlaSetup:
  # Which VirEnvCore drives the FIXS-native stack. Both read this file in full.
  # true  -> Carla/VirEnv/mainVirCarla.py (Python)   false -> VirCarlaEnv.exe (C++)
  EnablePythonBackend: {use_py}
  EnableVerboseLog: true
  EnableCosimulation: true
  EnableExternalControl: false          # #77 visualization-only
  UseVehicleTypeAsBlueprint: false
  EnableSpectatorFollow: false          # no ego to follow; run_cosim frames the view
  # Pace the co-sim to wall-clock time. Honoured by BOTH bridges (--fast overrides
  # for one run). false = run as fast as the hardware allows.
  RealtimePacing: {rt}
  # The CARLA world step (fixed_delta_seconds) - the analogue of CarMaker's solver
  # dt, and the ONLY timestep to set here. Must be the {FIXS_FEED_S} s feed or an
  # exact divisor of it ({', '.join(str(c) for c in CARLA_TICK_CHOICES)}); anything
  # finer than the feed makes the bridge interpolate traffic across the sub-steps
  # (position AND heading), which is what the CarMaker host has always done at
  # 0.001 s. {tick} s -> {interp}x.
  CarlaTimeStep: {tick}
  # How often traffic poses are re-applied. Same meaning as the CarMaker key: a
  # visual / RPC-cost knob, NOT the feed period. Omit (or 0) = every tick; coarser
  # than the tick = fewer ApplyBatch calls on a heavy scene while physics and
  # sensors still run at the tick rate.
  # TrafficRefreshRate: {tick}
  CarlaServerIP: {carla_host}
  CarlaServerPort: {carla_port}
  CarlaClientIP: 127.0.0.1
  CarlaClientPort: {bridge_port}
"""
    with open(path, "w", encoding="utf-8") as f:
        f.write(text)
    print(f"[cosim] generated VirCarlaEnv config -> {path}"
          + (f" ({len(junctions)} TL junctions)" if junctions else " (no TL sync)"))
    return path


HANDOFF_TIMEOUT_S = 60


def start_app(app, config_yaml=None, timeout=HANDOFF_TIMEOUT_S, sumo_only=False,
              sumocfg=None):
    """Start the app's `launch` command and collect the scenario it reports.

    Returns (proc, sumocfg-or-None). ONE process spans both moments a controller
    cares about: it builds its scenario before SUMO exists, and it subscribes to
    TrafficLayer once that exists. So this starts it, waits for its answer, and hands
    the still-running process back to be supervised with the rest of the stack.

    It answers by writing json to FIXS_HANDOFF - a file, not stdout, because attaching
    a pipe means having to drain it for the whole run or the child blocks on a full
    buffer. Polling for the file is why the app must write it atomically (tmp +
    os.replace); at 4 Hz a non-atomic write WILL eventually be read half-finished.

    Not answering is allowed: an app happy with the bundle's scenario reports {} (or
    nothing at all, and waits out the timeout), so `launch` is usable by an app that
    never learns this protocol exists. Dying before answering is not - that is a crash,
    and continuing would run a stack whose controller is already gone."""
    argv, cwd = app_catalog.launch_command(app)
    if not argv:
        sys.exit(f"[cosim] '{app['id']}' declares launch '{app['launch']}' but it is "
                 f"not there; not running the stack without its controller.")
    # RealSim_tmp, not %TEMP%: it is gitignored, and it is already where the
    # TrafficLayer logs go, so a run that goes wrong has everything in one place.
    handoff_dir = os.path.join(app_catalog.app_root(), "RealSim_tmp")
    os.makedirs(handoff_dir, exist_ok=True)
    handoff = os.path.join(handoff_dir, f"handoff_{app['id']}_{os.getpid()}.json")
    if os.path.isfile(handoff):
        os.remove(handoff)      # a crashed run's leftover is not this run's answer
    # Three variables, and only three. FIXS_PYTHON because a launch command is a
    # script and a script cannot pick an interpreter - while THIS one is the one we
    # re-exec'd into and applied the app's requirements.txt to, so an app that went
    # looking for its own could find a different env that never received them.
    # FIXS_CONFIG_YAML because the app must read the same yaml TrafficLayer gets -
    # the wire format is SimulationSetup.VehicleMessageField, so two yamls that
    # disagree decode the same bytes differently - and which one it is depends on
    # what was chosen from the config menu, so it cannot be a hardcoded basename.
    env = dict(os.environ, FIXS_HANDOFF=handoff, FIXS_PYTHON=sys.executable)
    if config_yaml:
        env["FIXS_CONFIG_YAML"] = config_yaml
    # FIXS_SUMO_ONLY because the yaml cannot answer "is CARLA in this run". Every
    # co-sim yaml has a CarlaSetup section - that is what makes it a co-sim yaml -
    # so an app checking the yaml to decide whether its geometry will be rendered
    # gets the same answer either way, and one that refuses to run a scenario CARLA
    # cannot render would refuse the very run that exists to do it without CARLA.
    # Only run_cosim knows, so run_cosim says.
    if sumo_only:
        env["FIXS_SUMO_ONLY"] = "1"
    # FIXS_SUMOCFG because --sumocfg names the scenario THE USER chose, and an app
    # that generates its own can only honour that by building FROM it. Without this
    # the flag reaches SUMO but never the app, so the only thing it could do to a
    # generating app was bypass it - which for a controller means SUMO running a
    # scenario with no ego in it while the controller waits for one. Apps are free
    # to ignore it, and one that does is bypassed exactly as before.
    if sumocfg:
        env["FIXS_SUMOCFG"] = sumocfg
    print(f"[APP]  {app['launch']}")
    proc = subprocess.Popen(argv, cwd=cwd, env=env)
    deadline = time.time() + timeout
    while not os.path.isfile(handoff):
        if proc.poll() is not None:
            sys.exit(f"[cosim] '{app['id']}' exited ({proc.returncode}) before "
                     f"reporting its scenario; not starting the stack.")
        if time.time() > deadline:
            print(f"[cosim]   ->   '{app['id']}' reported no scenario within "
                  f"{timeout}s; using the map's own.")
            return proc, None
        time.sleep(0.25)
    try:
        with open(handoff, encoding="utf-8") as f:
            reported = (json.load(f) or {}).get("sumocfg")
    except (OSError, ValueError) as exc:
        _kill_pid_tree(proc.pid)
        sys.exit(f"[cosim] '{app['id']}' wrote an unreadable handoff ({exc}); "
                 f"see {handoff}.")
    os.remove(handoff)
    if reported and not os.path.isfile(reported):
        _kill_pid_tree(proc.pid)
        sys.exit(f"[cosim] '{app['id']}' reported a scenario that is not there: "
                 f"{reported}")
    if reported:
        print(f"[cosim] SUMO scenario: '{app['id']}' generated {reported}")
    return proc, reported


def run_native_stack(config_yaml, sumocfg, tl_table, cfg, args, app=None,
                     ctl_sock=None, app_owns_scenario=False, app_proc=None,
                     sumo_only=False, bridge="cpp"):
    """FIXS-native bridge: launch SUMO (TraCI server) + TrafficLayer (-f config) +
    VirCarlaEnv (-f config -t tl_table), plus the app's own `launch` command if it
    declares one. CARLA is already up and the map loaded by run_cosim's preflight.
    Ports mirror tests/Sumo/Probes/TrafficLayer_SUMO_Carla: both ports come from the
    yaml (SimulationSetup.TrafficSimulatorPort and CarlaSetup.CarlaClientPort).
    TrafficLayer is the sole TraCI client, so it steps SUMO (an app controller
    subscribes to TrafficLayer on the application port, it does not drive SUMO).
    Blocks on VirCarlaEnv - the co-sim front-end - and tears the others down on
    exit.

    sumo_only drops VirCarlaEnv and changes nothing else: same SUMO, same
    TrafficLayer, same app controller, same ports. It is for driving a scenario
    CARLA cannot render - a network edited on the SUMO side, or a controller
    change worth checking before anyone cooks a map for it. With no CARLA
    front-end there is nothing to block on, so it blocks on the app's controller
    where there is one and on TrafficLayer where there is not."""
    import shutil
    tl_exe, vce_exe = _native_binaries()
    # Only require what this run will actually start: the bridge it selected, and no
    # bridge at all when there is no CARLA half. The C++ VirCarlaEnv is built only
    # when the libcarla dep is available, so demanding it for a Python-bridge run -
    # or for a SUMO-only one - would refuse a stack that has everything it needs.
    needed = [tl_exe] if sumo_only else [tl_exe, vce_exe if bridge == "cpp" else PY_BRIDGE]
    missing = [q for q in needed if not os.path.isfile(q)]
    if missing:
        sys.exit(f"[cosim] engine {bridge!r} needs these, not found:" + "\n  "
                 + "\n  ".join(missing)
                 + "\nRe-run initialize to fetch the bundle (VirCarlaEnv is built "
                   "only when the libcarla dep is available).")
    sumo_bin = "sumo-gui" if args.sumo_gui else "sumo"
    if not shutil.which(sumo_bin):
        sys.exit(f"[cosim] {sumo_bin} not on PATH (install SUMO / add %SUMO_HOME%/bin).")

    # Clear stale FIXS-side processes before launching (what the reference probe
    # .bat does with taskkill). A previous run closed by its window X - or killed
    # mid-tick - leaves SUMO holding the TraCI port or TrafficLayer holding the bridge
    # port, and the
    # new SUMO then cannot bind: the stack comes up but every component loops on
    # "Could not connect to TraCI server". CARLA is deliberately left alone (it is
    # slow to start and run_cosim's own preflight owns it).
    # Ports come from the scenario yaml, so moving one there moves it for every
    # component. AutoStart only decides whether sumo-gui gets --start.
    traci_port, bridge_port = read_stack_ports(config_yaml)
    sumo_autostart = read_sumo_autostart(config_yaml) and not args.sumo_no_start
    num_clients = read_sumo_num_clients(config_yaml)
    carla_tick, pose_refresh = read_cadence(config_yaml)
    # `bridge`, not a literal: this function serves BOTH VirEnvCores now, and a
    # banner that always said "cpp" contradicted the engine line printed two lines
    # earlier on every Python run.
    print(cadence_banner(bridge, carla_tick, pose_refresh,
                         read_realtime_pacing(config_yaml)))

    for label, port in (("SUMO (TraCI)", traci_port),
                        ("TrafficLayer (bridge)", bridge_port)):
        pid = _pid_on_port(port)
        if not pid:
            continue
        name = _process_name(pid) or "?"
        print(f"[cosim] port {port} still held by {name} (pid {pid}) from an earlier "
              f"run; stopping it so {label} can bind.")
        _kill_pid_tree(pid)
        for _ in range(10):
            if not _port_listening(port):
                break
            time.sleep(0.5)
        if _port_listening(port):
            sys.exit(f"[cosim] port {port} is still in use; close the leftover "
                     f"{name} window and re-run.")

    procs = []          # [(label, Popen)] in start order
    if app_proc is not None:
        # Already running: start_app launched it before SUMO so it could report
        # its scenario, and it is now waiting for TrafficLayer. Adopting it here
        # is the whole integration - supervision and teardown are the ones every
        # other component gets.
        procs.append((app["id"], app_proc))

    def _alive(p):
        return p.poll() is None

    def _check(label, p, hint=""):
        """Report whether a just-started component survived; die loudly if not - a
        silent early exit is the failure mode that reads as 'nothing is happening'."""
        if _alive(p):
            print(f"[cosim]   OK   {label} running (pid {p.pid})")
            return True
        print(f"[cosim]   DEAD {label} exited immediately (code {p.returncode}). {hint}")
        return False

    try:
        # Same builder the python bridge's SUMO gets, so the two engines cannot
        # drift apart in what they hand SUMO (they did: see sumo_launch_cmd).
        sumo_cmd, sumo_origins = sumo_launch_cmd(sumocfg, traci_port, num_clients,
                                                 args.sumo_gui, sumo_autostart, app,
                                                 app_owns_scenario)
        # SUMO blocks until ALL num_clients TraCI clients have connected and called
        # setOrder. TrafficLayer is one of them; anything above 1 means you are
        # attaching your own second client, so say so rather than looking hung.
        if num_clients > 1:
            print(f"[cosim]   ->   SumoSetup.NumClients={num_clients}: SUMO will not "
                  f"step until {num_clients} TraCI clients have connected "
                  f"(TrafficLayer is one).")
        print(f"[SUMO] {' '.join(sumo_cmd)}")
        print_sumo_args(sumo_origins)
        sumo = subprocess.Popen(sumo_cmd)
        procs.append(("SUMO", sumo))
        # SUMO must be listening before TrafficLayer connects as its TraCI client.
        for _ in range(30):
            if _port_listening(traci_port) or not _alive(sumo):
                break
            time.sleep(0.5)
        if not _check("SUMO", sumo, "check the sumocfg path / SUMO install."):
            return 1
        if _port_listening(traci_port):
            print(f"[cosim]   OK   SUMO TraCI listening on {traci_port}")
        else:
            # SUMO opens the TraCI socket before it loads the net, so 15s of silence
            # is a real problem (bad sumocfg, port taken), not a slow GUI.
            print(f"[cosim]   WARN SUMO is up but never opened TraCI {traci_port}; "
                  f"TrafficLayer will not be able to connect.")
        if args.sumo_gui and not sumo_autostart:
            print("[cosim]   ->   sumo-gui started PAUSED (AutoStart off): press Play "
                  "in its window when you want the simulation to run.")

        print(f"[TL]   {os.path.basename(tl_exe)} -f {config_yaml}")
        tl = subprocess.Popen([tl_exe, "-f", config_yaml])
        procs.append(("TrafficLayer", tl))
        # TrafficLayer serves the bridge port; wait for it before starting
        # VirCarlaEnv, which connects to it.
        for _ in range(30):
            if _port_listening(bridge_port) or not _alive(tl):
                break
            time.sleep(0.5)
        if not _check("TrafficLayer", tl,
                      f"check the config yaml (a bad key, or ports {bridge_port}/"
                      f"{traci_port} already in use)."):
            return 1
        if not _port_listening(bridge_port):
            print(f"[cosim]   WARN TrafficLayer is running but port {bridge_port} is not "
                  + ("open yet." if sumo_only else
                     "open yet; VirCarlaEnv may fail to subscribe."))
        else:
            print(f"[cosim]   OK   TrafficLayer serving the bridge on {bridge_port}")

        # What the run blocks on. Normally the CARLA bridge: it is the co-sim
        # front-end, and closing its window is how a run is ended. Without it the
        # longest-lived thing is the app's controller, and with no app either,
        # TrafficLayer.
        if sumo_only:
            front_name, front = (("TrafficLayer", tl) if app_proc is None
                                 else (app["id"], app_proc))
        else:
            # The two bridges take the same two arguments, so nothing downstream --
            # supervision, teardown, the "which component died" report -- has to know
            # which one is running.
            if bridge == "cpp":
                bridge_name = "VirCarlaEnv"
                vce_cmd = [vce_exe, "-f", config_yaml]
            else:
                bridge_name = "mainVirCarla"
                # sys.executable, not a bare "python": run_cosim already re-launched
                # itself under the interpreter that has the carla wheel, and that is
                # the only one the bridge can import carla from.
                vce_cmd = [sys.executable, PY_BRIDGE, "-f", config_yaml]
            if tl_table:
                vce_cmd += ["-t", tl_table]
            print(f"[VCE]  {bridge_name} -f {config_yaml}"
                  + (f" -t {tl_table}" if tl_table else ""))
            vce = subprocess.Popen(vce_cmd)
            procs.append((bridge_name, vce))
            time.sleep(3)   # long enough for a config/CARLA-connection failure to surface
            if not _check(bridge_name, vce,
                          f"check CARLA is reachable at {args.carla_host}:{args.carla_port} "
                          f"and see CarlaClient.log in {os.getcwd()}."):
                return 1
            front_name, front = bridge_name, vce

        if sumo_only:
            print("\n[cosim] SUMO-only stack up: SUMO + TrafficLayer"
                  + (f" + {app['id']}" if app_proc is not None else "") + ", no CARLA.\n"
                  f"[cosim] nothing is rendered - watch it in sumo-gui. Ctrl+C here, or "
                  f"let {front_name} finish, to stop.\n")
        else:
            print(f"\n[cosim] native stack up: SUMO + TrafficLayer + {front_name}"
                  + (f" + {app['id']}" if app_proc is not None else "") + ".\n"
                  "[cosim] vehicles should now appear in the CARLA window. Ctrl+C here, or "
                  f"close {front_name}, to stop.\n")

        # Supervise: block on the front process but surface it if any other
        # component dies first - otherwise a dead TrafficLayer just looks like a freeze.
        import peer
        while _alive(front):
            # The control socket is a component like any other: if the CARLA host
            # hangs up - its Ctrl+C, its CARLA dying, the network going - the feed
            # is over. Without this the first symptom is VirCarlaEnv dying seconds
            # later, which reports the wrong culprit.
            if peer.gone(ctl_sock):
                print("[cosim] the CARLA host disconnected; the feed has stopped. "
                      "Shutting the stack down.")
                break
            dead = [(n, p.returncode) for n, p in procs
                    if n != front_name and not _alive(p)]
            if dead:
                # An app component that ran to its own end is a FINISHED run, not a
                # failure - saying "the feed has stopped" for a clean exit sends
                # people looking for a crash that did not happen.
                if all(rc == 0 for _n, rc in dead):
                    print(f"[cosim] {', '.join(n for n, _ in dead)} finished; "
                          f"stopping the stack.")
                else:
                    broke = ", ".join(f"{n} ({rc})" for n, rc in dead if rc != 0)
                    print(f"[cosim] {broke} exited while the co-sim was running; "
                          f"the feed has stopped. Shutting the stack down.")
                break
            time.sleep(1.0)
        rc = front.poll()
        if rc is None:
            rc = 0
        else:
            print(f"[cosim] {front_name} exited ({rc}); stopping the stack.")
        return rc
    finally:
        for name, p in reversed(procs):
            if p.poll() is None:
                print(f"[cosim] stopping {name} ...")
                try:
                    p.terminate()
                except Exception:
                    pass
        for name, p in reversed(procs):
            try:
                p.wait(timeout=5)
            except Exception:
                # The app's launch command is a SCRIPT: on Windows terminate()/kill()
                # reach cmd.exe and leave the interpreter it started running, holding
                # the application port against the next run. Everything else here is
                # an exe with no children, so the tree kill is only needed for this
                # one - but for this one a plain kill is not a teardown.
                try:
                    _kill_pid_tree(p.pid)
                except Exception:
                    pass
                try:
                    p.kill()
                except Exception:
                    pass


def run_python_bridge(config_yaml, sumocfg, tl_table, tls_manager, no_net_offset,
                      args, app=None, app_owns_scenario=False, app_proc=None):
    """Standalone bridge: launch SUMO (TraCI server) + run_synchronization.py.

    run_cosim launches SUMO here rather than letting the bridge start its own, for
    one reason: sumo_launch_cmd then spells the SUMO command line exactly ONCE for
    both engines. It used to be spelled three times - here (implicitly, inside
    CARLA's SumoSimulation, which injected --lateral-resolution 0.25 and
    --collision.check-junctions), again in run_native_stack, and again in
    TrafficLayer's auto-launch - which is how the two bridges ended up running
    different traffic without anyone choosing that. The bridge attaches as a client
    via --sumo-host/--sumo-port, which it already supported.

    Cadence and pacing come from the scenario yaml, the same file the native stack
    reads, so switching --engine changes the bridge and nothing else."""
    # A release bundle has no standalone/ -- that is the point of #330 -- so say
    # so plainly instead of failing on a missing file.
    if not os.path.isfile(SYNC):
        sys.exit(os.linesep.join([
            f"[cosim] --standalone-bridge needs {SYNC}, which is not here.",
            "        It ships only in a source checkout: it speaks no FIXS, so it",
            "        is deliberately not part of the release bundle (#330). Drop",
            "        the flag to run the FIXS stack, which is what this is for.",
        ]))

    import shutil
    sumo_bin = "sumo-gui" if args.sumo_gui else "sumo"
    if not shutil.which(sumo_bin):
        sys.exit(f"[cosim] {sumo_bin} not on PATH (install SUMO / add %SUMO_HOME%/bin).")

    traci_port, _bridge_port = read_stack_ports(config_yaml)
    sumo_autostart = read_sumo_autostart(config_yaml) and not args.sumo_no_start
    num_clients = read_sumo_num_clients(config_yaml)
    carla_tick, pose_refresh = read_cadence(config_yaml)
    realtime = read_realtime_pacing(config_yaml)
    # "standalone", not "py": py now means the Python VirEnvCore, which is the
    # FIXS-native stack above. This is CARLA's own bridge (#330).
    print(cadence_banner("standalone", carla_tick, pose_refresh, realtime))

    # A leftover SUMO from a killed run still holds the TraCI port, and the new one
    # then cannot bind - same failure the native stack guards against.
    pid = _pid_on_port(traci_port)
    if pid:
        name = _process_name(pid) or "?"
        print(f"[cosim] port {traci_port} still held by {name} (pid {pid}) from an "
              f"earlier run; stopping it so SUMO can bind.")
        _kill_pid_tree(pid)
        for _ in range(10):
            if not _port_listening(traci_port):
                break
            time.sleep(0.5)

    sumo_cmd, sumo_origins = sumo_launch_cmd(sumocfg, traci_port, num_clients,
                                             args.sumo_gui, sumo_autostart, app,
                                             app_owns_scenario)
    print(f"[SUMO] {' '.join(sumo_cmd)}")
    print_sumo_args(sumo_origins)
    sumo = subprocess.Popen(sumo_cmd)
    try:
        for _ in range(30):
            if _port_listening(traci_port) or sumo.poll() is not None:
                break
            time.sleep(0.5)
        if sumo.poll() is not None:
            print(f"[cosim]   DEAD SUMO exited immediately (code {sumo.returncode}); "
                  f"check the sumocfg path / SUMO install.")
            return 1
        if _port_listening(traci_port):
            print(f"[cosim]   OK   SUMO TraCI listening on {traci_port}")
        if args.sumo_gui and not sumo_autostart:
            print("[cosim]   ->   sumo-gui started PAUSED (AutoStart off): press Play "
                  "in its window when you want the simulation to run.")

        cmd = [sys.executable, SYNC, sumocfg,
               "--tls-manager", tls_manager,
               "--sumo-host", "127.0.0.1", "--sumo-port", str(traci_port),
               "--step-length", str(FIXS_FEED_S),
               "--carla-tick", str(carla_tick),
               "--carla-host", args.carla_host, "--carla-port", str(args.carla_port),
               "--carla-timeout", str(args.carla_timeout)]
        if tl_table:
            cmd += ["--tl-table", tl_table]
        if no_net_offset:
            cmd.append("--no-net-offset")
        if not realtime:
            cmd.append("--no-realtime")
        print(f"[SYNC] {' '.join(cmd)}")
        return subprocess.call(cmd)
    finally:
        if sumo.poll() is None:
            print("[cosim] stopping SUMO ...")
            try:
                sumo.terminate()
                sumo.wait(timeout=5)
            except Exception:
                try:
                    sumo.kill()
                except Exception:
                    pass


def resolve_carla_env(reconfigure=False):
    """Return the saved CARLA env, running the setup prompt if missing/forced."""
    cfg = None if reconfigure else env.load_config()
    if cfg is None:
        if reconfigure:
            print("[cosim] reconfiguring CARLA env ...")
        else:
            print(f"[cosim] no CARLA env configured ({env.CONFIG_PATH}); "
                  "running first-time setup ...")
        cfg = env.run_setup()
    return cfg


# ensure_runtime and maybe_reexec used to live here, as run_cosim's private copies.
# Both now belong to carla_env_setup, the module that owns carla.json:
# env.ensure_runtime because --update-python needs the same work on demand and a
# second copy is how a repair path and an update path drift apart, and
# env.reexec_under_configured because every entry point - import_map, place_tls,
# place_signs, the world/spectator helpers - has to obey the same rule, and the
# rule was enforced only by whichever script you happened to start with.


def confirm_world_ready(client, expected_map, timeout):
    """After load_world, block until the loaded world IS the expected map AND it
    is producing frames (ticking). On a heavy source build, load_world can return
    while the map is still switching/streaming - this makes sure we don't start
    SUMO against the wrong or half-loaded world. Returns the map name, or None."""
    deadline = time.time() + timeout
    last = ""
    while time.time() < deadline:
        try:
            world = client.get_world()
            last = world.get_map().name
            if expected_map.lower() in last.lower():
                world.wait_for_tick(10.0)  # the server is actually producing frames
                return last
        except RuntimeError:
            pass  # server busy (e.g. compiling shaders) - retry
        time.sleep(1.0)
    return None


def _unreal_log_dir(cfg):
    """Where the editor writes CarlaUE4.log for this build, or None."""
    root = (cfg or {}).get("carla_root")
    if not root:
        return None
    d = os.path.join(root, "Unreal", "CarlaUE4", "Saved", "Logs")
    return d if os.path.isdir(d) else None


def report_unreal_crash(cfg):
    """Print the newest Unreal log's fatal lines, and its path.

    A CARLA that dies during LoadMap leaves everything needed to diagnose it in
    that file - the map it was loading and the exception - while the console says
    only that a port did not open. Reading three greps out of it here is the
    difference between a one-line answer and an afternoon.
    """
    d = _unreal_log_dir(cfg)
    if not d:
        return
    logs = [os.path.join(d, f) for f in os.listdir(d) if f.lower().endswith(".log")]
    if not logs:
        return
    newest = max(logs, key=os.path.getmtime)
    print(f"[cosim]   its log: {newest}")
    try:
        with open(newest, encoding="utf-8", errors="replace") as f:
            lines = f.readlines()
    except OSError:
        return
    keep = [ln.rstrip() for ln in lines
            if ("LoadMap:" in ln
                or "Critical error" in ln
                or "Fatal error" in ln
                or "Unhandled Exception" in ln)]
    for ln in keep[-6:]:
        print(f"[cosim]   | {ln.strip()}")


def wait_for_port(host, port, timeout=180, proc=None, cfg=None):
    """True once `port` accepts. False on timeout, or as soon as `proc` is gone.

    Watching the process matters: a server that crashed at second 13 and one still
    compiling shaders at second 179 are the same to a socket poll, and reporting
    both as a timeout sends you to wait longer for something that is not running.
    """
    deadline = time.time() + timeout
    while time.time() < deadline:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(2)
            if s.connect_ex((host, port)) == 0:
                return True
        if proc is not None and proc.poll() is not None:
            print(f"[cosim] the CARLA server exited ({proc.returncode}) before "
                  f"opening port {port} - it did not time out, it stopped.")
            report_unreal_crash(cfg)
            return False
        time.sleep(2)
    return False


def _carla_command(cfg, port, render_offscreen, quality_level=None, level=None):
    """Build the server launch command for a packaged build or source editor."""
    if cfg["mode"] == "packaged":
        exe = env.packaged_exe(cfg["carla_root"])
        if exe is None:
            raise FileNotFoundError(
                f"No CarlaUE4 launcher under {cfg['carla_root']}. "
                "Re-run carla_env_setup.py (or --reconfigure) to fix the path.")
        cmd = [exe, f"-carla-rpc-port={port}"]
    else:  # source build, launched through the Unreal editor in -game mode
        uproject, editor = env.source_paths(cfg["carla_root"], cfg["ue4_root"])
        if not os.path.isfile(editor) or not os.path.isfile(uproject):
            raise FileNotFoundError(
                f"Source CARLA not found (editor={editor}, uproject={uproject}). "
                "Re-run carla_env_setup.py (or --reconfigure) to fix the paths.")
        cmd = [editor, uproject]
        # Boot straight into the map we are about to drive. Without this the engine
        # browses to the project's GameDefaultMap, which is a per-build setting we
        # do not own - if it names a map that has since been removed, startup dies
        # with "Failed to enter <map>: Can't find file" before the RPC port opens.
        if level:
            cmd.append(level)
        cmd += ["-game", f"-carla-rpc-port={port}", *env.EDITOR_LAUNCH_FLAGS]
    if quality_level:
        cmd.append(f"-quality-level={quality_level}")  # Low is much cheaper to render
    if render_offscreen:
        cmd.append("-RenderOffScreen")  # headless, no display (Linux/CI)
    return cmd


def launch_carla(cfg, port=2000, render_offscreen=False, quality_level=None, level=None):
    cmd = _carla_command(cfg, port, render_offscreen, quality_level, level)
    print(f"[CARLA] launching ({cfg['mode']}): {' '.join(cmd)}")
    if platform.system() == "Windows":
        return subprocess.Popen(cmd)
    return subprocess.Popen(cmd, preexec_fn=os.setsid)


def report_traffic_lights(world, tls_manager, cfg, target_map):
    """Say whether the running CARLA actually exposes traffic lights to drive.

    This is the check that otherwise fails silently and late. SUMO's light sync
    drives `traffic.traffic_light` actors; if the world has none, the co-sim comes
    up healthy, runs to completion, and the signals simply never change. Nothing
    errors, so it reads as a SUMO problem or as "the lights look fine to me".

    Zero actors means one of two things, and both are worth naming because the fix
    differs:

      * the map was never prepped - no placement has run against it
      * a prop was placed whose parent class is not ATrafficLightBase, so CARLA
        renders the heads but never exposes them as traffic lights

    It matters most in client mode. There the CARLA is on another machine, so the
    cook/place preflights here are deliberately skipped, and this connection is the
    FIRST point at which this machine can observe whether that machine was ever
    prepared.
    """
    if tls_manager != "sumo":
        return None
    try:
        count = len(world.get_actors().filter("traffic.traffic_light*"))
    except Exception as exc:  # noqa: BLE001 - diagnostics must not break the run
        print(f"[cosim] could not query traffic lights ({exc}); continuing.")
        return None

    if count:
        print(f"[cosim] CARLA exposes {count} traffic.traffic_light actor(s) for the "
              f"SUMO sync.")
        return count

    mode = (cfg or {}).get("mode")
    print(f"[cosim] WARNING: '{target_map}' is loaded but CARLA exposes NO "
          f"traffic.traffic_light actors, so the SUMO light sync has nothing to "
          f"drive - the run will look healthy and the signals will never change.")
    if mode == "client":
        print(f"[cosim]          CARLA is on another machine, so placement had to "
              f"happen there. On the CARLA host run:")
        print(f"[cosim]              python run_cosim.py --map {target_map} --prep-only")
        print(f"[cosim]          (that machine needs carla.json in 'source' mode - a "
              f"cook and a level save need the editor and the content tree.)")
    else:
        print(f"[cosim]          Run:  python place_tls.py --map {target_map} "
              f"--tl-table <table> --force")
    print("[cosim]          If placement HAS run, the prop's parent class is not "
          "ATrafficLightBase - CARLA never exposes such an actor as a traffic light.")
    return 0


def _frame_from_actors(world):
    """Centroid + span (m) of the placed traffic-light actors, in CARLA coords."""
    tls = world.get_actors().filter("traffic.traffic_light*")
    locs = [t.get_transform().location for t in tls]
    if not locs:
        return None
    xs = [l.x for l in locs]; ys = [l.y for l in locs]; zs = [l.z for l in locs]
    return _frame_stats(xs, ys, zs, f"{len(locs)} traffic lights")


def _frame_from_map(world):
    """Centroid + span (m) of the map's spawn points - the last-resort anchor for a
    map with no signals at all (a RoadRunner export without them, or a bare
    generate_opendrive_world road). Spawn points sit on the drivable network and
    span it, so this frames the whole map rather than any one place in it.

    Without this the no-signal case left the spectator wherever the server put it,
    which in -game mode is the world origin - so the map rendered as a distant
    speck and looked like a failed load."""
    try:
        spawns = world.get_map().get_spawn_points()
    except RuntimeError:
        return None          # map not queryable yet; the caller just skips framing
    if not spawns:
        return None
    xs = [s.location.x for s in spawns]
    ys = [s.location.y for s in spawns]
    zs = [s.location.z for s in spawns]
    return _frame_stats(xs, ys, zs, f"map centre ({len(spawns)} spawn points)")


def _table_junctions(tl_table, no_net_offset):
    """junction_id -> list of (x, y, z) CARLA-coord points from the TL table.
    With --no-net-offset (RoadRunner-local maps) SUMO (x, y) -> CARLA (x, -y)."""
    import csv
    juncs = {}
    try:
        with open(tl_table, newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                x = float(row["x"])
                y = -float(row["y"]) if no_net_offset else float(row["y"])
                z = float(row.get("z") or 0.0)
                juncs.setdefault(row["junction_id"], []).append((x, y, z))
    except (OSError, KeyError, ValueError):
        return {}
    return juncs


def _frame_from_table(tl_table, no_net_offset, junction=None, whole=False):
    """Frame from the TL table. By default zooms to ONE intersection (the busiest
    by signal-head count) so the signal sync is legible; whole=True frames the
    entire network; junction=<id> targets a specific intersection."""
    juncs = _table_junctions(tl_table, no_net_offset)
    if not juncs:
        return None
    if whole:
        pts = [p for pts in juncs.values() for p in pts]
        anchor = f"{len(juncs)} junctions (network)"
    elif junction is not None:
        if junction not in juncs:
            print(f"[VIEW] junction '{junction}' not in table; using the busiest one")
            junction = None
        pts = juncs.get(junction) or max(juncs.values(), key=len)
        anchor = f"junction {junction}" if junction else "busiest junction"
    else:
        jid = max(juncs, key=lambda j: len(juncs[j]))
        pts = juncs[jid]
        anchor = f"junction {jid} ({len(pts)} heads)"
    xs = [p[0] for p in pts]; ys = [p[1] for p in pts]; zs = [p[2] for p in pts]
    return _frame_stats(xs, ys, zs, anchor)


def _frame_stats(xs, ys, zs, anchor):
    cx, cy, cz = sum(xs) / len(xs), sum(ys) / len(ys), sum(zs) / len(zs)
    span = max(max(xs) - min(xs), max(ys) - min(ys), 1.0)
    return cx, cy, cz, span, anchor


def position_spectator(world, frame, pitch=-55.0):
    """Point the spectator at the anchor from an angled overhead view. Height is
    scaled to the anchor's span (so a single intersection is seen up close), and
    the camera is offset back along -X so the anchor sits in the view centre."""
    import math
    import carla
    cx, cy, cz, span, anchor = frame
    height = min(max(span * 1.2, 35.0), 600.0)
    back = height / math.tan(math.radians(-pitch))  # so the look ray hits (cx, cy)
    world.get_spectator().set_transform(carla.Transform(
        carla.Location(x=cx - back, y=cy, z=cz + height),
        carla.Rotation(pitch=pitch, yaw=0.0, roll=0.0)))
    print(f"[VIEW] spectator -> ({cx - back:.0f}, {cy:.0f}, {cz + height:.0f}) "
          f"looking at ({cx:.0f}, {cy:.0f}), span~{span:.0f}m, anchored on {anchor}")


def _port_in_use(host, port):
    """True if `host:port` accepts a TCP connection.

    This OPENS AND DROPS A REAL CONNECTION, so it is only safe against servers
    that accept many clients (CARLA's RPC port). For the single-client servers in
    the native stack use _port_listening() instead - see the note there."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(1)
        return s.connect_ex((host, port)) == 0


def _port_listening(port):
    """True if something is LISTENING on `port`, observed WITHOUT connecting.

    Both servers in the native stack accept exactly ONE client and stop listening
    the moment they get it: SUMO's TraCI server defaults to --num-clients 1, and
    TrafficLayer counts the first accept on its bridge port as VirCarlaEnv. A
    connect() readiness probe against those is not a passive observation - it
    *consumes* the one client slot and then hangs up, which left SUMO holding a
    CLOSE_WAIT socket with no listener and TrafficLayer looping forever on
    "Could not connect to TraCI server at 127.0.0.1:1337" while TrafficLayer had
    already logged a phantom "Handling client #1 / All Clients Connected!" before
    VirCarlaEnv was even launched. Reading the OS socket table has no such side
    effect, so readiness polling is free to be as chatty as it likes."""
    return _pid_on_port(port) is not None


def _is_local_host(host):
    """True if `host` names this machine, so CARLA there is ours to launch/kill."""
    h = (host or "").strip().lower()
    if h in ("", "localhost", "127.0.0.1", "::1", "0.0.0.0"):
        return True
    try:
        if socket.gethostbyname(h) in ("127.0.0.1", "::1") \
                or h == socket.gethostname().lower():
            return True
        # Also match this machine's own LAN addresses. Without this, pointing
        # CarlaSetup.CarlaServerIP at your own 192.168.x.x looked like a perfectly
        # ordinary remote host: run_cosim implied --no-launch, waited out the full
        # connect timeout against itself, and reported nothing useful.
        return socket.gethostbyname(h) in _own_addresses()
    except OSError:
        return False


def _own_addresses():
    """Every IPv4 address this machine answers on, best effort."""
    addrs = {"127.0.0.1"}
    try:
        host = socket.gethostname()
        addrs.update(ai[4][0] for ai in socket.getaddrinfo(host, None, socket.AF_INET))
    except OSError:
        pass
    return addrs


def _pid_on_port(port):
    """PID LISTENING on `port` (any host), or None. Windows netstat / Linux lsof."""
    try:
        if platform.system() == "Windows":
            out = subprocess.check_output(["netstat", "-ano", "-p", "tcp"],
                                          text=True, stderr=subprocess.DEVNULL)
            for line in out.splitlines():
                p = line.split()
                if len(p) >= 5 and p[3].upper() == "LISTENING" and p[1].endswith(f":{port}"):
                    return int(p[-1])
        else:
            out = subprocess.check_output(["lsof", "-ti", f"tcp:{port}", "-sTCP:LISTEN"],
                                          text=True, stderr=subprocess.DEVNULL)
            return int(out.split()[0]) if out.strip() else None
    except Exception:
        return None
    return None


def _process_name(pid):
    try:
        if platform.system() == "Windows":
            out = subprocess.check_output(["tasklist", "/FI", f"PID eq {pid}", "/FO", "CSV", "/NH"],
                                          text=True, stderr=subprocess.DEVNULL).strip()
            return out.splitlines()[0].split(",")[0].strip('"') if out else ""
        return subprocess.check_output(["ps", "-p", str(pid), "-o", "comm="],
                                       text=True, stderr=subprocess.DEVNULL).strip()
    except Exception:
        return ""


def _is_carla_process(name):
    n = (name or "").lower()
    return "ue4editor" in n or "carlaue4" in n


def _carla_pid_on(port):
    """PID of a CARLA holding `port`, or None. The two questions - who is on the
    port, and is that a CARLA - are always asked together, and callers that only
    want the second should not have to know about the first."""
    pid = _pid_on_port(port)
    return pid if pid and _is_carla_process(_process_name(pid)) else None


def _kill_pid_tree(pid):
    """Kill a PID and its whole process tree (CARLA spawns shader workers / a game
    child / CrashReportClient that a plain kill leaves holding the RPC port)."""
    try:
        if platform.system() == "Windows":
            subprocess.call(["taskkill", "/F", "/T", "/PID", str(pid)],
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        else:
            # SIGTERM first, but do NOT assume it lands: UE4Editor frequently
            # ignores it (or takes tens of seconds over its own shutdown), which
            # leaves the window up and the RPC port held - so the next run finds
            # a "stale CARLA" it has to kill anyway. Escalate to SIGKILL once the
            # polite request has clearly been declined.
            try:
                pgid = os.getpgid(pid)
            except Exception:
                pgid = None
            def _signal(sig):
                if pgid is not None:
                    os.killpg(pgid, sig)
                else:
                    os.kill(pid, sig)
            _signal(signal.SIGTERM)
            grace = float(os.environ.get("FIXS_SHUTDOWN_GRACE", "20"))
            waited = 0.0
            while waited < grace:
                time.sleep(0.5)
                waited += 0.5
                try:
                    os.kill(pid, 0)                  # still there?
                except OSError:
                    return                           # went quietly
            # Deliberately NOT "ignored SIGTERM": UE4 logs RequestExit and then
            # spends real time tearing down a loaded map with hundreds of spawned
            # actors, so it is usually mid-shutdown rather than unresponsive. We
            # stop waiting because the RPC port must be free before the next run,
            # not because the process misbehaved.
            print(f"[cosim] PID {pid} still shutting down after {grace:g}s; "
                  f"sending SIGKILL to free the port "
                  f"(FIXS_SHUTDOWN_GRACE to change the wait).")
            try:
                _signal(signal.SIGKILL)
            except Exception:
                pass
    except Exception:
        pass


def kill_carla(proc):
    _kill_pid_tree(proc.pid)


def _net_from_sumocfg(sumocfg):
    """Absolute path to the <net-file> referenced by a .sumocfg, or None."""
    import xml.etree.ElementTree as ET
    try:
        nf = ET.parse(sumocfg).getroot().find(".//net-file")
        if nf is not None and nf.get("value"):
            return os.path.join(os.path.dirname(os.path.abspath(sumocfg)), nf.get("value"))
    except Exception:
        pass
    return None


def resolve_tl_table(sumocfg, force=False, cache_name=None):
    """Generate this scenario's traffic-light table from the SUMO net. It is cached in
    the map's per-map folder ~/.fixs/maps/<cache_name>/tl_table.csv (cache_name = the
    cooked map name), else the shared ~/.fixs/tables/<net>_tls.csv. `force`
    regenerates even if a cache exists (used on --reimport). Returns a path, or None
    if there is no net to generate from. Generation needs pandas/shapely but not SUMO
    installed.

    A traffic_light_table.csv sitting beside the sumocfg is deliberately NOT read. It
    used to win over generation, which tied head positions to whichever file happened
    to be next to the scenario rather than to the net: an app-owned scenario folder
    copied into the map cache brought its own table along, and mlk_untextured was
    baked from that stale copy - heads fanned across the full lane width - while the
    generator was producing the current layout into tl_table.csv, unused. The table is
    derived from the net, so the net is the only thing it should depend on.
    """
    net = _net_from_sumocfg(sumocfg)
    if not net or not os.path.isfile(net):
        print("[cosim] no TL table and no net to generate one from; TL sync off.")
        return None
    maps_root = os.path.join(os.path.dirname(env.CONFIG_PATH), "maps")
    if cache_name:
        cache = os.path.join(maps_root, cache_name)
        out = os.path.join(cache, "tl_table.csv")
    else:
        cache = os.path.join(os.path.dirname(env.CONFIG_PATH), "tables")
        out = os.path.join(cache, os.path.splitext(os.path.basename(net))[0] + "_tls.csv")
    if os.path.isfile(out) and not force:
        print(f"[cosim] TL table: cached generated {out}")
        return out
    try:
        sys.path.insert(0, os.path.join(HERE, "utils"))
        import extract_sumo_tls_as_table as gen
        os.makedirs(cache, exist_ok=True)
        gen.generate_table(net, out)
        print(f"[cosim] TL table: generated from {os.path.basename(net)} -> {out}")
        return out
    except Exception as exc:
        print(f"[cosim] could not generate TL table ({exc}); TL sync off.")
        return None


# --------------------------------------------------------------------------- #
# Application + saved run profile.
#
# Two features that only make sense together: apps/apps.json says WHAT can be run
# (and with which maps / scenario yamls), the run profile remembers WHAT YOU RAN
# so the next run is one keypress. Both are optional - no manifest and no profile
# gives exactly the pre-app-awareness behaviour.
#
# Precedence for every setting, strongest first:
#   explicit CLI flag  >  saved profile  >  app defaults  >  built-in default
# The CLI wins outright so `run_cosim --map atlanta` changes that one thing and
# prompts for nothing.
# --------------------------------------------------------------------------- #
# The setup deliberately owns no timestep. It used to carry step_length, which was
# handed to SUMO *and* written into the scenario yaml - two owners for one number,
# so they silently disagreed (a 0.05 s setup against the 0.1 s FIXS feed ran SUMO at
# half the CARLA clock). The feed is now a protocol constant and the CARLA tick lives
# in the yaml beside the rest of the cadence; the setup keeps only the choice of
# artifacts, plus whether SUMO gets a window. The CARLA ENDPOINT left for the same
# reason - the yaml is what VirCarlaEnv dials - which is why there is no slot -> args
# table here any more: run_profile.SLOTS names the rows, and each editor writes to
# whichever file actually owns its setting.
#
# The three with no sensible default: they must be answered before a first run.
# engine / carla / sumo always have one (the yaml, carla.json, the built-ins), so
# they start filled in and are edited only if you ask for them.
MUST_ANSWER = ("app", "map", "config")


def _interactive(args):
    """Whether THIS run is allowed to stop and ask a human. Asked once, here.

    A terminal is necessary but not sufficient. --serve has one and must still not
    prompt: the traffic machine already chose, and the person who could answer is
    sitting at the OTHER keyboard. Helpers that re-derive this from
    sys.stdin.isatty() are answering a different question - "is anyone reachable?"
    instead of "is anyone expected?" - and prompt anyway. That is how a serving
    host came to sit on the scenario menu while its peer printed "still waiting -
    it is launching CARLA and loading the map". It was not. Nobody was there.

    So: pass this value down. isatty() is the DEFAULT for `interactive`, never the
    authority."""
    return sys.stdin.isatty() and not getattr(args, "serve", False)


def _ask(prompt, default=None):
    """input() that treats EOF as 'take the default' rather than exploding."""
    try:
        return input(prompt).strip()
    except EOFError:
        return default or ""


def _config_menu(who, options, current=None, interactive=None):
    """The scenario row: pick a yaml, or edit the current one in place.

    Picking a file was the only thing this offered, and with a single generated
    yaml - the normal case - that meant selecting the row did nothing visible at
    all. What people want here is usually to change a value, not to switch files,
    so 'e' is on the same list as the files.

    `current` is the yaml the SETUP is running, and it has to be passed in: taking
    the first row instead put "(current)" on a file the setup was not using, so
    Enter ("keep") silently switched the scenario, and 'e' opened the wrong file.
    The caller has already matched it against the list, so a plain `in` holds."""
    paths = [p for p, _ in options]
    # A new setup, or one whose scenario was just invalidated, is running nothing
    # yet. The first row is still what Enter takes, but calling that "(current)"
    # would be inventing a state the setup does not have.
    known = current in paths
    current = current if known else options[0][0]
    while True:
        print(f"\n[cosim] Scenario config for {who}:")
        for i, (path, label) in enumerate(options, 1):
            mark = ("  (current)" if known else "  (default)") if path == current else ""
            print(f"   {i}) {label}{mark}")
        print("   ---")
        print("   e) edit the current one in your editor")
        if not (sys.stdin.isatty() if interactive is None else interactive):
            return current
        ans = _ask(f"[cosim] Which? [1-{len(options)} / e], Enter = keep "
                   f"{os.path.basename(current)}: ").strip().lower()
        if ans == "":
            return current
        if ans == "e":
            edit_yaml_in_editor(current)
            continue                       # redraw: the file may now say something else
        if ans.isdigit() and 1 <= int(ans) <= len(options):
            current = options[int(ans) - 1][0]
            print(f"[cosim] scenario config: {current}")
            return current
        print("[cosim] enter a number from the list, or e to edit.")


class _Parser(argparse.ArgumentParser):
    """argparse, minus the wall of text on a mistake.

    The default error() prints the whole usage line first - here, 47 options
    wrapping over twelve lines - and then one sentence saying what was actually
    wrong. Forgetting a value for --sumocfg should not answer with every flag the
    engine has; it buries the one line that matters and it is the same dump the
    wrapper's --help exists to avoid. Say what is wrong, then where to look."""

    def error(self, message):
        sys.stderr.write(f"\n[cosim] {message}\n\n"
                         f"        run_cosim --help          the common options\n"
                         f"        run_cosim.py --help       every engine option\n")
        sys.exit(2)


def _open_in_editor(path):
    """Open `path` in whatever editor this machine has. True if something started.

    Order matters. $VISUAL / $EDITOR first: they are set deliberately and they
    work with no display - which is the render host over SSH, exactly where
    someone is most likely to be poking at a config with no GUI. Then the
    platform default, and a terminal editor last so a headless box is never left
    with nothing."""
    env_editor = os.environ.get("VISUAL") or os.environ.get("EDITOR")
    if env_editor:
        try:
            subprocess.Popen([env_editor, path])
            return True
        except Exception:
            pass
    try:
        if platform.system() == "Windows":
            try:
                os.startfile(path)                  # the file association
                return True
            except Exception:
                subprocess.Popen(["notepad", path])
                return True
        if platform.system() == "Darwin":
            subprocess.Popen(["open", path])
            return True
        if os.environ.get("DISPLAY") or os.environ.get("WAYLAND_DISPLAY"):
            subprocess.Popen(["xdg-open", path])
            return True
        for term in ("nano", "vi"):
            if shutil.which(term):
                subprocess.call([term, path])       # blocks, and should
                return True
    except Exception:
        pass
    return False


def _yaml_snapshot(path):
    """Every scalar in the scenario yaml as {Block.Key: text}, for diffing an edit.

    Deliberately dumb - two-level indent is what this file is - because the point
    is to SHOW what changed, not to re-implement a parser."""
    snap, block = {}, ""
    try:
        with open(path, encoding="utf-8-sig") as f:
            for line in f:
                raw = line.rstrip("\n")
                if not raw.strip() or raw.lstrip().startswith("#"):
                    continue
                if not raw.startswith((" ", "\t")) and raw.rstrip().endswith(":"):
                    block = raw.strip().rstrip(":")
                elif ":" in raw and raw.startswith((" ", "\t")):
                    k, v = raw.split(":", 1)
                    v = v.split("#", 1)[0].strip()
                    if v:
                        snap[f"{block}.{k.strip()}"] = v
    except OSError:
        pass
    return snap


def _yaml_problems(path):
    """Validation that would otherwise only bite at runtime."""
    out = []
    tick = _yaml_float(path, "CarlaSetup", "CarlaTimeStep", 0.0)
    if tick:
        slots = FIXS_FEED_S / tick
        if tick > FIXS_FEED_S + 1e-9 or abs(slots - round(slots)) > 1e-6:
            out.append(f"CarlaSetup.CarlaTimeStep = {tick:g} - must be the FIXS feed "
                       f"({FIXS_FEED_S:g} s) or an exact divisor of it "
                       f"({', '.join(str(c) for c in CARLA_TICK_CHOICES)}), else no "
                       f"CARLA tick lands on an exchange boundary and the bridge "
                       f"trades nothing.")
    return out


def edit_yaml_in_editor(path):
    """Open the scenario yaml, wait for the user, then re-read and report.

    Do NOT wait on the editor process: notepad blocks, `code` returns instantly,
    vi blocks in the terminal. The Enter prompt covers all three, and it is also
    what lets someone keep the editor open while fixing a validation failure.

    The yaml is written to be READ - every block carries comments saying what a
    key means and what breaks if it is wrong - so handing the whole file to an
    editor beats prompting key by key, which shows a bare value and throws the
    explanation away."""
    before = _yaml_snapshot(path)
    print(f"\n[cosim] opening {os.path.basename(path)} in your editor ...\n"
          f"        {path}\n"
          f"        View it, change it, save it. You can leave the editor open.")
    if not _open_in_editor(path):
        print("[cosim] could not launch an editor - open the path above yourself.")
    while True:
        ans = _ask("[cosim] Press Enter when you have saved  (Q = leave it): ").lower()
        if ans.startswith("q"):
            return
        after = _yaml_snapshot(path)
        changed = [(k, before.get(k), after[k]) for k in sorted(after)
                   if before.get(k) != after[k]]
        gone = [k for k in sorted(before) if k not in after]
        if changed or gone:
            print(f"[cosim] {os.path.basename(path)} re-read:")
            for k, was, now in changed:
                print(f"          {k:<36} {was if was is not None else '(new)'}  ->  {now}")
            for k in gone:
                print(f"          {k:<36} removed")
        else:
            print(f"[cosim] {os.path.basename(path)} is unchanged.")
        # Validate HERE, not at runtime in C++. mainVirCarla already refuses a tick
        # that does not divide the feed - but only once a run has started, by which
        # point the editor is closed and the reason is a wall away.
        bad = _yaml_problems(path)
        if _read_scenario_config(path) is None:
            bad.append(f"{os.path.basename(path)} could not be parsed - check the "
                       f"indentation of what you just edited.")
        if not bad:
            before = after
            return
        for msg in bad:
            print(f"[cosim] FAIL {msg}")
        print("[cosim] Fix it and press Enter to re-check, or Q to leave it as is.")


def _menu(title, options, current=None):
    """Numbered single-choice menu. `options` is [(value, label)]. Enter keeps
    `current` when it is one of the values, else takes the first. Returns a value."""
    values = [v for v, _ in options]
    idx = values.index(current) + 1 if current in values else 1
    print(f"\n[cosim] {title}")
    for i, (value, label) in enumerate(options, 1):
        mark = "  (current)" if value == current else ""
        print(f"   {i}) {label}{mark}")
    if not sys.stdin.isatty():
        return values[idx - 1]
    while True:
        ans = _ask(f"[cosim] Which? [1-{len(options)}], Enter = {idx}: ")
        if ans == "":
            return values[idx - 1]
        if ans.isdigit() and 1 <= int(ans) <= len(options):
            return values[int(ans) - 1]
        print("[cosim] invalid choice; enter a number from the list.")


# --------------------------------------------------------------------------- #
# Slot editors. One per line of the summary, so every line the menu offers is a
# line the menu can actually change.
#
# Two of them - the bridge and the CARLA endpoint - write to the SCENARIO YAML
# rather than to the saved setup, because that yaml is what the engine actually
# reads: TrafficLayer and VirCarlaEnv take CarlaSetup.* straight from it. A copy
# of those values kept anywhere else is a second source of truth, and it goes
# wrong in both directions - hand-editing the yaml stops having any effect, and a
# stale saved value makes run_cosim probe one CARLA while VirCarlaEnv dials
# another. So the yaml owns them, and the summary DERIVES them from it every time
# it is drawn.
# --------------------------------------------------------------------------- #
def set_yaml_scalar(path, section, key, value, comment=None):
    """Set `section.key` to `value` in a scenario yaml, in place.

    A targeted line rewrite, not a parse-and-dump: these files are read by hand as
    much as by the engine, and a round-trip through a yaml library would strip
    every comment - the generated one is mostly comments explaining each knob.
    Inserts the key under its section when missing, which a hand-written app yaml
    often is. Returns True if the file now says what was asked.

    The trailing comment is kept as it was, EXCEPT when `comment` is given, which
    replaces it. That matters when the meaning of a key changes: a file whose value
    is corrected but whose comment still explains the old meaning is exactly the kind
    of half-truth this whole change is about removing."""
    import re
    try:
        with open(path, encoding="utf-8-sig") as f:   # tolerate a hand-editor's BOM
            lines = f.readlines()
    except OSError as exc:
        print(f"[cosim] cannot read {path} ({exc}).")
        return False

    in_section, sec_at, done = False, None, False
    for i, line in enumerate(lines):
        if re.match(rf"^{re.escape(section)}\s*:", line):
            in_section, sec_at = True, i
            continue
        if in_section:
            m = re.match(rf"^(\s+){re.escape(key)}\s*:\s*(.*?)(\s+#.*)?$", line)
            if m:
                trail = f"   # {comment}" if comment else (m.group(3) or "")
                lines[i] = f"{m.group(1)}{key}: {value}{trail}\n"
                done = True
                break
            if line.strip() and not line[0].isspace():
                in_section = False          # left the section without finding it
    if not done:
        if sec_at is None:
            print(f"[cosim] {os.path.basename(path)} has no '{section}:' section; "
                  f"set {key} by hand.")
            return False
        indent = "  "
        for line in lines[sec_at + 1:]:     # copy the section's own indentation
            if line.strip() and line[0].isspace():
                indent = line[:len(line) - len(line.lstrip())]
                break
        lines.insert(sec_at + 1, f"{indent}{key}: {value}\n")
        print(f"[cosim] {os.path.basename(path)} did not set {key}; adding it.")

    try:
        with open(path, "w", encoding="utf-8", newline="") as f:
            f.writelines(lines)
    except OSError as exc:
        print(f"[cosim] cannot write {path} ({exc}).")
        return False
    return True


def derived_from_yaml(config_yaml, staged, args=None):
    """The settings the SCENARIO YAML owns: which bridge, and which CARLA.

    Read fresh every time the summary is drawn, so editing the yaml by hand shows
    up immediately and is what actually runs. An app may declare which stack its
    yaml is written for, which settles the case the yaml itself leaves open:
    ConfigHelper defaults EnablePythonBackend to true, so a hand-written
    native-stack yaml that omits it would otherwise read as the python bridge."""
    if not config_yaml or not os.path.isfile(config_yaml):
        # No yaml yet - it is generated on the first run. What WILL be written is
        # the pending state on `args`: the CLI flags, and any edit made in this
        # session (the editors below park their result there when there is no file
        # to write to). Reporting the built-in defaults instead is what made a menu
        # change look like it had been ignored - you set engine=cpp, the write was
        # skipped because the file did not exist, and the summary redrew "py".
        return {"engine": (getattr(args, "engine", None)
                           or declared_engine(staged, config_yaml) or "cpp"),
                "carla_host": getattr(args, "carla_host", None),
                "carla_port": getattr(args, "carla_port", None),
                # None (not False) when there is no endpoint yet: an unset host is not
                # "this machine", and saying so would be a third wrong answer.
                "carla_local": (_is_local_host(getattr(args, "carla_host", None))
                                if getattr(args, "carla_host", None) else None),
                "carla_tick": getattr(args, "carla_tick", None),
                "realtime": None}
    host, port = read_carla_endpoint(config_yaml)
    # An EXPLICIT EnablePythonBackend wins over the app's declaration: the
    # declaration exists because a hand-written yaml usually omits the key and
    # ConfigHelper then defaults it to py, but once the key is in the file it is
    # the answer - edit_engine writes it there, and the row is labelled "from the
    # yaml". Reporting the declaration regardless made a menu change that HAD been
    # written look like it was ignored.
    return {"engine": (read_backend(config_yaml) if _declares_backend(config_yaml)
                       else declared_engine(staged, config_yaml)
                       or read_backend(config_yaml)),
            "carla_host": host, "carla_port": port,
            "carla_local": _is_local_host(host) if host else None,
            # The cadence and the pacing live here too, so the summary shows what
            # will actually run instead of a number the setup remembered.
            "carla_tick": _yaml_float(config_yaml, "CarlaSetup", "CarlaTimeStep", 0.0)
                          or FIXS_FEED_S,
            "realtime": read_realtime_pacing(config_yaml)}


def edit_engine(config_yaml, current):
    """Which bridge runs the co-sim. Written into the yaml (see the note above)."""
    picked = _menu("Co-sim engine:",
                   [("py", "py    Python VirEnvCore (TrafficLayer + mainVirCarla.py)"),
                    ("cpp", "cpp   C++ VirEnvCore    (TrafficLayer + VirCarlaEnv.exe)")],
                   current)
    if picked != current:
        if config_yaml and os.path.isfile(config_yaml):
            if set_yaml_scalar(config_yaml, "CarlaSetup", "EnablePythonBackend",
                               "true" if picked == "py" else "false"):
                print(f"[cosim] {os.path.basename(config_yaml)}: engine -> {picked}")
        else:
            # The yaml is generated on the first run, so there is nothing to write
            # to yet. Say so and let the caller carry it to generation, rather than
            # dropping the choice on the floor in silence.
            print(f"[cosim] engine -> {picked} (pending: written when the scenario "
                  f"yaml is generated on the first run)")
    return picked


def edit_sumo(config_yaml, gui, carla_tick):
    """How the co-sim runs: SUMO window or headless, and the CARLA tick.

    SUMO's own timestep is NOT offered: it is the FIXS exchange period, fixed by the
    protocol (see FIXS_FEED_S). What is left to choose is how finely CARLA renders
    between two exchanges, and that lives in the yaml - so, like the engine and the
    CARLA endpoint, it is edited straight into the file rather than remembered in the
    setup. Everything about how the TRAFFIC behaves belongs in the .sumocfg."""
    gui = _menu("SUMO:", [(True, "gui        sumo-gui window"),
                          (False, "headless   no window")], bool(gui))
    options = []
    for c in CARLA_TICK_CHOICES:
        n = int(round(FIXS_FEED_S / c))
        how = ("1:1 with the FIXS feed" if n == 1
               else f"{n} interpolated sub-steps per feed ({1.0 / c:g} Hz)")
        options.append((c, f"{c:<7g} {how}"))
    picked = _menu(f"CARLA tick (FIXS feed is fixed at {FIXS_FEED_S:g} s):",
                   options, carla_tick)
    if config_yaml and os.path.isfile(config_yaml) and abs(picked - carla_tick) > 1e-12:
        if set_yaml_scalar(config_yaml, "CarlaSetup", "CarlaTimeStep", f"{picked:g}"):
            print(f"[cosim] {os.path.basename(config_yaml)}: CarlaTimeStep -> {picked:g}")
    return gui


def edit_carla(cfg, config_yaml, host, port):
    """The CARLA to talk to. Two different things live behind this line:

    the INSTALL (which CarlaUE4 to launch) is a property of the machine and lives
    in ~/.fixs/carla.json, shared by every app; the ENDPOINT (which RPC server to
    dial) is a property of the scenario and lives in the yaml, because that is
    where VirCarlaEnv reads it. Neither is kept in the saved setup."""
    what = _menu(
        "CARLA:",
        [("endpoint", f"set the RPC endpoint  (now {host or DEFAULT_CARLA_HOST}:"
                      f"{port or DEFAULT_CARLA_PORT})"),
         ("install", f"switch the CARLA install  (now {(cfg or {}).get('mode', '?')} "
                     f"{(cfg or {}).get('carla_root', '?')})")],
        "endpoint")
    if what == "install":
        return env.run_setup(), host, port
    if sys.stdin.isatty():
        ans = _ask(f"[cosim] CARLA host [Enter = {host or DEFAULT_CARLA_HOST}]: ")
        host = ans or host or DEFAULT_CARLA_HOST
        while True:
            ans = _ask(f"[cosim] CARLA RPC port [Enter = {port or DEFAULT_CARLA_PORT}]: ")
            if ans == "":
                port = port or DEFAULT_CARLA_PORT
                break
            if ans.isdigit() and 0 < int(ans) < 65536:
                port = int(ans)
                break
            print("[cosim] enter a port number.")
    if _yaml_exists(config_yaml):
        wire = write_carla_endpoint(config_yaml, host, port)
        print(f"[cosim] {os.path.basename(config_yaml)}: CARLA -> {wire}:{port}")
    else:
        # Same as the engine editor: no file yet, so this is pending until the
        # scenario yaml is generated. Silence here sent people to inspect a yaml
        # that had never been written.
        print(f"[cosim] CARLA -> {wire}:{port} (pending: written when the scenario "
              f"yaml is generated on the first run)")
    return cfg, host, port


def _bind_app(rec, apps, ctx):
    """Resolve the setup's app id to its manifest entry and stage its yamls.
    Called whenever the record's app could have changed - on load and on edit -
    so `ctx` always describes the app the record currently names."""
    app = app_catalog.find_app(apps, rec.get("app")) if rec.get("app") else None
    if rec.get("app") and apps and app is None:
        print(f"[cosim] setup names app '{rec['app']}', which is not in "
              f"{app_catalog.catalog_path()}; continuing without it.")
    ctx["app"] = app
    ctx["staged"] = app_catalog.stage_configs(app) if app else []
    _scope_config(rec, ctx)
    return app


def _scope_config(rec, ctx):
    """Mark rec['config'] 'app' when it is one of this app's own staged yamls.

    Both kinds live in ~/.fixs/apps/<id>/ - the app's AUTHORED yamls and the ones
    generated per map - and only the scope tells them apart. --config records 'map'
    because it cannot know; here the app is bound, so it can. Getting this wrong is
    not cosmetic: the generator writes over a 'map'-scoped path that does not exist
    yet, which for an app yaml means replacing the file the app shipped with a
    probe-shaped one, and the run then quietly does something else."""
    chosen = rec.get("config")
    if not chosen:
        return
    staged = {os.path.normcase(os.path.abspath(s["path"])) for s in ctx.get("staged") or []}
    if os.path.normcase(os.path.abspath(chosen)) in staged:
        rec["config_scope"] = "app"


def _apply_cli(rec, args):
    """Overlay explicitly passed flags onto the setup. This is what makes
    `run_cosim --map atlanta` change exactly one thing: the flag wins over what was
    saved, everything else in the setup is untouched, and nothing is prompted."""
    if args.no_app:
        rec["app"] = None
    elif args.app is not None:
        rec["app"] = args.app
    if args.map is not None:
        rec["map"], rec["map_origin"], rec["map_local"] = args.map, None, None
    if args.config is not None:
        rec["config"] = args.config
        # Scope is corrected against the app's staged yamls once the app is bound
        # (_bind_app -> _scope_config): 'map' here is only the default for a path
        # that turns out not to be one of the app's own.
        rec["config_scope"] = "map"
    if args.engine is not None:
        rec["engine"] = args.engine
    if args.sumo_gui is not None:
        rec["sumo_gui"] = args.sumo_gui
    # --carla-tick, --fast and the CARLA endpoint (--peer / --carla-host /
    # --carla-port) are NOT overlaid here: they belong to the scenario yaml, and
    # they are written through to it - the endpoint by _push_cli_endpoint below,
    # the cadence by main() - so one file stays the single source of truth for what
    # the engines read. Parking a copy on the record was worse than useless: the
    # record's copy is stripped before saving and nothing ever read it, while the
    # summary kept deriving line 5 from the yaml, so --carla-host looked ignored.


def _disable_quickedit():
    """Turn off the Windows console's QuickEdit mode for this run.

    With QuickEdit on (the default), clicking anywhere in the window starts a text
    selection - and while a selection is active the console BLOCKS every write to
    stdout. That does not merely pause the log: the process is stopped in
    WriteFile, so a co-sim stalls mid-run and resumes only when someone presses
    Enter, at which point all the buffered output arrives at once. It reads
    exactly like a hang, and an accidental click is enough to cause it.

    ENABLE_EXTENDED_FLAGS must be set for the QuickEdit bit to take effect."""
    if platform.system() != "Windows":
        return
    try:
        import ctypes
        kernel32 = ctypes.windll.kernel32
        handle = kernel32.GetStdHandle(-10)          # STD_INPUT_HANDLE
        mode = ctypes.c_uint32()
        if not kernel32.GetConsoleMode(handle, ctypes.byref(mode)):
            return                                   # not a console (piped/redirected)
        ENABLE_QUICK_EDIT, ENABLE_EXTENDED_FLAGS = 0x0040, 0x0080
        if not mode.value & ENABLE_QUICK_EDIT:
            return
        kernel32.SetConsoleMode(
            handle, (mode.value & ~ENABLE_QUICK_EDIT) | ENABLE_EXTENDED_FLAGS)
        print("[cosim] console QuickEdit disabled for this run - a stray click "
              "would otherwise block stdout and stall the co-sim until Enter.\n"
              "        To copy text: right-click -> Mark, select, Enter. Or use "
              "--log and read the file.\n"
              "        (Windows Terminal does not have this problem; there, "
              "--no-quickedit-fix is the better setting.)")
    except Exception:
        pass                                         # never fail a run over this


def _yaml_exists(path):
    return bool(path) and os.path.isfile(path)


def resolve_peer(args):
    """Fold --peer HOST[:PORT] into the CARLA endpoint it names.

    --peer is the traffic host's word for "CARLA is on that machine over there",
    which is the vocabulary a two-machine run is actually described in - the other
    end runs --serve. It is not a second setting: it resolves to
    --carla-host/--carla-port here, once, so everything downstream (the yaml
    write-through, the --no-launch implication, the peer handshake, --doctor) keeps
    reading one endpoint.

    One token rather than two flags, because an address is written down and passed
    around as one. Given both spellings, it stops rather than picking a winner:
    which CARLA to talk to is not something to guess at."""
    if args.peer is None:
        return args
    spec = (args.peer or "").strip()
    host, sep, port = spec.rpartition(":")
    if not sep:
        host, port = spec, ""
    if not host:
        sys.exit(f"[cosim] --peer '{args.peer}': expected HOST or HOST:PORT, "
                 f"e.g. --peer 192.168.1.9 or --peer 192.168.1.9:2000.")
    if port and not (port.isdigit() and 0 < int(port) < 65536):
        sys.exit(f"[cosim] --peer '{args.peer}': '{port}' is not a port number.")
    clash = [f for f, v in (("--carla-host", getattr(args, "carla_host", None)),
                            ("--carla-port", getattr(args, "carla_port", None)))
             if v is not None]
    if clash:
        sys.exit(f"[cosim] --peer already says which CARLA to talk to; "
                 f"{' and '.join(clash)} sets the same thing. Pass one.")
    args.carla_host = host
    if port:
        args.carla_port = int(port)
    return args


def write_carla_endpoint(config_yaml, host, port):
    """Write the CARLA endpoint into the scenario yaml. Returns the address written.

    One place, one owner: VirCarlaEnv reads CarlaSetup.CarlaServerIP/Port out of
    this file itself (CommonLib/ConfigHelper.cpp), so anything that decides the
    endpoint has to land HERE or it only moves run_cosim's own probe. The C++ side
    takes a literal address, not a resolvable name, so 'localhost' is written as
    127.0.0.1."""
    wire = "127.0.0.1" if host in ("localhost", "") else host
    set_yaml_scalar(config_yaml, "CarlaSetup", "CarlaServerIP", wire)
    set_yaml_scalar(config_yaml, "CarlaSetup", "CarlaServerPort", port)
    return wire


def _push_cli_endpoint(config_yaml, args, ctx):
    """Write an endpoint given on the command line into the scenario yaml, once.

    --peer / --carla-host / --carla-port name the CARLA this run talks to, and the
    setup summary DERIVES that line from the scenario yaml (see derived_from_yaml).
    Written through only after the review loop, the flag was invisible on the very
    screen it was meant to change: you passed --peer 192.168.1.9, the summary drew
    the yaml's 127.0.0.1, and the run then dialled the peer - the flag looked
    ignored while actually being in force.

    Once per yaml, not once per redraw, so changing the CARLA row afterwards still
    wins: the flag is the starting point for this run, not a value that keeps
    overwriting the file behind the editor. A yaml that does not exist yet is left
    alone - generate_config_yaml seeds it from the same args."""
    if args is None or (args.carla_host is None and args.carla_port is None):
        return
    if not _yaml_exists(config_yaml):
        return
    key = os.path.normcase(os.path.abspath(config_yaml))
    pushed = ctx.setdefault("endpoint_pushed", set())
    if key in pushed:
        return
    pushed.add(key)
    was_host, was_port = read_carla_endpoint(config_yaml)
    host = args.carla_host or was_host or DEFAULT_CARLA_HOST
    port = args.carla_port or was_port or DEFAULT_CARLA_PORT
    wire = "127.0.0.1" if host in ("localhost", "") else host
    if wire == was_host and port == was_port:
        return
    print(f"[cosim] {os.path.basename(config_yaml)}: CARLA -> {wire}:{port} "
          f"(from the command line; the yaml said {was_host or '?'}:{was_port or '?'})")
    write_carla_endpoint(config_yaml, host, port)


def await_peer(args):
    """Serve mode: wait for the traffic machine and take its instructions.

    Returns the live control socket, which is held for the whole run - closing it
    is how either side says it is over. What arrives is a handful of scalars, not
    a config file: the scenario yaml stays on the machine that owns it, and the
    keys this side would find in one (CarlaTimeStep, TrafficRefreshRate, the
    backend) are read by VirCarlaEnv on the OTHER machine anyway."""
    import peer
    port = args.peer_port or peer.peer_port(args.carla_port or DEFAULT_CARLA_PORT)
    try:
        srv = peer.listen(port)
    except OSError as e:
        sys.exit(f"[serve] cannot listen on {port} ({e}). Another --serve may "
                 f"already be running here; check before killing it - it could be "
                 f"serving someone else's run. --peer-port moves this one.")
    print(f"[serve] waiting for a traffic peer on {port} (Ctrl+C to stop) ...")
    try:
        while True:
            sock, addr = srv.accept()
            sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            print(f"[serve] peer connected from {addr[0]}")
            try:
                r = peer.Reader(sock)
                msg = r.read(10.0)
                if not msg or msg.get("t") != "HELLO":
                    peer.send(sock, {"t": "ERROR", "why": "expected HELLO"})
                    sock.close(); continue
                peer.send(sock, {"t": "WELCOME", "proto": peer.PROTO,
                                 "fixs_version": _fixs_version(),
                                 "carla_mode": "source"})
                msg = r.read(60.0)
                if not msg or msg.get("t") != "SERVE":
                    sock.close(); continue
            except (peer.PeerError, OSError) as e:
                print(f"[serve] handshake failed ({e}); still listening.")
                try: sock.close()
                except OSError: pass
                continue
            # The peer decides; this side only carries it out - so it has to have
            # decided. With no map on either side the run would fall through to the
            # map picker, which is a prompt on a host whose whole contract is that
            # it prompts for nothing; the peer would then wait out --prep-timeout
            # against a menu. Refuse it as the protocol error it is, and stay up.
            if not (msg.get("map") or args.map):
                peer.fail(sock, "SERVE carried no map, and this host has no "
                                "--carla-map to fall back on", retryable=False)
                print("[serve] peer sent no map; still listening.")
                continue
            args.map = msg.get("map") or args.map
            args.carla_port = msg.get("carla_port") or args.carla_port
            args.quality_level = msg.get("quality_level") or args.quality_level
            args.render_offscreen = bool(msg.get("render_offscreen",
                                                 args.render_offscreen))
            # Stashed rather than acted on here: the map is not resolved until the
            # preflight, which is where the comparison belongs. Absent from an older
            # peer, which just means the check does not run.
            args.peer_map_sha = msg.get("map_sha")
            print(f"[serve] peer asked for map '{args.map}' on port "
                  f"{args.carla_port or DEFAULT_CARLA_PORT}")
            srv.close()
            global _SERVE_SOCK
            _SERVE_SOCK = sock       # so any later sys.exit can still say why
            return sock
    except KeyboardInterrupt:
        srv.close()
        sys.exit("\n[serve] stopped before any peer connected.")


def ask_peer_to_serve(args, target_map):
    """Traffic side: tell the CARLA host what to serve, and wait for it.

    Returns the live control socket, or None when no peer answers AND the user has
    said that is acceptable (--allow-manual-carla). Retrying is what makes start
    order irrelevant."""
    import peer
    port = args.peer_port or peer.peer_port(args.carla_port or DEFAULT_CARLA_PORT)
    try:
        sock = peer.connect(args.carla_host, port, wait=args.peer_wait)
    except peer.PeerError as e:
        # Continuing here used to be the default, and it is the one failure in this
        # protocol that does not look like a failure: nothing is listening, so the
        # run proceeds against whatever CARLA is up on that host - which, after a
        # --serve that died, is the PREVIOUS run's server, still holding the
        # previous map. It then works, for the wrong reason, and the peer protocol
        # is quietly not in use. Same class of bug as the one serve_forever's
        # docstring describes. So: say so and stop, unless asked not to.
        if not args.allow_manual_carla:
            sys.exit(f"[peer] {e}\n"
                     f"[peer] not continuing: a CARLA already running on "
                     f"{args.carla_host} may be serving a different map, and this "
                     f"run cannot tell.\n"
                     f"[peer] Start it there with 'run_cosim --serve', or pass "
                     f"--allow-manual-carla if you started CARLA by hand with "
                     f"'{target_map}' loaded.")
        print(f"[peer] {e}\n"
              f"[peer] --allow-manual-carla: continuing, assuming CARLA on "
              f"{args.carla_host} was started by hand with '{target_map}' loaded.")
        return None
    try:
        r, welcome = peer.hello(sock, _fixs_version())
    except peer.PeerError as e:
        sock.close()
        sys.exit(f"[peer] {e}")
    mine, theirs = _fixs_version(), welcome.get("fixs_version", "?")
    if mine != theirs:
        print(f"[peer] WARNING FIXS versions differ - here {mine}, peer {theirs}. "
              f"Mismatched builds is how the two ends come to disagree silently.")
    import import_map
    # Which bundle THIS machine's traffic data came from. Sent because only this
    # machine can know it: the CARLA host would otherwise compare its cooked map
    # against the library's current release, which is a different question - and
    # the wrong one whenever this side is running off a cached bundle.
    peer.send(sock, {"t": "SERVE", "map": target_map,
                     "carla_port": args.carla_port,
                     "quality_level": args.quality_level,
                     "render_offscreen": bool(args.render_offscreen),
                     "map_sha": import_map.read_cached_sha(target_map)})
    print(f"[peer] asked {args.carla_host} to serve '{target_map}'; waiting ...")
    waited = 0.0
    while True:
        try:
            # Short slices rather than one long block, so silence can be reported
            # as elapsed time. Launching CARLA and loading a map is a couple of
            # minutes of legitimate quiet, and a first cook is far longer - with
            # no output at all that is indistinguishable from a hang, which is
            # exactly how it looked the first time this ran for real.
            msg = r.read(5.0)
        except peer.PeerError:
            waited += 5.0
            if waited >= args.prep_timeout:
                sock.close()
                sys.exit(f"[peer] no word from {args.carla_host} for "
                         f"{args.prep_timeout:g}s; giving up.")
            if waited % 15 < 5:
                print(f"[peer] still waiting on {args.carla_host} ({waited:.0f}s) - "
                      f"it is launching CARLA and loading the map.")
            continue
        if msg is None:
            sock.close()
            sys.exit("[peer] the CARLA host disconnected before serving.")
        if msg.get("t") == "PROGRESS":
            print(f"[peer] {msg.get('stage', '')}: {msg.get('msg', '')}")
        elif msg.get("t") == "SERVING":
            print(f"[peer] CARLA is serving '{msg.get('map')}' on "
                  f"{args.carla_host}:{msg.get('rpc_port')}")
            return sock
        elif msg.get("t") == "ERROR":
            sock.close()
            # retryable says whether the render host thinks another attempt could
            # work. It goes back to listening either way, so the advice is what
            # differs: "run it again" is right for a busy port and actively
            # misleading for a map that host can never install.
            again = ("\n[peer] The render host is listening again - re-run this "
                     "once it is ready." if msg.get("retryable") else
                     "\n[peer] Retrying will not help until that is fixed on "
                     f"{args.carla_host}.")
            sys.exit(f"[peer] the CARLA host could not serve it: "
                     f"{msg.get('why')}{again}")


class _Tee:
    """Write to the console and to a log file at once."""

    def __init__(self, stream, fh):
        self._stream, self._fh = stream, fh

    def write(self, data):
        # FILE FIRST, console second. On Windows a console selection (QuickEdit)
        # blocks writes to the console, so console-first would let a stall take
        # the log with it - and the log would stop exactly when something
        # interesting was happening.
        try:
            self._fh.write(data)
            self._fh.flush()
        except Exception:
            pass
        self._stream.write(data)
        return len(data)

    def flush(self):
        self._stream.flush()
        try:
            self._fh.flush()
        except Exception:
            pass

    def isatty(self):
        # Follows the CONSOLE, not the file. run_cosim decides whether to prompt
        # from this, so returning the file's False would quietly turn an
        # interactive run non-interactive just because logging was on.
        return self._stream.isatty()

    def __getattr__(self, name):
        return getattr(self._stream, name)


def start_log(path=None):
    """Tee stdout/stderr into RealSim_tmp/run_cosim_<host>_<stamp>.log."""
    if path is None:
        out = os.path.join(os.getcwd(), "RealSim_tmp")
        os.makedirs(out, exist_ok=True)
        path = os.path.join(out, f"run_cosim_{socket.gethostname()}_"
                                 f"{time.strftime('%Y%m%d_%H%M%S')}.log")
    fh = open(path, "w", encoding="utf-8", errors="replace")
    fh.write(f"# run_cosim {' '.join(sys.argv[1:])}\n"
             f"# {socket.gethostname()}  {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
    sys.stdout, sys.stderr = _Tee(sys.stdout, fh), _Tee(sys.stderr, fh)
    print(f"[cosim] logging this run to {path}")
    return path


def _tell_peer(sock, stage, msg):
    """Report a milestone to the waiting traffic machine, if there is one.

    Cooking a map is minutes of legitimate silence on the other end. A heartbeat
    proves the process is alive; this says WHAT it is doing, which is the
    difference between "still waiting (300s)" and "cooking roosevelt_full"."""
    if sock is None:
        return
    import peer
    peer.progress(sock, stage, msg)


# The control socket of the run currently being served, or None. Module-level
# because serve_forever() has to reach it and main() does not hand it back:
# main() fails by sys.exit() from dozens of places, and the peer is entitled to
# the reason from every one of them. Threading a socket out of each would be a
# far larger change than the problem is worth, and --serve holds exactly one of
# these at a time (await_peer stops listening once a peer connects), so a single
# slot is the whole state.
_SERVE_SOCK = None


def _drop_serve_sock():
    """Close and forget the served run's control socket, if it is still open."""
    global _SERVE_SOCK
    sock, _SERVE_SOCK = _SERVE_SOCK, None
    if sock is None:
        return
    try:
        sock.close()
    except OSError:
        pass


def _fail_peer(why, retryable=False):
    """Tell the waiting traffic machine WHY this run is not happening, then close.

    Every render-side failure used to reach the peer as a bare socket close, which
    it reported as "the CARLA host disconnected before serving" - the same
    sentence for a map that is not installed, a busy RPC port, a failed cook and a
    pulled cable. The peer cannot act on that, and the person reading it is on the
    machine that has none of the evidence.

    `retryable` says whether re-running could help: the render host goes back to
    listening either way (serve_forever), but "the port was busy, try again" and
    "this CARLA is packaged and cannot cook that map" deserve different advice,
    and only this side can tell them apart."""
    global _SERVE_SOCK
    sock, _SERVE_SOCK = _SERVE_SOCK, None
    if sock is None:
        return
    import peer
    peer.fail(sock, why, retryable=retryable)


def _check_map_source(args, carla_root, target_map, mode):
    """Is the map already cooked here the one this run's SUMO data belongs to?

    Only meaningful for an ALREADY-cooked map: a fresh cook stamps whatever it just
    downloaded, so the two halves agree by construction. The stale case is the quiet
    one - the CARLA half is cooked once and kept while the SUMO half tracks the
    library, so an updated map leaves geometry and traffic data one release apart
    under the same name, with nothing failing.

    Silent unless both sides recorded a sha AND they differ. A map with no sha
    (local pick, hand-prepped, imported before this existed) is not checked at all
    rather than nagged about."""
    import import_map
    have = import_map.cooked_sha(carla_root, target_map, mode=mode)
    # What this run's traffic data came from: the peer's, when a traffic machine is
    # driving us, else this machine's own cached bundle.
    want = getattr(args, "peer_map_sha", None) or import_map.read_cached_sha(target_map)
    if not have or not want or have == want:
        return

    where = "the traffic machine" if getattr(args, "peer_map_sha", None) else "this machine"
    why = (f"map '{target_map}' was cooked from a different bundle than the one "
           f"{where} is running (cooked {have}, running {want}). Geometry and "
           f"traffic data are one release apart, which places vehicles against a "
           f"road that moved.")
    if args.serve and not args.allow_map_skew:
        # Refused rather than warned: under --serve the person who would read a
        # warning is at the other machine, looking at a different console. The exit
        # message reaches them as-is - serve_forever turns a sys.exit(str) into the
        # peer's ERROR reason - so it is phrased to be read from there.
        sys.exit(f"[cosim] this CARLA host: {why} Re-cook here with --reimport, or "
                 f"pass --allow-map-skew to run anyway.")
    print(f"[cosim] WARNING {why}\n"
          f"        Fix: re-run with --reimport to re-cook it.")


def _who_has_port(port):
    """(pid, process name) LISTENING on `port`, or None. Handed to doctor so it can
    name what is squatting on the CARLA port instead of only noting that something
    is."""
    pid = _pid_on_port(port)
    return (pid, _process_name(pid) or "unknown") if pid else None


def _doctor_role(doctor, cfg, args):
    """role= and why= for doctor.run(). An explicit --role wins and reports no
    inference, because there is nothing inferred to explain."""
    if args.role:
        return {"role": args.role, "why": None}
    role, why = doctor.detect_role(cfg, FIXS_ROOT)
    return {"role": role, "why": why}


def _peek_any_endpoint():
    """Best-effort (host, port) from the most recently used setup's yaml, so
    --doctor and --version can check the CARLA you actually talk to without
    being told. Returns (None, None) when there is nothing to read."""
    try:
        doc = run_profile.load_doc()
        name = doc.get("last")
        rec = (doc.get("setups") or {}).get(name) or {}
        path = rec.get("config")
        if path and os.path.isfile(path):
            return read_carla_endpoint(path)
    except Exception:
        pass
    return None, None


def print_fingerprint(cfg, host, port):
    """One block identifying what is installed here. The first thing to paste
    into a bug report, and the quickest way to see two machines disagree."""
    py = (cfg or {}).get("python") or sys.executable
    print(f"[cosim] run fingerprint - {socket.gethostname()} ({platform.system()})")
    print(f"  FIXS       {_fixs_version()}")
    build = os.path.join(FIXS_ROOT, "BUILD_INFO.txt")
    if os.path.isfile(build):
        with open(build, encoding="utf-8-sig", errors="replace") as f:
            for line in f:
                if line.strip().startswith("Git Commit:"):
                    print(f"  commit     {line.split(':', 1)[1].strip()}")
                    break
    print(f"  python     {py}")
    for mod in ("carla", "traci", "yaml", "pandas", "shapely"):
        rc = subprocess.call([py, "-c", f"import {mod}"],
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print(f"    {mod:<8} {'present' if rc == 0 else 'MISSING'}")
    sumo = shutil.which("sumo") or shutil.which("sumo-gui")
    print(f"  SUMO       {sumo or 'not on PATH'}")
    print(f"  CARLA      mode {(cfg or {}).get('mode', 'not configured')}, "
          f"endpoint {host}:{port}"
          f"{'  [this machine]' if _is_local_host(host) else '  [remote]'}")
    return 0


def _fixs_version():
    try:
        # utf-8-sig for the same reason as _fixs_tag_repo: a BOM left on line 1 rides
        # into the returned string, and printing that to a cp1252 console raises
        # UnicodeEncodeError - which is how --version crashed on Windows.
        with open(os.path.join(FIXS_ROOT, "FIXS_VERSION.txt"), encoding="utf-8-sig") as f:
            return f.readline().strip()
    except OSError:
        return "unknown"


def hold_carla(carla_proc, target_map, args, sock=None):
    """Keep a launched CARLA alive for a run driven from the other machine.

    Two ways to be told it is over, and both have to work: the traffic machine
    hangs up (sock closes - covers its Ctrl+C, a crash, a pulled cable), or CARLA
    dies under us. Polling both beats waiting on either, and the caller's
    `finally: kill_carla(...)` does the teardown once this returns."""
    import peer
    where = f"{args.carla_host}:{args.carla_port}"
    if sock is None:
        print(f"\n[cosim] --carla-only: CARLA is up on {where} serving "
              f"'{target_map}'.\n"
              f"[cosim] Drive it from the traffic machine: set CarlaSetup."
              f"CarlaServerIP to this host in its scenario yaml and run run_cosim "
              f"there.\n[cosim] Ctrl+C here to stop CARLA.\n")
    else:
        print(f"\n[serve] CARLA is up on {where} serving '{target_map}'; the peer "
              f"is driving it. Ctrl+C to stop.\n")
        try:
            peer.send(sock, {"t": "SERVING", "rpc_port": args.carla_port,
                             "map": target_map,
                             "carla_pid": getattr(carla_proc, "pid", None)})
        except OSError:
            print("[serve] peer hung up before we could confirm; stopping.")
            return 0
    reader = peer.Reader(sock) if sock is not None else None
    try:
        while carla_proc is None or carla_proc.poll() is None:
            if reader is not None:
                try:
                    msg = reader.read(1.0)
                except peer.PeerError:
                    continue                    # just a quiet second
                if msg is None:
                    print("[serve] peer disconnected; stopping CARLA.")
                    return 0
                if msg.get("t") == "BYE":
                    print(f"[serve] peer finished"
                          f"{': ' + msg['why'] if msg.get('why') else ''}; "
                          f"stopping CARLA.")
                    return 0
            else:
                time.sleep(1.0)
        # CARLA died under us. Tell the peer WHY before the socket closes,
        # otherwise it sees a bare disconnection and reports the wrong cause -
        # the traffic side would blame the network or its own bridge.
        print("[cosim] CARLA exited on its own; nothing left to serve.")
        if sock is not None:
            peer.bye(sock, "CARLA exited on the render host")
    except KeyboardInterrupt:
        print("\n[cosim] stopping CARLA ...")
        if sock is not None:
            peer.bye(sock, "stopped from the render host (Ctrl+C)")
    return 0


def _sumo_for_local_pick(rec, ctx, local, args):
    """Fill the SUMO slot now, when the map just picked leaves it empty and saying
    so is free.

    The scenario is normally resolved late, in main(), because four of the five
    things that fill it need work done first: a bundle has to be OPENED to know it
    ships a sumo/, and an app has to be STARTED to know it reports its own. Asking
    every run would either ask what the bundle answers, or drag a ~380MB download
    into a loop whose whole point is that it decides and downloads nothing.

    A LOCAL pick has neither problem - it is a path on this disk, so the question is
    one directory walk. A raw export never carries a .sumocfg, and deferring that
    put the prompt between the cook and the signal placement, in the middle of the
    one stretch you cannot walk away from.

    Stores nothing new: choose_sumo_source caches under ~/.fixs/maps/<map>/sumo,
    which main()'s chain already reads, so a skipped or cancelled pick lands back on
    the late resolver unchanged."""
    import import_map
    if not local or args is None or not _interactive(args):
        return
    if args.sumocfg is not None:
        return
    if (ctx.get("app") or {}).get("launch"):
        return
    if import_map.local_pick_carries_sumo(local):
        return
    # NOT gated on args.reimport the way the flag check above is: main()'s own
    # cached_sumo_dir ignores this same cache whenever --reimport is set (a
    # deliberate "refresh from the source" rule - see its docstring), so an early
    # check that skipped here because the cache exists would just watch the late
    # chain re-ask anyway - after the cook has already started, which is the exact
    # failure this function exists to prevent. Skipping the cache is therefore
    # keyed to the SAME flag the late chain uses, not exempted by it.
    if not args.reimport and import_map.map_sumo_dir(rec["map"]):
        return                    # a previous run already picked one for this map
    import_map.choose_sumo_source(cache_name=rec["map"])


def _edit_slots(slots, rec, ctx, cfg, apps, catalog, repo, tag_prefix, args=None,
                auto=()):
    """Run the editor for each requested slot. Always in SLOT_KEYS order, so a
    dependency is settled before the thing that depends on it (pick the app, then
    its map, then the scenario for that pairing). Returns the CARLA config, which
    the CARLA editor may have replaced.

    `auto` are the slots being filled as a CONSEQUENCE - a new setup's blanks, or
    what cascade() invalidated - rather than because the user selected that row.
    A slot with one obvious answer settles itself there and reports what it took;
    the row on the summary is how you disagree with it. Only the scenario slot has
    such an answer today: the app declares its yaml, so asking which one to run
    before the user has even seen the setup is a question with a single sensible
    reply. app and map have no default worth guessing and always ask."""
    import import_map
    # Each editor marks what the setup is currently using, so Enter keeps it. That
    # only holds while the value is still THIS app's / THIS map's: when an earlier
    # slot in the same pass has just changed, what the record still remembers
    # belongs to what we left, and offering it as current would be the same lie.
    # Which one survives what is cascade()'s rule, applied here to the values.
    was = {"app": rec.get("app"), "map": rec.get("map")}
    for slot in run_profile.SLOT_KEYS:
        if slot not in slots:
            continue
        if slot == "app":
            app = app_catalog.choose_app(apps, current=rec.get("app")) if apps else None
            rec["app"] = app["id"] if app else None
            _bind_app(rec, apps, ctx)
            if app:
                print(f"[cosim] app: {app['title']}")
                if app.get("note"):
                    print(f"[cosim]   {app['note']}")
                # App defaults sit under the CLI and the saved setup, above the
                # built-ins - so switching app brings its preferred engine with it.
                for key in ("engine", "sumo_gui"):
                    if app["defaults"].get(key) is not None:
                        rec[key] = app["defaults"][key]
        elif slot == "map":
            app = ctx.get("app")
            if rec.get("app") != was["app"]:
                rec["map"] = None          # belonged to the app we just left
            target, tag, local = import_map.choose_map(
                repo, tag_prefix, cfg.get("carla_root") if cfg else None,
                catalog=catalog, preferred=(app or {}).get("maps"),
                app_label=(app or {}).get("title"), current=rec.get("map"))
            ent = import_map.catalog_entry(catalog, target)
            rec["map"] = ent["map_name"] if ent else target
            rec["map_origin"] = ("Digital-Twin-Library" if tag else
                                 "local file" if local else "cooked")
            rec["map_local"] = local
            ctx["picked_local"] = local
            # A map chosen from the menu right now is the only case where "this is
            # already cooked - reimport it?" is worth asking; a replayed one is not.
            ctx["picked_now"] = True
            print(f"[cosim] selected map: {rec['map']}")
            _sumo_for_local_pick(rec, ctx, local, args)
        elif slot == "config":
            if not rec.get("map"):
                continue                      # nothing to scope a scenario to yet
            # An app-owned yaml is written against the app, so it survives a map
            # change; a per-map one does not. Neither survives an app change.
            keep = rec.get("app") == was["app"] and (
                rec.get("map") == was["map"]
                or (rec.get("config_scope") or "map") == "app")
            if slot in auto:
                rec["config"], rec["config_scope"] = default_config(
                    ctx.get("staged"), rec["map"],
                    rec.get("app") or run_profile.GENERIC,
                    rec.get("config") if keep else None)
            else:
                rec["config"], rec["config_scope"] = edit_config(
                    ctx.get("staged"), (ctx.get("app") or {}).get("title"),
                    rec["map"], rec.get("app") or run_profile.GENERIC,
                    rec.get("config") if keep else None,
                    interactive=(_interactive(args) if args is not None else None))
        elif slot == "engine":
            # Derived from the yaml, not from the record: the yaml owns it, so the
            # value being edited must be the one currently in the file.
            now = derived_from_yaml(rec.get("config"), ctx.get("staged"), args)
            picked = edit_engine(rec.get("config"), now["engine"])
            # No yaml to write to yet? Park it on args, which is where the CLI
            # flags live and what generate_config_yaml reads - so a menu choice
            # and --engine take the identical path instead of one being dropped.
            # Park it on args either way. With no yaml this is the only record of
            # the choice; with one, edit_engine has just written it there, and
            # args.engine is what makes THIS run honour it regardless of read order.
            if args is not None:
                args.engine = picked
        elif slot == "carla":
            now = derived_from_yaml(rec.get("config"), ctx.get("staged"), args)
            cfg, host, port = edit_carla(cfg, rec.get("config"),
                                         now["carla_host"], now["carla_port"])
            if args is not None and not _yaml_exists(rec.get("config")):
                args.carla_host, args.carla_port = host, port
        elif slot == "sumo":
            now = derived_from_yaml(rec.get("config"), ctx.get("staged"), args)
            rec["sumo_gui"] = edit_sumo(rec.get("config"), rec.get("sumo_gui", True),
                                        now["carla_tick"] or FIXS_FEED_S)
    return cfg


# The answers configure_run has collected so far, for the case where it never
# returns. Module-level because an exception unwinds its locals before main() can
# read them, and because the alternative - threading a holder through the
# signature - puts a parameter on a function whose callers do not care.
_IN_PROGRESS = {}

# The setup name the pre-import checkpoint wrote, if any. A dict rather than a bare
# name so main() can set it without a `global` declaration at the top of a very long
# function.
_CHECKPOINT = {}


def _say_checkpoint_kept():
    """Point at the saved setup, once, on a run that failed after it was written.

    The pre-import checkpoint is silent by design: it fires on EVERY run, before
    the cook, and cannot know yet whether the cook will succeed - announcing it
    there would print a line on every good run to report something that only
    matters on a bad one. So the notice hangs off the failure instead, which is the
    one moment it is both abnormal and useful. Nothing is printed when the
    questionnaire itself was interrupted: that path says its own piece, and the
    checkpoint below it never ran."""
    name = _CHECKPOINT.pop("name", None)
    if name:
        print(f"[cosim] setup '{name}' is saved; the next run opens on it.")


def _checkpoint_interrupted_setup():
    """Keep the answers given so far when setup is abandoned part-way.

    The questionnaire is where most of the typing happens, and Ctrl+C in it used to
    throw all of it away - the pre-import checkpoint sits AFTER configure_run
    returns, so it only ever covered a failed cook. This covers the other half.

    Two guards, both about not making things worse than losing the answers:
    `dirty` skips a setup that was opened and not edited - there is nothing to
    keep, and writing would only re-tag a good record as unfinished. And an
    existing name is never overwritten: reaching the confirm step is what offers
    to fork, and an interrupted edit is exactly a fork nobody got to name, so it
    lands beside the original instead of on top of it."""
    rec, doc = _IN_PROGRESS.get("rec"), _IN_PROGRESS.get("doc") or {}
    if not rec or not _IN_PROGRESS.get("dirty"):
        return
    name = _IN_PROGRESS.get("name")
    if not name or name in (doc.get("setups") or {}):
        name = run_profile.suggest_name(rec.get("app"), doc)
    run_profile.save_partial(name, rec, "setup")
    print(f"\n[cosim] kept the answers so far as '{name}'; "
          f"it is offered on the next run.")


def configure_run(args, cfg, repo, tag_prefix, catalog):
    """Decide everything this run needs, looping until the user confirms.

    Opens on the list of saved setups (or straight on one, when --profile / --app
    names it), then on that setup's settings, which are edited and redrawn until
    the user runs it. Every one of the six is a pure decision - no map is
    downloaded, cooked or loaded while you are still deciding what to run - so the
    loop is cheap and nothing is half-done if you quit out of it.

    Returns (app, setup_name, staged_configs, rec, cfg, ctx)."""

    apps = [] if args.no_app else app_catalog.load_catalog()
    # --serve has a terminal but no decisions to make: the traffic machine already
    # chose, and await_peer put its answers on args. Prompting here would ask the
    # wrong human - two people picking independently is exactly how the two ends
    # come to disagree about the map, and that failure lands minutes later inside
    # a cook. So a serving host runs the same code path a non-tty run takes.
    interactive = _interactive(args)
    doc = run_profile.load_doc()
    ctx = {"app": None, "staged": [], "picked_local": None, "picked_now": False}

    # 1. Which saved setup do we open on?
    name, rec = None, None
    if not args.fresh:
        if args.profile:
            name = args.profile
            stored = (doc["setups"] or {}).get(name)
            rec = dict(stored) if stored else None
            if rec is None:
                print(f"[cosim] no saved setup '{name}'; starting a new one.")
                name = None
        elif args.app:
            # A pinned app skips the list: reuse that app's most recent setup.
            for n in run_profile.order(doc):
                if (doc["setups"][n].get("app") or "") == args.app:
                    name, rec = n, dict(doc["setups"][n])
                    break
        else:
            picked = run_profile.choose_setup(doc, interactive)
            if picked == run_profile.QUIT:
                sys.exit("[cosim] cancelled.")
            if picked:
                name, rec = picked, dict(doc["setups"][picked])
    elif doc["setups"]:
        print("[cosim] --fresh: ignoring the saved setups and starting a new one.")

    # 2. Open it, edit it, confirm it. `dirty` decides whether running asks for a
    #    name: an untouched setup keeps its own, an edited one offers to fork.
    dirty = rec is None
    while True:
        # Publish the state an interrupt should keep. Once per redraw is enough:
        # rec is edited in place, so the entry below tracks the edits themselves -
        # this only has to catch rec being REPLACED (a new setup, or switching to
        # another saved one).
        _IN_PROGRESS.update(name=name, rec=rec, dirty=dirty, doc=doc)
        if rec is None:
            rec = {"app": None, "sumo_gui": True}
            _apply_cli(rec, args)
            _bind_app(rec, apps, ctx)
            # A new setup has no answer for these three, and no default worth
            # guessing; the other three come from the yaml / carla.json / built-ins.
            pending = {s for s in MUST_ANSWER if not rec.get(s)}
            if args.serve:
                # The render host makes exactly one decision - which map - and the
                # peer already made it. An APPLICATION is a traffic-side concept:
                # it supplies SUMO arguments, subscriptions and the choice of
                # bridge, none of which run here. Leaving it in MUST_ANSWER meant
                # a fresh render host (no saved setups) stopped to ask a human
                # sitting at the wrong machine for an answer nothing would read.
                # ('none' is a legitimate answer, but `not rec.get("app")` cannot
                # tell "chose none" from "not asked yet", so it is set here.)
                # The SCENARIO YAML is the same story, and was missed. peer.py's
                # contract is "decisions, never files - the yaml stays on the
                # machine that owns it", and every key a render host would read
                # out of one is consumed elsewhere: Backend picks a bridge this
                # host never dispatches (--serve returns at hold_carla),
                # CarlaTimeStep / TrafficRefreshRate / RealtimePacing are read by
                # VirCarlaEnv and TrafficLayer on the TRAFFIC machine, and
                # CarlaServerIP/Port would be written back to a local file nothing
                # local reads. So it changes nothing here and cost a prompt: every
                # serve on a fresh host stopped on the scenario menu.
                rec["app"] = None
                pending -= {"app", "config"}
            # Every one of these is a blank being filled, not a row the user chose.
            auto = set(pending)
            dirty = True
        else:
            _apply_cli(rec, args)
            _bind_app(rec, apps, ctx)
            pending, auto = set(), set()

        while True:
            if pending:
                cfg = _edit_slots(pending, rec, ctx, cfg, apps, catalog, repo,
                                  tag_prefix, args, auto=auto)
                dirty = True
                pending, auto = set(), set()
            # An endpoint named on the command line has to be in the file the
            # summary reads before the summary reads it - see _push_cli_endpoint.
            _push_cli_endpoint(rec.get("config"), args, ctx)
            # Re-read the yaml-owned settings on every redraw, so a yaml edited by
            # hand (or by the editors above) is what the summary shows.
            derived = derived_from_yaml(rec.get("config"), ctx.get("staged"), args)
            action = run_profile.ask(
                name or "(unsaved)", rec, cfg, interactive,
                can_switch=bool(doc["setups"]), derived=derived,
                why=(f"--serve: running what the peer asked for "
                     f"(map '{rec.get('map')}'); this host chooses nothing"
                     if args.serve else
                     "non-interactive: running this setup as-is "
                     "(pass flags to override, --fresh to start over)"))
            if action == run_profile.QUIT:
                sys.exit("[cosim] cancelled.")
            if action == run_profile.RUN:
                name = run_profile.name_setup(doc, name, rec.get("app"), dirty,
                                              interactive)
                _rec_to_args(rec, args)
                return ctx.get("app"), name, ctx.get("staged"), rec, cfg, ctx
            if action == run_profile.NEW:
                name, rec = None, None
                break
            if action == run_profile.SWITCH:
                picked = run_profile.choose_setup(doc, interactive)
                if picked == run_profile.QUIT:
                    sys.exit("[cosim] cancelled.")
                name = picked
                rec = dict(doc["setups"][picked]) if picked else None
                dirty = rec is None
                break
            # `action` is what the user selected; cascade() adds what that
            # invalidated. Widened HERE rather than inside ask() so the two stay
            # distinguishable: a row you picked opens its editor, a row that merely
            # fell over with it settles itself where it can (see _edit_slots).
            pending = run_profile.cascade(rec, action)
            auto = pending - action
            if auto:
                labels = ", ".join(l for s, l in run_profile.SLOTS if s in auto)
                print(f"[cosim] also settling: {labels} "
                      f"(it depended on what you changed)")


def _rec_to_args(rec, args):
    """Publish the settled setup onto args, which the rest of the run reads.

    Only what the SETUP owns. engine, the CARLA endpoint and the cadence are
    deliberately left alone: they live in the scenario yaml, and main() resolves them
    from it - an explicit --engine / --carla-host / --carla-tick still overrides for
    that one run (written through to the yaml), which is why they are not set here."""
    args.app = rec.get("app")
    args.map = rec.get("map")
    args.config = rec.get("config")
    args.sumo_gui = bool(rec.get("sumo_gui", True))


def default_config(staged, map_name, setup_app_id, current=None):
    """The scenario yaml to run without asking. Returns (path, scope).

    Filling this slot is not a question when the application has already answered
    it: an app DECLARES its scenario yamls in apps.json, and the first one it lists
    is the one it means - the rest are variants you switch to deliberately. Opening
    a co-sim with a list of yamls asked which config to run before the user had
    seen the app or the map it belonged to, and the honest answer was almost always
    "the one the app ships".

    An app that declares none gets the per-map path, ~/.fixs/apps/<app>/maps/<map>/
    config.yaml. That file need not exist: main() generates it on the first run,
    which is what makes "or create a new one" a real answer rather than an error.

    `current` is what the setup already runs, when that survived the change that
    got us here; keeping it is more right than re-deriving a default over the top
    of a choice the user has already made. edit_config (summary row 3) is where
    every candidate is still listed."""
    import import_map
    # Same one-shot migration edit_config does, because either path can be the
    # first to touch this app+map pairing.
    app_catalog.migrate_scenarios(setup_app_id, map_name,
                                  import_map._map_cache_dir(map_name))
    staged_paths = {os.path.normcase(os.path.abspath(c["path"])) for c in staged or []}
    if current and os.path.isfile(current):
        scope = "app" if os.path.normcase(os.path.abspath(current)) in staged_paths \
            else "map"
        return current, scope
    if staged:
        pick = staged[0]
        print(f"[cosim] scenario config: {os.path.basename(pick['path'])} "
              f"({pick['title']}) - this app's own; row 3 lists the alternatives.")
        return pick["path"], "app"
    generated = app_catalog.scenario_path(setup_app_id, map_name)
    how = "existing" if os.path.isfile(generated) else "generated on the first run"
    print(f"[cosim] scenario config: {os.path.basename(generated)} for '{map_name}' "
          f"({how}); row 3 lists the alternatives.")
    return generated, "map"


def edit_config(staged, app_title, map_name, setup_app_id, current=None,
                interactive=None):
    """Pick the scenario yaml, from every candidate that exists for this pairing.

    Two kinds are legitimate and both are listed together, because from where the
    user sits they are simply "which config do I run":
      app-owned  the app's own yamls, staged in ~/.fixs/apps/<id>/ - they carry
                 that application's subscriptions and endpoints and are valid on
                 whichever map it runs against
      per-map    ~/.fixs/apps/<app>/maps/<map>/*.yaml - generated for this app on
                 this map, plus any variant the user dropped beside it
    `current` is the yaml the setup is already running, so the menu can mark it and
    Enter can genuinely keep it. It is matched case-insensitively, because the path
    makes a round trip through run_profiles.json between runs; the menu is then
    handed the spelling from the list, which is what the app/per-map test below
    compares against.

    Returns (path, scope). The scope matters downstream twice: an app-owned yaml is
    AUTHORED, so the generator must never overwrite it, and only a per-map one is
    invalidated when the map changes."""
    import import_map
    # One-shot: lift pre-app yamls out of ~/.fixs/maps/<map>/ into the app tree.
    app_catalog.migrate_scenarios(setup_app_id, map_name,
                                  import_map._map_cache_dir(map_name))
    app_home = app_catalog.scenario_dir(setup_app_id)
    generated = app_catalog.scenario_path(setup_app_id, map_name)
    staged_paths = {os.path.normcase(c["path"]) for c in staged}
    try:
        found = sorted(os.path.join(app_home, f) for f in os.listdir(app_home)
                       if f.lower().endswith((".yaml", ".yml"))
                       and os.path.normcase(os.path.join(app_home, f)) not in staged_paths)
    except OSError:
        found = []
    # The map's own generated yaml leads the per-map group, and so is what Enter
    # takes. Sorting alone would put a variant named for it (roosevelt_full_fast)
    # after it, but an unrelated name (config_fast) ahead of it - offering a
    # variant as the default for a map it may not even belong to.
    found.sort(key=lambda p: (os.path.normcase(p) != os.path.normcase(generated),
                              os.path.normcase(p)))

    options = [(c["path"], f"{os.path.basename(c['path']):<42} {c['title']}"
                           + (f"  [engine {c['engine']}]" if c["engine"] else ""))
               for c in staged]
    for p in found:
        note = ("generated for this map" if os.path.normcase(p) == os.path.normcase(generated)
                else "variant in this app's folder")
        options.append((p, f"{os.path.basename(p):<42} {note}"))
    if not any(os.path.normcase(p) == os.path.normcase(generated) for p in found):
        options.append((generated, f"{os.path.basename(generated):<42} "
                                   f"auto-generate for this map"))

    # The setup's own yaml has to be ON the list before it can be marked on it.
    # Normally it is already there; the two ways it is not are both worth saying out
    # loud rather than quietly falling back to another file, since the fallback is
    # what the user would then run.
    if current:
        match = next((p for p, _ in options
                      if os.path.normcase(p) == os.path.normcase(current)), None)
        if match:
            current = match
        elif os.path.isfile(current):
            # Authored elsewhere (a scratch yaml, a path passed as --config). It is
            # what the setup runs, so it belongs on the list; listed last because it
            # is not part of this app's folder.
            options.append((current, f"{os.path.basename(current):<42} "
                                     f"in use, outside this app's folder"))
        else:
            print(f"[cosim] note: this setup's scenario yaml is gone "
                  f"({current}); pick one below.")
            current = None

    who = f"{app_title} on '{map_name}'" if app_title else f"'{map_name}'"
    app_paths = {c["path"] for c in staged}
    # WHICH yaml is only half of it: mostly there is one, and what people actually
    # want is to change what is IN it. So the list always offers the file itself.
    picked = _config_menu(who, options, current, interactive=interactive)
    print(f"[cosim] scenario config: {picked}")
    return picked, ("app" if picked in app_paths else "map")


def _declares_backend(config_yaml):
    """Whether the yaml sets CarlaSetup.EnablePythonBackend itself.

    read_backend cannot say: it answers 'py' both for "the file says py" and for
    "the file says nothing", which is exactly the ambiguity declared_engine exists
    to cover. A textual check distinguishes them without a second parse.
    """
    try:
        with open(config_yaml, encoding="utf-8", errors="replace") as f:
            return any(ln.split("#", 1)[0].strip().startswith("EnablePythonBackend:")
                       for ln in f)
    except OSError:
        return False


def declared_engine(staged, config_yaml):
    """The bridge an app declares its yaml is written for, or None.

    Needed because a hand-written app yaml usually omits
    CarlaSetup.EnablePythonBackend, and ConfigHelper defaults that to true - so a
    yaml built for TrafficLayer + VirCarlaEnv would silently run the python bridge
    instead. The app says which stack it means; --engine still overrides."""
    for c in staged or []:
        if c["engine"] and os.path.normcase(c["path"]) == os.path.normcase(config_yaml or ""):
            return c["engine"]
    return None


def main():
    ap = _Parser(description=__doc__,
                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--app", default=None,
                    help="application to run, by id, from the app repo's apps/apps.json. "
                         "Omit it and you pick from the declared apps; the choice "
                         "pre-selects that app's map(s) and scenario yamls.")
    ap.add_argument("--no-app", action="store_true",
                    help="ignore apps/apps.json entirely (generic, map-only co-sim)")
    ap.add_argument("--profile", default=None,
                    help="run setup to reuse from ~/.fixs/run_profiles.json (default: the "
                         "one saved for the chosen app, else whatever ran last)")
    ap.add_argument("--fresh", action="store_true",
                    help="ignore the saved run profile and ask every question again")
    ap.add_argument("--sumocfg", default=None,
                    help="SUMO .sumocfg. Optional: if omitted it comes from the "
                         "application's apps.json 'sumocfg' if it declares one, else "
                         "the chosen map bundle's sumo/ (a DT-Library map ships its "
                         "scenario). Pass it to run your own demand - an app that "
                         "generates its scenario is given it and builds from it, so "
                         "its ego and outputs stay with the network you named.")
    ap.add_argument("--map", default=None,
                    help="Map to run: a Digital-Twin-Library location (e.g. 'roosevelt'), "
                         "or an already-cooked map name. If omitted, pick from the catalog "
                         "/ local ~/.fixs/maps (lists options, you choose, it auto-downloads).")
    ap.add_argument("--repo", default=None,
                    help="owner/repo whose map-* releases to offer when --map is omitted "
                         "(or a repo= line in --map-config)")
    ap.add_argument("--tag-prefix", default=None,
                    help="release tag prefix for the picker (e.g. 'map-'); the map name is "
                         "the tag minus this prefix")
    ap.add_argument("--auto-import", action="store_true",
                    help="source build: if the map isn't imported, cook it before launching "
                         "(implied when you pick a version via --repo)")
    ap.add_argument("--map-package", default=None,
                    help="import package name if it differs from --map")
    ap.add_argument("--map-package-url", default=None,
                    help="URL of the map package zip for --auto-import (e.g. a release asset)")
    ap.add_argument("--map-config", default=None,
                    help="text file declaring the import package (package= and url= lines)")
    ap.add_argument("--reimport", nargs="?", const=True, default=False,
                    metavar="PATH",
                    help="re-import the map even if already cooked (re-download + "
                         "re-cook). With PATH: for a LOCAL map, re-import from "
                         "that .fbx/.xodr file or folder directly, skipping the "
                         "file dialog, and remember it for next time - PATH is "
                         "ignored (with a note) for a Digital-Twin-Library map, "
                         "which has nothing local to browse to.")
    ap.add_argument("--purge-map", nargs="?", const="", default=None,
                    metavar="NAME[,NAME]",
                    help="delete imported maps from this machine, then stop. With no "
                         "NAME: list what is here and ask (accepts 1,3,4 or 'all'). "
                         "Keeps the downloaded bundle unless asked otherwise.")
    ap.add_argument("--purge-cache", dest="purge_cache", action="store_true",
                    default=None,
                    help="with --purge-map: drop ~/.fixs/maps/<name>/ too "
                         "(the next import re-downloads it)")
    ap.add_argument("--keep-cache", dest="purge_cache", action="store_false",
                    help="with --purge-map: keep ~/.fixs/maps/<name>/ without asking")
    ap.add_argument("--clean-import", action="store_true",
                    help="before importing, clear stale per-map staging out of "
                         "CARLA's Import/ (everything except the map this run "
                         "imports). CARLA's own files and anything unrecognised are "
                         "listed, never removed. Complements --purge-map, which "
                         "clears what ONE map owns everywhere.")
    ap.add_argument("--tl-table", default=None, help="traffic_light_table.csv (for --tls-manager sumo)")
    ap.add_argument("--tls-manager", default=None, choices=["sumo", "carla", "none"],
                    help="who drives the lights (default: the map's catalog setting, else 'sumo')")
    # The CARLA world step. default=None so "was this flag given?" stays answerable:
    # unset means "whatever the scenario yaml says", an explicit value is written
    # THROUGH to the yaml (the --carla-host pattern) so both engines read one file.
    # SUMO's step is not offered - it is the FIXS exchange period (FIXS_FEED_S).
    ap.add_argument("--carla-tick", type=float, default=None,
                    choices=list(CARLA_TICK_CHOICES),
                    help=f"CARLA world step in seconds / fixed_delta_seconds, written "
                         f"to CarlaSetup.CarlaTimeStep. The FIXS feed (and SUMO's "
                         f"--step-length) is fixed at {FIXS_FEED_S:g} s; a finer tick "
                         f"makes the bridge interpolate traffic across the sub-steps.")
    ap.add_argument("--step-length", type=float, default=None,
                    help=argparse.SUPPRESS)   # deprecated: see --carla-tick
    ap.add_argument("--quality-level", choices=["Low", "Medium", "High", "Epic"], default=None,
                    help="CARLA render quality; Low is much faster on heavy maps")
    ap.add_argument("--fast", action="store_true",
                    help="do not pace the co-sim to real time (run as fast as "
                         "possible). Written to CarlaSetup.RealtimePacing, so it "
                         "applies to whichever bridge runs.")
    # ON by default. Every tl_table FIXS writes is in SUMO coordinates - measured
    # across five cooked maps here (uga, roosevelt, mlk x2, atlanta): 1156 rows, not
    # one negative y. CARLA's world is left-handed, so SUMO (x, y) is CARLA (x, -y),
    # and without the flip the spectator is sent to +y where there is no road: on the
    # UGA campus map the busiest junction sits at y=1200, so the camera framed a point
    # 2400 m north of it and the map looked unloaded. Off was never right for a
    # FIXS-generated table; it is only right for a table already in CARLA coordinates,
    # which is what --net-offset is for.
    ap.add_argument("--no-net-offset", dest="no_net_offset", action="store_true",
                    default=None,
                    help="zero the SUMO net offset - read the TL table's y as SUMO y "
                         "and flip it into CARLA's left-handed world. ON by default.")
    ap.add_argument("--net-offset", dest="no_net_offset", action="store_false",
                    help="the opposite: take the TL table's y as CARLA y unchanged, "
                         "for a table already written in CARLA coordinates")
    ap.add_argument("--carla-host", default=None,
                    help="CARLA RPC host (default: CarlaSetup.CarlaServerIP from "
                         "the scenario yaml, else localhost). A non-local host "
                         "implies --no-launch.")
    ap.add_argument("--carla-port", type=int, default=None,
                    help="CARLA RPC port (default: CarlaSetup.CarlaServerPort from "
                         "the scenario yaml, else 2000)")
    ap.add_argument("--carla-timeout", type=float, default=10.0,
                    help="CARLA client connect timeout in seconds (default: 10; "
                         "raise for heavy source-build maps)")
    # SUMO + TrafficLayer + the app's controller, with no CARLA anywhere: not
    # launched, not connected to, not rendered into. For a scenario CARLA cannot
    # render yet - a network edited on the SUMO side, or a controller change worth
    # checking before a map is cooked for it.
    #
    # SUPPRESSed rather than documented: this is a development escape hatch, and
    # run_cosim's whole promise is that what you pick is a co-simulation. Offering
    # "...but without the simulator half" in --help invites picking it to make a
    # CARLA problem go away, which is how you end up with results nobody can place.
    # Un-suppress it when it is something users are meant to reach for.
    ap.add_argument("--sumo-only", action="store_true", help=argparse.SUPPRESS)
    ap.add_argument("--connect-timeout", type=float, default=15.0,
                    help="seconds to wait for the CARLA RPC handshake (default 15). "
                         "Separate from --load-timeout: reaching a server is fast or "
                         "not happening, while loading a freshly cooked map is slow.")
    ap.add_argument("--load-timeout", type=float, default=300.0,
                    help="client timeout (s) for load_world; the first load of a freshly "
                         "imported map compiles shaders and can take minutes (default 300)")
    ap.add_argument("--no-launch", action="store_true", help="CARLA is already running")
    ap.add_argument("--prep-only", action="store_true",
                    help="import the map + place traffic lights and signs, then stop "
                         "(do not launch CARLA or run the co-sim)")
    ap.add_argument("--doctor", action="store_true",
                    help="check this machine can run a co-sim (python deps, SUMO, "
                         "FIXS binaries, CARLA, peer, maps, gh auth) and exit; "
                         "non-zero exit if anything is broken")
    ap.add_argument("--role", choices=["traffic", "render"], default=None,
                    help="which half of a distributed co-sim this machine is; "
                         "--doctor infers it from what is installed here, and this "
                         "overrides that")
    ap.add_argument("--version", action="store_true",
                    help="print the run fingerprint (FIXS, CARLA, SUMO, python) and "
                         "exit - what to paste into a bug report")
    ap.add_argument("--log", action="store_true",
                    help="tee this run's console output to "
                         "RealSim_tmp/run_cosim_<host>_<stamp>.log. Run it on BOTH "
                         "machines to get the two halves of a distributed run.")
    ap.add_argument("--log-file", default=None,
                    help="log to this path instead of the default (implies --log)")
    ap.add_argument("--no-quickedit", action="store_true",
                    help="Windows: turn off the console's QuickEdit mode for this "
                         "run. QuickEdit is what lets you select text with the mouse, "
                         "but a stray click then blocks stdout and stalls the co-sim "
                         "until you press Enter. Worth it for a long unattended run; "
                         "costs you mouse copy/paste.")
    ap.add_argument("--peer", default=None, metavar="HOST[:PORT]",
                    help="traffic host: the machine running CARLA, which is where "
                         "the other half of a distributed run sits (it runs "
                         "--serve). Written to the scenario yaml's "
                         "CarlaSetup.CarlaServerIP/Port, so every component here "
                         "dials it; implies --no-launch. The same setting as "
                         "--carla-host/--carla-port, spelled the way a two-machine "
                         "run is described.")
    ap.add_argument("--serve", action="store_true",
                    help="CARLA host: wait for the traffic machine, serve the map it "
                         "asks for, and hold CARLA until it disconnects. Like "
                         "--carla-only but driven by the peer instead of by flags.")
    ap.add_argument("--peer-port", type=int, default=None,
                    help="control-channel port (default: CarlaServerPort + 400). Set "
                         "it on BOTH machines if the default is taken.")
    ap.add_argument("--allow-map-skew", action="store_true",
                    help="run even when the cooked map came from a different bundle "
                         "than the SUMO data being run against it. Warned about on a "
                         "normal run; refused under --serve, which this overrides.")
    ap.add_argument("--peer-wait", type=float, default=120.0,
                    help="traffic host: seconds to keep retrying the CARLA peer "
                         "(default 120), so the two machines can start in any order.")
    ap.add_argument("--prep-timeout", type=float, default=2700.0,
                    help="traffic host: seconds to wait for the peer to cook and "
                         "launch (default 2700). A first cook is minutes of Unreal "
                         "work plus shader compilation.")
    ap.add_argument("--allow-manual-carla", action="store_true",
                    help="traffic host: run against a CARLA started by hand on the "
                         "peer host when no 'run_cosim --serve' answers. Off by "
                         "default - a leftover server from an earlier run happily "
                         "accepts the connection while holding a DIFFERENT map.")
    ap.add_argument("--carla-only", action="store_true",
                    help="launch CARLA, load the map and HOLD it open, running no "
                         "bridge here - the CARLA half of a two-machine run, driven "
                         "by run_cosim on the traffic machine. Ctrl+C stops it.")
    ap.add_argument("--reconfigure", action="store_true",
                    help="re-run CARLA env setup before launching (pick a different CARLA)")
    ap.add_argument("--update-python", action="store_true",
                    help="re-resolve ONLY the python env (carla + SUMO client) and save "
                         "it to carla.json, then exit. The CARLA / UE4 paths are kept. "
                         "Use after creating the env setup asked for, or to move off one "
                         "picked by mistake; every entry point follows carla.json, so "
                         "this changes them all at once")
    ap.add_argument("--render-offscreen", action="store_true", help="headless CARLA")
    ap.add_argument("--no-spectator", action="store_true",
                    help="do not auto-frame the CARLA spectator on the scene")
    ap.add_argument("--spectator-all", action="store_true",
                    help="frame the whole network instead of one intersection")
    ap.add_argument("--spectator-junction", default=None,
                    help="frame this junction id (default: the busiest intersection)")
    # Tri-state for the same reason as --step-length: the one-click launchers always
    # pass --sumo-gui, so a plain store_true would make "headless" unsaveable in a
    # run profile. None = not specified, fill from the profile.
    ap.add_argument("--sumo-gui", dest="sumo_gui", action="store_true", default=None,
                    help="run SUMO with its GUI (default)")
    ap.add_argument("--no-sumo-gui", dest="sumo_gui", action="store_false",
                    help="run SUMO headless")
    ap.add_argument("--engine", choices=["py", "cpp"], default=None,
                    help="which VirEnvCore drives the FIXS-native stack: "
                         "py=Carla/VirEnv/mainVirCarla.py (the Python core), "
                         "cpp=VirCarlaEnv.exe (the C++ core). Both run SUMO + "
                         "TrafficLayer + a bridge and read the same yaml. Overrides "
                         "CarlaSetup.EnablePythonBackend.")
    ap.add_argument("--standalone-bridge", action="store_true",
                    help="run CARLA's own run_synchronization.py instead of the FIXS "
                         "stack. It speaks no FIXS and drives SUMO over its own TraCI "
                         "connection, so no controller, XIL component or TrafficLayer "
                         "subscription takes part. Outside the normal FIXS route; "
                         "kept for comparison against upstream CARLA.")
    ap.add_argument("--config", default=None,
                    help="[cpp] scenario yaml for the native bridge (default: "
                         "~/.fixs/maps/<cooked>/config.yaml, generated if missing).")
    ap.add_argument("--refresh-deps", action="store_true",
                    help="re-apply the selected application's 'requirements' into "
                         "the configured interpreter even if they were already "
                         "applied. Normally they are installed once and skipped "
                         "while the file is unchanged.")
    ap.add_argument("--no-update-check", action="store_true",
                    help="skip the FIXS-bundle freshness check against the release")
    ap.add_argument("--sumo-no-start", action="store_true",
                    help="[cpp] launch sumo-gui but omit --start, so it opens loaded "
                         "and waits for you to press Play (overrides SumoSetup.AutoStart)")
    args = ap.parse_args()
    # --reimport PATH parses as a string in args.reimport; split it out here so
    # every OTHER site in this file - there are over a dozen - can go on reading
    # args.reimport as the plain bool it has always been.
    args.reimport_path = args.reimport if isinstance(args.reimport, str) else None
    args.reimport = bool(args.reimport)

    # Resolved before anything reads the endpoint, so --doctor, --version and the
    # run itself all see one setting rather than two spellings of it.
    resolve_peer(args)

    # OFF by default. Disabling QuickEdit costs mouse selection in cmd, and losing
    # copy/paste on every run is a worse trade than an occasional freeze that Enter
    # clears - the freeze is at least recoverable, and the title bar says "Select"
    # while it is happening. --no-quickedit for unattended runs, where nobody is
    # there to press Enter.
    if args.no_quickedit:
        _disable_quickedit()
    if args.log or args.log_file:
        start_log(args.log_file)

    # --update-python answers a question and stops, and it is the one flag that must
    # NOT re-exec first: re-execing runs it under the very interpreter it exists to
    # replace, so a config pointing at a broken env could never be repaired from the
    # front door. Handled before everything else for the same reason.
    if args.update_python:
        return env.update_python()

    # --doctor and --version answer a question and stop. Neither touches a map, a
    # setup or a server, so they are safe to run at any time - including while a
    # co-sim is going, which is exactly when someone wants them.
    if args.doctor or args.version:
        cfg = env.load_config()
        host, port = args.carla_host, args.carla_port
        if host is None or port is None:
            peek_host, peek_port = _peek_any_endpoint()
            host = host or peek_host or DEFAULT_CARLA_HOST
            port = port or peek_port or DEFAULT_CARLA_PORT
        if args.version:
            return print_fingerprint(cfg, host, port)
        import doctor
        import peer
        return doctor.run(cfg, env, FIXS_ROOT,
                          os.path.join(os.path.dirname(env.CONFIG_PATH), "maps"),
                          host, port, _fixs_version(),
                          peer_port=args.peer_port or peer.peer_port(port),
                          who_has_port=_who_has_port,
                          **_doctor_role(doctor, cfg, args))

    # --purge-map answers a question about this machine's disk and stops. Sits with
    # --doctor rather than further down because it is pure filesystem work: it needs
    # the saved config to find CARLA, but never the carla module, so it must not pay
    # for (or be blocked by) the re-exec and env repair everything below depends on.
    #
    # The work is import_map's - it owns where a cooked map, its staging and its
    # bundle live, and those rules differ by flavour. What is passed in is what only
    # this process can answer: whether anyone is there to be asked, and whether a
    # CARLA is holding Content/ open. Same shape as doctor.run(who_has_port=...).
    if args.purge_map is not None:
        import import_map
        cfg = env.load_config() or {}
        mode = cfg.get("mode") or "source"
        port = args.carla_port or DEFAULT_CARLA_PORT
        return import_map.purge(
            # client mode means no CARLA on this machine, so there is no cooked
            # content here to find - only the bundle cache, which is still local.
            None if mode == "client" else cfg.get("carla_root"),
            mode=mode, named=args.purge_map, drop_cache=args.purge_cache,
            interactive=_interactive(args),
            carla_busy=lambda: _carla_pid_on(port),
            carla_kill=_kill_pid_tree)

    # --carla-only holds a CARLA this machine launched, so the flags that mean
    # "launch nothing" contradict it outright. Caught here rather than later
    # because --prep-only returns before the launch, which would look like
    # --carla-only silently doing nothing.
    if args.carla_only:
        for flag, on in (("--no-launch", args.no_launch), ("--prep-only", args.prep_only)):
            if on:
                sys.exit(f"[cosim] --carla-only launches and holds CARLA here; "
                         f"{flag} means the opposite. Pick one.")

    # --step-length used to be the SHARED timestep, handed to SUMO and to CARLA at
    # once. SUMO's is now the FIXS exchange period (a constant), so the only thing
    # left to choose is CARLA's: accept the old flag as the tick rather than breaking
    # a saved command line, and say what it now means.
    if args.step_length is not None:
        if args.carla_tick is None and args.step_length in CARLA_TICK_CHOICES:
            args.carla_tick = args.step_length
            print(f"[cosim] --step-length is now --carla-tick: SUMO always steps at "
                  f"the {FIXS_FEED_S:g} s FIXS feed, so {args.step_length:g} s is "
                  f"taken as the CARLA tick.")
        else:
            print(f"[cosim] ignoring --step-length {args.step_length:g}: SUMO steps at "
                  f"the {FIXS_FEED_S:g} s FIXS feed (fixed by the protocol). Use "
                  f"--carla-tick "
                  f"({', '.join(str(c) for c in CARLA_TICK_CHOICES)}) for the CARLA "
                  f"world step.")

    # Advisory: nudge to update a stale local FIXS bundle before doing real work.
    maybe_update_fixs(no_check=args.no_update_check, interactive=_interactive(args))

    # Resolve the saved CARLA env (running first-time setup if needed) and make
    # sure we are on its python before importing carla. --no-launch still needs
    # the env for the python/carla client, but won't force CARLA-path setup.
    cfg = env.load_config()
    if not args.no_launch and (cfg is None or args.reconfigure):
        cfg = resolve_carla_env(reconfigure=args.reconfigure)
    if cfg is not None:
        cfg = env.ensure_runtime(cfg)   # repair stale configs that lack a python env
        # may not return (re-execs under the env python). --reconfigure is dropped:
        # it has already been honoured above, and the child must not ask again.
        env.reexec_under_configured(__file__, cfg, drop=("--reconfigure",), tag="cosim")
        if cfg.get("mode") == "client" and args.carla_only:
            sys.exit("[cosim] --carla-only serves a CARLA on THIS machine, but "
                     "carla.json is 'client' mode (no CARLA here). Run it on the "
                     "machine that has CARLA, or re-run setup_carla and pick one.")
        if cfg.get("mode") == "client" and not args.no_launch:
            # There is no CARLA here to launch, cook into, or place actors in -
            # that is what the mode means. Decided HERE, before anything reads
            # carla_root, so the rest of the run takes the path a remote host
            # already takes. (The cook/place preflights are additionally gated
            # on mode == "source", so they stay skipped either way.)
            print("[cosim] carla.json says client mode (no CARLA on this machine); "
                  "implying --no-launch.")
            args.no_launch = True
    elif args.no_launch:
        print("[cosim] no CARLA env configured; running under the current python. "
              "If 'import carla' fails, run setup_carla first.")

    import import_map
    repo, tag_prefix = import_map.resolve_map_source(args.repo, args.tag_prefix)
    catalog = import_map.fetch_catalog(repo)

    # Everything the run needs - application, map, scenario yaml, bridge, CARLA,
    # SUMO - is decided here, from a saved setup you pick and edit until you
    # confirm. Deliberately AFTER the re-exec: it prompts, and re-execing under
    # the env python afterwards would ask the same questions twice. And
    # deliberately BEFORE anything heavy: no download, cook or load happens while
    # the user is still deciding what to run, so quitting out of it leaves nothing
    # half-done.
    # --serve: block here until the traffic machine says what to run, then fall
    # through into the ordinary flow with its answers. Deliberately BEFORE
    # configure_run, because a serving host must not run the picker - two people
    # choosing independently is how the two ends come to disagree about the map.
    ctl_sock = None
    if args.serve:
        ctl_sock = await_peer(args)

    # Ctrl+C at a prompt, and the deliberate quits, both leave by an exception; the
    # answers are only worth keeping on the way out, so the checkpoint hangs off
    # the exit rather than costing a write per question.
    try:
        app, setup_name, staged_configs, setup, cfg, ctx = configure_run(
            args, cfg, repo, tag_prefix, catalog)
    except (KeyboardInterrupt, SystemExit):
        _checkpoint_interrupted_setup()
        raise
    finally:
        _IN_PROGRESS.clear()

    # The app is settled and we are already running under the configured interpreter
    # (reexec_under_configured above), so this is the first point where "what does THIS app
    # need" is answerable. Deliberately here rather than later: it is still before
    # any download, cook or load, so a missing package is reported while nothing
    # slow has happened yet - instead of as an ImportError minutes into a run.
    if app and app.get("requirements"):
        if not env.ensure_app_deps(sys.executable, app["id"],
                                   os.path.join(app_catalog.app_dir(app),
                                                app["requirements"]),
                                   refresh=args.refresh_deps):
            sys.exit(f"[cosim] '{app['id']}' is missing its declared dependencies; "
                     f"not starting the run.")

    # Two slots to fill: a CARLA map (to cook + load) and a SUMO scenario. A
    # Digital-Twin-Library bundle fills both. The map itself was settled above; what
    # is derived here is how to GET it - the release to download, the per-map
    # settings - which comes from the catalog by name and so never has to be stored
    # in the setup or re-asked.
    target_map = args.map
    if not target_map:
        sys.exit("[cosim] no map chosen; pass --map or pick one when prompted.")
    ent = import_map.catalog_entry(catalog, target_map)
    settings = ent.get("settings", {}) if ent else {}
    picked_tag = ent["release"] if ent else None      # DT release to (lazily) download
    if ent and ent["map_name"] != target_map:
        print(f"[cosim] map '{target_map}' -> '{ent['map_name']}'  "
              f"(DT release {picked_tag})")
        target_map = ent["map_name"]
    map_origin = setup.get("map_origin") or ("Digital-Twin-Library" if ent else "cooked")
    # A local .zip/folder pick cannot be re-derived from a name, so it rides along
    # in the setup; it is only reused while the file is still there.
    picked_local = ctx.get("picked_local") or setup.get("map_local")
    if picked_local and not os.path.exists(picked_local):
        picked_local = None

    # --reimport on a LOCAL map used to silently trust whatever path this setup
    # remembered from however long ago - which goes stale the moment the export
    # moves, and re-cooks last week's files with no error at all if the old
    # folder is still sitting there with old content in it. A path given on the
    # command line, or a fresh pick made just now in the editor (ctx), always
    # wins; a bare --reimport with neither reopens the same browse dialog used at
    # first setup instead of trusting the old one. The result becomes this run's
    # picked_local, which the checkpoint save right below writes back into
    # map_local - so the NEXT --reimport remembers whatever was just chosen here.
    if args.reimport:
        if map_origin != "local file":
            if args.reimport_path:
                print(f"[cosim] --reimport {args.reimport_path!r} ignored: "
                      f"'{target_map}' is a {map_origin} map, not a local pick.")
        elif args.reimport_path:
            # An explicit path wins even over a fresh pick just made in the
            # editor (ctx) - it is the more deliberate of the two - so whatever
            # that pick's own _sumo_for_local_pick call already asked about is
            # stale the moment this line runs. Ask again, for the path that will
            # actually be cooked.
            picked_local = args.reimport_path
            _sumo_for_local_pick(setup, ctx, picked_local, args)
        elif not ctx.get("picked_local"):
            # No fresh pick this run either: the browse dialog opened just below
            # is the only place that named a source, so it is the only place
            # that can ask about its SUMO half. A fresh pick THROUGH THE EDITOR
            # (which would have made ctx["picked_local"] truthy) already asked,
            # via _edit_slots's own call - asking again here would be a second
            # dialog for the same answer.
            picked_local = import_map._select_package("map", None,
                                                       inside=("xodr", "fbx"))
            _sumo_for_local_pick(setup, ctx, picked_local, args)
    # An app that BUILDS its scenario from the map's needs the map's scenario on disk
    # BEFORE it starts, which is the opposite of the default the comment below
    # describes - so it is opened here, and only for an app that says it needs it.
    # `sumo_dir` is the same one the SUMO slot further down reuses; opening the
    # bundle twice would prompt twice over the same archive.
    sumo_dir = None              # dir holding the chosen bundle's .sumocfg (set on open)
    map_sumocfg = None
    if app and app.get("needs_map_sumo") and args.sumocfg is None:
        sumo_dir = import_map.map_sumo_dir(target_map)
        if sumo_dir is not None:
            # This app BUILDS its scenario from the map's, so the map's files decide
            # what the traffic does. Say whether they are still the published ones
            # before anything is built on top of them.
            import_map.report_bundle_parity(
                target_map, "sumo", where=import_map.bundle_sumocfg(sumo_dir),
                repo=repo, tag=(ent or {}).get("release"),
                asset=(ent or {}).get("asset"))
        if sumo_dir is None and (picked_local or picked_tag):
            bundle = picked_local
            if not bundle and picked_tag:
                bundle = import_map.download_release_zip(
                    repo, picked_tag, force_redownload=args.reimport,
                    cache_name=target_map, asset=(ent or {}).get("asset"))
            # A precooked .tar.gz is the CARLA half only - there is no sumo/ in it.
            if bundle and not str(bundle).lower().endswith(".tar.gz"):
                _carla_src, sumo_dir = import_map.open_bundle(bundle,
                                                              cache_name=target_map)
        map_sumocfg = import_map.bundle_sumocfg(sumo_dir)
        if map_sumocfg:
            print(f"[cosim] '{app['id']}' builds its scenario from the map's: "
                  f"{map_sumocfg}")
        else:
            print(f"[cosim] '{app['id']}' declares needs_map_sumo, but '{target_map}' "
                  f"ships no SUMO scenario to build from; it falls back to its own.")

    # The application is launched further down - see "The application starts HERE".
    # Bound now because cached_sumo_dir, defined immediately below, reads
    # app_sumocfg, and a closure over a name that does not exist yet is a
    # NameError waiting for the first caller.
    app_proc, app_sumocfg = (None, None)

    def cached_sumo_dir(name):
        """An already-extracted ~/.fixs/maps/<name>/sumo, or None.

        Consulted at EVERY site that would otherwise reach for the map bundle,
        because opening the bundle is not free: download_release_zip prompts
        "[U]se it / [R]e-download" over a ~380MB archive that a map with its
        sumo/ already extracted would immediately throw away. There is more than
        one such site - the source-build preflight, and the SUMO slot below that
        also runs for --no-launch / packaged builds - and fixing only one of them
        just moves the prompt. --sumocfg, an app-reported scenario and --reimport
        deliberately bypass it: the first two supply the scenario outright, the
        last means "refresh from the bundle"."""
        if args.sumocfg is not None or app_sumocfg is not None or args.reimport:
            return None
        found = import_map.map_sumo_dir(name)
        if found:
            # Not just WHICH scenario, but whether it is still the published one.
            # A cached sumo/ is reused for runs and runs, and nothing else on screen
            # would ever mention that its contents had drifted from the library's.
            import_map.report_bundle_parity(
                name, "sumo", where=import_map.bundle_sumocfg(found),
                repo=repo, tag=(ent or {}).get("release"),
                asset=(ent or {}).get("asset"))
        return found

    # Checkpoint. Everything the questionnaire asked is now decided, and the next
    # thing that runs - the map import - is the one that fails for reasons outside
    # this script (a cook that crashes the editor, a bundle that will not open).
    # The full save is ~400 lines below, past that import, so a failed cook used to
    # discard the app, map and local pick just chosen. config/config_scope are NOT
    # passed: they are resolved after the import, and `setup` already carries
    # whatever the previous run knew, which is the right thing to come back to.
    run_profile.save_partial(setup_name, {**setup,
                                          "app": app["id"] if app else None,
                                          "map": target_map,
                                          "map_origin": map_origin,
                                          "map_local": picked_local,
                                          "sumo_gui": bool(args.sumo_gui)},
                             "map import")
    _CHECKPOINT["name"] = setup_name
    # Where this map's CARLA half came from. Only a LOCAL pick needs it: a
    # release download already stamps .source_sha, and a pick has no
    # equivalent - which is why a hand-picked map carries no origin at all
    # today. Written before the import, so a cook that fails still leaves a
    # record of what was attempted.
    if picked_local:
        import_map.record_source(target_map, "carla", picked_local)
    # Was the map chosen from the menu on THIS run? Only then is "you just picked a
    # map that is already cooked - reimport it?" worth asking. A replayed setup is a
    # deliberate re-run, and prompting about re-cooking it every time is noise.
    picked_now = bool(ctx.get("picked_now"))

    # Per-map settings are defaults; explicit CLI flags override.
    # None = neither --no-net-offset nor --net-offset was given, so the map's own
    # setting decides and its default is ON (see the flag). A map opts out with
    # net_offset: "keep"; "zero" is the historical spelling of ON and still reads as
    # ON here, so an existing per-map setting keeps meaning what it meant.
    no_net_offset = (args.no_net_offset if args.no_net_offset is not None
                     else settings.get("net_offset") != "keep")
    tls_manager = args.tls_manager or settings.get("tls_manager") or "sumo"

    # /Game/... path to boot CARLA into (set by the source-build preflight). None
    # (packaged build / --no-launch) = let the engine pick.
    target_level = None

    # Is CARLA on this machine? Decided BEFORE the source-build preflight below,
    # because everything that preflight does - cooking the package into carla_root
    # via Util/BuildTools/Import.py, placing TLs/signs through the local UE4 editor
    # - writes to a LOCAL CARLA install and is wasted (minutes of cooking) when the
    # server we will actually talk to is on another host. Best-effort by design: a
    # map with no yaml yet cannot name a remote CARLA, and the yaml we are about to
    # generate for it will name a local one. The authoritative host/port resolution
    # still happens after config_yaml is final.
    if args.carla_host is None:
        peek = args.config or app_catalog.scenario_path(
            setup.get("app") or app_catalog.GENERIC, target_map)
        if os.path.isfile(peek):
            peek_host, _peek_port = read_carla_endpoint(peek)
            if peek_host and not _is_local_host(peek_host):
                args.carla_host = peek_host
    if args.carla_only and args.carla_host and not _is_local_host(args.carla_host):
        sys.exit(f"[cosim] --carla-only serves CARLA from THIS machine, but the "
                 f"scenario yaml points CarlaSetup.CarlaServerIP at "
                 f"{args.carla_host}. Run --carla-only on that host, or clear the "
                 f"setting so this machine serves.")
    if args.carla_host and not _is_local_host(args.carla_host) and not args.no_launch:
        print(f"[cosim] CARLA host is {args.carla_host} (not this machine); "
              f"implying --no-launch. NOTE: importing/cooking the map and placing "
              f"TLs/signs are local-only operations - that CARLA must already have "
              f"'{target_map}' cooked with TLs/signs placed.")
        args.no_launch = True

    # Asked for before the import, not after: Import/ is the folder CARLA's
    # Import.py cooks out of, so clearing it is only meaningful while there is
    # still a cook ahead. `keep` is this run's map, which must survive.
    if args.clean_import and cfg is not None and cfg.get("carla_root"):
        import_map.clean_import_dir(cfg["carla_root"], keep=target_map,
                                    interactive=_interactive(args),
                                    assume_yes=not _interactive(args))

    # Source-build preflight: a custom map must be cooked into the build before
    # CARLA can load it. Import it if missing - from a DT-Library bundle (downloaded
    # + cached, split into carla/ + sumo/) or a local pick - else fail clearly.
    if not args.no_launch and cfg is not None and cfg.get("mode") == "source":
        resolved = None if args.reimport else \
            import_map.resolve_cooked_map(cfg["carla_root"], target_map, mode="source")
        if resolved is not None:
            _check_map_source(args, cfg["carla_root"], target_map, "source")

        # Already imported? If this was a FRESH source pick (an online release or a
        # local .zip/folder), offer to reimport - re-cook + re-place TLs/signs +
        # regenerate the TL table. A pick of an already-imported map (both picked_*
        # None) is run as-is, no prompt.
        if resolved is not None and picked_now and (picked_tag or picked_local) \
                and not args.reimport and _interactive(args):
            ans = input(f"[cosim] '{resolved[0]}' is already imported. Reimport "
                        f"(re-cook + re-place TLs/signs + regen TL table)? [y/N]: ").strip().lower()
            if ans.startswith("y"):
                args.reimport = True
                resolved = None

    # The application starts HERE: after the last question this script asks, and
    # before anything reaches for a map bundle.
    #
    # BEFORE THE BUNDLE, because the app may be the one that says which scenario to
    # run - and an app that generates its own from scratch needs no sumo/ half at all,
    # so reaching for one would prompt over a ~380MB archive whose SUMO content is
    # about to be thrown away.
    #
    # AFTER THE LAST QUESTION, because the controller prints "Waiting for TrafficLayer
    # on <host>:<port> ..." to this same terminal the moment it starts. Launched
    # before the reimport prompt, that line landed on top of the question:
    #
    #     [cosim] 'mlk_no_signal' is already imported. Reimport (re-cook + re-place
    #     TLs/signs + regen TL table)? [y/N]: Waiting for TrafficLayer on 127.0.0.1:430
    #
    # - a question and an unrelated status line sharing one row, with the cursor
    # parked after the wrong one. The prompt is the LAST thing this script asks, so
    # starting the app just after it is enough; there is no output to interleave with
    # from here on.
    #
    # There is a second reason to start it late. Every sys.exit between the
    # questionnaire and this point - an unreachable remote CARLA, a map that will not
    # resolve, a source check that fails - used to leave the controller running as an
    # orphan, waiting on a bridge that was never going to come.
    #
    # It keeps running from this point: it is the controller, and it waits for
    # TrafficLayer while the map is cooked and CARLA comes up. A first cook is
    # minutes, so its wait for the bridge has to be patient - run_cosim stops it if
    # anything below fails.
    if app and app.get("launch"):
        # The yaml this run will hand TrafficLayer. Known already for a config that
        # was chosen (--config, or the one the profile remembers); a first run that
        # GENERATES a per-map config has none yet, and the app falls back to its own.
        app_proc, app_sumocfg = start_app(app, args.config or setup.get("config"),
                                          sumo_only=args.sumo_only,
                                          sumocfg=args.sumocfg or map_sumocfg)

    # Same condition as the preflight above, resumed. Split rather than moved so the
    # app can start between the two: everything above this line only DECIDES what the
    # import will do, everything below it acts on that decision, and the app has to
    # start in between - after the last prompt, before the first bundle read.
    if not args.no_launch and cfg is not None and cfg.get("mode") == "source":
        # The bundle fills two slots - the CARLA package to cook, and the SUMO
        # scenario - so check what is actually still missing before touching it.
        if sumo_dir is None:
            sumo_dir = cached_sumo_dir(target_map)

        # download_release_zip caches under ~/.fixs/maps; open_bundle splits it.
        carla_src = None
        need_bundle = (resolved is None                          # must cook the map
                       or (args.sumocfg is None                  # need its sumo/
                           and app_sumocfg is None
                           and sumo_dir is None))
        if need_bundle and (picked_local or picked_tag):
            zip_path = picked_local or import_map.download_release_zip(
                repo, picked_tag, force_redownload=args.reimport,
                cache_name=target_map, asset=(ent or {}).get("asset"))
            carla_src, bundle_sumo = import_map.open_bundle(zip_path, cache_name=target_map)
            if bundle_sumo:            # keep a cached sumo/ if this bundle has none
                sumo_dir = bundle_sumo

        if resolved is None:
            if carla_src is not None:                    # a DT/local bundle or raw export
                # descriptor_only: a bundle that SHIPS a descriptor names the
                # package CARLA cooks, and that name is not ours to change. A raw
                # export ships none - FIXS generates it - so the name chosen at the
                # import prompt is the name. The old .xodr-stem fallback could not
                # tell those apart and always won, which silently discarded the
                # rename: the prompt took 'uga_untextured', the export was
                # ugaaa.xodr, and the cook, the descriptor and /Game all said
                # 'ugaaa' while the run profile said 'uga_untextured'.
                real = import_map.map_name_in(carla_src, descriptor_only=True) or target_map
                verb = "re-importing" if args.reimport else "importing"
                print(f"[cosim] {verb} '{real}' before launch ...")
                _tell_peer(ctl_sock, "cook", f"importing and cooking '{real}' - "
                           f"this is the slow one (Unreal + shaders)")
                import_map.ensure_map(real, carla_root=cfg["carla_root"],
                                      ue4_root=cfg.get("ue4_root"),
                                      package_dir=carla_src, force=args.reimport,
                                      source_sha=import_map.read_cached_sha(target_map))
                target_map = real
            elif args.auto_import or args.reimport:      # legacy explicit --map + url/config
                url = args.map_package_url
                if args.map_config:
                    url = url or import_map.read_map_config(args.map_config).get("url")
                verb = "re-importing" if args.reimport else "importing"
                print(f"[cosim] {verb} map '{target_map}' before launch ...")
                _tell_peer(ctl_sock, "cook", f"importing and cooking "
                           f"'{target_map}' - this is the slow one (Unreal + shaders)")
                import_map.ensure_map(target_map, carla_root=cfg["carla_root"],
                                      ue4_root=cfg.get("ue4_root"),
                                      package_url=url, force=args.reimport,
                                      source_sha=import_map.read_cached_sha(target_map))
            else:
                sys.exit(
                    f"[cosim] map '{target_map}' is not imported into {cfg['carla_root']}.\n"
                    f"        Pick a DT-Library map (--map <location>), a local bundle "
                    f"(--package-dir <zip/folder>), or --auto-import [--map-package-url <zip>].")
            resolved = import_map.resolve_cooked_map(cfg["carla_root"], target_map,
                                                     mode="source")

        if resolved is None:
            print(f"[cosim] could not tell which cooked map '{target_map}' provides; "
                  f"pick the one to load:")
            target_map = import_map.choose_imported_map(cfg["carla_root"], mode="source",
                                                        interactive=_interactive(args))
            target_level = import_map.choose_level_path(cfg["carla_root"], target_map,
                                                       mode="source",
                                                       interactive=_interactive(args))
        else:
            if resolved[0] != target_map:
                print(f"[cosim] package '{target_map}' provides map '{resolved[0]}'")
            target_map, target_level = resolved

        note = import_map.duplicate_level_note(cfg["carla_root"], target_map,
                                               target_level, mode="source")
        if note:
            print(note)

    # Packaged-build preflight: the same job as above, by the only mechanism a
    # packaged CARLA has. It cannot cook anything - cooking runs the Unreal editor,
    # which a packaged build does not ship - so the map must arrive ALREADY cooked,
    # as the DT-Library's precooked .tar.gz, and installing that is a plain extract
    # into the package root (see import_map.install_cooked).
    #
    # Without this block a packaged run skipped the CARLA slot entirely: the SUMO
    # slot below still filled, the run looked healthy, and the first sign of trouble
    # was load_world failing on a map that was never there.
    #
    # `client` never reaches here - that mode has no local CARLA to install into,
    # and the map is the remote server's business.
    if not args.no_launch and cfg is not None and cfg.get("mode") == "packaged":
        resolved = None if args.reimport else \
            import_map.resolve_cooked_map(cfg["carla_root"], target_map, mode="packaged")
        if resolved is not None:
            _check_map_source(args, cfg["carla_root"], target_map, "packaged")

        if resolved is None:
            # Say so BEFORE the download. A precooked package is gigabytes, and the
            # source branch above already narrates its cook - so this branch staying
            # silent meant the traffic machine sat through the longest step of a
            # packaged bring-up being told only "it is launching CARLA and loading
            # the map", which is not what is happening yet.
            _tell_peer(ctl_sock, "install", f"installing the precooked '{target_map}' "
                       f"package - a download and extract, several GB")
            # Which asset, and the three "not from here" cases, live in import_map -
            # the same call --import-map makes, so the two front doors cannot come to
            # disagree about what a packaged build can install (#276).
            target_map = import_map.install_precooked(
                cfg["carla_root"], target_map, repo=repo, tag=picked_tag, entry=ent,
                local=picked_local, force=args.reimport, log="cosim")
            resolved = import_map.resolve_cooked_map(cfg["carla_root"], target_map,
                                                     mode="packaged")

        if resolved is None:
            print(f"[cosim] could not tell which installed map '{target_map}' provides; "
                  f"pick the one to load:")
            target_map = import_map.choose_imported_map(cfg["carla_root"], mode="packaged",
                                                        interactive=_interactive(args))
            target_level = import_map.choose_level_path(cfg["carla_root"], target_map,
                                                        mode="packaged",
                                                        interactive=_interactive(args))
        else:
            target_map, target_level = resolved

        note = import_map.duplicate_level_note(cfg["carla_root"], target_map,
                                               target_level, mode="packaged")
        if note:
            print(note)

        # TLs and signs are placed through the UE4 editor, which a packaged build
        # does not have - so whatever the asset was cooked with is what you get, for
        # the whole run. Said out loud because the failure is silent: a map cooked
        # without traffic lights runs fine and produces a co-sim with no TL sync,
        # i.e. plausible numbers that are wrong.
        print(f"[cosim] note: packaged CARLA - traffic lights and signs cannot be "
              f"placed here (that needs a source build's editor). TL sync depends on "
              f"what '{target_map}' was cooked with.")

        # The other silent one: a cook made for a different shader platform. The
        # level loads and every actor is where it should be, so nothing downstream
        # notices - the road just renders as default-material grey.
        have_sp = import_map.shader_platforms_in(
            import_map.cooked_content_dir(cfg["carla_root"], target_map, mode="packaged"))
        want_sp = import_map.host_shader_platform()
        if have_sp and want_sp not in have_sp:
            print(f"[cosim] WARNING: '{target_map}' was cooked for "
                  f"{'/'.join(sorted(have_sp))}, not {want_sp}. A packaged CARLA has no "
                  f"shader compiler, so its materials will fall back to the default one - "
                  f"geometry and traffic lights will be correct, the road surface will "
                  f"render grey.")
            if want_sp == "d3d" and "vulkan" in have_sp:
                print(f"[cosim]   Launching CARLA with -vulkan may resolve them "
                      f"(unverified). Otherwise use a source build, or ask the map "
                      f"library for a Windows cook. See FIXS_Applications#29.")

    # SUMO slot: --sumocfg wins; else the scenario the app reported; else an
    # already-extracted sumo/, else the chosen bundle's. This also runs for the paths
    # that skip the source-build preflight above (--no-launch, packaged builds), so
    # the cache is checked here too - the bundle is the LAST resort, not the first.
    #
    # The app sits above the bundle because a bundle ships ONE app-independent
    # scenario, and an application's may not exist until the run starts: a run
    # directory, its own demand, output paths written into the config. That cannot be
    # a path anyone declares in advance, which is why the app reports it instead.
    sumocfg = args.sumocfg
    # An app that REPORTED a scenario owns it, and owning it is what turns the SUMO
    # convention off. Declaring `launch` is not enough: an app whose controller is
    # happy with the map's own scenario reports nothing, claims nothing, and keeps
    # the convention - which is what a roosevelt-shaped app with a controller wants.
    # ...and it owns it whether or not --sumocfg was passed: the app was handed that
    # flag as FIXS_SUMOCFG, so what it reports is what it built FROM it. Running the
    # flag's own file instead would throw away the ego, the demand and the output
    # paths the app just put in place.
    app_owns_scenario = app_sumocfg is not None
    if app_owns_scenario:
        if sumocfg is not None and os.path.abspath(sumocfg) != os.path.abspath(app_sumocfg):
            print(f"[cosim] --sumocfg reached '{app['id']}' as FIXS_SUMOCFG; running "
                  f"what it built from it")
            print(f"[cosim]   you named : {sumocfg}")
            print(f"[cosim]   it reports: {app_sumocfg}")
        sumocfg = app_sumocfg
    if sumocfg is None:
        if sumo_dir is None:
            sumo_dir = cached_sumo_dir(target_map)
        # A local pick that is a precooked .tar.gz holds no sumo/ - it is the CARLA
        # half only - so it is not a bundle to open; fall through to the picker.
        local_bundle = None if (picked_local or '').lower().endswith('.tar.gz') \
            else picked_local
        if sumo_dir is None and (local_bundle or picked_tag):
            zip_path = local_bundle or import_map.download_release_zip(
                repo, picked_tag, cache_name=target_map,
                asset=(ent or {}).get("asset"))
            _carla, sumo_dir = import_map.open_bundle(zip_path, cache_name=target_map)
        sumocfg = import_map.bundle_sumocfg(sumo_dir)
        # Still nothing (first time, no cache): pick one now. choose_sumo_source
        # caches it under ~/.fixs/maps/<name>/sumo so the next run reuses it.
        if sumocfg is None:
            sumo_dir = import_map.choose_sumo_source(cache_name=target_map)
            sumocfg = import_map.bundle_sumocfg(sumo_dir)
    if sumocfg is None:
        sys.exit("[cosim] no SUMO scenario to run: pass --sumocfg, or choose a map "
                 "bundle / sumo source that provides one.")

    # TL table for signal sync + placement: --tl-table wins, else committed next to
    # the sumocfg, else generated from the SUMO net (cached ~/.fixs/tables).
    tl_table = args.tl_table
    if tls_manager == "sumo" and not tl_table:
        tl_table = resolve_tl_table(sumocfg, force=args.reimport, cache_name=target_map)

    # The scenario yaml, written like tl_table.csv: always, on first use (refreshed
    # by --reimport). Deliberately BEFORE the CARLA launch and the --prep-only
    # return - it is a config artifact, so it must not depend on a running CARLA.
    #
    # Every scenario yaml is APP-BOUNDED, under ~/.fixs/apps/<app>/, because yamls
    # are edited and ~/.fixs/maps/ is a deletable cache of downloaded artifacts.
    # Two kinds live there, and config_scope records which one won:
    #   'app'  the app's own yaml, staged from the repo - map-independent, AUTHORED,
    #          so the generator below must never touch it
    #   'map'  apps/<app>/maps/<map>/config.yaml - generated for this app on this map
    # The scope also tells the run profile whether changing the map invalidates the
    # yaml choice.
    config_yaml = args.config or app_catalog.scenario_path(
        setup.get("app") or app_catalog.GENERIC, target_map)
    config_scope = setup.get("config_scope") or "map"

    if config_scope == "map" and (args.reimport or not os.path.isfile(config_yaml)):
        # A regenerate must not silently undo hand edits: carry the existing
        # Backend choice over, and keep the old file as .bak to fall back on.
        prior = read_backend(config_yaml) if os.path.isfile(config_yaml) else None
        if prior:
            import shutil as _sh
            _sh.copy2(config_yaml, config_yaml + ".bak")
            print(f"[cosim] regenerating {os.path.basename(config_yaml)} "
                  f"(previous kept as .bak; Backend={prior} preserved)")
        generate_config_yaml(config_yaml, tl_table,
                             args.carla_host or DEFAULT_CARLA_HOST,
                             args.carla_port or DEFAULT_CARLA_PORT,
                             realtime=not args.fast,
                             carla_tick=args.carla_tick or FIXS_FEED_S,
                             backend=args.engine or prior or "cpp")

    # --carla-tick / --fast are scenario settings, so they are written THROUGH to the
    # yaml instead of living for one process: both bridges then read the same file,
    # and the file stops disagreeing with what just ran. Same reasoning as the CARLA
    # endpoint below.
    if os.path.isfile(config_yaml):
        if args.carla_tick is not None:
            if set_yaml_scalar(config_yaml, "CarlaSetup", "CarlaTimeStep",
                               f"{args.carla_tick:g}",
                               comment=f"CARLA world step; the {FIXS_FEED_S:g} s FIXS "
                                       f"feed is interpolated across it"):
                print(f"[cosim] {os.path.basename(config_yaml)}: CarlaTimeStep -> "
                      f"{args.carla_tick:g}")
        if args.fast and read_realtime_pacing(config_yaml):
            if set_yaml_scalar(config_yaml, "CarlaSetup", "RealtimePacing", "false"):
                print(f"[cosim] {os.path.basename(config_yaml)}: RealtimePacing -> "
                      f"false (--fast)")
        # The pre-#219 shape: TrafficRefreshRate carried the FEED period. It now means
        # the pose re-apply cadence, so a leftover value is not merely stale, it is
        # ACTIVE and wrong in a way that reproduces the very bug being fixed: a
        # generated yaml saying 0.1 against a 0.05 s tick re-applies poses at 10 Hz, so
        # each pose is held for two ticks and the interpolation it just enabled does
        # nothing. A generated yaml is ours, so clear it and let the default (every
        # tick) apply; an authored one is not, so only warn.
        stale = _yaml_float(config_yaml, "CarlaSetup", "TrafficRefreshRate", 0.0)
        tick_now = _yaml_float(config_yaml, "CarlaSetup", "CarlaTimeStep", 0.0) \
            or FIXS_FEED_S
        if stale > 1e-9:
            if config_scope == "map":
                if set_yaml_scalar(config_yaml, "CarlaSetup", "TrafficRefreshRate", "0.0",
                                   comment="pose re-apply cadence (0 = every CARLA "
                                           "tick); NOT the feed period"):
                    print(f"[cosim] {os.path.basename(config_yaml)}: cleared "
                          f"TrafficRefreshRate ({stale:g}), which used to carry the FIXS "
                          f"feed period. It now means the pose re-apply cadence, and the "
                          f"default is every CARLA tick ({tick_now:g} s). Set it again "
                          f"only to re-apply poses LESS often than the tick.")
            elif stale > tick_now + 1e-9:
                print(f"[cosim]   ->   {os.path.basename(config_yaml)} sets "
                      f"CarlaSetup.TrafficRefreshRate = {stale:g}, coarser than the "
                      f"{tick_now:g} s CARLA tick: traffic poses are re-applied every "
                      f"{int(round(stale / tick_now))} ticks. That is now what this key "
                      f"means (it used to be the feed period) - clear it for every tick.")
            elif stale < tick_now - 1e-9:
                print(f"[cosim]   WARN {os.path.basename(config_yaml)} sets "
                      f"CarlaSetup.TrafficRefreshRate = {stale:g}, FINER than the CARLA "
                      f"tick, which the engine rejects. That key is now the pose "
                      f"re-apply cadence, not the feed period - move the value to "
                      f"CarlaTimeStep by hand.")

    # Which bridge runs. Resolved once, here, rather than at the dispatch below, so
    # the saved profile records the bridge that ACTUALLY ran instead of the raw
    # (usually empty) --engine flag. An app may declare which stack its yaml is
    # written for; --engine still wins over everything.
    # An EXPLICIT EnablePythonBackend outranks the app's declaration, for the same
    # reason the summary row does: the declaration covers a yaml that OMITS the key,
    # and edit_engine's whole job is to put it there. Ranking the declaration first
    # made the engine menu unreachable for any config an app declares an engine for -
    # the choice was written into the file and then ignored on this very line.
    backend = (args.engine
               or (read_backend(config_yaml) if _declares_backend(config_yaml)
                   else None)
               or declared_engine(staged_configs, config_yaml)
               or read_backend(config_yaml))
    args.engine = backend

    # CARLA RPC endpoint: the scenario yaml is the source of truth, because that is
    # what VirCarlaEnv dials - it reads CarlaSetup.CarlaServerIP/Port itself
    # (CommonLib/ConfigHelper.cpp), it is not told by us. So the endpoint is READ
    # from the yaml, never remembered anywhere else.
    yaml_host, yaml_port = read_carla_endpoint(config_yaml)
    cli_host, cli_port = args.carla_host, args.carla_port
    args.carla_host = cli_host or yaml_host or DEFAULT_CARLA_HOST
    args.carla_port = cli_port or yaml_port or DEFAULT_CARLA_PORT

    # An explicit --carla-host/--carla-port still wins - but it is written THROUGH
    # to the yaml rather than held only in this process. Otherwise the flag moves
    # run_cosim's probe and leaves VirCarlaEnv dialling the old address: the co-sim
    # comes up reporting a healthy CARLA and then times out against a different one.
    # There is one endpoint per scenario, and this is where it is written down.
    wire = "127.0.0.1" if args.carla_host in ("localhost", "") else args.carla_host
    if os.path.isfile(config_yaml) and (
            (cli_host and wire != yaml_host) or (cli_port and args.carla_port != yaml_port)):
        print(f"[cosim] --carla-host/--carla-port differ from "
              f"{os.path.basename(config_yaml)} ({yaml_host}:{yaml_port}); updating it "
              f"so every component dials the same CARLA.")
        set_yaml_scalar(config_yaml, "CarlaSetup", "CarlaServerIP", wire)
        set_yaml_scalar(config_yaml, "CarlaSetup", "CarlaServerPort", args.carla_port)

    # Client mode means "no CARLA on this machine", so an endpoint pointing back
    # at this machine cannot be satisfied by anything. Caught here rather than at
    # connect time because the failure would otherwise surface as a bare RPC
    # timeout, which reads as "CARLA is down" instead of "nothing was ever there".
    if cfg is not None and cfg.get("mode") == "client" and _is_local_host(args.carla_host):
        sys.exit(
            f"[cosim] carla.json is 'client' mode (no CARLA on this machine), but "
            f"{os.path.basename(config_yaml)} points CarlaSetup.CarlaServerIP at "
            f"{args.carla_host}, which IS this machine.\n"
            f"        Set it to the host running CARLA (or pass --carla-host), else "
            f"re-run setup_carla and pick a local CARLA.")

    # Catches the case the early peek could not see: --config picked a different
    # yaml, or the file only existed after generation. Same rule as above - a CARLA
    # we cannot reach the filesystem of is one we can only connect to.
    if not _is_local_host(args.carla_host) and not args.no_launch:
        print(f"[cosim] CARLA host is {args.carla_host} (not this machine); "
              f"implying --no-launch.")
        args.no_launch = True

    # Everything is decided: save the setup under its name, so the next run opens on
    # it. Saved BEFORE the launch on purpose - a setup that failed to start is
    # exactly the one you want to come back to and tweak one setting of. The values
    # written are the RESOLVED ones (the real cooked map name, the bridge that will
    # actually run), not the raw flags, so replaying it reproduces this run.
    setup.update({
        "app": app["id"] if app else None,
        "map": target_map,
        "map_origin": map_origin,
        "map_local": picked_local,
        "config": config_yaml,
        "config_scope": config_scope,
        "sumo_gui": bool(args.sumo_gui),
    })
    # engine / carla_host / carla_port / the cadence are NOT stored: the scenario yaml
    # owns them, and a copy here would go stale the moment someone edited that yaml by
    # hand. step_length is dropped for a stronger reason - it was a SECOND owner of a
    # number the yaml also carried, which is how a 0.05 s setup came to drive SUMO at
    # half the CARLA clock without either file looking wrong.
    for stale in ("engine", "carla_host", "carla_port", "step_length"):
        setup.pop(stale, None)
    run_profile.save(setup_name, setup)
    args.map = target_map

    # TL + sign placement (source build only; idempotent via markers). Runs after
    # the import + sumocfg/TL-table resolution so the table exists to place from.
    if not args.no_launch and cfg is not None and cfg.get("mode") == "source":
        imported_now = (picked_tag is not None or picked_local is not None
                        or args.auto_import or args.reimport)
        if tl_table:
            import place_tls
            import props as props_mod
            # The bundle's placement.yaml is part of "are the lights already placed?":
            # the same map placed from DIFFERENT numbers is not placed. Without this a
            # corrected z_offset_cm never takes effect on a machine that placed once,
            # because the marker only ever recorded that placement happened.
            # Props are SHARED across maps - they install to one map-independent
            # content path - so they are fetched once into ~/.fixs/props rather
            # than shipped inside each bundle, where the last map imported would
            # silently decide which prop every other map places.
            #
            # The map's own dir comes FIRST: find_manifest takes the first hit, so a
            # bundle that ships its own placement.yaml overrides the shared numbers
            # (a map whose TL table uses the opposite heading convention needs its
            # own flip_yaw_180), and otherwise the library's defaults apply.
            props_cache = import_map.fetch_props(repo)
            bundle_dirs = [d for d in (import_map.map_cache_dir(target_map),
                                       props_cache) if d]
            try:
                tl_fingerprint = props_mod.bundle_fingerprint(
                    *bundle_dirs, shared=import_map.props_cache_dir())
            except props_mod.ManifestError as exc:
                sys.exit(f"[props] {exc}")

            # A map placed FROM a manifest, whose manifest is now gone, would
            # otherwise sail through as "already placed" with nothing printed - the
            # silent fallback this ticket exists to remove. Say so instead.
            prior = place_tls.marker_fields(cfg["carla_root"], target_map)
            if tl_fingerprint is None and prior.get("manifest"):
                print(f"[props] warning: '{target_map}' was placed from "
                      f"{prior['manifest']} (blueprint={prior.get('blueprint')}, "
                      f"z_offset_cm={prior.get('z_offset_cm')}), but its bundle ships "
                      f"no placement.yaml now. Keeping the lights already in the map; "
                      f"restore the manifest to change them.")

            placed = place_tls.tls_placed(cfg["carla_root"], target_map, tl_fingerprint)
            if (not placed) or args.reimport:
                # A manifest change re-places on its own: unlike a fresh import, it
                # needs no --reimport, since the numbers it carries are exactly what
                # the saved .umap baked in.
                if imported_now or not placed:
                    print(f"[cosim] placing traffic lights for '{target_map}' before launch ...")
                    _tell_peer(ctl_sock, "place_tls",
                               f"placing traffic lights for '{target_map}'")
                    place_tls.place_tls(target_map, tl_table, carla_root=cfg["carla_root"],
                                        ue4_root=cfg.get("ue4_root"), force=args.reimport,
                                        bundle_dirs=bundle_dirs)
                else:
                    print(f"[cosim] note: traffic lights not placed for '{target_map}'. Run "
                          f"place_tls (or --reimport) to add them, else no TL sync.")
        import place_signs
        if (not place_signs.signs_placed(cfg["carla_root"], target_map)) or args.reimport:
            if imported_now:
                print(f"[cosim] placing road signs for '{target_map}' before launch ...")
                _tell_peer(ctl_sock, "place_signs",
                           f"placing road signs for '{target_map}'")
                place_signs.place_signs(target_map, carla_root=cfg["carla_root"],
                                        ue4_root=cfg.get("ue4_root"), force=args.reimport)

    elif cfg is not None and cfg.get("mode") == "client":
        # Cooking a map and placing actors both write into a CARLA content tree and
        # save a .umap, so they can only happen where CARLA lives. In client mode
        # that is another machine, and this is the only place that says so - the
        # preflights above are simply skipped, which on its own looks like nothing
        # needed doing. The check after the world loads reports whether it actually
        # was done; this says who has to do it.
        print("[cosim] client mode: the map cook, prop install and traffic-light "
              "placement all happen on the machine running CARLA.")
        print(f"[cosim]   there:  python run_cosim.py --map {target_map} --prep-only")
        print("[cosim]   (that machine needs carla.json in 'source' mode; the Linux "
              "and Windows paths are the same command.)")

    if args.prep_only:
        print(f"[cosim] --prep-only: '{target_map}' imported + prepped (TLs/signs); not launching.")
        return 0

    # --sumo-only leaves before any of the CARLA preflight below: no server is
    # started, no client connects, no world is loaded. Returning here rather than
    # threading a flag through that block is deliberate - the block's job is to put
    # a specific map in front of a specific server, and a run with no server has no
    # part of it to keep. Everything the stack needs (the scenario yaml, the
    # sumocfg, the app and its controller) is already resolved above.
    if args.sumo_only:
        # No check on `backend` here on purpose. It selects WHICH CARLA bridge would
        # run (cpp = VirCarlaEnv, py = run_synchronization), and this mode runs
        # neither - so a yaml declaring either is equally fine, and refusing 'py'
        # would reject a SUMO-only scenario for naming a bridge nothing will start.
        print(f"[cosim] --sumo-only: SUMO + TrafficLayer"
              + (f" + {app['id']}" if app is not None else "")
              + ", no CARLA. config " + str(config_yaml))
        # TrafficLayer serves whoever the yaml says will subscribe, and waits for
        # all of them. A CARLA scenario yaml lists the bridge port among them, so
        # with no bridge running TrafficLayer waits for a client that never comes
        # and tears the run down at the end of warm-up - the app then dies on a
        # connection reset, which reads as an app bug rather than a missing
        # subscriber. Say it up front, at the point where it is still cheap to fix.
        try:
            _, _bridge_port = read_stack_ports(config_yaml)
            if os.path.isfile(config_yaml):
                _txt = open(config_yaml, encoding="utf-8", errors="ignore").read()
                if re.search(rf"^\s*port:\s*\[\s*{_bridge_port}\s*\]", _txt, re.MULTILINE):
                    print(f"[cosim] WARN {os.path.basename(config_yaml)} lists the bridge "
                          f"port {_bridge_port} as a subscriber, but --sumo-only starts no "
                          f"bridge.\n"
                          f"[cosim]      TrafficLayer will wait for a client that never "
                          f"connects and stop after the warm-up.\n"
                          f"[cosim]      Use a scenario yaml with no bridge subscriber "
                          f"(SUMO + the app only).")
        except Exception:
            pass
        return run_native_stack(config_yaml, sumocfg, tl_table, cfg, args, app,
                                ctl_sock=None, app_owns_scenario=app_owns_scenario,
                                app_proc=app_proc, sumo_only=True)

    carla_proc = None
    try:
        if not args.no_launch:
            # If the RPC port is already taken, a CARLA from a previous run is
            # probably still alive. Launching anyway gives a split brain (the new
            # server can't bind the port, so it sits on the default map while we
            # drive the OLD one). Auto-kill a *stale CARLA* holding the port; only
            # abort if a non-CARLA process owns it (don't kill something unrelated).
            if _port_in_use(args.carla_host, args.carla_port):
                pid = _pid_on_port(args.carla_port)
                name = _process_name(pid) if pid else ""
                if pid and _is_carla_process(name):
                    print(f"[cosim] a stale CARLA is holding port {args.carla_port} "
                          f"(PID {pid}, {name}); terminating it ...")
                    _kill_pid_tree(pid)
                    for _ in range(40):
                        if not _port_in_use(args.carla_host, args.carla_port):
                            break
                        time.sleep(0.5)
                    if _port_in_use(args.carla_host, args.carla_port):
                        sys.exit(f"[cosim] could not free port {args.carla_port}; "
                                 f"kill CARLA manually and retry.")
                    print(f"[cosim] port {args.carla_port} freed; launching a fresh CARLA.")
                else:
                    sys.exit(
                        f"[cosim] port {args.carla_port} is in use by a non-CARLA process "
                        f"({name or 'unknown'}); free it or pass --no-launch to use what's running.")
            _tell_peer(ctl_sock, "launch", "starting the CARLA server")
            carla_proc = launch_carla(cfg, args.carla_port, args.render_offscreen,
                                      args.quality_level, target_level)
            if not wait_for_port(args.carla_host, args.carla_port,
                                 proc=carla_proc, cfg=cfg):
                sys.exit("[cosim] CARLA never became reachable; not starting the "
                         "stack. See the lines above for why.")

        # A remote CARLA with a peer listening: ask it to serve this map rather
        # than requiring someone to have started it by hand with the right one.
        # Best effort - no peer answering just means the old manual workflow, so
        # this never makes a working setup stop working.
        if (not args.serve and not args.carla_only
                and not _is_local_host(args.carla_host)):
            ctl_sock = ask_peer_to_serve(args, target_map)

        import carla
        # Announce it BEFORE blocking. get_world() below waits the full timeout on
        # an unreachable server, with the last line on screen being whatever ran
        # before it - which reads as a freeze rather than as a connection attempt.
        print(f"[CARLA] connecting to {args.carla_host}:{args.carla_port} "
              f"(timeout {args.connect_timeout:g}s) ...")
        client = carla.Client(args.carla_host, args.carla_port)
        client.set_timeout(args.connect_timeout)
        try:
            client.get_server_version()
        except Exception as exc:
            sys.exit(f"[cosim] no CARLA at {args.carla_host}:{args.carla_port} ({exc}).\n"
                     f"        Start it on that host (run_cosim --carla-only there), "
                     f"check the address is not this machine, and open ports "
                     f"{args.carla_port}-{args.carla_port + 2} on it.")
        # Past the handshake: raise the ceiling for the map load, which genuinely
        # takes minutes when a freshly cooked map compiles its shaders.
        client.set_timeout(args.load_timeout)
        # Load by full /Game/... path when we have one: a bare name makes CARLA
        # pick the first .umap of that name it happens to find, which is the wrong
        # copy as soon as two packages ship the same map name.
        load_arg = target_level or target_map
        # Reloading a world the server is ALREADY running is pure cost: on a heavy
        # source-build map it takes minutes, and it is a real crash risk (the UE4
        # server in -game mode can die mid-switch, after which every later client
        # call reports "connection refused" and the run looks like a CARLA problem).
        # So: ask what is loaded first, and skip the load when it already matches.
        # A skipped load also skips load_world's implicit settings reset, so undo
        # synchronous mode explicitly - a bridge that was killed mid-tick leaves it
        # on, and confirm_world_ready would then wait for a tick nobody is driving.
        already = ""
        try:
            already = client.get_world().get_map().name
        except Exception:
            pass
        if already and target_map.lower() in already.lower():
            print(f"[CARLA] '{already}' is already loaded; skipping the reload "
                  f"(--reimport or a different --map forces one).")
            try:
                w = client.get_world()
                s = w.get_settings()
                if s.synchronous_mode:
                    s.synchronous_mode = False
                    s.fixed_delta_seconds = None
                    w.apply_settings(s)
                    print("[CARLA] cleared synchronous mode left behind by an earlier run.")
            except Exception as exc:
                print(f"[CARLA] could not reset world settings ({exc}); continuing.")
        else:
            print(f"[CARLA] loading world: {load_arg}")
            print("[CARLA] (the first load of a freshly imported map compiles shaders - "
                  "this can take a few minutes; later loads are fast)")
            client.load_world(load_arg)
        # Don't trust load_world's return alone on a heavy source map: confirm the
        # world IS this map and is ticking before we start SUMO.
        _tell_peer(ctl_sock, "load", f"loading '{target_map}' into CARLA "
                   f"(a freshly cooked map compiles shaders here)")
        print(f"[CARLA] confirming '{target_map}' is loaded and ready ...")
        loaded = confirm_world_ready(client, target_map, args.load_timeout)
        if loaded is None:
            current = ""
            try:
                current = client.get_world().get_map().name
            except Exception:
                pass
            sys.exit(f"[cosim] map '{target_map}' not confirmed loaded "
                     f"(current world: '{current}'); aborting before SUMO.")
        print(f"[CARLA] map ready: {loaded}")
        world = client.get_world()

        # First point at which this machine can observe whether the CARLA it is
        # talking to was ever prepped - which, in client mode, happened on a
        # different machine or not at all.
        report_traffic_lights(world, tls_manager, cfg, target_map)

        if not args.no_spectator:
            # Default: zoom to one intersection from the TL table so the signal
            # sync is legible. --spectator-all frames the whole network (placed TL
            # actors, else the full table). A map with no signals at all falls back
            # to the map centre - anywhere on the map beats the world origin, which
            # is where the server otherwise leaves the camera.
            frame = None
            if tl_table and not args.spectator_all:
                frame = _frame_from_table(tl_table, no_net_offset,
                                          junction=args.spectator_junction)
            if frame is None:
                frame = _frame_from_actors(world)
            if frame is None and tl_table:
                frame = _frame_from_table(tl_table, no_net_offset, whole=True)
            if frame is None:
                frame = _frame_from_map(world)
            if frame is not None:
                position_spectator(world, frame)
            else:
                print("[VIEW] no TL actors/table and no spawn points to anchor on; "
                      "leaving spectator as is")

        # The scenario yaml is a per-map artifact like tl_table.csv: always written
        # on first use (and refreshed by --reimport), whichever bridge runs. That
        # keeps it inspectable/editable BEFORE a cpp run - and makes its
        # CarlaSetup.Backend a real switch, since a missing file would otherwise
        # always read as 'py' and could never generate itself.
        # --carla-only: the CARLA half of a two-machine run. Everything above has
        # happened (cook check, stale-server kill, launch, load_world, readiness,
        # spectator) - we simply do NOT dispatch a bridge, and hold the server open
        # for the machine that will. Stopping here rather than in a separate
        # launcher is what makes the level path, the readiness check and the
        # teardown identical to a local run instead of a second implementation.
        # The `finally` below kills CARLA when this returns.
        if args.carla_only or args.serve:
            return hold_carla(carla_proc, target_map, args, sock=ctl_sock)

        # The standalone bridge is deliberately NOT reachable from a scenario key:
        # it speaks no FIXS, so a yaml that configures TrafficLayer subscriptions,
        # an XIL component and a controller would have every one of them silently
        # ignored. Asking for it has to be an explicit act.
        if args.standalone_bridge:
            print("[cosim] standalone bridge (run_synchronization.py) -- no FIXS, no "
                  "TrafficLayer, no controller.")
            return run_python_bridge(config_yaml, sumocfg, tl_table, tls_manager,
                                     no_net_offset, args, app,
                                     app_owns_scenario=app_owns_scenario,
                                     app_proc=app_proc)

        # Engine dispatch: CarlaSetup.EnablePythonBackend picks WHICH VirEnvCore
        # drives the same FIXS-native stack (--engine overrides). See read_backend.
        print(f"[cosim] engine={backend} "
              f"({'Python' if backend == 'py' else 'C++'} VirEnvCore); "
              f"config {config_yaml}")
        return run_native_stack(config_yaml, sumocfg, tl_table, cfg, args, app,
                                ctl_sock=ctl_sock,
                                app_owns_scenario=app_owns_scenario,
                                app_proc=app_proc, bridge=backend)
    finally:
        # Say goodbye BEFORE tearing down locally: the peer is blocked on this
        # socket, and a clean BYE lets it report "the peer finished" rather than a
        # bare disconnection. This runs on every exit path, Ctrl+C included, so a
        # CARLA host never keeps a server alive for a run that has ended.
        if ctl_sock is not None and not args.serve:
            import peer
            peer.bye(ctl_sock, "traffic side finished")
        if carla_proc is not None:
            print("[CARLA] terminating server")
            kill_carla(carla_proc)


def serve_forever():
    """--serve is a service, not a single run.

    await_peer() stops listening once a peer connects, because one CARLA cannot
    serve two co-sims at a time. But the process used to EXIT when that run
    ended, so the second run from the traffic machine found nothing listening,
    spent --peer-wait retrying, and then silently fell back to 'assume CARLA was
    started by hand' - which appeared to work, because the previous run had left
    CARLA up. That is the worst kind of working: right answer, wrong reason, and
    the peer protocol quietly not in use.

    So loop: serve one run, tear its CARLA down, listen again.

    A FAILED run has to loop too, and used not to. `rc = main(); if rc != 0` was
    nearly dead code: almost every failure in main() is sys.exit("message"), which
    raises SystemExit straight past that check and out of the process. So any
    error at all - a map that is not installed, a busy RPC port, a failed cook -
    took the whole service down, and the next run from the traffic machine found
    nothing listening and fell into exactly the "assume CARLA was started by hand"
    path described above. The clean-exit hole was closed; this is the same hole on
    the error path.

    Ctrl+C is the one thing that still stops serving: it is the operator at THIS
    machine saying so, which is the only voice this loop takes."""
    n = 0
    consecutive = 0
    while True:
        n += 1
        try:
            rc = main()
        except KeyboardInterrupt:
            _fail_peer("stopped from the render host (Ctrl+C)", retryable=True)
            print("\n[serve] stopped.")
            return 0
        except SystemExit as e:
            # sys.exit("message") carries the reason in .code as a string, and that
            # message is the most useful thing this side has. sys.exit(int) means a
            # status with no words; sys.exit() / sys.exit(None) is a clean stop.
            code = e.code
            if code is None or code == 0:
                rc = 0
            else:
                rc = code if isinstance(code, int) else 1
                _fail_peer(code if isinstance(code, str) else
                           f"the render host exited with status {code}",
                           retryable=False)
                print(f"\n[serve] run {n} FAILED - reported to the peer.")
        except Exception as e:                 # noqa: BLE001 - a service must not die
            # An unhandled exception is a bug, not a decision. Print the traceback
            # so it is diagnosable, tell the peer, and go back to listening rather
            # than leaving the traffic machine to time out against a dead port.
            traceback.print_exc()
            _fail_peer(f"unexpected error on the render host: {e!r}", retryable=True)
            rc = 1
            print(f"\n[serve] run {n} FAILED ({e!r}) - reported to the peer.")
        else:
            if rc != 0:
                _fail_peer(f"the render host exited with status {rc}", retryable=False)

        # Whatever happened, this run's control socket is finished with. hold_carla
        # returns without closing it on the two paths where the peer spoke first
        # (it disconnected, or it said BYE), so without this the next run would
        # start holding a dead socket in the slot _fail_peer reports through.
        _drop_serve_sock()

        consecutive = consecutive + 1 if rc != 0 else 0
        if consecutive >= SERVE_MAX_CONSECUTIVE_FAILURES:
            # Something here is broken in a way retrying cannot fix, and a service
            # silently failing every run looks identical to one nobody is using.
            print(f"[serve] {consecutive} runs failed in a row; stopping so the "
                  f"problem is not hidden by an endlessly listening port.")
            return rc
        print(f"\n[serve] run {n} {'finished' if rc == 0 else 'failed'}; listening "
              f"again (Ctrl+C to stop serving).\n")


if __name__ == "__main__":
    # Parsed here only to decide whether this is a one-shot run or a service;
    # main() does the real argument handling.
    if "--serve" in sys.argv and "--peer" in sys.argv:
        # Caught before the service loop, not inside main(): serve_forever treats
        # every SystemExit as one failed run and goes back to listening, so a
        # contradiction in the FLAGS would be re-discovered on every iteration
        # until the failure cap stopped it. The two halves of a distributed run are
        # two machines - --serve where CARLA is, --peer where the traffic is.
        sys.exit("[cosim] --serve is the CARLA half of a distributed run and "
                 "--peer is the traffic half; they run on different machines.")
    # Wrapped so a run that dies AFTER the setup was checkpointed says where the
    # answers went. Both exits are covered: sys.exit with a message (the usual way
    # a cook failure is reported) and a non-zero return.
    try:
        _rc = serve_forever() if "--serve" in sys.argv else main()
    except SystemExit as exc:
        if exc.code not in (0, None):
            _say_checkpoint_kept()
        raise
    except KeyboardInterrupt:
        _say_checkpoint_kept()
        raise
    if _rc:
        _say_checkpoint_kept()
    sys.exit(_rc)
