"""
run_profile.py - named run setups (~/.fixs/run_profiles.json) and the review loop.

A co-sim run is defined by six choices: which application, which map, which
scenario yaml, which bridge, which CARLA, and how SUMO runs. Answering all six on
every run is the friction this removes. They are saved as a NAMED SETUP, and every
later run starts from the list of setups rather than from a blank prompt:

    [cosim] Run setups:
       1) roosevelt_default   roosevelt | roosevelt_full | config.yaml | py | gui
       2) roosevelt_fast      roosevelt | roosevelt_full | config_fast.yaml | cpp | gui
       3) mlk_dspace          mlk_eco_driving | mlk_full | ..._dSPACE.yaml | cpp | gui
       N) new setup
    [cosim] Which? [1-3 / N], Enter = 1 (last run):

Pick one and you get its settings, which you edit until they are right and then
confirm:

    [cosim] Run setup 'roosevelt_default'  (last run 2026-07-29 13:21)
       1) app       roosevelt
       2) map       roosevelt_full            (Digital-Twin-Library)
       3) scenario  config.yaml               (generated, this app on this map)
       4) engine    py                        (Python VirEnvCore: mainVirCarla.py)
       5) CARLA     source  C:/src_ext/Carla  ->  127.0.0.1:2000
       6) SUMO      gui, step 0.05

    [cosim] Enter = run it | 1-6 = change | S = switch setup | N = new | Q = quit:

Editing loops: after each change the summary is redrawn, so you see the result and
can keep going. Nothing heavy happens until you confirm - all six are pure
decisions, so no map is downloaded, cooked or loaded while you are still deciding.

A row opens its editor when you SELECT it. A row that is merely blank, or that
another change invalidated, settles itself where there is an obvious answer and
says what it took - the summary is then where you disagree with it. That is why
ask() returns exactly what was picked and the caller applies cascade(): the two
cases are answered differently, and only the caller can tell them apart. Today
only the scenario yaml has such an answer (the application declares it); app and
map have none worth guessing, so they always ask.

Running a setup saves it back under the same name; `N` is how you keep the old one
and start another. The store is one file so the whole list can be read, shown and
switched in one place - and a future front-end can drive exactly the same file:

    {"schema": 2, "last": "roosevelt_default", "setups": {"<name>": {...}}}

Schema 2 dropped `step_length`. A setup is a choice of ARTIFACTS (app, map, scenario
yaml) plus how you want to watch it (sumo_gui); every timestep now lives in the
scenario yaml, or is a protocol constant. Keeping a copy here made the store a second
owner of a number the yaml also carried, and the two disagreed silently. A schema-1
file still loads - the stale key is simply dropped the next time the setup is saved.

Explicit CLI flags outrank a saved setup: the caller applies a slot only when the
corresponding flag was not given, so `run_cosim --map atlanta` changes exactly that
one thing and prompts for nothing.
"""
import json
import os
from datetime import datetime

import app_catalog
import carla_env_setup as env

SCHEMA = 2
# Identity used in a setup's `app` field when no application is selected.
GENERIC = app_catalog.GENERIC

# slot key -> (menu label, slots invalidated when it changes)
SLOTS = [
    ("app", "app"),
    ("map", "map"),
    ("config", "scenario"),
    ("engine", "engine"),
    ("carla", "CARLA"),
    ("sumo", "SUMO"),
]
SLOT_KEYS = [k for k, _ in SLOTS]


def profiles_path():
    """~/.fixs/run_profiles.json - beside carla.json, outside any repo."""
    return os.path.join(os.path.dirname(env.CONFIG_PATH), "run_profiles.json")


# --------------------------------------------------------------------------- #
# Store
# --------------------------------------------------------------------------- #
def load_doc():
    """The whole store: {"schema", "last", "setups": {name: rec}}.

    A missing or corrupt file reads as empty - a lost setup costs one extra round
    of prompts, never a failed run. An older {active, profiles:{<app_id>: rec}}
    file is upgraded in memory: each app's remembered run becomes a named setup
    '<app_id>_default', which is exactly what it was."""
    # utf-8-SIG, not utf-8: this file gets hand-edited, and on Windows that means
    # Notepad or PowerShell's Set-Content, both of which write a UTF-8 BOM. json.load
    # over a plain utf-8 handle rejects a BOM, which lands in the except below and
    # reads as "no setups at all" - every saved setup silently gone, and the picker
    # starts asking from scratch. utf-8-sig accepts a BOM and a file without one.
    try:
        with open(profiles_path(), encoding="utf-8-sig") as f:
            doc = json.load(f)
    except (OSError, ValueError) as exc:
        # Distinguish "no file yet" (normal, first run) from "there is a file and it
        # does not parse" (an edit went wrong - say so, or the loss looks like amnesia).
        if os.path.isfile(profiles_path()) and not isinstance(exc, FileNotFoundError):
            print(f"[cosim] {profiles_path()} could not be read ({exc}); starting from "
                  f"no saved setups. Fix or delete that file to stop seeing this.")
        doc = {}
    if not isinstance(doc, dict):
        doc = {}
    if not isinstance(doc.get("setups"), dict):
        legacy = doc.get("profiles")
        if isinstance(legacy, dict):
            doc["setups"] = {f"{k}_default": v for k, v in legacy.items()
                             if isinstance(v, dict)}
            active = doc.get("active")
            doc["last"] = f"{active}_default" if active else None
        else:
            doc["setups"] = {}
    doc.setdefault("schema", SCHEMA)
    doc.setdefault("last", None)
    doc.pop("profiles", None)
    doc.pop("active", None)
    # schema 1 -> 2: the setup no longer owns a timestep (the scenario yaml does).
    # Said once, out loud, rather than dropping it silently: a 0.05 s value there was
    # what drove SUMO at half the CARLA clock, so anyone who set it deliberately
    # should know where it went.
    stale = [n for n, r in doc["setups"].items()
             if isinstance(r, dict) and "step_length" in r]
    if stale:
        print(f"[cosim] run setup(s) {', '.join(sorted(stale))} carried step_length; "
              f"the scenario yaml now owns the cadence (CarlaSetup.CarlaTimeStep) and "
              f"SUMO always steps at the FIXS feed period. Dropping the stale key.")
        for n in stale:
            doc["setups"][n].pop("step_length", None)
        doc["schema"] = SCHEMA
    return doc


def save_doc(doc):
    path = profiles_path()
    try:
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8") as f:
            json.dump(doc, f, indent=2)
            f.write("\n")
    except OSError as exc:
        print(f"[cosim] could not save the run setup ({exc}); this run is not remembered.")


def save(name, rec):
    """Store `rec` under `name` and mark it as the one that ran last.

    Clears `partial`: arriving here means every stage a checkpoint was standing in
    for has completed, so the record stops advertising itself as unfinished."""
    doc = load_doc()
    rec = dict(rec)
    rec.pop("partial", None)
    rec["updated"] = datetime.now().isoformat(timespec="seconds")
    doc["setups"][name] = rec
    doc["last"] = name
    save_doc(doc)
    return rec


def save_partial(name, rec, stage):
    """Store the choices made so far, tagged with the stage that had not run yet.

    The full save happens once a run is completely resolved, which is AFTER the map
    import. A cook that fails therefore used to take the app, map and config just
    chosen down with it, and the next run asked the whole questionnaire again - the
    worst moment to re-ask, because a failed import is precisely when you want to
    change one answer and retry. Writing the record early, under the same name,
    means the retry starts from the answers instead of from nothing; the full save
    later overwrites this one rather than leaving a second entry behind.

    `stage` names what had not finished, and is shown in the list: "map import"
    reads as a thing to retry, where a bare "incomplete" reads as corruption."""
    doc = load_doc()
    rec = dict(rec)
    rec["partial"] = stage
    rec["updated"] = datetime.now().isoformat(timespec="seconds")
    doc["setups"][name] = rec
    doc["last"] = name
    save_doc(doc)
    return rec


def delete(name):
    doc = load_doc()
    if doc["setups"].pop(name, None) is not None:
        if doc.get("last") == name:
            doc["last"] = None
        save_doc(doc)
        return True
    return False


def order(doc):
    """Setup names for display: the one that ran last first, then the rest by recency.

    `last` leads explicitly rather than by sorting on its timestamp, because the
    timestamps are second-resolution and two setups saved in the same second tie -
    which left the entry Enter selects sitting somewhere down the list. It is also
    just the right reading order: the default is item 1."""
    setups = doc.get("setups") or {}
    rest = sorted(setups, key=lambda n: (setups[n].get("updated") or ""), reverse=True)
    last = doc.get("last")
    if last in setups:
        rest.remove(last)
        return [last] + rest
    return rest


def suggest_name(app_id, doc):
    """A free name for a new setup: '<app>_default', then '<app>_2', '<app>_3'..."""
    base = app_id or "generic"
    setups = doc.get("setups") or {}
    name = f"{base}_default"
    n = 2
    while name in setups:
        name = f"{base}_{n}"
        n += 1
    return name


# --------------------------------------------------------------------------- #
# Rendering
# --------------------------------------------------------------------------- #
def summarize(rec):
    """One-line digest of a setup, for the list.

    Deliberately no engine or CARLA endpoint: those live in the scenario yaml, and
    printing a copy of them here would mean reading every setup's yaml to draw a
    list - or, worse, showing a remembered value that the yaml no longer says. The
    yaml is named, which is the part that identifies the setup anyway."""
    bits = [rec.get("app") or "no app",
            rec.get("map") or "no map",
            os.path.basename(rec.get("config") or "") or "auto config",
            "gui" if rec.get("sumo_gui", True) else "headless"]
    line = " | ".join(bits)
    # An unfinished setup is still worth opening - that is the whole point of
    # keeping it - but it must not read as one that ran, or the list would claim a
    # map is cooked when the cook is what failed.
    stage = rec.get("partial")
    return f"{line}  [unfinished: {stage}]" if stage else line


def _same_path(a, b):
    """Same file or folder? Paths make a round trip through run_profiles.json, so
    they come back with whatever spelling was current when they were written."""
    if not a or not b:
        return False
    return os.path.normcase(os.path.abspath(a)) == os.path.normcase(os.path.abspath(b))


def _fmt(slot, rec, carla_cfg, derived=None):
    """The value text for one summary line.

    `derived` carries the settings the SCENARIO YAML owns - the bridge and the
    CARLA endpoint - read fresh by the caller each time this is drawn. They are
    shown but not stored, so a yaml edited by hand is reflected here immediately
    instead of the summary repeating a stale copy."""
    derived = derived or {}
    if slot == "app":
        return rec.get("app") or "(none - generic co-sim)"
    if slot == "map":
        origin = rec.get("map_origin")
        return f"{rec.get('map') or '(not chosen)':<26} " + (f"({origin})" if origin else "")
    if slot == "config":
        path = rec.get("config")
        if not path:
            return "(auto-generated for this map)"
        # config_scope stores two values because two is all the BEHAVIOUR needs:
        # whether the generator may overwrite the file, and whether a map change
        # invalidates it. It is too coarse to name the file by - "not app-owned"
        # was being printed as "generated", which is what a yaml you dropped in
        # yourself got called. Which file it is, is a question about the path.
        scope = rec.get("config_scope") or "map"
        app_id = rec.get("app") or GENERIC
        generated = app_catalog.scenario_path(app_id, rec["map"]) if rec.get("map") else None
        if scope == "app":
            where = "app-owned, edit in ~/.fixs/apps"
        elif generated and _same_path(path, generated):
            where = "generated, this app on this map"
        elif _same_path(os.path.dirname(path), app_catalog.scenario_dir(app_id)):
            where = "your variant, in ~/.fixs/apps"
        else:
            where = "your variant, outside ~/.fixs/apps"
        return f"{os.path.basename(path):<26} ({where})"
    if slot == "engine":
        eng = derived.get("engine") or "py"
        how = ("TrafficLayer + mainVirCarla.py" if eng == "py"
               else "TrafficLayer + VirCarlaEnv")
        return f"{eng:<26} ({how}, from the yaml)"
    if slot == "carla":
        # Two different things on one line, so label them: the INSTALL comes from
        # ~/.fixs/carla.json, the ENDPOINT from the scenario yaml. A bare "?" for
        # the install path appeared whenever there was no carla_root - which is
        # exactly what 'client' mode means, so it read as "unknown or broken"
        # rather than as the point of the mode.
        mode = (carla_cfg or {}).get("mode") or "not configured"
        root = (carla_cfg or {}).get("carla_root")
        where = root or ("none on this machine" if mode == "client" else "path unset")
        host = derived.get("carla_host")
        port = derived.get("carla_port")
        # Say whose machine that address is: pointing the endpoint at this
        # machine's own LAN address looks identical to a remote host right up
        # until it times out talking to itself.
        local = derived.get("carla_local")
        whose = "" if local is None else ("  [this machine]" if local else "  [remote]")
        pending = "" if host else "  (default; written when the yaml is generated)"
        return (f"{mode} ({where})  ->  {host or '127.0.0.1'}:"
                f"{port or 2000}{whose}{pending}")
    if slot == "sumo":
        # No SUMO timestep here: it is the FIXS exchange period, fixed by the
        # protocol. The tick and the pacing come from the yaml via `derived`, so this
        # line shows what will actually run rather than a remembered copy.
        gui = "gui" if rec.get("sumo_gui", True) else "headless"
        tick = derived.get("carla_tick")
        pace = derived.get("realtime")
        bits = [gui]
        if tick:
            bits.append(f"CARLA tick {tick:g} s")
        if pace is not None:
            bits.append("realtime" if pace else "as fast as possible")
        return ", ".join(bits)
    return "?"


def show(name, rec, carla_cfg=None, derived=None):
    """Print one setup as a numbered list of its settings."""
    when = (rec.get("updated") or "").replace("T", " ")
    stamp = f"  (last run {when})" if when else "  (new)"
    print(f"\n[cosim] Run setup '{name}'{stamp}:")
    for i, (slot, label) in enumerate(SLOTS, 1):
        print(f"   {i}) {label:<10}{_fmt(slot, rec, carla_cfg, derived)}")


def _input(prompt, default=""):
    try:
        return input(prompt).strip()
    except EOFError:
        return default


def choose_setup(doc, interactive=True):
    """List the saved setups and ask which to open. Returns a name, None to build a
    new one, or QUIT. Enter takes the one that ran last; a non-interactive session
    takes it silently."""
    names = order(doc)
    if not names:
        return None
    last = doc.get("last") if doc.get("last") in names else names[0]
    if not interactive:
        # Say it out loud. Interactively the list carries the marker, but a
        # headless run shows no list, and silently replaying a setup whose map
        # never cooked is how the failure gets rediscovered further downstream.
        stage = (doc["setups"].get(last) or {}).get("partial")
        if stage:
            print(f"[cosim] resuming '{last}', which did not finish ({stage}).")
        return last
    width = min(max(len(n) for n in names), 24)
    while True:
        print("\n[cosim] Saved run setups:")
        for i, n in enumerate(names, 1):
            mark = "   <- last run" if n == last else ""
            print(f"   {i:>2}) {n:<{width}}  {summarize(doc['setups'][n])}{mark}")
        print("    N) new setup")
        print("    D) delete a setup")
        ans = _input(f"[cosim] Which? [1-{len(names)} / N / D], "
                     f"Enter = {names.index(last) + 1} ({last}): ").lower()
        if ans == "":
            return last
        if ans == "n":
            return None
        if ans in ("q", "quit"):
            return QUIT
        if ans == "d":
            # Same answer syntax as --purge-map, from the same parser: these are the
            # two lists a user deletes from, and one of them accepting '1,3,4' while
            # the other took a single number was a difference with nothing behind it.
            import import_map
            picked = import_map.parse_selection(
                _input(f"[cosim] Delete which? [1-{len(names)}, a list like 1,3,4, "
                       f"or 'all'; Enter to cancel]: "), len(names))
            if picked:
                doomed = [names[i] for i in picked]
                # One setup goes on the word 'delete' alone, as it always has.
                # Several are named back first: a mistyped range is the one way to
                # lose work here that a single number cannot.
                if len(doomed) > 1:
                    print(f"[cosim] about to delete {len(doomed)} setup(s):")
                    for n in doomed:
                        print(f"[cosim]    {n}")
                    if not _input("[cosim] Delete them? [y/N]: ").startswith("y"):
                        print("[cosim] cancelled; nothing was deleted.")
                        continue
                for n in doomed:
                    delete(n)
                print(f"[cosim] deleted {', '.join(repr(n) for n in doomed)}.")
                # IN PLACE, not `doc = load_doc()`. The caller passed this dict and
                # goes on using it after we return - to name the setup, to suggest a
                # free name, to decide whether switching is possible - so rebinding a
                # local here left every one of those reading setups that no longer
                # exist. It showed up as "'uga_default' exists. Overwrite it?" for a
                # setup deleted moments earlier in this very menu. Nothing was ever
                # resurrected by it: save/save_partial/delete each re-read the file,
                # so the damage was confined to decisions made from the stale view.
                doc.clear()
                doc.update(load_doc())
                names = order(doc)
                if not names:
                    return None
                last = doc.get("last") if doc.get("last") in names else names[0]
                width = min(max(len(n) for n in names), 24)
            continue
        if ans.isdigit() and 1 <= int(ans) <= len(names):
            return names[int(ans) - 1]
        print("[cosim] enter a number from the list, N for a new setup, or D to delete.")


def name_setup(doc, current, app_id, dirty, interactive=True):
    """The name to save this run under, asked only when it can matter.

    Untouched setup -> keep its name silently; running something you did not edit
    should not interrogate you. Edited, or brand new -> ask, defaulting to the
    obvious answer. That single prompt is what makes "change one thing" a choice
    between amending this setup and forking a new one, instead of a silent
    overwrite of a setup you meant to keep."""
    if current and not dirty:
        return current
    default = current or suggest_name(app_id, doc)
    if not interactive:
        return default
    if current:
        prompt = (f"[cosim] Settings changed. Save as [Enter = '{current}' "
                  f"(overwrite), or type a new name]: ")
    else:
        prompt = f"[cosim] Name this setup [Enter = '{default}']: "
    while True:
        ans = _input(prompt)
        if ans == "":
            return default
        clean = "".join(c for c in ans if c.isalnum() or c in "-_.")
        if not clean:
            print("[cosim] use letters, digits, '-', '_' or '.'.")
            continue
        if clean in (doc.get("setups") or {}) and clean != current:
            if _input(f"[cosim] '{clean}' exists. Overwrite it? [y/N]: ").lower() \
                    not in ("y", "yes"):
                continue
        return clean


def cascade(rec, changed):
    """Widen `changed` with the slots it invalidates.

    Changing the app invalidates the map and the scenario yaml - both belonged to
    the app you are leaving. Changing the map invalidates the scenario yaml only
    when that yaml was the map's own generated config; an app-owned yaml is written
    against the app, not the map, so it survives."""
    out = set(changed)
    if "app" in out:
        out.update({"map", "config"})
    if "map" in out and (rec.get("config_scope") or "map") == "map":
        out.add("config")
    return out


#: what ask() returns besides a set of slots to edit
RUN, QUIT, SWITCH, NEW = "run", "quit", "switch", "new"


def ask(name, rec, carla_cfg=None, interactive=True, can_switch=True, derived=None,
        why="non-interactive: running this setup as-is "
            "(pass flags to override, --fresh to start over)"):
    """Show a setup and ask what to do with it.

    Returns RUN / QUIT / SWITCH / NEW, or a set of slot keys to edit. A
    non-interactive session prints the summary and runs it - scheduled and
    scripted runs reuse a setup verbatim, and CLI flags are how they deviate."""
    if not interactive:
        show(name, rec, carla_cfg, derived)
        print(f"[cosim] {why}.")
        return RUN
    while True:
        show(name, rec, carla_cfg, derived)
        extra = " | S = switch setup | N = new" if can_switch else ""
        ans = _input(f"[cosim] Enter = run it | 1-{len(SLOTS)} = change "
                     f"(e.g. \"2 4\"){extra} | Q = quit: ").lower()
        if ans == "":
            return RUN
        if ans in ("q", "quit"):
            return QUIT
        if can_switch and ans == "s":
            return SWITCH
        if can_switch and ans == "n":
            return NEW
        if ans in ("a", "all"):
            return set(SLOT_KEYS)
        picks = [t for t in ans.replace(",", " ").split() if t]
        if picks and all(t.isdigit() and 1 <= int(t) <= len(SLOTS) for t in picks):
            # Exactly what was selected. cascade() is applied by the caller, which
            # needs to tell a row the user opened from one that only fell over with
            # it - the first asks, the second settles itself where it can.
            return {SLOT_KEYS[int(t) - 1] for t in picks}
        print("[cosim] enter numbers to change, Enter to run"
              + (", S / N to switch" if can_switch else "") + ", or Q.")
