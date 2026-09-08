"""
app_catalog.py - the `apps/` manifest contract for FIXS application repos.

FIXS is the enabler, not the owner: this module defines and parses the canonical
application manifest, but hardcodes no application name. An app repo that sits
above a fetched FIXS bundle (<repo>/FIXS/Carla/... , hence <repo>/apps/) declares
everything run_cosim needs to know about its applications in ONE file:

    <repo>/apps/apps.json

If that file is absent, every entry point here degrades to "no apps", and
run_cosim behaves exactly as it did before app awareness existed. Same shape as
the Digital-Twin-Library `catalog.json` that import_map already consumes: FIXS
owns the schema + tooling, the repo owns the data.


Schema (schema: 1)
------------------
{
  "schema": 1,
  "apps": [
    {
      "id":     "roosevelt",            # required, unique. Also the default folder
                                        #   name under apps/ and the profile key.
      "title":  "Roosevelt Ave co-sim", # optional display name (default: id)
      "dir":    "roosevelt",            # optional folder under apps/ (default: id)
      "note":   "...",                  # optional, printed when the app is picked
      "maps":   ["roosevelt", ...],     # optional, DEFAULT ["<id>"]; first = default pick
      "configs":[ <config>, ... ],      # optional app-owned scenario yamls
      "needs_map_sumo": true,           # optional: this app BUILDS its scenario from
                                        #   the chosen map's, so run_cosim must open
                                        #   the bundle before starting it. Default
                                        #   false - see below.
      "launch": "run_my_app",           # optional: a command run alongside the stack
                                        #   (the app's controller / XIL host), which
                                        #   may also report the scenario to run. See
                                        #   below - run_cosim does not read its args.
      "defaults": {                     # optional per-app run defaults (CLI wins)
        "engine": "py"|"cpp", "sumo_gui": true
      },                                # no timestep: the scenario yaml owns the
                                        # cadence, and SUMO's step is the FIXS feed
      "sumo_args": {                    # optional: this app's deviations from the
        "--lateral-resolution": "0",    #   co-sim SUMO convention (run_cosim's
        "--time-to-teleport": "-1",     #   SUMO_CONVENTION). null drops a flag.
        "--step-length": null           #   Contract flags cannot be overridden.
      }
    }
  ]
}

Why `sumo_args` and not an edit to the map's .sumocfg: a Digital-Twin-Library map
ships ONE app-independent scenario, and a co-sim requirement does not belong in a
shared artifact - otherwise every new map needs the same edit before it works. So
run_cosim injects the convention on the SUMO command line (printed, with each flag's
origin) and an app deviates HERE, tracked and reviewed next to its declaration,
valid on every map that app runs against.

`launch` is how an application gets to run under the one entry point. run_cosim starts
SUMO, TrafficLayer and the bridge; an app that also has a controller names it here, and
the user keeps typing `run_cosim` and nothing else. Deliberately opaque: run_cosim
resolves the command in the app folder (extensionless -> .bat on Windows, .sh
elsewhere, the convention run_cosim / import_map / place_tls already use) and passes
every argument after the first token through UNTOUCHED. It never adds, removes or reads
one, so what a controller needs to be told is the app's business and adding an app
costs no engine change.

It is started FIRST, before SUMO, and it may report the scenario to run. run_cosim
gives it a path in FIXS_HANDOFF and waits for a json object to appear there:

    {"sumocfg": "<abs path>"}       run this scenario instead of the bundle's
    {}                             nothing to report; carry on

That inverts who owns the SUMO scenario, which is the point. An application whose
scenario is GENERATED per run - a run directory, its own demand, output paths written
into the config - cannot declare a static path for it, and a config file is the only
place some SUMO outputs can be redirected at all (<timedEvent dest=> has no command
line flag). So the app builds the scenario and says where it put it.

Reporting one also switches the SUMO CONVENTION off. The convention exists because a
Digital-Twin-Library .sumocfg is a SHARED artifact that must not carry one consumer's
co-sim requirement; an app that generated its own has no shared artifact and no such
problem, and imposing sublane lane-changing on it would be changing a scenario behind
its author's back. The CONTRACT (--step-length: one SUMO step per FIXS exchange) still
applies - that is the protocol, not a preference - and `sumo_args` still adds whatever
else an app wants, so what SUMO got is still one printed list either way.

FIXS_HANDOFF is also how the command knows the stack is not its to start: SUMO and
TrafficLayer are already being launched, and a second copy would fight for the TraCI
and bridge ports. An app that ignores the variable entirely still works - it reports
nothing, keeps the convention, and runs the bundle's scenario - so `launch` is usable
without knowing any of this.

A map is just a NAME - the word the picker matches against what already exists:
a Digital-Twin-Library location / cooked map name / release tag (catalog_entry
matches all three), or the name of a map already cooked into this CARLA. Nothing
is downloaded on an app's say-so and nothing is hardcoded per app; the name is a
hint that hoists that map to the top of the picker and makes it the Enter-default.

"maps" defaults to the app's own id, which is the whole point of naming an app
after its location: `roosevelt` and `atlanta` match the library entries of the
same name and declare no maps at all. Declare "maps" only when the map is named
differently from the app (a study app on a shared map, or several maps per app).

A name that matches nothing is simply not offered in the app section - the picker
falls through to the library list, the cooked list, and the local-file option,
i.e. exactly the menu you get with no app selected. So a wrong or not-yet-published
name costs a missing shortcut, never a failed run.

<config> is either a plain string (a path under the app folder) or an object:

    "MLK_Sumo_Scenario/config_Sumo_Carla_default.yaml"
    {
      "path":   "MLK_Sumo_Scenario/config_Sumo_Carla_dSPACE.yaml",   # required
      "title":  "dSPACE XIL",                                        # optional label
      "engine": "cpp"                   # optional: which bridge this yaml is written
                                        #   for. Declared because a hand-written yaml
                                        #   that omits CarlaSetup.EnablePythonBackend
                                        #   otherwise reads as the python bridge by
                                        #   default - silently running the wrong stack.
    }


Why app yamls are staged into ~/.fixs
-------------------------------------
A committed yaml carries machine-specific values (CARLA server IP, dSPACE ports).
Editing it in place makes every user's working tree dirty and invites those
values into the repo. So a declared config is COPIED to

    ~/.fixs/apps/<app_id>/<basename>.yaml

on first use and read from there. Edits are yours and never tracked. The copy is
never silently clobbered: stage_configs() records the source hash, refreshes the
copy only while you have not touched it, and otherwise drops the new upstream
version beside it as <basename>.yaml.new and says so.
"""
import hashlib
import json
import os
import sys

import carla_env_setup as env

SCHEMA = 1
HERE = os.path.dirname(os.path.abspath(__file__))

# Identity for a run with no application selected. It is a real key, not a null:
# a generic run still has scenario yamls and a saved profile, and they need a home
# that cannot collide with an app id (hence the leading underscore).
GENERIC = "_generic"


# --------------------------------------------------------------------------- #
# Locations
# --------------------------------------------------------------------------- #
def app_root():
    """The application repo root: two levels up from this file
    (<repo>/FIXS/Carla/app_catalog.py -> <repo>). Mirrors import_map._app_root()."""
    return os.path.dirname(os.path.dirname(HERE))


def catalog_path(root=None):
    """Path to the app manifest: $FIXS_APPS_JSON, else <repo>/apps/apps.json."""
    override = os.environ.get("FIXS_APPS_JSON")
    if override:
        return override
    return os.path.join(root or app_root(), "apps", "apps.json")


def apps_home(app_id=None):
    """Machine-local app state: ~/.fixs/apps[/<app_id>]. Holds the staged copies of
    an app's scenario yamls. Kept beside carla.json (outside any repo) so edits are
    per-machine and survive `initialize`, which wipes FIXS/."""
    d = os.path.join(os.path.dirname(env.CONFIG_PATH), "apps")
    if app_id:
        d = os.path.join(d, app_id)
    return d


def app_dir(app, root=None):
    """Absolute path of the app's folder under apps/ (entry['dir'], else its id)."""
    return os.path.join(root or app_root(), "apps", app.get("dir") or app["id"])


def launch_command(app, root=None):
    """(argv, cwd) for the app's `launch` command, or (None, None) if it declares none.

    The first token is resolved in the app folder and given the platform's script
    extension when it has none - `run_mlk_eco_driving` -> run_mlk_eco_driving.bat on
    Windows, .sh elsewhere - which is the convention run_cosim / import_map /
    place_tls already ship both halves of. Everything after the first token is passed
    through verbatim and never interpreted: the app owns its own arguments."""
    if not app or not app.get("launch"):
        return None, None
    import shlex
    parts = shlex.split(app["launch"], posix=(os.name != "nt"))
    if not parts:
        return None, None
    here = app_dir(app, root)
    exe = parts[0]
    if not os.path.splitext(exe)[1]:
        exe += ".bat" if os.name == "nt" else ".sh"
    path = exe if os.path.isabs(exe) else os.path.join(here, exe)
    if not os.path.isfile(path):
        _warn(f"app '{app['id']}': launch command '{app['launch']}' not found "
              f"at {path}; nothing will be started for it.")
        return None, None
    return [path] + parts[1:], here


def scenario_dir(app_id, map_name=None):
    """Where an app's scenario yamls live: ~/.fixs/apps/<app_id>/, flat.

    One folder per application, mirroring apps/<app_id>/ in the repo, with the
    yamls sitting directly in it - the staged copies of the app's own configs and
    the generated per-map ones side by side. `map_name` is accepted and ignored so
    callers can pass it; the map is in the FILE name, not in a subfolder.

    Scenario yamls are app-bounded, never map-bounded, because they are edited and
    ~/.fixs/maps/ is a cache of downloaded artifacts a user should be able to
    delete wholesale to reclaim gigabytes. GENERIC is the key for a run with no
    application selected."""
    return apps_home(app_id or GENERIC)


def scenario_path(app_id, map_name):
    """The generated scenario yaml: ~/.fixs/apps/<app_id>/<map_name>.yaml.

    Named for the map rather than nested under one, so the app folder stays flat
    and two maps under the same app still get their own file - the CARLA endpoint
    and TL subscriptions inside are specific to (app, map)."""
    return os.path.join(scenario_dir(app_id), f"{map_name}.yaml")


def migrate_scenarios(app_id, map_name, legacy_dir, quiet=False):
    """Move scenario yamls from where older FIXS versions put them, once.

    Two earlier homes, both left behind rather than abandoned - these files are
    hand-edited, so a user who tuned CarlaServerIP in one must not silently get a
    freshly generated default instead:

        ~/.fixs/maps/<map>/*.yaml                 (before yamls were app-bounded)
        ~/.fixs/apps/<app>/maps/<map>/*.yaml      (before the app folder went flat)

    The map's own config.yaml becomes <map>.yaml; anything beside it keeps its
    name, prefixed with the map if that would collide. Never overwrites: a name
    already taken in the destination is left where it is."""
    import shutil
    dest = scenario_dir(app_id)
    moved = []
    sources = [legacy_dir, os.path.join(dest, "maps", map_name)]
    for src in sources:
        try:
            names = sorted(f for f in os.listdir(src)
                           if f.lower().endswith((".yaml", ".yml")))
        except OSError:
            continue
        for name in names:
            stem, ext = os.path.splitext(name)
            # Everything here belonged to ONE map, so everything gets that map's
            # name: config.yaml is the generated one and becomes <map>.yaml, a
            # variant beside it becomes <map>_<variant>.yaml. Without the prefix a
            # variant would sort ahead of the generated yaml in the flat folder and
            # be offered as the default - and would read as if it applied to every
            # map the app runs, which it does not.
            target = f"{map_name}{ext}" if stem == "config" else f"{map_name}_{name}"
            if os.path.exists(os.path.join(dest, target)):
                continue                   # already migrated; leave the original
            try:
                os.makedirs(dest, exist_ok=True)
                shutil.move(os.path.join(src, name), os.path.join(dest, target))
                moved.append(f"{name} -> {target}")
            except OSError as exc:
                _warn(f"could not move {name} ({exc}); leaving it in {src}.")
        # Tidy the emptied nested folder so the old shape does not linger.
        try:
            os.rmdir(src)
            os.rmdir(os.path.dirname(src))
        except OSError:
            pass
    if moved and not quiet:
        print(f"[apps] scenario configs live flat under the app now; moved "
              f"{', '.join(moved)}\n[apps]   -> {dest}")
    return moved


# --------------------------------------------------------------------------- #
# Parsing
# --------------------------------------------------------------------------- #
def _warn(msg):
    print(f"[apps] {msg}")


def _normalize_map(raw, app_id):
    """A declared map -> its name, or None if unusable. An object with a 'name' key
    is accepted too, so a manifest that grows richer map entries later still loads
    on today's FIXS."""
    if isinstance(raw, dict):
        raw = raw.get("name")
    if not isinstance(raw, str) or not raw.strip():
        _warn(f"app '{app_id}': ignoring a map entry that is not a name.")
        return None
    return raw.strip()


def _normalize_config(raw, app_id):
    """A <config> entry -> {path (relative), title, engine}, or None if unusable."""
    if isinstance(raw, str):
        raw = {"path": raw}
    if not isinstance(raw, dict):
        _warn(f"app '{app_id}': ignoring a config entry that is neither a string nor an object.")
        return None
    path = (raw.get("path") or "").strip()
    if not path:
        _warn(f"app '{app_id}': ignoring a config entry with no 'path'.")
        return None
    engine = (raw.get("engine") or "").strip().lower() or None
    if engine not in (None, "py", "cpp"):
        _warn(f"app '{app_id}': config '{path}' declares engine '{engine}'; "
              f"expected 'py' or 'cpp'. Ignoring the declaration.")
        engine = None
    return {"path": path.replace("\\", "/"),
            "title": (raw.get("title") or "").strip(),
            "engine": engine}


def _normalize_app(raw):
    """One manifest entry -> a normalized app dict, or None if unusable."""
    if not isinstance(raw, dict):
        _warn("ignoring a manifest entry that is not an object.")
        return None
    app_id = (raw.get("id") or "").strip()
    if not app_id:
        _warn("ignoring an app entry with no 'id'.")
        return None
    defaults = raw.get("defaults")
    if not isinstance(defaults, dict):
        if defaults is not None:
            _warn(f"app '{app_id}': 'defaults' is not an object; ignoring it.")
        defaults = {}
    # This app's deviations from the co-sim SUMO convention. Kept as raw text keyed by
    # the SUMO flag, so a new SUMO option needs no support here; run_cosim decides
    # precedence and prints where each flag came from. A null value drops a convention
    # flag. Keys must look like flags, or a typo would be passed to SUMO verbatim.
    sumo_args = raw.get("sumo_args")
    if not isinstance(sumo_args, dict):
        if sumo_args is not None:
            _warn(f"app '{app_id}': 'sumo_args' is not an object; ignoring it.")
        sumo_args = {}
    clean_args = {}
    for flag, value in sumo_args.items():
        if isinstance(flag, str) and flag.startswith("--"):
            clean_args[flag] = value
        else:
            _warn(f"app '{app_id}': sumo_args key '{flag}' is not a --flag; ignoring it.")
    sumo_args = clean_args
    # No "maps" -> the app id IS the map name to look for. Apps named after their
    # location (roosevelt, atlanta) therefore need no map declaration at all.
    maps = [m for m in (_normalize_map(m, app_id) for m in raw.get("maps") or []) if m]
    if not maps:
        maps = [app_id]
    configs = [c for c in (_normalize_config(c, app_id) for c in raw.get("configs") or []) if c]
    # Extra python packages this app needs on top of the engine's own env, as a path
    # relative to the app folder. The engine owns environment.yml and nothing else;
    # an app's plotting or analysis stack is the app's business, and pushing it
    # upstream would put every FIXS consumer's env at the mercy of one application.
    # Absent -> the app declares none, and nothing is installed for it.
    requirements = (raw.get("requirements") or "").strip() or None
    # A command run alongside the co-sim stack - the app's controller, XIL host, or
    # whatever else attaches to TrafficLayer, and the thing that may report which
    # scenario to run (see FIXS_HANDOFF above). Kept as one opaque string: run_cosim
    # resolves it in the app folder and never reads its arguments, so what an app
    # needs to pass itself costs no change here.
    launch = (raw.get("launch") or "").strip() or None
    # Whether the app needs the chosen map's sumo/ on disk BEFORE it starts.
    # run_cosim otherwise starts an app first and reaches for the bundle after,
    # because an app that generates its own scenario from scratch needs no sumo/
    # half and asking for one would prompt over a ~380MB archive it discards.
    # An app that generates its scenario FROM the map's wants the opposite, and
    # only the app knows which it is.
    needs_map_sumo = bool(raw.get("needs_map_sumo"))
    return {"id": app_id,
            "title": (raw.get("title") or "").strip() or app_id,
            "dir": (raw.get("dir") or "").strip() or app_id,
            "note": (raw.get("note") or "").strip() or None,
            "maps": maps,
            "configs": configs,
            "defaults": defaults,
            "sumo_args": sumo_args,
            "requirements": requirements,
            "needs_map_sumo": needs_map_sumo,
            "launch": launch}


def load_catalog(root=None):
    """The declared applications, newest schema rules applied: a list of normalized
    app dicts (possibly empty). Never raises - a missing, unreadable or malformed
    manifest degrades to [] with a warning, because app awareness is an enhancement
    to run_cosim, not a precondition for it."""
    path = catalog_path(root)
    if not os.path.isfile(path):
        return []
    try:
        with open(path, encoding="utf-8") as f:
            doc = json.load(f)
    except (OSError, ValueError) as exc:
        _warn(f"could not read {path} ({exc}); continuing without apps.")
        return []
    if not isinstance(doc, dict):
        _warn(f"{path}: top level must be an object; continuing without apps.")
        return []
    declared = doc.get("schema")
    if declared is not None and declared != SCHEMA:
        _warn(f"{path}: schema {declared}, this FIXS understands {SCHEMA}. "
              f"Reading it anyway; update FIXS if entries look wrong.")
    apps = [a for a in (_normalize_app(a) for a in doc.get("apps") or []) if a]
    seen = {}
    for a in apps:
        if a["id"] in seen:
            _warn(f"duplicate app id '{a['id']}' in {path}; the first one wins.")
        seen.setdefault(a["id"], a)
    return list(seen.values())


def find_app(apps, ident):
    """The app known by `ident` - its id, its folder, or its title (case-insensitive)
    - or None. Accepts a folder path too, so `--app apps/roosevelt` works."""
    if not ident:
        return None
    key = os.path.basename(str(ident).replace("\\", "/").rstrip("/")).strip().lower()
    for a in apps or []:
        if key in (a["id"].lower(), a["dir"].lower(), a["title"].lower()):
            return a
    return None


# --------------------------------------------------------------------------- #
# Picking
# --------------------------------------------------------------------------- #
def choose_app(apps, root=None, current=None):
    """Numbered menu of the declared applications; returns the chosen app dict, or
    None for "no app" (the generic, pre-app-awareness run). Auto-selects nothing:
    even a single app is offered, because the None escape has to stay reachable.
    Returns None (silently) in a non-interactive session so callers keep working
    from --app / a saved profile.

    `current` is the app id the setup is running: it is marked, and Enter keeps it.
    Without it Enter always meant item 1, so opening this row on a setup running any
    other app and pressing Enter switched the app - and took the map and the
    scenario yaml with it, since both are invalidated by an app change."""
    if not apps:
        return None
    if not sys.stdin.isatty():
        return None
    ids = [a["id"] for a in apps]
    idx = ids.index(current) + 1 if current in ids else 1
    print("\n[apps] Pick an application to run:")
    for i, a in enumerate(apps, 1):
        missing = "" if os.path.isdir(app_dir(a, root)) else "   (folder missing)"
        maps = ", ".join(a["maps"]) or "-"
        mark = "  (current)" if a["id"] == current else ""
        print(f"   {i:>2}) {a['title']:<34} maps: {maps}{missing}{mark}")
    print("    0) none - just pick a map (generic co-sim)")
    while True:
        try:
            ans = input(f"[apps] Which? [0-{len(apps)}], Enter = {idx}: ").strip()
        except EOFError:
            return None
        if ans == "":
            return apps[idx - 1]
        if ans == "0":
            return None
        if ans.isdigit() and 1 <= int(ans) <= len(apps):
            return apps[int(ans) - 1]
        print("[apps] invalid choice; enter a number from the list.")


# --------------------------------------------------------------------------- #
# Scenario yamls: repo -> ~/.fixs/apps/<id>/
# --------------------------------------------------------------------------- #
def _sha256(path):
    h = hashlib.sha256()
    try:
        with open(path, "rb") as f:
            for chunk in iter(lambda: f.read(65536), b""):
                h.update(chunk)
    except OSError:
        return None
    return h.hexdigest()


def _stage_index_path(app_id):
    return os.path.join(apps_home(app_id), ".sources.json")


def _load_stage_index(app_id):
    try:
        with open(_stage_index_path(app_id), encoding="utf-8") as f:
            doc = json.load(f)
        return doc if isinstance(doc, dict) else {}
    except (OSError, ValueError):
        return {}


def _save_stage_index(app_id, index):
    try:
        os.makedirs(apps_home(app_id), exist_ok=True)
        with open(_stage_index_path(app_id), "w", encoding="utf-8") as f:
            json.dump(index, f, indent=2)
    except OSError:
        pass  # the index is an optimisation; a lost one only costs a refresh notice


def stage_configs(app, root=None, quiet=False):
    """Copy the app's declared scenario yamls into ~/.fixs/apps/<id>/ and return
    [{path, title, engine, source}] for the ones that exist (path = the STAGED copy,
    which is what run_cosim reads and the user edits).

    Update policy, so a machine-specific edit is never lost and an upstream fix is
    never silently withheld:
      - copy missing         -> copy it
      - upstream unchanged   -> leave it alone
      - upstream changed, copy untouched -> refresh in place (you had no edits)
      - upstream changed, copy edited    -> keep yours, write <name>.yaml.new, say so
    """
    import shutil
    staged = []
    if not app or not app.get("configs"):
        return staged
    dest_dir = apps_home(app["id"])
    index = _load_stage_index(app["id"])
    changed = False
    for cfg in app["configs"]:
        src = os.path.join(app_dir(app, root), *cfg["path"].split("/"))
        if not os.path.isfile(src):
            _warn(f"app '{app['id']}': declared config not found, skipping: {src}")
            continue
        base = os.path.basename(src)
        dst = os.path.join(dest_dir, base)
        src_hash = _sha256(src)
        recorded = (index.get(base) or {}).get("hash")
        try:
            os.makedirs(dest_dir, exist_ok=True)
            if not os.path.isfile(dst):
                shutil.copy2(src, dst)
                if not quiet:
                    print(f"[apps] staged {base} -> {dst}\n"
                          f"[apps]   edit that copy for machine-specific values "
                          f"(IPs, ports); it is never committed.")
                index[base] = {"source": cfg["path"], "hash": src_hash}
                changed = True
            elif src_hash and src_hash != recorded:
                if recorded and _sha256(dst) == recorded:
                    shutil.copy2(src, dst)          # untouched copy: safe to refresh
                    if not quiet:
                        print(f"[apps] {base} updated upstream and your copy was "
                              f"unedited; refreshed {dst}")
                else:
                    new = dst + ".new"
                    shutil.copy2(src, new)
                    if not quiet:
                        print(f"[apps] {base} changed upstream but your copy has local "
                              f"edits.\n[apps]   keeping yours; new version written to "
                              f"{os.path.basename(new)} - merge if you want it.")
                index[base] = {"source": cfg["path"], "hash": src_hash}
                changed = True
        except OSError as exc:
            _warn(f"app '{app['id']}': could not stage {base} ({exc}); using the repo copy.")
            dst = src
        staged.append({"path": dst, "title": cfg["title"] or base,
                       "engine": cfg["engine"], "source": src})
    if changed:
        _save_stage_index(app["id"], index)
    return staged
