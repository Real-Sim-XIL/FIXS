"""
import_map.py - import a RoadRunner/OpenDRIVE map into the configured CARLA.

Generalizes the proven one-click import (run CARLA's
`Util/BuildTools/Import.py --package=<name>` directly, with UE4_ROOT set) into a
shippable, parameterized helper. Used by run_cosim's --auto-import and by the
per-app one-click import_*.bat / .sh wrappers.

A map "package" is the `<name>.json` descriptor plus its fbx/xodr/fbm asset
folder. This helper makes sure that package is staged under <carla_root>/Import,
then runs the cook.

A raw RoadRunner export is accepted too: it ships no `<name>.json`, so one is
generated from its filenames, and its geometry is renamed to the map name you
are importing as (see generate_descriptor). That way `--package mlk_no_signal`
cooks `mlk_no_signal` whether the export inside was called `MLK_no_signal_0805`
or anything else, and next month's re-export lands on the same map rather than a
new one.

The package is sourced, in order:
  --pick-release        list the repo's map releases and prompt which VERSION to
                        import, then download it via gh (no URL/version to
                        hand-maintain - you pick from what's published). gh is
                        needed to LIST and to DOWNLOAD, but not to import: when it
                        is missing or cannot see the library, the picker says so,
                        lists nothing online, and still offers the local file, or
  already staged        files already present under <carla_root>/Import, or
  --package-dir <dir>   a local folder holding the package files, or
  --package-url <url>   tried via the GitHub CLI if installed (handles private
                        repos), else you are prompted to point at a copy you
                        downloaded by hand from the release - the portable path,
                        needing only browser access, no gh / auth.

Both CARLA flavours are served, by the route each one supports - the flavour is
read from the saved config and dispatched on, not used to refuse:

  source    COOKS the package. The cook runs the Unreal editor, so this is the
            only flavour that can turn an fbx/xodr into a map.
  packaged  INSTALLS the precooked package (the Digital-Twin Library's
            `*_cooked.tar.gz`) into the package root - a plain extract, no editor
            (`install_precooked` -> `install_cooked`).

run_cosim's preflight calls the same `install_precooked`, so the two front doors
cannot disagree about what a packaged build can install. carla_root / ue4_root
default to the saved env config (~/.fixs/carla.json) written by carla_env_setup.py.

Examples:
  # pick which published version to import (lists releases tagged map-*):
  python import_map.py --pick-release --repo ORNL-Real-Sim/FIXS_Applications --tag-prefix map-
  python import_map.py --package RP_Ver0529 --package-dir C:/src_ext/Carla/Import
  python import_map.py --package RP_Ver0529 --package-url https://.../RP_Ver0529_import.zip
"""
import argparse
import fnmatch
import os
import platform
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
import time
import xml.etree.ElementTree as ET
import zipfile

import carla_env_setup as env


def _mode(mode=None):
    """The CARLA flavour to resolve paths for: the caller's, else the saved
    config's, else 'source'. Defaulting from the config is what lets every
    existing (source-build) call site stay unchanged."""
    if mode:
        return mode
    return (env.load_config() or {}).get("mode") or "source"


def package_root(carla_root):
    """The folder a packaged CARLA's own ImportAssets.sh cds into - the one that
    directly holds CarlaUE4/Content.

    env.packaged_exe accepts carla_root pointing EITHER at that folder or at a
    wrapper holding WindowsNoEditor/ (LinuxNoEditor/), which are one level apart;
    resolving through the launcher we already found is what keeps the two spellings
    from producing two different Content roots."""
    exe = env.packaged_exe(carla_root)
    return os.path.dirname(exe) if exe else carla_root


def content_root(carla_root, mode=None):
    """The Content/ dir holding cooked assets for this CARLA flavour.

    A source build keeps it under Unreal/CarlaUE4/; a packaged build has it beside
    the launcher. Everything below resolves through here, so the packaged flavour
    needed no new naming rules - only this one prefix differs."""
    if _mode(mode) == "packaged":
        return os.path.join(package_root(carla_root), "CarlaUE4", "Content")
    return os.path.join(carla_root, "Unreal", "CarlaUE4", "Content")


def cooked_content_dir(carla_root, name, mode=None):
    """The Content/<name> folder produced by a successful import."""
    return os.path.join(content_root(carla_root, mode), name)


def cooked_map_path(carla_root, name, mode=None):
    """Where the cooked .umap lands after a successful import."""
    return os.path.join(cooked_content_dir(carla_root, name, mode),
                        "Maps", name, name + ".umap")


def map_is_imported(carla_root, name, mode=None):
    return os.path.isfile(cooked_map_path(carla_root, name, mode))


def package_descriptor(carla_root, name, mode=None):
    """The <name>.Package.json the cook writes beside the imported content. It is
    the authoritative record of what the package actually holds - the map's real
    name and its /Game/... path - neither of which is *guaranteed* to equal the
    package name (see _package_from_tag: that is a release convention, not a rule).
    A precooked package ships the same file, so a packaged build is resolved by the
    same evidence a source build is."""
    return os.path.join(cooked_content_dir(carla_root, name, mode),
                        "Config", name + ".Package.json")


def declared_maps(carla_root, name, mode=None):
    """[(map_name, /Game/... level path)] declared by the package descriptor.
    Entries whose .umap is missing are dropped: a descriptor outlives the content
    it describes (a half-wiped re-import leaves one behind), and a name we hand to
    load_world must point at a level that is really on disk."""
    import json
    try:
        with open(package_descriptor(carla_root, name, mode), encoding="utf-8") as f:
            declared = json.load(f).get("maps") or []
    except (OSError, ValueError):
        return []
    found = []
    for m in declared:
        mname, mpath = m.get("name"), m.get("path")
        if not mname or not str(mpath).startswith("/Game/"):
            continue
        umap = os.path.join(content_root(carla_root, mode),
                            *mpath[len("/Game/"):].split("/"), mname + ".umap")
        if os.path.isfile(umap):
            found.append((mname, f"{mpath}/{mname}"))
    return found


def find_level_paths(carla_root, name, mode=None):
    """Every cooked level called <name>.umap, as /Game/... object paths.

    More than one means load_world(<name>) is a coin flip: CARLA resolves a bare
    name by recursive file search and silently takes the first hit (see
    UCarlaEpisode::LoadNewEpisode -> PathList[0]), so which copy you get is
    directory-traversal order, not a choice. A full /Game/... path is exact."""
    content = content_root(carla_root, mode)
    umap = name + ".umap"
    found = []
    for root, _dirs, files in os.walk(content):
        if umap in files:
            rel = os.path.relpath(root, content).replace(os.sep, "/")
            found.append((f"/Game/{rel}/{name}", os.path.getsize(os.path.join(root, umap))))
    return sorted(found)


def choose_level_path(carla_root, name, mode=None, interactive=None):
    """Full /Game/... path to load for `name`. Only reached when the path could NOT
    be worked out from the package itself, so there is nothing to default to: if
    several cooked levels share the name, only the user can say which was meant.

    `interactive` is the caller's answer to "may this stop and ask?"; isatty() is
    only the default. A --serve host has a terminal and no one at it."""
    found = find_level_paths(carla_root, name, mode)
    if len(found) <= 1:
        return found[0][0] if found else None

    print(f"\n[cosim] {len(found)} cooked maps are named '{name}' and nothing "
          f"identifies which is wanted - pick one:")
    for i, (path, size) in enumerate(found, 1):
        print(f"   {i}) {path}  ({size / 1048576:.1f} MB)")
    if not (sys.stdin.isatty() if interactive is None else interactive):
        sys.exit("[cosim] non-interactive and the name is ambiguous; pass --map "
                 "<package> so the path resolves, or delete the stale copies.")
    while True:
        ans = _prompt(f"[cosim] Which one? [1-{len(found)}]: ").strip()
        if ans.isdigit() and 1 <= int(ans) <= len(found):
            return found[int(ans) - 1][0]
        print("[cosim] invalid choice; enter a number from the list.")


def duplicate_level_note(carla_root, name, using, mode=None):
    """Heads-up text when other cooked levels share `name`, else "". Informational
    only - we load by exact /Game/... path, so the copies cannot be picked up by
    mistake; they are just leftovers from repeat imports, worth deleting."""
    others = [p for p, _ in find_level_paths(carla_root, name, mode) if p != using]
    if not others:
        return ""
    return (f"[cosim] note: {len(others)} other cooked map(s) also named '{name}'; "
            f"ignored (loading by full path):\n"
            + "\n".join(f"           {p}" for p in others))


def resolve_cooked_map(carla_root, name, mode=None):
    """(map_name, /Game/... level path) for an imported package, or None when it
    cannot be resolved *unambiguously*.

    Resolve, never assume. The picked release gives a PACKAGE name; what CARLA
    loads is a MAP name, and the two agree only by naming convention. So: accept
    the conventional layout when it is genuinely on disk, else believe the
    package's own descriptor, else give up - so the caller can ask the user
    rather than invent a name that load_world will fail to open."""
    if map_is_imported(carla_root, name, mode):
        return name, f"/Game/{name}/Maps/{name}/{name}"
    found = declared_maps(carla_root, name, mode)
    return found[0] if len(found) == 1 else None


def _descriptor(carla_root, name):
    return os.path.join(carla_root, "Import", name + ".json")


def _stage_from_path(path, import_dir):
    """Copy/extract a downloaded package (a .zip or an extracted folder) into
    <carla_root>/Import, preserving the descriptor + asset-folder layout."""
    if os.path.isfile(path) and path.lower().endswith(".zip"):
        print(f"[import] extracting {path} -> {import_dir}")
        with zipfile.ZipFile(path) as z:
            z.extractall(import_dir)
    elif os.path.isdir(path):
        print(f"[import] copying {path} -> {import_dir}")
        for item in os.listdir(path):
            src = os.path.join(path, item)
            dst = os.path.join(import_dir, item)
            if os.path.isdir(src):
                shutil.copytree(src, dst, dirs_exist_ok=True)
            else:
                shutil.copy2(src, dst)
    else:
        sys.exit(f"[import] package path is not a .zip or a folder: {path}")


def _gh_release_ref(url):
    """(repo, tag, asset) if url is a github release-asset URL, else None."""
    m = re.match(r"https?://github\.com/([^/]+/[^/]+)/releases/download/([^/]+)/(.+)$", url)
    return (m.group(1), m.group(2), m.group(3)) if m else None


def _try_gh_download(package_url):
    """Opportunistic auto-download via the GitHub CLI (handles private repos).
    Returns (zip_path, tmpdir) on success, else (None, None) so we fall back to a
    manual prompt - many machines won't have gh installed/authenticated."""
    ref = _gh_release_ref(package_url or "")
    gh = shutil.which("gh")
    if not ref or not gh:
        return None, None
    repo, tag, asset = ref
    tmp = tempfile.mkdtemp(prefix="fixs-map-")
    print(f"[import] trying gh to download {asset} from {repo}@{tag} ...")
    rc = subprocess.call([gh, "release", "download", tag, "-R", repo,
                          "-p", asset, "-D", tmp, "--clobber"])
    got = os.path.join(tmp, asset)
    if rc == 0 and os.path.isfile(got):
        return got, tmp
    print("[import] gh download unavailable; will ask for a local copy instead.")
    shutil.rmtree(tmp, ignore_errors=True)
    return None, None


def _select_package(name, package_url, precooked=False, inside=("xodr", "fbx")):
    """Let the user point at a package they downloaded by hand - a native file
    picker, falling back to a typed path. This is the portable path: no GitHub
    CLI / auth needed, just browser access to the release.

    `inside` names the files that identify this kind of thing once it has been
    extracted: a map export is known by its .xodr/.fbx, a SUMO scenario by its
    .sumocfg. They are what makes one dialog enough - see below - so they are the
    caller's to state. Hardcoding the map's extensions here filtered a SUMO
    scenario's own files out of its dialog, which opened on an empty listing and
    read as the explorer having failed to appear.

    `precooked` says which artefact this CARLA can actually take, and it has to be
    asked for by name. A PACKAGED build cooks nothing, so the only thing it can
    install is the library's `*_cooked.tar.gz` (install_precooked) - yet this
    dialog offered "[Z = zip, F = folder]" and filtered `*.zip`, steering the one
    flavour that needs the tarball towards the two artefacts it must reject (#283).
    A SOURCE build is the mirror image: it cooks, so it wants the .zip or the
    extracted folder and cannot use a cooked tarball.

    ONE explorer, and no zip-or-folder question. tkinter cannot select a file or a
    directory in the same box - but it does not need to, because picking any file
    INSIDE an extracted package identifies it just as well as selecting its folder
    would. So the dialog lists the bundle (.zip) and the `inside` files, and
    anything that is not an archive resolves to its containing folder. The
    zip-or-folder decision ends up where it belongs - in what the user clicks -
    instead of in a question asked before the explorer even opens, and there is
    never a second dialog to cancel into. Everything downstream already takes
    either (_stage_from_path copies a tree or unpacks an archive)."""
    shown = "/".join("." + e for e in inside)
    what = ("precooked package (*_cooked.tar.gz)" if precooked
            else f".zip (or an extracted folder, by its {shown})")
    print(f"\n[import] Select the downloaded '{name}' {what}.")
    if not precooked:
        print(f"[import] Either the .zip, or - if it is already extracted - the "
              f"{shown} inside it (the folder is taken from it).")
    if package_url:
        print("[import] If you don't have it yet, download it (browser is fine - "
              "you need access to the release):")
        print(f"             {package_url}")
    start = _browse_start_dir()
    try:
        import tkinter as tk
        from tkinter import filedialog
        root = tk.Tk()
        root.withdraw()
        root.update()
        if precooked:
            path = filedialog.askopenfilename(
                title=f"Select the downloaded precooked {name} package (*_cooked.tar.gz)",
                initialdir=start,
                filetypes=[("Precooked map packages", "*.tar.gz"), ("All files", "*.*")])
        else:
            pats = " ".join("*." + e for e in inside)
            path = filedialog.askopenfilename(
                title=f"Select the {name} (.zip), or the {shown} of an "
                      f"extracted one",
                initialdir=start,
                filetypes=[(f"{name} (.zip or {shown})", f"*.zip {pats}"),
                           ("Zip archives", "*.zip"),
                           (f"Extracted ({shown})", pats),
                           ("All files", "*.*")])
            path = _folder_of_export_file(path)
        root.destroy()
        if path:
            _report_pick(path, inside)
            return path
    except Exception as exc:  # no display / no tkinter
        print(f"[import] file picker unavailable ({exc}); type the path instead.")
    if not sys.stdin.isatty():
        sys.exit(f"[import] non-interactive session: pass --package-dir with the "
                 f"downloaded {what}.")
    # _prompt, not input: isatty() is unreliable through some Windows shells, so the
    # guard above can let a closed stdin through. A source build never noticed - the
    # zip-or-folder question above absorbed the EOF first - but a precooked pick asks
    # nothing, which made this the first read and turned it into an EOFError traceback.
    path = _prompt(f"[import] Path to the downloaded {what}: ").strip().strip('"')
    if not path or not os.path.exists(path):
        sys.exit(f"[import] path not found: {path!r}")
    _report_pick(path)
    return path


def _folder_of_export_file(path):
    """A pick from the single explorer, resolved to what staging actually wants.

    An archive is the thing itself. Anything else was clicked to point AT a folder
    - that is the whole reason the dialog offers .xodr/.fbx - so hand back the
    directory containing it. A folder typed or dragged in already is left alone."""
    if not path or os.path.isdir(path):
        return path
    if path.lower().endswith((".zip", ".tar.gz", ".tgz")):
        return path
    parent = os.path.dirname(path)
    print(f"[import] taking the export folder of {os.path.basename(path)}")
    return parent


def _browse_start_dir():
    """Where the file dialogs open. The map cache holds everything import_map has
    downloaded, so it is where a hand-fetched bundle most often lands too. Falls
    back to the home directory rather than to wherever the process happens to be -
    a picker opening in FIXS/Carla helps nobody."""
    for d in (_map_cache_dir(), os.path.expanduser("~")):
        if d and os.path.isdir(d):
            return d
    return None


def _report_pick(path, inside=("xodr", "fbx")):
    """Say what is actually in what was just picked.

    A folder is only the right one if it holds the thing, and until now nothing
    said so until _describe_export failed several steps later with "no .xodr
    under ...". Naming what was found makes a good pick obvious; listing the
    subfolders of a bad one turns "wrong folder" into "it is one level down",
    which is the mistake the nesting in these bundles invites
    (Import/UGA_Campus/UGA_Campus/Carla_material/Exports).

    `inside` is the caller's - what counts as found differs by what is being
    picked, and reporting "no .xodr here" about a SUMO scenario would be noise
    dressed up as a diagnosis."""
    try:
        if os.path.isfile(path):
            print(f"[import] picked {os.path.basename(path)} "
                  f"({os.path.getsize(path) / (1 << 20):.0f} MB)")
            return
        names = sorted(os.listdir(path))
        hits = [f for f in names
                if f.lower().endswith(tuple("." + e for e in inside))]
        print(f"[import] picked {path}")
        if hits:
            print(f"[import]   contains {', '.join(hits[:6])}"
                  + (f" (+{len(hits) - 6} more)" if len(hits) > 6 else ""))
            return
        shown = "/".join("." + e for e in inside)
        subs = [f for f in names if os.path.isdir(os.path.join(path, f))]
        print(f"[import]   no {shown} directly here"
              + (f"; subfolders: {', '.join(subs[:8])}" if subs else "")
              + ("" if not subs else " - it may be one of these."))
    except OSError:
        pass          # reporting must never be what stops an import


def _has_descriptor(src):
    """True if the package at `src` (a .zip or a folder) already carries a CARLA
    map descriptor - any *.json other than the road-painter decals file. A raw
    RoadRunner export carries none (only .fbx/.xodr/.geojson/.rrdata.xml), which
    is how stage_package tells a hand-authored package from one whose descriptor
    must be generated.

    `.geojson` deliberately does not count: like CARLA's own `fnmatch("*.json")`,
    the check needs the literal '.json', so `<map>.geojson` is ignored."""
    def is_desc(fname):
        base = os.path.basename(fname).lower()
        return base.endswith(".json") and base != "roadpainter_decals.json"
    if os.path.isfile(src) and src.lower().endswith(".zip"):
        with zipfile.ZipFile(src) as z:
            return any(is_desc(n) for n in z.namelist())
    if os.path.isdir(src):
        for _root, _dirs, files in os.walk(src):
            if any(is_desc(f) for f in files):
                return True
    return False


def _tile_split(fbx_name):
    """(stem, x, y) parsed from a strict `<stem>_Tile_<x>_<y>.fbx`, else None.

    Strict on purpose: CARLA reads the streaming-grid index off the last two
    underscore tokens of the tile name (LoadAssetMaterialsCommandlet.cpp), so a
    loose `<map>_Tile_0_0_final.fbx` would silently cook as tile (0,0) and
    collide. Anything not exactly `<stem>_Tile_<int>_<int>.fbx` is not a tile -
    the caller warns about it rather than guessing.

    Returns the stem too, so a tile set can be recognised without knowing its
    name in advance: a raw export names its tiles after itself, not after the
    map we are about to cook."""
    if not fbx_name.lower().endswith(".fbx"):
        return None
    m = re.fullmatch(r"(.+)_Tile_(\d+)_(\d+)", fbx_name[:-4])
    return (m.group(1), int(m.group(2)), int(m.group(3))) if m else None


def _find_export(scan_root, map_name):
    """(asset_dir, stem) of the RoadRunner export staged under `scan_root`, where
    `stem` is the name the export gave itself - the stem of its `.xodr`.

    Discovered, never assumed. A RoadRunner export carries the author's working
    name (`MLK_no_signal_0805`, `JC_Newest_Ver_MLK_noped1002_final_debug`), which
    is essentially never the name the map should be cooked under, so requiring
    `<map_name>.xodr` here just fails on every real export.

    One package holds one map. That is the rule for a Digital-Twin-Library bundle
    and for a folder picked by hand, and a tiled map is still one map - so a
    second .xodr is not a choice to offer, it is a packaging mistake to report.
    Both shapes of it land on the same destination anyway: this import is headed
    for a single `/Game/<pkg>/Maps/<map_name>`, which two maps would overwrite in
    turn."""
    hits = []
    for root, _dirs, files in os.walk(scan_root):
        for f in sorted(files):
            if f.lower().endswith(".xodr"):
                hits.append((root, f[:-5]))
    if not hits:
        sys.exit(f"[import] cannot describe '{map_name}': no .xodr under {scan_root}. "
                 f"A raw RoadRunner export must ship its OpenDRIVE road network "
                 f"(<export>.xodr) beside its <export>.fbx.")
    if len(hits) == 1:
        return hits[0]

    rel = lambda h: os.path.relpath(os.path.join(h[0], h[1] + ".xodr"), scan_root)
    what = (f"{hits[0][1]}.xodr is staged in {len(hits)} places (staged more than once?)"
            if len({h[1] for h in hits}) == 1 else
            f"{len(hits)} maps are staged here, and one package holds one map")
    where = "\n".join(f"             {rel(h)}" for h in sorted(hits))
    sys.exit(f"[import] cannot describe '{map_name}': {what}. Keep one, so a single "
             f"descriptor maps to a single destination:\n{where}")


def _export_fbx(asset_dir, stem):
    """(source_fbx, [tile_fbx, ...]) - bare filenames - for the export `stem` in
    `asset_dir`. Exactly one of the two is populated: a map is single-source or
    tiled, never both.

    Normally the fbx is named after the .xodr - that is what RoadRunner writes.
    When it is not, one-map-per-package (_find_export) still identifies it: a
    lone .fbx, or a single tile set, can only be this map's geometry. It is taken
    with a WARNING rather than silently, because a stem mismatch usually means
    the package was assembled by hand, and the one .fbx staged might be scenery
    rather than the road network - which the cook would not catch.

    Several unrelated .fbx (a RoadRunner layer-split export) cannot be resolved
    that way, so it is reported: cooking the wrong layer as the whole map only
    shows up after the cook, and the fix belongs in the package."""
    fbx = sorted(f for f in os.listdir(asset_dir) if f.lower().endswith(".fbx"))
    single = stem + ".fbx" if stem + ".fbx" in fbx else None
    tiles = [f for f in fbx if (_tile_split(f) or (None,))[0] == stem]
    if single and tiles:
        sys.exit(f"[import] cannot describe '{stem}': both {stem}.fbx and "
                 f"{stem}_Tile_*.fbx are present - a map is single-source or tiled, "
                 f"not both.")
    if single or tiles:
        return single, tiles
    if not fbx:
        sys.exit(f"[import] cannot describe '{stem}': found {stem}.xodr but no .fbx "
                 f"beside it in {asset_dir}.")

    # The .xodr and the .fbx were named apart.
    plain = [f for f in fbx if _tile_split(f) is None]
    prefixes = sorted({t[0] for t in (_tile_split(f) for f in fbx) if t})
    if len(prefixes) == 1 and not plain:
        chosen = [f for f in fbx if _tile_split(f)[0] == prefixes[0]]
        print(f"[import] warning: {stem}.xodr has no {stem}_Tile_<x>_<y>.fbx. Taking "
              f"the only tile set staged, {prefixes[0]}_Tile_*.fbx ({len(chosen)} "
              f"tiles) - check that is this map's geometry.")
        return None, chosen
    if len(plain) == 1 and not prefixes:
        print(f"[import] warning: {stem}.xodr has no {stem}.fbx. Taking the only "
              f"geometry staged, {plain[0]} - check that is this map's road network "
              f"and not, say, scenery.")
        return plain[0], []
    listed = "\n".join(f"             {f}" for f in fbx)
    sys.exit(f"[import] cannot describe '{stem}': {stem}.xodr has no {stem}.fbx or "
             f"{stem}_Tile_<x>_<y>.fbx, and several geometries could be it:\n{listed}\n"
             f"         One package holds one map: name the .fbx after the .xodr, or "
             f"stage only that map's geometry.")


def _rename_export(asset_dir, stem, map_name, source, tiles):
    """Rename the staged export's geometry from `stem` to `map_name`, in place.
    Returns (source, tiles) under their new names.

    Why rename rather than name the map after the export: the cook writes the
    walker navmesh as `Maps/<name>/Nav/<fbx_stem>.bin` (CARLA's
    Util/BuildTools/Import.py build_binary_for_navigation), while at runtime
    FNavigationMesh::Load asks for `<MapName>.bin` - so geometry whose stem is
    not the map name costs that map its pedestrian navigation, silently. Keeping
    package == map == umap == fbx stem also keeps a re-export importable under
    the SAME name, so run profiles, app manifests and the library catalog keep
    resolving instead of orphaning on every export date.

    Only the .fbx moves. CARLA copies the .xodr to `<name>.xodr` itself, and the
    .geojson / .rrdata.xml stay put so Import/<map_name>/ still shows which
    export it came from. A sibling `<stem>.fbm` (embedded textures - normally
    created by the importer, occasionally shipped) moves with its .fbx so the
    pair cannot drift."""
    def move(old, new):
        if old == new:
            return new
        src, dst = os.path.join(asset_dir, old), os.path.join(asset_dir, new)
        if os.path.exists(dst):
            sys.exit(f"[import] cannot rename {old} -> {new}: {new} already exists in "
                     f"{asset_dir}. Stage this export on its own.")
        os.rename(src, dst)
        fbm = os.path.join(asset_dir, old[:-4] + ".fbm")
        if os.path.isdir(fbm):
            os.rename(fbm, os.path.join(asset_dir, new[:-4] + ".fbm"))
        return new

    if source:
        source = move(source, map_name + ".fbx")
    tiles = [move(t, f"{map_name}_Tile_{xy[1]}_{xy[2]}.fbx")
             for t, xy in ((t, _tile_split(t)) for t in tiles)]
    print(f"[import] renamed the export geometry '{stem}' -> '{map_name}' so the cooked "
          f"map, its assets and its navmesh all answer to one name.")
    return source, tiles


def _tile_size(asset_dir):
    """RoadRunner tile edge length, in metres, for a tiled export in `asset_dir`.

    Prefer the export's own TilesInfo.txt (`firstTileCenterX,firstTileCenterY,
    tileSize`); fall back to 2000 when it is absent - the common case. 2000 is
    both the cook commandlet's own default (PrepareAssetsForCookingCommandlet)
    and the maximum Unreal honours (FbxStaticMeshImport clamps TileSize > 2000),
    so the descriptor and the cook agree with nothing to infer."""
    info = os.path.join(asset_dir, "TilesInfo.txt")
    if os.path.isfile(info):
        try:
            with open(info, encoding="utf-8") as f:
                for line in f:
                    nums = re.findall(r"-?\d+(?:\.\d+)?", line)
                    if len(nums) >= 3:
                        return int(round(float(nums[2])))
        except (OSError, ValueError):
            pass
        print(f"[import] warning: {info} present but unparseable; using tile_size=2000")
    return 2000


def generate_descriptor(import_dir, map_name):
    """Synthesise Import/<map_name>.json for a raw RoadRunner export that shipped
    no CARLA descriptor, deriving everything from the staged filenames (see
    FIXS_Applications #4). stage_package isolates a raw export under
    Import/<map_name>/, so that subtree is scanned - never the shared Import/
    root. Returns the descriptor path.

    The export names itself and we rename to match. RoadRunner writes the
    author's working name, so nothing here may assume `<map_name>.xodr` exists:
    _find_export discovers the export, _rename_export renames its geometry to
    `map_name`, and the descriptor is written under that single name. Cooking
    under the export's own name instead would make every re-export a *different*
    CARLA map and orphan the run profiles, app manifests and catalog entries
    pointing at the old one - and would break the walker navmesh (_rename_export).

    Resolve or report, never guess. One package holds one map (a tiled map is
    still one map), so anything that breaks that rule - no .xodr, several .xodr,
    geometry that cannot be tied to the road network - is a packaging mistake
    named on the spot rather than a prompt or a coin flip."""
    import json

    subtree = os.path.join(import_dir, map_name)
    scan_root = subtree if os.path.isdir(subtree) else import_dir
    asset_dir, stem = _find_export(scan_root, map_name)
    source, tiles = _export_fbx(asset_dir, stem)
    if stem != map_name:
        source, tiles = _rename_export(asset_dir, stem, map_name, source, tiles)

    rel = lambda f: os.path.relpath(os.path.join(asset_dir, f),
                                    import_dir).replace(os.sep, "/")
    entry = {"name": map_name,
             # Left under the export's own name: CARLA copies it to <name>.xodr
             # on import (Import.py, "Move maps XODR files if any").
             "xodr": rel(stem + ".xodr"),
             "use_carla_materials": False}  # RoadRunner ships its own materials
    if source:
        entry["source"] = rel(source)
        kind = "single-source"
    else:
        entry["tile_size"] = _tile_size(asset_dir)
        entry["tiles"] = [rel(t) for t in sorted(tiles)]
        kind = f"tiled, {len(tiles)} tiles, tile_size={entry['tile_size']}"
    if stem != map_name:
        # Provenance: which export this map was cooked from. CARLA's importer
        # copies keys it does not know straight through, so this costs nothing
        # and survives in the descriptor for whoever asks "which export is this?".
        entry["exported_as"] = stem

    # Surface, don't silently drop, RoadRunner layer-split exports: extra .fbx
    # that are neither the source nor a strict tile. CARLA's own generator
    # ignores them; a dropped layer should at least be visible.
    kept = {source} if source else set(tiles)
    for f in sorted(os.listdir(asset_dir)):
        if f.lower().endswith(".fbx") and f not in kept:
            print(f"[import] warning: ignoring unexpected fbx {f!r} (not {map_name}.fbx "
                  f"or {map_name}_Tile_<x>_<y>.fbx); not part of the descriptor.")

    descriptor = os.path.join(import_dir, map_name + ".json")
    with open(descriptor, "w", encoding="utf-8") as f:
        json.dump({"maps": [entry], "props": []}, f, indent=2)
        f.write("\n")
    print(f"[import] generated {descriptor} ({kind})")
    return descriptor


def stage_package(carla_root, name, package_url=None, package_dir=None, package_pick=False):
    """Ensure <carla_root>/Import has <name>.json (+ its assets). Sources the
    package, in order: an already-staged descriptor, an explicit --package-dir,
    a hand-picked download (--package-pick, always asks), an opportunistic gh
    download of --package-url, else a file picker for a hand-downloaded copy."""
    import_dir = os.path.join(carla_root, "Import")
    descriptor = _descriptor(carla_root, name)
    # What is already in Import/ under this name is what CARLA would cook, so it is
    # announced and offered before anything is written over it - see
    # resolve_existing_staging. The defaults reproduce the previous behaviour.
    have_source = bool(package_url or package_dir or package_pick)
    if resolve_existing_staging(carla_root, name, have_source) == "use":
        if os.path.isfile(descriptor):
            print(f"[import] using the staged package: {descriptor}")
            return import_dir
        print(f"[import] no descriptor beside the staged folder; staging afresh.")

    os.makedirs(import_dir, exist_ok=True)
    tmpdir = None
    try:
        if package_dir:
            src = package_dir
        elif package_pick:
            src = _select_package(name, package_url)  # forced manual select
        else:
            src, tmpdir = _try_gh_download(package_url)
            if src is None:
                src = _select_package(name, package_url)
        if _has_descriptor(src):
            # Hand-authored package: <name>.json + its <name>/ asset folder land
            # directly under Import/.
            _stage_from_path(src, import_dir)
        else:
            # Raw RoadRunner export (no CARLA .json): isolate it under
            # Import/<name>/ so repeat imports of different maps cannot collide,
            # then synthesise Import/<name>.json from the fbx/xodr (see #4).
            print(f"[import] '{name}' ships no CARLA descriptor; treating it as a "
                  f"raw RoadRunner export and generating one.")
            raw_dest = os.path.join(import_dir, name)
            # REPLACE, never merge. This directory is derived from `src`, and
            # _describe_export renames the geometry inside it to the map name - so
            # a run that staged successfully and then died later (a cook that
            # crashed) leaves <name>.fbx here, and _stage_from_path copies the
            # export's own ugaaa.fbx back in beside it. _export_fbx then sees two
            # unrelated .fbx and refuses, identically, on every retry: the import
            # became unrecoverable without deleting this folder by hand. The old
            # `fresh` guard could not help - it only covered a descriptor failure
            # on a directory that same call had created, and by the second attempt
            # the directory was no longer new.
            if os.path.isdir(raw_dest):
                print(f"[import] re-staging: replacing {raw_dest}")
                shutil.rmtree(raw_dest, ignore_errors=True)
            os.makedirs(raw_dest, exist_ok=True)
            _stage_from_path(src, raw_dest)
            try:
                generate_descriptor(import_dir, name)
            except SystemExit:
                # Unconditional now: the directory above is always this call's, so
                # there is never someone else's staging to preserve.
                shutil.rmtree(raw_dest, ignore_errors=True)
                print(f"[import] removed the partially staged {raw_dest}")
                raise
    finally:
        if tmpdir:
            shutil.rmtree(tmpdir, ignore_errors=True)

    if not os.path.isfile(descriptor):
        sys.exit(f"[import] after staging, descriptor still missing: {descriptor}\n"
                 f"         (a packaged map must contain {name}.json; a raw export "
                 f"must be named after the map so one can be generated)")
    return import_dir


def _looks_like_bundle(names):
    """True if a package's entries are rooted under a top-level `carla/` - the
    Digital-Twin-Library combined layout (`carla/` + `sumo/`) - rather than a
    flat/legacy CARLA-only package."""
    tops = {n.replace("\\", "/").split("/", 1)[0] for n in names if n.strip()}
    return "carla" in tops


def _bundle_members(z, sumo_only):
    """Which entries of a bundle zip to extract: everything, or only `sumo/`."""
    if not sumo_only:
        return None                      # extractall's own default
    return [n for n in z.namelist()
            if n.replace("\\", "/").split("/", 1)[0] == "sumo"]


def open_bundle(src, cache_name=None):
    """Split a map source into its CARLA package and its SUMO scenario.

    A Digital-Twin-Library map ships as one bundle - a zip (or folder) with a
    top-level `carla/` (the CARLA import package) and `sumo/` (the scenario).
    Returns `(carla_src, sumo_dir)`:
      - bundle  -> (`<cache>/carla`, `<cache>/sumo`); a zip is extracted once
                   (re-extracted only when the zip is newer). `cache_name` (the
                   cooked map name) extracts into ~/.fixs/maps/<cache_name>/, else
                   a sibling `<stem>_unpacked/`. A folder is used in place.
      - bundle, CLIENT mode -> `(None, <cache>/sumo)`; the CARLA half is not
                   extracted at all, because a machine with no local CARLA has
                   nothing to import it into (FIXS#309).
      - legacy CARLA-only zip/folder -> `(src, None)`, unchanged behavior.
    `carla_src` is what to hand `ensure_map`/`stage_package`; `sumo_dir` (or None)
    is where run_cosim finds the `.sumocfg`."""
    if os.path.isdir(src):
        carla = os.path.join(src, "carla")
        sumo = os.path.join(src, "sumo")
        if os.path.isdir(carla):
            return carla, (sumo if os.path.isdir(sumo) else None)
        # Handed something INSIDE carla/ - what the picker returns for an
        # already-extracted bundle, where the user clicks the .xodr rather
        # than the .zip. Recover the sibling sumo/ instead of dropping it;
        # carla_src stays `src`, so staging and naming are unchanged.
        return src, _bundle_sumo_beside(src)
    if os.path.isfile(src) and src.lower().endswith(".zip"):
        with zipfile.ZipFile(src) as z:
            if not _looks_like_bundle(z.namelist()):
                return src, None
            unpacked = _map_cache_dir(cache_name) if cache_name else os.path.join(
                os.path.dirname(os.path.abspath(src)),
                os.path.splitext(os.path.basename(src))[0] + "_unpacked")
            # A client-mode machine has no local CARLA, so it never imports: the
            # bundle's carla/ half is written once and read never (368 MB of
            # roosevelt_full's 370). Skip it. Nothing to record about the omission -
            # a machine that later gains a CARLA finds no cooked map, so its
            # preflight re-downloads and extracts the bundle in full.
            sumo_only = _mode() == "client"
            # Freshness is judged against a directory we ACTUALLY extract: sentinel
            # on carla/ while extracting sumo-only would be permanently absent, so
            # every run would re-extract. `unpacked` itself cannot serve, since in a
            # per-map cache it also holds the downloaded zip. Clear only the bundle's
            # own subdirs, so that sibling zip survives - and props/ is in the list
            # because a prop left behind by an older bundle would otherwise be
            # installed forever (FIXS#223).
            landmark = os.path.join(unpacked, "sumo" if sumo_only else "carla")
            if not os.path.isdir(landmark) or \
                    os.path.getmtime(src) > os.path.getmtime(landmark):
                for sub in ("carla", "sumo", "props"):
                    shutil.rmtree(os.path.join(unpacked, sub), ignore_errors=True)
                os.makedirs(unpacked, exist_ok=True)
                z.extractall(unpacked, members=_bundle_members(z, sumo_only))
                print(f"[import] unpacked {'sumo half of ' if sumo_only else ''}"
                      f"bundle -> {unpacked}")
        carla = os.path.join(unpacked, "carla")
        sumo = os.path.join(unpacked, "sumo")
        if os.path.isdir(carla):
            carla_src = carla
        else:
            # None rather than `unpacked` when the CARLA half was skipped on purpose:
            # handing back a directory that does not contain a CARLA package would
            # send an importer off to fail on it. Absent by choice is not the same as
            # a legacy layout.
            carla_src = None if sumo_only else unpacked
        return carla_src, (sumo if os.path.isdir(sumo) else None)
    return src, None


def bundle_sumocfg(sumo_dir):
    """The single `.sumocfg` inside a bundle's `sumo/` dir, or None if there is no
    `sumo/`. Exits if the dir holds more than one (ambiguous - caller should pass
    an explicit --sumocfg)."""
    if not sumo_dir or not os.path.isdir(sumo_dir):
        return None
    cfgs = sorted(f for f in os.listdir(sumo_dir) if f.lower().endswith(".sumocfg"))
    if not cfgs:
        return None
    if len(cfgs) > 1:
        sys.exit(f"[import] {sumo_dir} has {len(cfgs)} .sumocfg files {cfgs}; "
                 f"pass --sumocfg to choose one.")
    return os.path.join(sumo_dir, cfgs[0])


def map_name_in(carla_src, descriptor_only=False):
    """The real map/package name a staged CARLA source describes - the stem of its
    lone `<name>.json` descriptor, else its lone `<name>.xodr`. This is the name
    CARLA actually cooks/loads, which need NOT equal a release/location tag (e.g.
    the `roosevelt` bundle's carla/ describes `Roosevelt_07142026`). None if it
    cannot be told unambiguously (0 or >1 candidates).

    `descriptor_only` drops the .xodr fallback, for callers that must distinguish
    "this bundle DECLARES its package name, and it is not ours to change" from
    "there is only an export here, so the name is still open". Those are different
    answers and the fallback conflated them: a raw export always yields its .xodr
    stem, which then overrode a name the user had just been asked for."""
    if not carla_src or not os.path.isdir(carla_src):
        return None

    def stems(ext):
        found = []
        for root, _dirs, files in os.walk(carla_src):
            for f in files:
                if f.lower().endswith(ext):
                    found.append(f[:-len(ext)])
        return found

    jsons = [j for j in stems(".json") if j.lower() != "roadpainter_decals"]
    if len(jsons) == 1:
        return jsons[0]
    if descriptor_only:
        return None
    xodrs = list(set(stems(".xodr")))
    if len(xodrs) == 1:
        return xodrs[0]
    return None


def _dir_with_sumocfg(root):
    """The directory under `root` that directly holds a .sumocfg (e.g. a lone
    'SUMO files/' wrapper inside a scenario zip), or None."""
    if not root or not os.path.isdir(root):
        return None
    for cur, _dirs, files in os.walk(root):
        if any(f.lower().endswith(".sumocfg") for f in files):
            return cur
    return None


def _sumo_scenario_dir(src, cache_name=None):
    """If `src` (a .zip or folder) is a SUMO-only scenario - a .sumocfg present,
    no CARLA asset (.xodr/.fbx) - return the dir holding the .sumocfg. A zip is
    unpacked once into the map's folder ~/.fixs/maps/<cache_name>/sumo/ (so a
    separately-picked scenario lands under its map, cooked-name), else a sibling
    `<stem>_unpacked/`. Else None. The sumo half of classify_source."""
    if os.path.isfile(src) and src.lower().endswith(".zip"):
        with zipfile.ZipFile(src) as z:
            names = [n.lower() for n in z.namelist()]
            if not any(n.endswith(".sumocfg") for n in names):
                return None
            if any(n.endswith((".xodr", ".fbx")) for n in names):
                return None  # carries CARLA geometry -> not sumo-only
            unpacked = os.path.join(_map_cache_dir(cache_name), "sumo") if cache_name else \
                os.path.join(os.path.dirname(os.path.abspath(src)),
                             os.path.splitext(os.path.basename(src))[0] + "_unpacked")
            if not os.path.isdir(unpacked) or os.path.getmtime(src) > os.path.getmtime(unpacked):
                shutil.rmtree(unpacked, ignore_errors=True)
                os.makedirs(unpacked, exist_ok=True)
                z.extractall(unpacked)
        return _dir_with_sumocfg(unpacked)
    if os.path.isdir(src):
        has_cfg = has_carla = False
        for _cur, _dirs, files in os.walk(src):
            for f in files:
                fl = f.lower()
                has_cfg = has_cfg or fl.endswith(".sumocfg")
                has_carla = has_carla or fl.endswith((".xodr", ".fbx"))
        if has_cfg and not has_carla:
            src_dir = _dir_with_sumocfg(src)
            # Cache a folder pick under the map (cooked-name), like the zip branch
            # above, so a later Local (already-imported) pick of this map reuses it
            # without a re-prompt. Skip the copy if it is already the cached dir.
            if cache_name and src_dir:
                dest = os.path.join(_map_cache_dir(cache_name), "sumo")
                if os.path.abspath(src_dir) != os.path.abspath(dest):
                    shutil.rmtree(dest, ignore_errors=True)
                    shutil.copytree(src_dir, dest)
                return _dir_with_sumocfg(dest)
            return src_dir
    return None


def classify_source(src, cache_name=None):
    """What a map source provides, as (carla_src, sumo_src):
      - bundle (carla/ + sumo/) -> (carla dir, sumo dir)
      - carla-only pkg/export   -> (carla src, None)
      - sumo-only scenario      -> (None, sumo dir)
    A source fills the CARLA slot, the SUMO slot, or both; run_cosim fills any slot
    a pick leaves empty. Zips are unpacked (into ~/.fixs/maps/<cache_name>/ when the
    cooked map name is known) as needed."""
    carla_src, sumo_dir = open_bundle(src, cache_name)   # bundle split, else (src, None)
    if sumo_dir is not None:
        return carla_src, sumo_dir                  # bundle: both slots
    sdir = _sumo_scenario_dir(src, cache_name)
    if sdir is not None:
        return None, sdir                           # sumo-only
    return carla_src, None                          # carla-only


def _bundle_sumo_beside(src):
    """The sumo/ half of the bundle `src` sits inside, or None.

    Ascends, because the file picker returns the folder holding the clicked
    .xodr/.fbx and its depth inside carla/ varies across the library (atlanta ships
    carla/<name>.xodr, roosevelt and mlk ship carla/<name>/<name>.xodr). Bounded,
    and the ascent must pass through the bundle's own carla/, so an export sitting
    beside an unrelated sumo/ is never adopted."""
    if not src or not os.path.isdir(src):
        return None
    cur = os.path.normpath(src)
    for _ in range(4):
        parent = os.path.dirname(cur)
        if not parent or parent == cur:
            return None
        if os.path.basename(cur).lower() == "carla" and \
                os.path.isdir(os.path.join(parent, "sumo")):
            return os.path.join(parent, "sumo")
        cur = parent
    return None


def local_pick_carries_sumo(path):
    """Does a locally-picked map source already come with a SUMO scenario?

    Read-only and cheap - a zip is listed, a folder walked, nothing extracted or
    copied - which is what lets the run questionnaire ask it while it is still only
    making decisions. classify_source answers the same question but EXTRACTS a
    bundle to do it, so it stays after the decisions rather than inside them.

    False for a raw RoadRunner export and for a precooked *_cooked.tar.gz: the two
    picks that leave run_cosim's SUMO slot empty."""
    if not path:
        return False
    if os.path.isfile(path) and path.lower().endswith(".zip"):
        try:
            with zipfile.ZipFile(path) as z:
                return any(n.lower().endswith(".sumocfg") for n in z.namelist())
        except (OSError, zipfile.BadZipFile):
            return False               # unreadable: let the late chain report it
    if os.path.isdir(path):
        return (_dir_with_sumocfg(path) is not None
                or _bundle_sumo_beside(path) is not None)
    return False


def fetch_catalog(repo):
    """The DT-Library catalog (list of map entries), fetched fresh via gh from
    `repo`'s catalog.json and cached at ~/.fixs/catalog.json. Falls back to the
    cache when offline / gh is unavailable; [] if neither works. Read every run so
    the map list, real names, and per-map settings stay current."""
    import json
    cache = os.path.join(os.path.dirname(env.CONFIG_PATH), "catalog.json")
    gh = shutil.which("gh")
    if gh:
        try:
            out = subprocess.run(
                [gh, "api", f"repos/{repo}/contents/catalog.json",
                 "-H", "Accept: application/vnd.github.raw+json"],
                capture_output=True, text=True, timeout=15)
            if out.returncode == 0 and out.stdout.strip():
                maps = json.loads(out.stdout).get("maps", [])
                try:
                    with open(cache, "w", encoding="utf-8") as f:
                        f.write(out.stdout)
                except OSError:
                    pass
                return maps
        except Exception:
            pass
    if os.path.isfile(cache):
        try:
            with open(cache, encoding="utf-8") as f:
                return json.load(f).get("maps", [])
        except (OSError, ValueError):
            pass
    return []


def catalog_entry(catalog, name):
    """The catalog entry known by `name`, or None.

    Matches any of the three names one entry answers to: its `location` (the
    picker's Online label, and what --map takes), its `map_name` (the REAL cooked
    CARLA name, which is what a Local pick and resolve_cooked_map deal in), and
    its `release` tag. These deliberately differ - location 'roosevelt' vs
    map_name 'roosevelt_full' - so matching `location` alone silently dropped the
    entry whenever the caller held a cooked name. That made one map resolve two
    different ways depending on which picker entry you came in through: picking
    'roosevelt' applied the entry's settings (net_offset: zero), while picking
    'roosevelt_full' matched nothing, fell back to defaults, and left the CARLA
    spectator framed on a sign-flipped y."""
    if not name:
        return None
    for m in catalog or []:
        if name in (m.get("location"), m.get("map_name"), m.get("release")):
            return m
    return None


def run_import(carla_root, ue4_root, name):
    """Run CARLA's Import.py to cook the staged package. Returns its exit code."""
    import_py = os.path.join(carla_root, "Util", "BuildTools", "Import.py")
    if not os.path.isfile(import_py):
        sys.exit(f"[import] {import_py} not found - is {carla_root} a CARLA source build?")
    proc_env = dict(os.environ)
    if ue4_root:
        proc_env["UE4_ROOT"] = ue4_root
    if not proc_env.get("UE4_ROOT"):
        print("[import] WARNING: UE4_ROOT not set; the cook commandlet may fail.")
    # #311 again, on the one editor launch FIXS does not build the argv for.
    # env.EDITOR_LAUNCH_FLAGS carries -DisableFrameTraceCapture onto every UE4Editor
    # FIXS starts itself (run_cosim, place_tls, place_signs), but the cook goes
    # through CARLA's Util/BuildTools/Import.py, which assembles its own commandlet
    # command line - so the flag never reached it and the RenderDoc "Locate main
    # RenderDoc executable..." dialog still blocked the cook. UE4 appends whatever
    # UE-CmdLineArgs holds to any command line it parses (Engine/Source/Runtime/Core,
    # LogSuppressionInterface.cpp:634), which is how a flag gets into an argv owned
    # by someone else without patching their script.
    extra = " ".join(env.EDITOR_LAUNCH_FLAGS)
    proc_env["UE-CmdLineArgs"] = (
        f"{proc_env['UE-CmdLineArgs']} {extra}" if proc_env.get("UE-CmdLineArgs") else extra)
    cmd = [sys.executable, import_py, f"--package={name}"]
    print(f"[import] running: {' '.join(cmd)}  (cwd={carla_root})")
    print("[import] cooking the map can take several minutes ...")
    # CARLA's Import.py cooks EVERY *.json under Import/ (its --package flag does
    # not filter), so a leftover package from an earlier import cross-contaminates
    # this cook - and if it names an already-cooked map, the whole run aborts.
    # Isolate: stash the other descriptors (+ their asset folders) for the cook,
    # restore them after.
    restore = _isolate_import(os.path.join(carla_root, "Import"), name)
    try:
        return subprocess.call(cmd, cwd=carla_root, env=proc_env)
    finally:
        restore()


def _stash_dir(import_dir):
    """Where set-aside packages wait out a cook: beside Import/, never in TEMP.

    A sibling of Import/ rather than a child, because CARLA's Import.py cooks what
    it finds under Import/ and a stash living there could be swept into the very
    cook it is being hidden from. Beside it is invisible to that scan, sits next to
    the data it belongs to, and - unlike TEMP - is not something Windows deletes on
    its own schedule."""
    return os.path.join(os.path.dirname(os.path.abspath(import_dir)),
                        ".fixs-import-stash")


def _restore_stash(import_dir, stash, names=None):
    """Move `names` (default: everything) back from `stash` into `import_dir`."""
    if not os.path.isdir(stash):
        return []
    back = []
    for n in (names if names is not None else sorted(os.listdir(stash))):
        src, dst = os.path.join(stash, n), os.path.join(import_dir, n)
        if os.path.exists(src) and not os.path.exists(dst):
            shutil.move(src, dst)
            back.append(n)
    if not os.listdir(stash):
        os.rmdir(stash)
    return back


def _isolate_import(import_dir, keep):
    """Temporarily move every package under `import_dir` except `keep` aside, so
    CARLA's Import.py cooks only `keep`. Returns a restore() to move them back
    (call it in a finally). `keep`'s own descriptor + asset folder and the shared
    roadpainter_decals.json stay put.

    Recovers first. restore() runs in a finally, which covers an exception but not
    a killed process - and a cook is long, so it is exactly the thing people kill.
    That used to strand every other package in a TEMP directory nobody would think
    to look in: 1.3 GB of maps, sitting where Windows cleans up on its own
    schedule, with Import/ simply looking like the maps had been deleted. Now the
    stash is somewhere findable and the next import puts it back on its own."""
    if not os.path.isdir(import_dir):
        return lambda: None
    stash = _stash_dir(import_dir)
    recovered = _restore_stash(import_dir, stash)
    if recovered:
        print(f"[import] a previous cook did not finish; restored "
              f"{len(recovered)} set-aside Import/ item(s) first.")
    os.makedirs(stash, exist_ok=True)
    moved = []
    for f in sorted(os.listdir(import_dir)):
        if not f.lower().endswith(".json") or f.lower() == "roadpainter_decals.json":
            continue
        base = f[:-len(".json")]
        if base == keep:
            continue
        shutil.move(os.path.join(import_dir, f), os.path.join(stash, f))
        moved.append(f)
        folder = os.path.join(import_dir, base)
        if os.path.isdir(folder):
            shutil.move(folder, os.path.join(stash, base))
            moved.append(base)
    if moved:
        print(f"[import] isolating '{keep}' for the cook (set aside {len(moved)} "
              f"other Import/ item(s), restored after)")

    def restore():
        _restore_stash(import_dir, stash, moved)
        # Only if empty: rmtree would delete anything an earlier crash left that
        # this cook did not set aside itself, which is the opposite of the point.
        if os.path.isdir(stash) and not os.listdir(stash):
            os.rmdir(stash)
    return restore


def _resolve_carla(carla_root=None, ue4_root=None, need_source=True):
    """Resolve (carla_root, ue4_root) from the saved env config, running first-time
    setup if nothing is configured yet. Exits with a clear message if no CARLA is
    configured. Explicit args win over the saved config.

    `need_source` is the operation's requirement, not a property of the machine:
    COOKING an fbx/xodr runs the Unreal editor and genuinely needs a source build,
    but INSTALLING an already-cooked package (install_cooked) is a file copy and
    works on either flavour. The two were conflated here, which is why a packaged
    CARLA could not consume a precooked map at all."""
    cfg = env.load_config()
    if cfg is None and not carla_root:
        # first use on a fresh clone: configure the CARLA env, just like run_cosim
        print("[import] no CARLA env configured; running first-time setup ...")
        cfg = env.run_setup()
        # ... and continue under the interpreter setup just bound. main() already
        # re-exec'd on the config as it stood on entry; there WAS no config then, so
        # this is the first moment the answer exists - and run_import below hands
        # sys.executable to CARLA's Import.py.
        env.reexec_under_configured(__file__, cfg, tag="import")
    cfg = cfg or {}
    carla_root = carla_root or cfg.get("carla_root")
    ue4_root = ue4_root or cfg.get("ue4_root")
    if not carla_root:
        if cfg.get("mode") == "client":
            sys.exit("[import] this machine is configured as a CARLA CLIENT - CARLA runs "
                     "on another host, so there is no local install to import into.\n"
                     "        Import the map on that host, or re-run setup_carla here and "
                     "pick a local packaged or source build.")
        sys.exit("[import] no CARLA configured - run setup_carla first.")
    if need_source and cfg.get("mode") == "packaged":
        sys.exit("[import] the configured CARLA is PACKAGED; cooking a custom map from "
                 "fbx/xodr needs a SOURCE build (run setup_carla and pick source).\n"
                 "        A map published with a precooked asset installs here without "
                 "one - run it through run_cosim, which picks that path automatically.")
    return carla_root, ue4_root


def ensure_map(name, carla_root=None, ue4_root=None, package_url=None,
               package_dir=None, force=False, prompt_if_exists=False,
               package_pick=False, source_sha=None):
    """Make sure `name` is imported into the (source-build) CARLA, importing it
    if needed. Returns 0 on success; exits with a clear message otherwise.

    If the map already exists: `force` re-imports unconditionally; otherwise, when
    `prompt_if_exists` and the session is interactive, the user is asked whether to
    re-import (re-download + re-cook) - handy for updating a map or testing.

    `source_sha` is the digest of the release asset being cooked; it is stamped on
    the result so a later run can tell which bundle this map came from."""
    carla_root, ue4_root = _resolve_carla(carla_root, ue4_root)

    umap = cooked_map_path(carla_root, name)
    already = os.path.isfile(umap)
    if already and not force:
        print(f"[import] '{name}' already imported: {umap}")
        if not (prompt_if_exists and sys.stdin.isatty()):
            return 0
        ans = input("[import] re-import it now (re-download + re-cook)? [y/N]: ").strip().lower()
        if ans != "y":
            print("[import] keeping the existing map.")
            return 0
        print("[import] re-importing ...")

    stage_package(carla_root, name, package_url, package_dir, package_pick)

    # CARLA's ImportAssets commandlet imports cleanly into an empty destination
    # but often fails to *replace* existing cooked content (the .umap is left
    # untouched). So for a re-import we move the old content aside first and
    # import fresh; the backup is restored if the cook fails, so a working map is
    # never lost.
    content_dir = cooked_content_dir(carla_root, name)
    backup = content_dir + ".bak_reimport"
    if already and os.path.isdir(content_dir):
        shutil.rmtree(backup, ignore_errors=True)
        os.rename(content_dir, backup)
        print(f"[import] moved existing content aside for a clean re-import (restored on failure)")

    rc = run_import(carla_root, ue4_root, name)
    ok = os.path.isfile(umap)

    # An Unreal commandlet that dies takes Import.py's remaining steps with it
    # (invoke_commandlet uses check_call). Report it every time - the exit code is
    # otherwise swallowed by the .umap check below - and retry ONCE when the map
    # did not survive. Retrying is worth doing because the crash we see is not
    # deterministic: the same commandlet on the same package succeeds on a re-run.
    # It is deliberately not retried when the map IS there; that work succeeded,
    # and repeating a multi-minute cook to re-attempt one skipped step is a worse
    # trade than saying clearly what was skipped.
    if rc != 0:
        _report_import_failure(name, rc, ok,
                               capture_import_evidence(carla_root, name))
        if not ok:
            print(f"[import] retrying the import for '{name}' (attempt 2 of 2) ...")
            rc = run_import(carla_root, ue4_root, name)
            ok = os.path.isfile(umap)
            if rc != 0:
                _report_import_failure(name, rc, ok,
                                       capture_import_evidence(carla_root, name,
                                                               tag="_retry"))
            elif ok:
                print(f"[import] the retry succeeded; '{name}' imported.")

    if os.path.isdir(backup):
        if ok:
            shutil.rmtree(backup, ignore_errors=True)
        else:
            shutil.rmtree(content_dir, ignore_errors=True)
            os.rename(backup, content_dir)
            print("[import] import did not produce the map; restored the previous one.")

    if not ok:
        sys.exit(f"[import] import failed: Import.py exited {rc} and {umap} was not produced.")
    if rc != 0:
        print(f"[import] note: Import.py exited {rc} (CARLA's cook commonly flags non-fatal "
              f"warnings as errors), but the map .umap was written.")
    # After the restore-on-failure dance above, so the stamp only ever describes
    # content that actually got cooked - a failed re-import restores the previous
    # map together with its previous stamp.
    _write_sha(cooked_content_dir(carla_root, name), source_sha)
    print(f"[import] done: '{name}' imported -> {umap}")
    return 0


# ------------------------------------------------- precooked (packaged CARLA)

def _safe_members(tar, dest):
    """Yield `tar`'s members, refusing any that would write outside `dest`.

    tarfile.extractall follows absolute paths, `..` and symlinks straight out of
    the destination; we extract release assets, so the archive is only as
    trustworthy as whoever can publish one. Python 3.12's filter='data' does this,
    but the shipped env is 3.10 (environment.yml), so it is done here."""
    root = os.path.realpath(dest)
    for m in tar.getmembers():
        target = os.path.realpath(os.path.join(dest, m.name))
        if target != root and not target.startswith(root + os.sep):
            sys.exit(f"[import] refusing to extract '{m.name}': it escapes {dest}")
        if m.issym() or m.islnk():
            link = os.path.realpath(os.path.join(os.path.dirname(target), m.linkname))
            if not link.startswith(root + os.sep):
                sys.exit(f"[import] refusing link '{m.name}' -> '{m.linkname}': "
                         f"it escapes {dest}")
        yield m


def cooked_asset_name(asset):
    """The precooked asset that pairs with source bundle `asset`, by convention:
    `<stem>_cooked.tar.gz` (mlk_no_signal.zip -> mlk_no_signal_cooked.tar.gz).

    A fallback, not the truth. The catalog is meant to name the cooked asset
    outright (FIXS_Applications#27); until that field lands this derives it, and
    a derived name that is not actually published fails the same way a wrong
    catalog entry would - at the download, by name, before anything is cooked or
    launched."""
    if not asset:
        return None
    stem = asset[:-4] if asset.lower().endswith(".zip") else asset
    return stem + "_cooked.tar.gz"


def catalog_cooked_asset(entry):
    """The precooked asset name for a catalog entry, or None if that map has no
    precooked build published. `cooked_asset` when the catalog says so; else the
    conventional name derived from the source bundle."""
    if not entry:
        return None
    named = entry.get("cooked_asset")
    if named:
        return named
    # An explicit false/empty cooked_asset is a deliberate "this map has none";
    # only a MISSING key falls through to the convention.
    if "cooked_asset" in entry:
        return None
    return cooked_asset_name(entry.get("asset"))


def install_precooked(carla_root, name, repo=None, tag=None, entry=None,
                      local=None, force=False, log="import"):
    """Resolve and install an ALREADY-cooked map package into a PACKAGED CARLA.
    Returns the map name the package actually provides (may differ from `name`).

    This is the packaged flavour's ENTIRE import path. A packaged build cannot cook
    - cooking runs the Unreal editor it does not ship - so the map has to arrive
    cooked, and putting it in place is a plain extract (install_cooked). What sits
    in front of that extract is the part worth sharing: which asset to fetch, and
    the three cases where the honest answer is "not from here".

    Shared by run_cosim's packaged preflight and import_map's picker. It lived only
    in run_cosim, which is why `--import-map` - the command whose whole name is
    importing a map - could not import one on a packaged build (#276).

    `local` is a path the user picked; only a precooked package is usable, and it is
    recognised by what is inside it rather than by its name. Else the asset name
    comes from the catalog `entry` and is downloaded from `repo`/`tag`.
    `log` is the caller's message prefix, so the user reads one voice."""
    # Ask the archive, not the file name. The suffix was standing in for "is a
    # precooked package", and the two disagree at the edges: a `.tgz`, or a tarball
    # renamed on the way through a browser, is perfectly installable and was
    # refused as a "source bundle", while any `.tar.gz` at all was accepted and
    # failed later inside install_cooked. cooked_name_in_tar is the same evidence
    # install_cooked itself uses, so the two can no longer disagree.
    if local and cooked_name_in_tar(local):
        tar_path = local
    elif local:
        # A source bundle (carla/ with fbx+xodr) would have to be cooked, which is
        # the one thing this flavour cannot do.
        sys.exit(f"[{log}] '{local}' is a source bundle and this CARLA is PACKAGED, "
                 f"which cannot cook one. Point at the map's precooked "
                 f"*_cooked.tar.gz, or configure a source build (run_cosim --setup).")
    elif not tag:
        sys.exit(f"[{log}] map '{name}' is not installed in {carla_root} and is not a "
                 f"Digital-Twin-Library map, so there is no precooked package to "
                 f"install. Pick a library map, or point --package-dir at a precooked "
                 f"*_cooked.tar.gz.")
    else:
        asset = catalog_cooked_asset(entry)
        # Say "not published" by NAME, before downloading anything. The alternative
        # is a gh pattern-miss the user has to decode - and for a map whose release
        # genuinely has no cooked asset (atlanta, at the time of writing) that is the
        # expected path, not the exceptional one.
        published = release_assets(repo, tag)
        if asset is None or (published is not None and asset not in published):
            have = ", ".join(published) if published else "unknown"
            sys.exit(
                f"[{log}] no precooked package published for '{name}' "
                f"(release '{tag}' of {repo}).\n"
                f"        A PACKAGED CARLA can only run maps that ship one.\n"
                f"        expected asset: {asset or '(none named in the catalog)'}\n"
                f"        published:      {have}\n"
                f"        Use a source build (run_cosim --setup) to cook this map "
                f"yourself, or ask for a precooked build of it.")
        tar_path = download_cooked_tar(repo, tag, asset, force_redownload=force,
                                       cache_name=name)

    real = install_cooked(carla_root, tar_path, force=force)
    if real != name:
        print(f"[{log}] precooked package provides map '{real}'")
    # Same stamp as the source path, from the .tar.gz asset instead of the .zip:
    # a packaged install goes stale exactly the same way a cook does. A local
    # package leaves it unrecorded, which is the "not checked" state.
    _write_sha(cooked_content_dir(carla_root, real, mode="packaged"),
               _read_sha(os.path.dirname(tar_path)))
    return real


def install_cooked(carla_root, tar_path, name=None, force=False):
    """Install a precooked map package into a PACKAGED CARLA. Returns the map name.

    CARLA's own Util/ImportAssets.sh is exactly

        for f in `find Import/ -name "*.tar.gz"`; do tar --keep-newer-files -xvf $f; done

    run from the package root - a plain extract, no manifest, no commandlet, no
    registration step. That is worth stating because it is the whole basis for
    doing it here in `tarfile` instead: ImportAssets.sh is bash-only and CARLA
    ships no .bat counterpart, so shelling out would leave Windows with no
    packaged path at all. Reimplementing the extract gives both OSes the same one.

    We deliberately do NOT copy `--keep-newer-files`. That flag exists because the
    script re-extracts every tarball sitting in Import/ on each run and must not
    clobber newer local edits; we extract one named asset on purpose, and an
    install that silently skipped files because of mtimes would be a confusing
    half-map. `force` instead removes the old Content/<name> first, mirroring
    ensure_map's clean-reimport."""
    dest = package_root(carla_root)
    if not os.path.isdir(dest):
        sys.exit(f"[import] packaged CARLA root not found: {dest}")

    with tarfile.open(tar_path, "r:*") as tar:
        members = list(_safe_members(tar, dest))
        if name is None:
            name = _cooked_name_in(members)
            if name is None:
                sys.exit(f"[import] cannot tell which map {tar_path} provides "
                         f"(expected CarlaUE4/Content/<name>/Config/<name>.Package.json); "
                         f"pass the name explicitly.")
        content_dir = cooked_content_dir(carla_root, name, mode="packaged")
        if os.path.isdir(content_dir):
            if not force:
                print(f"[import] '{name}' already installed: {content_dir}")
                return name
            print(f"[import] removing existing content for a clean re-install: {content_dir}")
            shutil.rmtree(content_dir, ignore_errors=True)
        print(f"[import] installing precooked '{name}' -> {dest}")
        tar.extractall(dest, members=members)

    umap = cooked_map_path(carla_root, name, mode="packaged")
    if not os.path.isfile(umap):
        sys.exit(f"[import] install finished but {umap} is missing - "
                 f"{tar_path} does not look like a precooked package for '{name}'.")
    print(f"[import] done: '{name}' installed -> {umap}")
    return name


_SHADER_MARKERS = {"d3d": b"DXBC", "vulkan": b"GLSL.std.450"}


def shader_platforms_in(content_dir, size_cap=2 * 1024 * 1024):
    """Which shader platforms a cooked package carries shaders for - a subset of
    {'d3d', 'vulkan'}, empty if none is recognised.

    UE4 compiles a material's shaders per shader platform and bakes the result
    into the cooked asset, so a Linux cook ships SPIR-V and a Windows one ships
    D3D bytecode; CARLA's own Windows package ships BOTH. A packaged build has no
    shader compiler, so a material carrying no map for the running platform
    silently falls back to the default material - the level loads, geometry and
    placed actors are correct, and the road surface renders as grey checkerboard.
    Nothing downstream can detect that, which is why it is detected here.

    Every *_cooked.tar.gz the Digital-Twin-Library publishes today is a Linux cook
    (FIXS_Applications#29), so on Windows this fires.

    A heuristic by necessity: this pattern-matches container magic rather than
    parsing UE4 packages, so it is used to WARN, never to refuse. Only small .uexp
    are read - shader maps live in materials (~80KB), never in the multi-MB .umap
    or mesh blobs - which keeps a full scan to a few hundred KB."""
    found = set()
    for root, _dirs, files in os.walk(content_dir):
        for f in files:
            if not f.endswith(".uexp"):
                continue
            path = os.path.join(root, f)
            try:
                if os.path.getsize(path) > size_cap:
                    continue
                with open(path, "rb") as fh:
                    blob = fh.read()
            except OSError:
                continue
            found.update(k for k, marker in _SHADER_MARKERS.items() if marker in blob)
            if len(found) == len(_SHADER_MARKERS):
                return found          # nothing more to learn
    return found


def host_shader_platform():
    """The shader platform a packaged CARLA uses here by default: D3D on Windows,
    Vulkan elsewhere. Windows CarlaUE4.exe also accepts -vulkan, so a 'vulkan'-only
    package is not necessarily unusable there - only unusable as launched."""
    return "d3d" if platform.system() == "Windows" else "vulkan"


def _cooked_name_in(members):
    """The map name a precooked archive describes - the stem of its lone
    CarlaUE4/Content/<name>/Config/<name>.Package.json. None if 0 or >1, for the
    same reason map_name_in gives up: a name we hand to load_world must be
    resolved from the package, never guessed from the file name."""
    found = set()
    for m in members:
        parts = m.name.replace("\\", "/").split("/")
        if len(parts) >= 5 and parts[-2] == "Config" and parts[-1].endswith(".Package.json"):
            found.add(parts[-1][:-len(".Package.json")])
    return found.pop() if len(found) == 1 else None


def cooked_name_in_tar(path):
    """The map a precooked *.tar.gz provides, read out of the archive itself, or
    None if it is not one / does not say.

    The file name is not the answer: `mlk_no_signal_cooked.tar.gz` happens to carry
    `mlk_no_signal`, but that is a publishing convention, not a rule - which is why
    install_cooked resolves the name from the .Package.json inside. Peeking here
    lets a hand-picked tarball be named before it is installed, so the picker can
    say which map it selected instead of asking the user to guess a name that the
    install would then override anyway."""
    try:
        with tarfile.open(path, "r:*") as tar:
            return _cooked_name_in(tar.getmembers())
    except (OSError, tarfile.TarError):
        return None


def read_map_config(path):
    """Parse a simple `key=value` map config (keys: package, url). Lets an app
    declare its map in one text file instead of hard-coding the URL in a wrapper."""
    cfg = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, val = line.split("=", 1)
            cfg[key.strip().lower()] = val.strip()
    return cfg


GH_HOWTO = ("Install gh (https://cli.github.com) and run `gh auth login`; if it is "
            "already\n        installed, `gh auth status` shows which account and "
            "scopes it is using -\n        a token without `repo` cannot see a "
            "private library and reports it as 404.")


def _require_gh():
    """Path to the GitHub CLI, or exit saying how to proceed without it.

    Only DOWNLOADING needs this. Listing degrades instead (see list_map_releases):
    a machine that cannot reach the library must still be able to import a package
    the user fetched by hand, and that path needs no gh at all."""
    gh = shutil.which("gh")
    if not gh:
        sys.exit(f"[import] downloading a published map needs the GitHub CLI (gh).\n"
                 f"        {GH_HOWTO}\n"
                 f"        Or import a copy you downloaded yourself: --package <name> "
                 f"--package-dir <file>.")
    return gh


def _package_from_tag(tag, tag_prefix=""):
    """Package name = release tag with its prefix stripped. Convention:
    tag `map-<pkg>` <-> asset `<pkg>_carla_import.zip` <-> descriptor `<pkg>.json`."""
    return tag[len(tag_prefix):] if tag_prefix and tag.startswith(tag_prefix) else tag


def list_map_releases(repo, tag_prefix=""):
    """Map releases in `repo` (tag starts with `tag_prefix`), newest first, via gh.
    Each item: {tag, title, date 'YYYY-MM-DD', prerelease}. Drafts are skipped -
    their assets aren't downloadable.

    Returns [] - after saying why - when the releases cannot be listed: no gh, gh
    not authenticated, no network, or a token that cannot see a private library.
    This used to sys.exit, which made a gh problem fatal to the whole picker: the
    menu is printed by choose_map AFTER this call, so the `L) local file` row - the
    one path built to need nothing but browser access - was never reached on
    exactly the failure it exists for (#283). Not listing the library is a smaller
    fact than not being able to import, and only the first one is true here.
    fetch_catalog already degrades the same way, for the same reason."""
    import json
    gh = shutil.which("gh")
    if not gh:
        print(f"[import] the GitHub CLI (gh) is not installed, so the published maps "
              f"cannot be listed.\n        {GH_HOWTO}")
        return []
    try:
        out = subprocess.check_output(
            [gh, "release", "list", "-R", repo, "-L", "100", "--json",
             "tagName,name,publishedAt,isPrerelease,isDraft"],
            text=True, stderr=subprocess.PIPE)
    except (OSError, subprocess.CalledProcessError) as exc:
        detail = (getattr(exc, "stderr", "") or "").strip() or str(exc)
        print(f"[import] `gh release list` failed for {repo}, so the published maps "
              f"cannot be listed:\n{detail}\n        {GH_HOWTO}")
        return []
    rels = []
    for r in json.loads(out):
        tag = r.get("tagName", "")
        if r.get("isDraft") or (tag_prefix and not tag.startswith(tag_prefix)):
            continue
        rels.append({"tag": tag, "title": r.get("name") or "",
                     "date": (r.get("publishedAt") or "")[:10],
                     "prerelease": bool(r.get("isPrerelease"))})
    rels.sort(key=lambda r: r["date"], reverse=True)
    return rels


def _infer_package_name(path):
    """Best-effort map/package name from a downloaded zip or folder: the stem of
    the <name>.json descriptor it contains, else - for a raw RoadRunner export,
    which ships no descriptor - the stem of its lone <name>.xodr. Returns None if
    neither is unambiguous. (Handles Windows-built zips with backslash entries.)

    The .xodr fallback is what map_name_in() already does for a staged bundle;
    without it the picker had to ask for "the name matching <name>.json" on a
    package that has no .json, and any answer but the export's own name then
    failed downstream."""
    entries = []
    if os.path.isfile(path) and path.lower().endswith(".zip"):
        with zipfile.ZipFile(path) as z:
            entries = z.namelist()
    elif os.path.isdir(path):
        entries = [os.path.join(r, f) for r, _d, fs in os.walk(path) for f in fs]
    bases = [os.path.basename(e.replace("\\", "/").rstrip("/")) for e in entries]
    for base in bases:
        low = base.lower()
        if low.endswith(".json") and low != "roadpainter_decals.json":
            return base[:-5]
    xodrs = {b[:-5] for b in bases if b.lower().endswith(".xodr")}
    return xodrs.pop() if len(xodrs) == 1 else None


def _prompt(msg):
    """input() that converts a non-interactive / closed stdin (EOF) into a clean
    exit instead of an EOFError traceback. isatty() is unreliable through some
    Windows shells, so we guard the call itself rather than trust isatty alone."""
    try:
        return input(msg)
    except EOFError:
        sys.exit("[import] non-interactive session (no input); pass an explicit "
                 "--map / --package to run without prompting.")


def _app_map_choice(name, catalog, cooked):
    """How an app-declared map name resolves, as (kind, label, flag, tag), or None
    if it resolves to nothing:
      'release' -> the name matches a Digital-Twin-Library entry (by location,
                   cooked map_name or release tag - see catalog_entry)
      'cooked'  -> not in the library, but already cooked into this CARLA
    A name that is neither costs the app its shortcut and nothing else: the picker
    then behaves exactly as it would with no application selected. That is
    deliberate - a map that has not been published yet must not be able to block a
    run, and an app manifest is data, so a typo in it should not be fatal."""
    ent = catalog_entry(catalog, name)
    if ent:
        flag = "  (Digital-Twin-Library)"
        if (ent.get("map_name") or "") in cooked:
            flag += "  (already imported)"
        return "release", (ent.get("map_name") or name), flag, ent.get("release")
    if name in cooked:
        return "cooked", name, "  (already imported)", None
    return None


def library_bundles(releases, catalog, tag_prefix=""):
    """The source bundles `releases` publish, one row per bundle.

    A release can carry more than one source bundle - `mlk` holds the corridor
    and its U-turn variant - and the thing anyone picks between is the bundle,
    not the tag it happens to be hosted on. catalog.json already calls itself
    the index for the picker and already says which bundles exist and which .zip
    each one is, so that is what the menu is built from. Listing releases
    instead made a second bundle on a tag unreachable from the menu, and left
    `--map <tag>` resolving to whichever entry happened to come first.

    Each row: {name, label, asset, release}.
      name    what the caller returns and what --map takes (the `location`)
      label   the real cooked map name, so an Online row and its Local twin read
              as the same map ('roosevelt_full' in both) rather than as two
              unrelated things ('roosevelt' vs 'roosevelt_full')
      asset   the exact .zip to download - what stops a release carrying several
              from being a lottery, the same way catalog_cooked_asset does for
              the precooked tier
      release the release it is published on

    A release no catalog entry claims still gets one row, named from its tag: a
    bundle published before its catalog entry lands must not be invisible.

    Precooked packages are not rows here. They are .tar.gz published beside the
    source zips, they carry the same map, and which one a packaged CARLA needs is
    answered by catalog_cooked_asset at download time - not by a second menu
    line for the same map.
    """
    by_release = {}
    for m in catalog or []:
        by_release.setdefault(m.get("release"), []).append(m)
    rows = []
    for r in releases:
        pkg = _package_from_tag(r["tag"], tag_prefix)
        entries = by_release.get(r["tag"]) or by_release.get(pkg) or []
        if not entries:
            rows.append({"name": pkg, "label": pkg, "asset": None, "release": r})
            continue
        for ent in entries:
            name = ent.get("location") or pkg
            rows.append({"name": name,
                         "label": ent.get("map_name") or name,
                         "asset": ent.get("asset"),
                         "release": r})
    return rows


def choose_map(repo, tag_prefix="", carla_root=None, catalog=None,
               preferred=None, app_label=None, current=None):
    """Interactive chooser. Two sections plus `L` for a file the user downloaded by
    hand - a .zip / folder on a source build, a precooked *_cooked.tar.gz on a
    packaged one, which is also the only route left when gh cannot reach the
    library (list_map_releases then returns [] and section 1 is simply empty):
      1. ONLINE - Digital-Twin-Library releases in `repo`. When an application is
                  selected and the library HAS its map(s), this section is narrowed
                  to those: an app pinned to Roosevelt should not be offered
                  Atlanta as if the two were interchangeable. If the app's map is
                  not in the library (not published yet, or cooked by hand), there
                  is nothing to narrow to and the full library is listed instead.
      2. LOCAL  - every map already cooked into `carla_root`. NEVER filtered: what
                  is cooked into this CARLA is a property of the machine, not of
                  the application, so an app can never hide a map you could
                  actually run. App maps are marked here, not promoted.
    To browse the whole library while a narrowing app is selected, pick "none" at
    the application prompt.

    Enter takes `current` - the map the setup is already running - wherever it
    landed; then the app's map, wherever IT landed (library or cooked); else item 1.
    A setup pinned to a map has to be able to open this row and back out of it
    unchanged, which "Enter = item 1" did not allow. The cooked copy wins the
    default over the library one: same map, but no re-import to reach it.

    Returns (name, tag, local_path):
      online release -> (name, tag,  None)
      local cooked   -> (name, None, None)   # already imported: run as-is
      local file     -> (name, None, path)
    Exits if the session is non-interactive."""
    releases = list_map_releases(repo, tag_prefix)
    cooked = list_imported_maps(carla_root) if carla_root else []
    preferred = preferred or []
    # Which artefact `L` may hand back is a property of the CARLA installed here,
    # not of the choice - a packaged build can only install a precooked tarball.
    # Resolved once, and used for both the menu wording and the dialog.
    packaged = _mode() == "packaged"

    resolved_app = [r for r in (_app_map_choice(n, catalog, cooked) for n in preferred) if r]
    app_tags = {tag for _k, _l, _f, tag in resolved_app if tag}
    app_names = {label for _k, label, _f, _t in resolved_app}
    bundles = library_bundles(releases, catalog, tag_prefix)
    # Narrow to the app's own maps by matching the BUNDLE, not its tag: two
    # bundles can share a release, so an app pinned to one of them must not be
    # offered the other as if the two were interchangeable.
    if app_tags:
        bundles = [b for b in bundles
                   if b["label"] in app_names or b["release"]["tag"] in app_tags]

    print("\n[import] Pick a map to run:")
    menu = []          # menu number -> ("bundle", bundle) | ("cooked", name)
    default_idx = 1    # menu number Enter selects; the app's map when there is one
    current_idx = 0    # ... unless the setup already runs one of these
    if bundles:
        who = f" for {app_label}" if app_tags and app_label else ""
        print(f"  Online (Digital-Twin-Library){who}:")
        for b in bundles:
            menu.append(("bundle", b))
            r, label = b["release"], b["label"]
            flag = "  (pre-release)" if r["prerelease"] else ""
            if label in cooked:
                flag += "  (already imported)"
            if current in (label, b["name"]):
                flag += "  (current)"
                current_idx = len(menu)
            print(f"   {len(menu):>2}) {label:<26} {r['date']}{flag}")
    if cooked:
        print("  Local (already imported into CARLA):")
        for name in cooked:
            menu.append(("cooked", name))
            mark = ""
            if name in app_names:
                mark = "  (app map)"
                # An app map the library does not carry still gets to be the
                # default - it just lives in this section instead of the one above.
                if not app_tags:
                    default_idx = len(menu)
            if name == current:
                mark += "  (current)"
                current_idx = len(menu)     # cooked beats the library copy
            print(f"   {len(menu):>2}) {name}{mark}")
    if not menu:
        print("   (no online bundles or imported maps found)")
    print("   L) select a local precooked *_cooked.tar.gz instead" if packaged
          else "   L) select a local .zip / folder instead")
    if current_idx:
        default_idx = current_idx

    if not sys.stdin.isatty():
        sys.exit("[import] non-interactive session: cannot prompt. Pass --map / --package "
                 "(+ --package-url/--package-dir) to choose non-interactively.")
    while True:
        hint = f"[1-{len(menu)} / L]" if menu else "[L]"
        default = f", Enter = {default_idx}" if menu else ""
        ans = _prompt(f"[import] Which? {hint}{default}: ").strip().lower()
        if ans == "" and menu:
            ans = str(default_idx)
        if ans == "l":
            path = _select_package("map", None, precooked=packaged)
            if packaged:
                # Nothing to ask: a precooked package carries its own name, and
                # install_cooked reads it from the same .Package.json this does. A
                # name typed here could only disagree with the archive, and the
                # archive would win - so state what was picked instead of asking.
                name = cooked_name_in_tar(path)
                if not name:
                    sys.exit(f"[import] {path} does not look like a precooked map "
                             f"package (no CarlaUE4/Content/<name>/Config/"
                             f"<name>.Package.json inside).\n"
                             f"        A PACKAGED CARLA can only install one of "
                             f"those - it ships no editor to cook a source bundle.")
                print(f"[import] that package provides map '{name}'")
                return name, None, path
            name = _infer_package_name(path)
            if _has_descriptor(path):
                # A packaged map is already named: its descriptor filename IS the
                # package CARLA cooks under, so it is not ours to rename here.
                if not name:
                    name = _prompt("[import] map name (matches <name>.json inside "
                                   "the package): ").strip()
            else:
                # A raw export offers only the author's working name
                # ('MLK_no_signal_0805'). Cooking under it would make the next
                # export a different map, so offer it as a default and let the
                # map be named something that outlives this export.
                hint = f" [Enter = {name}]" if name else ""
                name = _prompt(f"[import] cook this raw export as which map name?"
                               f"{hint}: ").strip() or name
            if not name:
                sys.exit("[import] no package name given; cannot import.")
            return name, None, path
        if ans.isdigit() and 1 <= int(ans) <= len(menu):
            kind, payload = menu[int(ans) - 1]
            if kind == "bundle":
                return payload["name"], payload["release"]["tag"], None
            return payload, None, None  # cooked: already imported, run as-is
        print("[import] invalid choice; enter a number, or L for a local file.")


def choose_sumo_source(cache_name=None):
    """Prompt for a SUMO scenario (a .zip or folder holding a .sumocfg) and return
    the dir that contains it, or None. Fills the SUMO slot separately when the
    chosen CARLA source ships no sumo/ - e.g. a carla-only local pick or a raw
    export. `cache_name` (the cooked map name) lands the extracted scenario under
    ~/.fixs/maps/<cache_name>/sumo/. Returns None in a non-interactive session so
    the caller can fail with a clear 'pass --sumocfg' message."""
    if not sys.stdin.isatty():
        return None
    print("\n[cosim] the chosen map has no SUMO scenario; select one now "
          "(a .zip or folder containing a .sumocfg).")
    # A scenario is known by its .sumocfg, not by a map's .xodr/.fbx - state it,
    # or the dialog filters this scenario's own files out and opens empty.
    path = _select_package("SUMO scenario", None, inside=("sumocfg",))
    _carla_src, sumo_dir = classify_source(path, cache_name)
    if sumo_dir is None:
        print(f"[cosim] no .sumocfg found in {path}")
    else:
        record_source(cache_name, "sumo", path)
    return sumo_dir


def list_imported_maps(carla_root, mode=None):
    """Names of maps already cooked under this CARLA (i.e. that have a <name>.umap).
    Used to let tools that operate on an existing map (e.g. place_tls) offer a
    local choice without needing gh / the network."""
    content = content_root(carla_root, mode)
    if not os.path.isdir(content):
        return []
    return [name for name in sorted(os.listdir(content))
            if os.path.isfile(cooked_map_path(carla_root, name, mode))]


def choose_imported_map(carla_root, mode=None, interactive=None):
    """Numbered menu of the maps already cooked into this CARLA; returns the chosen
    name. Auto-selects when there is exactly one; exits if there are none, or if a
    choice is needed but the session is non-interactive.

    `interactive` is the caller's answer to "may this stop and ask?"; isatty() is
    only the default. A --serve host has a terminal and no one at it."""
    maps = list_imported_maps(carla_root, mode)
    if not maps:
        sys.exit(f"[import] no cooked maps found under {carla_root}. Import one first "
                 "(e.g. import_<app>_map).")
    if len(maps) == 1:
        return maps[0]
    print("\n[import] Imported maps:")
    for i, m in enumerate(maps, 1):
        print(f"   {i}) {m}")
    if not (sys.stdin.isatty() if interactive is None else interactive):
        sys.exit("[import] non-interactive session: pass --map <name> to choose one.")
    while True:
        ans = _prompt(f"[import] Which map? [1-{len(maps)}]: ").strip()
        if ans.isdigit() and 1 <= int(ans) <= len(maps):
            return maps[int(ans) - 1]
        print("[import] invalid choice; enter a number from the list.")


def _map_cache_dir(name=None):
    """Local map cache: ~/.fixs/maps (next to carla.json), or $FIXS_MAP_CACHE. With
    `name`, the per-map subfolder ~/.fixs/maps/<name>/ - named by the cooked map
    name so it matches CARLA's Content/<name>/; it holds that map's bundle zip,
    extracted carla/ + sumo/, and generated tl_table.csv. Kept outside FIXS/ so
    `initialize` (which wipes FIXS/) never deletes it.

    Everything under here is RE-CREATABLE: downloaded, extracted, or derived from
    what was extracted. Nothing a user hand-edits lives here - scenario yamls are
    app-bounded and live under ~/.fixs/apps/ (see app_catalog.scenario_dir), so
    deleting this tree to reclaim disk can never destroy someone's config. It is
    also shared: one 700MB bundle serves every app that runs that map."""
    d = os.environ.get("FIXS_MAP_CACHE") or os.path.join(os.path.dirname(env.CONFIG_PATH), "maps")
    if name:
        d = os.path.join(d, name)
    os.makedirs(d, exist_ok=True)
    return d


def map_cache_dir(name):
    """The per-map cache dir ~/.fixs/maps/<name>/ - where this map's bundle was
    extracted, and so where its placement.yaml is if it ships one."""
    return _map_cache_dir(name)


def props_cache_dir():
    """Shared props cache ~/.fixs/props/ - ONE copy, for every map.

    Props install to a map-independent content path (/Game/FIXS/Props), so a copy
    shipped inside each map bundle would mean the last map imported silently decides
    which prop every other map places. Fetching one shared copy removes that.

    Sits beside maps/ rather than under it for the same reason, and is equally
    re-creatable: everything here is downloaded, never hand-edited."""
    d = os.environ.get("FIXS_PROPS_CACHE") or os.path.join(
        os.path.dirname(env.CONFIG_PATH), "props")
    os.makedirs(d, exist_ok=True)
    return d


def repo_file(repo, path, ref="main"):
    """Raw bytes of a file in `repo` at `ref`, or None if it cannot be fetched.

    The one place that knows HOW bytes come out of the map library.

    Note the media type: `application/vnd.github.raw`, NOT the `+json` variant
    fetch_catalog uses. On a binary payload the `+json` form fails with
    "transform: short source buffer" because gh tries to transform it as JSON, and
    the caller gets an empty file that then fails md5 verification for a reason
    that has nothing to do with the file. Output is captured as BYTES for the same
    reason - text=True would corrupt a .uasset.

    TODO(no-gh): Digital-Twin-Library is private today, so this needs gh's auth.
    Once it is public this becomes a plain stdlib GET of
        https://raw.githubusercontent.com/{repo}/{ref}/{path}
    with no external binary and no auth, and this function is the only thing that
    changes. FIXS's own repo is already public, so its release downloads can move
    first. Keep new callers going through here rather than shelling to gh directly.
    """
    gh = shutil.which("gh")
    if not gh:
        return None
    try:
        out = subprocess.run(
            [gh, "api", f"repos/{repo}/contents/{path}?ref={ref}",
             "-H", "Accept: application/vnd.github.raw"],
            capture_output=True, timeout=120)     # bytes: no text=True
        if out.returncode == 0 and out.stdout:
            return out.stdout
    except Exception:
        pass
    return None


def props_source(repo):
    """(subdir, ref) for the shared props, from the cached catalog's `props` block.

    Defaults to ("props", "main") so a catalog that predates the block still works.
    Keeping this in the catalog means the ref can be pinned to a tag or sha later
    without a schema change - the catalog IS the pin."""
    import json
    cache = os.path.join(os.path.dirname(env.CONFIG_PATH), "catalog.json")
    block = {}
    if os.path.isfile(cache):
        try:
            with open(cache, encoding="utf-8") as f:
                block = json.load(f).get("props") or {}
        except (OSError, ValueError):
            block = {}
    return block.get("path", "props"), block.get("ref", "main")


def fetch_props(repo, subdir=None, ref=None):
    """Fetch the map library's shared props into ~/.fixs/props. Returns that dir,
    or None if nothing could be fetched and nothing was cached.

    Self-describing, by design: placement.yaml declares install_root and the
    blueprints under it, so the asset list comes from the manifest itself
    (props.assets_declared) rather than a directory listing or a second list in the
    catalog. No listing is needed, which is what lets this move to
    raw.githubusercontent.com unchanged, and no second list means nothing to drift.

    Each .uasset is verified against provenance.json's md5 before it is kept, so a
    truncated or transformed download fails here rather than as a broken asset in
    the editor later. Falls back to whatever is already cached when offline."""
    import props as props_mod

    dest = props_cache_dir()
    want_subdir, want_ref = props_source(repo)
    subdir = subdir or want_subdir
    ref = ref or want_ref

    def cached_ok():
        return os.path.isfile(os.path.join(dest, props_mod.MANIFEST_NAME))

    manifest_bytes = repo_file(repo, f"{subdir}/{props_mod.MANIFEST_NAME}", ref)
    prov_bytes = repo_file(repo, f"{subdir}/provenance.json", ref)
    if not manifest_bytes or not prov_bytes:
        # Name the source. "could not reach" alone is a guess: an unreachable network
        # and a subdir that does not exist at that ref look identical from here, and
        # they need opposite fixes.
        where = f"{repo}:{subdir}@{ref}"
        if cached_ok():
            print(f"[props] no props at {where} (or the library is unreachable); "
                  f"using the cached props in {dest}.")
            return dest
        print(f"[props] no props at {where} (or the library is unreachable), "
              f"and nothing is cached.")
        return None

    with open(os.path.join(dest, props_mod.MANIFEST_NAME), "wb") as fh:
        fh.write(manifest_bytes)
    with open(os.path.join(dest, "provenance.json"), "wb") as fh:
        fh.write(prov_bytes)

    import json
    try:
        declared_md5 = (json.loads(prov_bytes).get("exported") or {}).get("md5")
    except ValueError:
        declared_md5 = None

    try:
        manifest = props_mod.load(os.path.join(dest, props_mod.MANIFEST_NAME))
        names = props_mod.assets_declared(manifest)
    except props_mod.ManifestError as exc:
        print(f"[props] fetched manifest is unreadable: {exc}")
        return dest if cached_ok() else None

    for name in names:
        target = os.path.join(dest, name + ".uasset")
        if os.path.isfile(target) and declared_md5 \
                and props_mod.md5_of(target) == declared_md5:
            continue                                  # already current
        blob = repo_file(repo, f"{subdir}/{name}.uasset", ref)
        if not blob:
            print(f"[props] could not fetch {name}.uasset from {repo}")
            if not os.path.isfile(target):
                return None
            continue
        with open(target, "wb") as fh:
            fh.write(blob)
        got = props_mod.md5_of(target)
        if declared_md5 and got != declared_md5:
            os.remove(target)
            print(f"[props] {name}.uasset failed verification "
                  f"({got} != {declared_md5}); discarded.")
            return None
        print(f"[props] fetched {name}.uasset ({len(blob)} bytes)")
    return dest


def map_sumo_dir(name):
    """The cached SUMO scenario dir for an already-imported map: the folder under
    ~/.fixs/maps/<name>/sumo that directly holds a .sumocfg (where a bundle's sumo/
    was extracted, or a separately-picked sumo landed), else None. Lets a Local
    (already-imported) pick reuse its sumo without re-prompting for one."""
    return _dir_with_sumocfg(os.path.join(_map_cache_dir(name), "sumo"))


def _cached_bundle_zip(cache_name):
    """The bundle zip sitting in ~/.fixs/maps/<name>/, or None.

    download_release_zip leaves it there so a re-import is free; it is also the only
    local record of what the library actually published for this map, which is what
    makes the parity check below possible without a network round trip."""
    d = _map_cache_dir(cache_name)
    if not os.path.isdir(d):
        return None
    zips = sorted(f for f in os.listdir(d) if f.lower().endswith(".zip"))
    if not zips:
        return None
    # More than one only happens if a map was re-fetched under a new asset name;
    # prefer the one named after the map, else the sole candidate.
    named = [z for z in zips if os.path.splitext(z)[0] == cache_name]
    return os.path.join(d, (named or zips)[0])


def scenario_outputs(sumo_dir):
    """Basenames the scenario itself writes into its own directory.

    SUMO resolves a relative path in a .sumocfg against the CONFIG FILE, not the
    process working directory, so a scenario that names its outputs relatively
    writes them next to itself - into the map cache. MLK's does: its additional-file
    carries

        <timedEvent type="SaveTLSSwitchStates" dest="signal_result.xml"/>

    and that one cannot be redirected from the command line the way --fcd-output and
    --tripinfo-output are, so signal_result.xml reappears in sumo/ after every run.

    Those files are byproducts of running, not evidence that anyone edited the
    scenario, and a parity check that flagged them would fire on every launch - which
    is the same as not warning at all, except louder. Collected from the cfg's own
    output declarations and from the `dest` of every timedEvent in the additional
    files it names."""
    outputs = set()
    cfg = bundle_sumocfg(sumo_dir)
    if not cfg:
        return outputs
    try:
        root = ET.parse(cfg).getroot()
    except (OSError, ET.ParseError):
        return outputs
    additional = []
    for el in root.iter():
        val = el.get("value")
        if val is None:
            continue
        if el.tag.endswith("-output") or el.tag in ("log", "error-log"):
            outputs.update(os.path.basename(v.strip()) for v in val.split(",") if v.strip())
        elif el.tag in ("additional-files", "route-files"):
            additional += [v.strip() for v in val.split(",") if v.strip()]
    for rel in additional:
        path = rel if os.path.isabs(rel) else os.path.join(sumo_dir, rel)
        try:
            for el in ET.parse(path).getroot().iter():
                dest = el.get("dest")
                if dest:
                    outputs.add(os.path.basename(dest.strip()))
        except (OSError, ET.ParseError):
            continue
    return outputs


def bundle_parity(cache_name, half="sumo"):
    """Whether the extracted `half`/ still holds exactly what the bundle ships.

    Returns (verdict, changed, extra, missing):
      "match"    - every file the bundle ships is present, byte for byte
      "diverged" - the three lists say how
      "unknown"  - no cached zip here to compare against, so nothing can be claimed

    Compared by CRC32, which the zip already stores per entry in its central
    directory, against zlib.crc32 of the file on disk. That reads each side once and
    needs no extraction, no temp copy and no second download - cheap enough to run on
    every launch, which is the point: a check that only runs when asked is a check
    that answers after the divergent run, not before it.

    A run whose results are meant to be comparable to anyone else's depends on this
    being "match". The cache is declared re-creatable (see _map_cache_dir), so
    divergence is not a thing to protect - it is a thing to SAY, because the same
    .sumocfg name over different bytes produces different traffic and nothing else in
    the run would ever mention it."""
    import zlib
    zip_path = _cached_bundle_zip(cache_name)
    root = os.path.join(_map_cache_dir(cache_name), half)
    if not zip_path or not os.path.isdir(root):
        return "unknown", [], [], []

    prefix = half + "/"
    changed, missing = [], []
    shipped = set()
    try:
        with zipfile.ZipFile(zip_path) as z:
            for info in z.infolist():
                if not info.filename.startswith(prefix) or info.filename.endswith("/"):
                    continue
                rel = info.filename[len(prefix):]
                shipped.add(rel)
                disk = os.path.join(root, rel.replace("/", os.sep))
                if not os.path.isfile(disk):
                    missing.append(rel)
                    continue
                crc = 0
                with open(disk, "rb") as f:
                    for chunk in iter(lambda: f.read(1 << 20), b""):
                        crc = zlib.crc32(chunk, crc)
                if crc != info.CRC:
                    changed.append(rel)
    except (OSError, zipfile.BadZipFile):
        return "unknown", [], [], []
    if not shipped:
        return "unknown", [], [], []

    # A file the scenario writes itself is a byproduct of running it, not a sign that
    # anyone changed it - see scenario_outputs.
    generated = scenario_outputs(root) if half == "sumo" else set()
    extra = []
    for dirpath, _dirs, files in os.walk(root):
        for f in files:
            rel = os.path.relpath(os.path.join(dirpath, f), root).replace(os.sep, "/")
            if rel not in shipped and os.path.basename(rel) not in generated:
                extra.append(rel)

    verdict = "match" if not (changed or extra or missing) else "diverged"
    return verdict, sorted(changed), sorted(extra), sorted(missing)


def report_bundle_parity(cache_name, half="sumo", where=None):
    """Print what this run is about to use and whether it is what the library ships.

    Always prints something. A cached half that silently differs from the published
    bundle is the failure this exists to prevent: the run looks ordinary, the .sumocfg
    has the name it always had, and the numbers come out different from everyone
    else's with nothing on screen to explain it. Returns the verdict."""
    verdict, changed, extra, missing = bundle_parity(cache_name, half)
    at = f": {where}" if where else ""
    if verdict == "match":
        print(f"[cosim] using the cached {half.upper()} half of '{cache_name}'{at}")
        print(f"[cosim]   matches the published bundle "
              f"({os.path.basename(_cached_bundle_zip(cache_name))}) exactly")
        return verdict
    if verdict == "unknown":
        print(f"[cosim] using the cached {half.upper()} half of '{cache_name}'{at}")
        print(f"[cosim]   no local copy of the bundle here, so it cannot be compared "
              f"with what the Digital-Twin-Library publishes. --reimport re-downloads "
              f"and re-extracts.")
        return verdict
    print(f"[cosim] using the cached {half.upper()} half of '{cache_name}'{at}")
    print(f"[cosim]   *** it DIFFERS from the published bundle "
          f"({os.path.basename(_cached_bundle_zip(cache_name))}) ***")
    for label, items in (("modified", changed), ("missing", missing),
                         ("not in the bundle", extra)):
        if not items:
            continue
        head = ", ".join(items[:4]) + (f", +{len(items) - 4} more" if len(items) > 4 else "")
        print(f"[cosim]   {label:>18}: {head}")
    print(f"[cosim]   this run will use the files ON DISK, not the published ones. "
          f"--reimport restores the bundle's copy.")
    return verdict


# ------------------------------------------------ which bundle produced a map

# A cooked map is identified by a DIRECTORY NAME (resolve_cooked_map), so it has no
# memory of what produced it. That is fine while both halves of a bundle age
# together in one cache, and wrong as soon as they do not: the CARLA half is cooked
# once and kept, while the SUMO half tracks the library. Same map name on both
# sides, no error anywhere, vehicles placed against geometry that moved.
#
# So the map carries the sha256 of the release asset it came from. NOTHING IS
# HASHED HERE - GitHub already publishes a digest per release asset, so it is
# queried and copied along. A map with no sha (a local pick, a hand-prepped map,
# anything imported before this existed) simply is not checked.
SHA_FILE = ".source_sha"


def _read_sha(directory):
    try:
        with open(os.path.join(directory, SHA_FILE), encoding="utf-8") as f:
            return f.read().strip() or None
    except OSError:
        return None                      # no note is the normal case, not an error


def _write_sha(directory, sha):
    """Best effort: an unwritable note costs the check, not the run. Writing
    NOTHING when there is no sha is the part that matters - an empty note would
    read back as a value and make two unrelated maps look like a match."""
    if not sha:
        return
    try:
        with open(os.path.join(directory, SHA_FILE), "w", encoding="utf-8") as f:
            f.write(sha.strip() + "\n")
    except OSError:
        pass


def record_source(cache_name, half, path):
    """Note where a locally-picked half came from, in ~/.fixs/maps/<name>/source.json.

    A local pick otherwise leaves no trace: .source_sha is written only for release
    downloads, so _check_map_source declines to check a hand-picked map at all. A
    note per HALF rather than one digest, because the two halves of a local map come
    from unrelated trees (a RoadRunner export and a SUMO folder), so there is no
    shared identity to compare - only an origin to record. Best effort: a missing
    note costs bookkeeping, not a run."""
    import json
    if not cache_name or not path:
        return
    dest = os.path.join(_map_cache_dir(cache_name), "source.json")
    try:
        doc = {}
        if os.path.isfile(dest):
            with open(dest, encoding="utf-8") as f:
                doc = json.load(f) or {}
        doc[half] = {"path": os.path.abspath(path),
                     "mtime": int(os.stat(path).st_mtime),
                     "recorded": int(time.time())}
        os.makedirs(os.path.dirname(dest), exist_ok=True)
        with open(dest, "w", encoding="utf-8") as f:
            json.dump(doc, f, indent=2, sort_keys=True)
    except (OSError, ValueError):
        pass


def read_cached_sha(cache_name):
    """Which release asset the bundle cached for `cache_name` came from, or None."""
    return _read_sha(_map_cache_dir(cache_name))


def cooked_sha(carla_root, name, mode=None):
    """Which release asset the imported map `name` was built from, or None."""
    return _read_sha(cooked_content_dir(carla_root, name, mode))


def _release_asset_sha(repo, tag, pattern):
    """The sha256 GitHub publishes for the release asset matching `pattern`, or
    None if it cannot be determined (no gh, offline, or a release old enough that
    the API reports a null digest). Queryable without downloading the asset."""
    gh = shutil.which("gh")
    if not gh:
        return None
    try:
        out = subprocess.check_output(
            [gh, "release", "view", tag, "-R", repo, "--json", "assets",
             "--jq", '.assets[] | .name + " " + (.digest // "")'],
            text=True, stderr=subprocess.DEVNULL)
    except (OSError, subprocess.CalledProcessError):
        return None
    for line in out.splitlines():
        name, _, digest = line.strip().partition(" ")
        if digest and fnmatch.fnmatch(name, pattern):
            return digest
    return None


def _download_release_asset(repo, tag, pattern, suffix, force_redownload=False,
                            cache_name=None, what="asset"):
    """Shared body of download_release_zip / download_cooked_tar: return a local
    path to the release asset matching `pattern`, downloading it via gh into the
    ~/.fixs/maps/<cache_name or tag>/ cache. `suffix` is how a cached copy is
    recognised, and is what keeps the two callers from seeing each other's file -
    the source bundle (.zip) and the precooked package (.tar.gz) share a cache
    dir, and a map may well have both downloaded."""
    gh = _require_gh()
    tag_dir = _map_cache_dir(cache_name or tag)
    cached = [f for f in os.listdir(tag_dir) if f.lower().endswith(suffix)] \
        if os.path.isdir(tag_dir) else []

    if cached and not force_redownload:
        path = os.path.join(tag_dir, cached[0])
        if sys.stdin.isatty():
            ans = input(f"[import] cached '{cached[0]}' found for '{tag}'. "
                        "[U]se it / [R]e-download / [C]ancel? [U]: ").strip().lower()
            if ans.startswith("c"):
                sys.exit("[import] cancelled by user.")
            force_redownload = ans.startswith("r")
        if not force_redownload:
            print(f"[import] using cached {path}")
            # Deliberately NOT backfilled with the release's current sha: a cached
            # copy may predate the release it came from, so recording today's digest
            # against yesterday's zip would assert something untrue. Unrecorded is
            # the honest state, and it only costs the check.
            return path

    os.makedirs(tag_dir, exist_ok=True)
    print(f"[import] downloading release '{tag}' from {repo}\n"
          f"[import]   into cache {tag_dir} (set FIXS_MAP_CACHE to relocate) ...")
    rc = subprocess.call([gh, "release", "download", tag, "-R", repo,
                          "-p", pattern, "-D", tag_dir, "--clobber"])
    if rc != 0:
        sys.exit(f"[import] `gh release download {tag} -p {pattern}` failed (rc={rc}).")
    got = [f for f in os.listdir(tag_dir) if f.lower().endswith(suffix)]
    if not got:
        sys.exit(f"[import] release '{tag}' has no {what} matching '{pattern}'.")
    # An exact asset name wins over whatever else the cache dir holds. The
    # download itself already fetched only that one, but a dir shared by two
    # bundles would otherwise hand back a sibling by listing order.
    if pattern in got:
        return os.path.join(tag_dir, pattern)
    # Note WHICH asset this is, while we still know. The cache dir is named for the
    # map, not the release, so nothing else here can answer that afterwards.
    _write_sha(tag_dir, _release_asset_sha(repo, tag, pattern))
    return os.path.join(tag_dir, got[0])


def download_release_zip(repo, tag, force_redownload=False, cache_name=None,
                         asset=None):
    """Return a local path to the release's .zip asset, downloading it via gh into
    the ~/.fixs/maps/<cache_name or tag>/ cache (cache_name = the cooked map name,
    so the zip sits beside the extracted carla/+sumo/). If a cached copy already
    exists, ask whether to reuse or re-download (default reuse); force_redownload
    skips the prompt. The zip stays in the cache so re-imports are free.

    `asset` is the exact published name, from the catalog entry. Without it the
    glob picks whichever .zip the release happens to list first, which is wrong
    the moment a release carries more than one bundle - the same reason
    download_cooked_tar has always taken its asset by name."""
    return _download_release_asset(repo, tag, asset or "*.zip", ".zip",
                                   force_redownload, cache_name,
                                   what="source bundle (.zip)")


def download_cooked_tar(repo, tag, asset, force_redownload=False, cache_name=None):
    """Return a local path to the release's precooked .tar.gz, downloading it via
    gh into the same per-map cache the source bundle uses. `asset` is the exact
    published name (from the catalog, or derived - see catalog_cooked_asset), so a
    release that carries several is not a lottery."""
    return _download_release_asset(repo, tag, asset, ".tar.gz", force_redownload,
                                   cache_name, what="precooked package (.tar.gz)")


def release_assets(repo, tag):
    """Names of the assets published on release `tag`, or None if they cannot be
    listed. Used to say 'this map has no precooked build published' BEFORE
    downloading anything, and to name what is published instead - the alternative
    is a gh failure the user has to interpret."""
    gh = shutil.which("gh")
    if not gh:
        return None
    try:
        out = subprocess.check_output(
            [gh, "release", "view", tag, "-R", repo, "--json", "assets",
             "--jq", ".assets[].name"], text=True, stderr=subprocess.DEVNULL)
    except (OSError, subprocess.CalledProcessError):
        return None
    return [line.strip() for line in out.splitlines() if line.strip()]


def pick_and_import(repo, tag_prefix="", carla_root=None, ue4_root=None, force=False):
    """List `repo`'s map releases (or accept a local .zip), ask which to import,
    then put it into the configured CARLA by the route that flavour supports:
    a SOURCE build cooks it, a PACKAGED build installs the precooked package.
    If the chosen version is already there, skip the work (unless the user opts to
    re-import, or force=True).

    need_source=False on purpose. Which flavour is installed is not a property of
    this operation - "import a map" is meaningful on both - so the flavour is
    resolved first and then dispatched on, instead of being used to refuse. Gating
    the whole command on a source build is what made --import-map dead-end on a
    packaged CARLA while run_cosim quietly did the same job (#276)."""
    carla_root, ue4_root = _resolve_carla(carla_root, ue4_root, need_source=False)
    mode = _mode()
    catalog = fetch_catalog(repo)
    name, tag, local = choose_map(repo, tag_prefix, carla_root, catalog=catalog)

    # The picker returns what the BUNDLE is called ('mlk'); the cooked map inside
    # it is often named differently ('mlk_no_signal'). Everything below - the
    # already-imported check, the install, the message - is about the map, so
    # resolve it here, exactly as run_cosim does before its own preflight.
    entry = catalog_entry(catalog, name)
    if entry and entry.get("map_name") and entry["map_name"] != name:
        print(f"[import] map '{name}' -> '{entry['map_name']}'  (DT release {tag})")
        name = entry["map_name"]

    if map_is_imported(carla_root, name, mode) and not force:
        print(f"[import] '{name}' is already imported: "
              f"{cooked_map_path(carla_root, name, mode)}")
        if not sys.stdin.isatty():
            return 0
        if input("[import] re-import it (re-download + re-cook)? [y/N]: ").strip().lower() != "y":
            print("[import] keeping the existing map.")
            return 0
        force = True

    if mode == "packaged":
        # install_cooked reports the finished install; no second "done" here.
        install_precooked(carla_root, name, repo=repo, tag=tag, entry=entry,
                          local=local, force=force)
        return 0

    zip_path = local if local else download_release_zip(
        repo, tag, force_redownload=force,
        asset=(entry or {}).get("asset"))
    return ensure_map(name, carla_root=carla_root, ue4_root=ue4_root,
                      package_dir=zip_path, force=force,
                      source_sha=_read_sha(os.path.dirname(zip_path)))


def import_named(name, carla_root=None, ue4_root=None, package_url=None,
                 package_dir=None, force=False, prompt_if_exists=False,
                 package_pick=False, repo=None, tag_prefix=None):
    """Import the map called `name` by the route the configured CARLA supports.

    The named counterpart of pick_and_import: same dispatch, no menu. A SOURCE
    build cooks the package (ensure_map); a PACKAGED build installs the precooked
    one. `--package <name>` used to go straight to the cook and so refused on a
    packaged build, for the same reason the picker did (#276)."""
    carla_root, ue4_root = _resolve_carla(carla_root, ue4_root, need_source=False)
    if _mode() != "packaged":
        return ensure_map(name, carla_root, ue4_root, package_url, package_dir,
                          force, prompt_if_exists, package_pick,
                          source_sha=(_read_sha(os.path.dirname(package_dir))
                                      if package_dir else None))

    repo, tag_prefix = resolve_map_source(repo, tag_prefix)
    catalog = fetch_catalog(repo)
    entry = catalog_entry(catalog, name)
    if entry and entry.get("map_name") and entry["map_name"] != name:
        print(f"[import] map '{name}' -> '{entry['map_name']}'  "
              f"(DT release {entry.get('release')})")
        name = entry["map_name"]
    if package_url:
        # There is no cook here to feed, and a source zip is not installable as-is.
        print(f"[import] note: this CARLA is PACKAGED, so --package-url is not used; "
              f"the precooked package is resolved from the map catalog instead.")

    umap = cooked_map_path(carla_root, name, "packaged")
    if os.path.isfile(umap) and not force:
        print(f"[import] '{name}' is already installed: {umap}")
        if not (prompt_if_exists and sys.stdin.isatty()):
            return 0
        if input("[import] re-install it (re-download + re-extract)? [y/N]: ").strip().lower() != "y":
            print("[import] keeping the existing map.")
            return 0
        force = True

    # install_cooked says "done: <name> installed -> <umap>" itself; saying it again
    # here just made every packaged install report success twice.
    install_precooked(carla_root, name, repo=repo, tag=(entry or {}).get("release"),
                      entry=entry, local=package_dir, force=force)
    return 0


# ------------------------------------------------------------- purge a map

# Content/ subdirectories that belong to CARLA itself, or to FIXS's own installed
# assets, and never to an imported map. Named explicitly because the structural
# test below is not sufficient on its own: CARLA's Content/Carla/ ships a
# Config/Carla.Package.json exactly like an imported package does, so "has a
# package descriptor" would offer the engine's own content up for deletion.
ENGINE_CONTENT = frozenset((
    "Carla", "Collections", "Developers", "FIXS", "Movies", "Splash",
    "Cinematics", "HDRIBackdrop", "Localization", "PropsFactory",
))


def _dir_size(path):
    """Bytes under `path`; 0 when it is absent. Entries that cannot be stat'd are
    skipped rather than raising: this number exists for a human deciding what to
    delete, and one unreadable file must not cost them the whole listing."""
    total = 0
    if not path or not os.path.isdir(path):
        return 0
    for root, _dirs, files in os.walk(path):
        for name in files:
            try:
                total += os.path.getsize(os.path.join(root, name))
            except OSError:
                pass
    return total


def human_bytes(n):
    """'1.4 GB', for a listing someone reads before agreeing to a deletion."""
    if not n:
        return "-"
    size = float(n)
    for unit in ("B", "KB", "MB", "GB", "TB"):
        if size < 1024 or unit == "TB":
            return f"{size:.0f} {unit}" if unit == "B" else f"{size:.1f} {unit}"
        size /= 1024.0


def purge_candidates(carla_root=None, mode=None):
    """What every map on this machine occupies locally: one record per name,
    sorted, as {"name", "pieces", "cooked_umap"}. `pieces` is a list of
    (label, path, bytes) with label in ("cooked", "staged", "cache").

    A name qualifies on ANY ONE of four pieces of evidence, because the three
    locations age independently and a purge exists precisely for the states where
    they disagree:

      - Maps/<name>/<name>.umap        a finished cook
      - Config/<name>.Package.json     a cook that died part-way, which is exactly
                                       the wreckage this command exists to clear
      - Import/<name>/                 staging a past import left behind
      - ~/.fixs/maps/<name>/           a downloaded bundle, cooked or not

    A lone Import/<name>.json never qualifies a name by itself - it is swept up
    only once something else has - because CARLA's own Import/ holds descriptors
    (roadpainter_decals.json) that name no map at all. ENGINE_CONTENT is
    subtracted last, so no amount of evidence can offer CARLA's own content up.

    carla_root=None is a valid call: a machine with no configured CARLA still has
    a bundle cache, and being able to reclaim it is the whole point of asking."""
    names = set()

    content = content_root(carla_root, mode) if carla_root else None
    if content and os.path.isdir(content):
        for name in os.listdir(content):
            if not os.path.isdir(os.path.join(content, name)):
                continue
            if os.path.isfile(cooked_map_path(carla_root, name, mode)) or \
                    os.path.isfile(package_descriptor(carla_root, name, mode)):
                names.add(name)

    import_dir = os.path.join(carla_root, "Import") if carla_root else None
    if import_dir and os.path.isdir(import_dir):
        for entry in os.listdir(import_dir):
            if os.path.isdir(os.path.join(import_dir, entry)):
                names.add(entry)

    cache_root = _map_cache_dir()
    if os.path.isdir(cache_root):
        for name in os.listdir(cache_root):
            if os.path.isdir(os.path.join(cache_root, name)):
                names.add(name)

    records = []
    for name in sorted(names - ENGINE_CONTENT):
        pieces = []
        if carla_root:
            cooked = cooked_content_dir(carla_root, name, mode)
            if os.path.isdir(cooked):
                pieces.append(("cooked", cooked, _dir_size(cooked)))
            # The .bak_reimport a failed re-import leaves behind is the same map's
            # content under another name; purging the map without it would leave a
            # full copy on disk that nothing will ever look at again.
            backup = cooked + ".bak_reimport"
            if os.path.isdir(backup):
                pieces.append(("cooked", backup, _dir_size(backup)))
            for path in staged_import_paths(carla_root, name):
                pieces.append(("staged", path,
                               _dir_size(path) if os.path.isdir(path)
                               else os.path.getsize(path)))
        # os.path.join, NOT _map_cache_dir(name): that helper CREATES the folder
        # it names, and an inventory must not bring into being the very thing it
        # is reporting on. An empty one is skipped too - there is nothing there to
        # reclaim, and offering it would make a purge look bigger than it is.
        cache = os.path.join(cache_root, name)
        if os.path.isdir(cache) and os.listdir(cache):
            pieces.append(("cache", cache, _dir_size(cache)))
        if not pieces:
            continue
        records.append({
            "name": name,
            "pieces": pieces,
            "cooked_umap": bool(carla_root) and os.path.isfile(
                cooked_map_path(carla_root, name, mode)),
        })
    return records


def staged_import_paths(carla_root, name):
    """The staging a past import left in CARLA's Import/ for `name`: the folder and
    the descriptor beside it. Either can outlive the other, so both are looked for
    independently."""
    import_dir = os.path.join(carla_root, "Import")
    return [p for p in (os.path.join(import_dir, name),
                        os.path.join(import_dir, name + ".json"))
            if os.path.exists(p)]


# --------------------------------------------------------------------------- #
# Import/ hygiene. CARLA's Import.py cooks whatever it finds in that one folder,
# so what is left there from previous imports is not inert - it is input. The two
# helpers below make that visible: one before a map is staged over an existing
# copy, one to clear the copies nothing is going to use again.
# --------------------------------------------------------------------------- #
def staging_report(carla_root, name):
    """What Import/ already holds for `name`, as [(path, bytes, mtime), ...]."""
    out = []
    for p in staged_import_paths(carla_root, name):
        size = _dir_size(p) if os.path.isdir(p) else os.path.getsize(p)
        out.append((p, size, os.path.getmtime(p)))
    return out


def _mb(n):
    return "%.1f MB" % (n / (1024.0 * 1024.0)) if n >= 1024 * 1024 else "%.0f KB" % (n / 1024.0)


def _stamp(t):
    return time.strftime("%Y-%m-%d %H:%M", time.localtime(t))


def resolve_existing_staging(carla_root, name, have_source, interactive=None):
    """Report an Import/ staging that already exists for `name`, and decide what to
    do about it. Returns "use", "restage" or "abort".

    Only INTERRUPTS when have_source is False - when nothing tells this call where
    to copy from, so reusing whatever is already in Import/<name>/ is a guess: it
    could be a leftover from an earlier import of a DIFFERENT export that happened
    to share this name. That guess is what deserves a human's eyes.

    have_source=True means the opposite: a path, a URL, or an explicit pick was
    already given, so there is nothing to decide - restaging from it is not a
    guess, it is what was just asked for. Interrupting there anyway (every prior
    version of this function did) turned a certainty into a keystroke on every
    single --reimport, forever, for a caller who by definition already knows the
    answer before being asked."""
    existing = staging_report(carla_root, name)
    if not existing:
        return "restage" if have_source else "use"

    if have_source:
        print(f"[import] replacing the staged package for '{name}' with the "
              f"source you gave.")
        return "restage"

    print("")
    print(f"[import] Import/ ALREADY holds a staged package for '{name}':")
    for path, size, mtime in existing:
        kind = "folder" if os.path.isdir(path) else "file  "
        print(f"[import]   {kind} {path}  ({_mb(size)}, {_stamp(mtime)})")
    print(f"[import]   Nothing else was supplied, so THIS copy is what would be "
          f"cooked - not a fresh one.")

    if not (sys.stdin.isatty() if interactive is None else interactive):
        print("[import]   non-interactive: continuing with 'use'.")
        return "use"

    while True:
        ans = _prompt("[import] [U]se the staged copy / [A]bort? [U]: ").strip().lower()
        if ans in ("", "u"):
            return "use"
        if ans.startswith("a"):
            sys.exit("[import] aborted at the staging prompt; nothing was changed.")
        print("[import] please answer with one of the letters shown.")


# Files in Import/ that belong to CARLA (or to us) rather than to a staged map.
IMPORT_KEEP = {"readme.md", "roadpainter_decals.json"}


def import_staging_inventory(carla_root, keep=None):
    """Split CARLA's Import/ into (staged, other).

    `staged` is one record per package - {"name", "paths", "bytes"} - built from a
    <name>.json descriptor, a <name>/ asset folder, or both, since either can
    outlive the other. `keep` (the map about to be imported) is excluded.

    `other` is everything this must not decide about: CARLA's own files, and the
    strays that accumulate in Import/ by hand (a .zip, a .bak). They are reported,
    never removed - guessing at a file nobody staged is how a clean-up becomes a
    data loss."""
    import_dir = os.path.join(carla_root, "Import")
    if not os.path.isdir(import_dir):
        return [], []
    names, other = set(), []
    for entry in sorted(os.listdir(import_dir)):
        full = os.path.join(import_dir, entry)
        low = entry.lower()
        if low in IMPORT_KEEP or entry == os.path.basename(_stash_dir(import_dir)):
            continue
        if os.path.isdir(full):
            names.add(entry)
        elif low.endswith(".json"):
            names.add(entry[:-len(".json")])
        else:
            other.append((full, os.path.getsize(full)))
    staged = []
    for name in sorted(n for n in names if n != keep):
        paths = staged_import_paths(carla_root, name)
        if not paths:
            continue
        total = sum(_dir_size(p) if os.path.isdir(p) else os.path.getsize(p)
                    for p in paths)
        staged.append({"name": name, "paths": paths, "bytes": total})
    return staged, other


def clean_import_dir(carla_root, keep=None, interactive=None, assume_yes=False):
    """Clear stale per-map staging out of CARLA's Import/. Returns bytes reclaimed.

    Complements --purge-map rather than duplicating it: a purge removes everything
    ONE map owns (cooked content, staging, cache), and deliberately will not let a
    lone Import/<name>.json qualify a name on its own. This is the other axis -
    every staged package except the one being imported now - which is what clears
    the descriptors a purge leaves behind and the packages of maps that are long
    gone.

    Never touches CARLA's own files, the set-aside stash, or anything it cannot
    identify as staging: those are listed instead, for a human to judge."""
    staged, other = import_staging_inventory(carla_root, keep)
    if not staged:
        print(f"[import] Import/ has no stale staging to clear"
              + (f" (keeping '{keep}')." if keep else "."))
        return 0

    total = sum(s["bytes"] for s in staged)
    print("")
    print(f"[import] Import/ holds staging for {len(staged)} package(s) "
          f"not being imported now ({_mb(total)}):")
    for s in staged:
        print(f"[import]   {s['name']:<24} {_mb(s['bytes'])}")
        for p in s["paths"]:
            print(f"[import]       {p}")
    if keep:
        print(f"[import]   (keeping '{keep}', which this run imports)")
    if other:
        print(f"[import]   NOT touching {len(other)} unrecognised item(s) - "
              f"remove by hand if you want them gone:")
        for path, size in other:
            print(f"[import]       {path}  ({_mb(size)})")

    if not assume_yes:
        if not (sys.stdin.isatty() if interactive is None else interactive):
            print("[import] non-interactive and no confirmation: nothing removed.")
            return 0
        ans = _prompt(f"[import] remove the {len(staged)} staged package(s) above? "
                      f"[y/N]: ").strip().lower()
        if not ans.startswith("y"):
            print("[import] left Import/ as it was.")
            return 0

    freed = 0
    for s in staged:
        for p in s["paths"]:
            try:
                if os.path.isdir(p):
                    shutil.rmtree(p)
                else:
                    os.remove(p)
            except OSError as exc:
                print(f"[import]   could not remove {p}: {exc}")
                continue
        freed += s["bytes"]
        print(f"[import]   removed staging for '{s['name']}'")
    print(f"[import] Import/ cleaned: {_mb(freed)} reclaimed.")
    return freed


def capture_import_evidence(carla_root, name, dest_root=None, tag=""):
    """Copy Unreal's logs and its newest crash dump somewhere they will survive.

    Unreal rotates Saved/Logs on EVERY editor launch, and one import is four
    launches - so by the time anyone looks, the log of the run that failed has
    usually been pushed out. A cook that dies is exactly when those files matter,
    so they are taken at that moment rather than hunted for afterwards. Returns
    the directory, or None if there was nothing to take."""
    saved = os.path.join(carla_root, "Unreal", "CarlaUE4", "Saved")
    logs, crashes = os.path.join(saved, "Logs"), os.path.join(saved, "Crashes")
    if not os.path.isdir(logs):
        return None
    dest_root = dest_root or os.path.join(os.getcwd(), "RealSim_tmp")
    stamp = time.strftime("%Y%m%d_%H%M%S")
    dest = os.path.join(dest_root, f"import_crash_{name}{tag}_{stamp}")
    try:
        os.makedirs(dest, exist_ok=True)
        shutil.copytree(logs, os.path.join(dest, "Logs"), dirs_exist_ok=True)
        if os.path.isdir(crashes):
            dumps = [os.path.join(crashes, d) for d in os.listdir(crashes)]
            dumps = [d for d in dumps if os.path.isdir(d)]
            if dumps:
                newest = max(dumps, key=os.path.getmtime)
                shutil.copytree(newest, os.path.join(dest, os.path.basename(newest)),
                                dirs_exist_ok=True)
    except OSError as exc:
        print(f"[import] could not save crash evidence: {exc}")
        return None
    return dest


def _report_import_failure(name, rc, produced, evidence):
    """Say plainly that Unreal failed, what it cost, and where to look.

    Never silent, and never dressed up as a warning: Import.py exiting non-zero
    means an Unreal process died. When the .umap survives it (the cook writes it
    one step before the step that usually crashes) the run can go on, but 'went on'
    is not 'was fine' - the steps after the crash did not run, and this is the only
    place that difference is visible."""
    bar = "=" * 68
    signed = rc - (1 << 32) if rc and rc > 0x7FFFFFFF else rc
    hexrc = f" (0x{rc & 0xFFFFFFFF:08X})" if rc and rc not in (1, 2) else ""
    print(f"\n[import] {bar}")
    print(f"[import] Import.py FAILED for '{name}': exit {signed}{hexrc}")
    if hexrc and (rc & 0xFFFFFFFF) == 0xC0000005:
        print(f"[import]   0xC0000005 = access violation: an Unreal commandlet crashed.")
    if produced:
        print(f"[import]   The map WAS written, so the run can continue - but "
              f"Import.py")
        print(f"[import]   stopped at the crash, so the steps after it did not run "
              f"(the")
        print(f"[import]   Traffic-Manager binary Maps/{name}/TM/ is the one that "
              f"goes missing;")
        print(f"[import]   a SUMO-driven co-sim never reads it).")
    else:
        print(f"[import]   The map was NOT written. Retrying once.")
    if evidence:
        print(f"[import]   Unreal's logs + crash dump saved to:")
        print(f"[import]     {evidence}")
        print(f"[import]   (Unreal rotates Saved/Logs on every launch; without this "
              f"copy they")
        print(f"[import]    are usually gone within minutes.)")
    print(f"[import] {bar}\n")


def record_bytes(record, with_cache=False):
    """Bytes a purge of `record` would actually reclaim, given the cache choice."""
    return sum(b for label, _p, b in record["pieces"]
               if with_cache or label != "cache")


def purge_maps(records, drop_cache=False):
    """Delete what `records` describe. Returns (removed, failed), both lists of
    (name, label, path, bytes); `failed` rows carry the OSError message in place of
    the byte count's usefulness.

    Takes the records purge_candidates built rather than names to re-resolve, so
    what is deleted is exactly what the caller printed and the user agreed to -
    there is no second lookup that could drift from the one shown."""
    removed, failed = [], []
    for record in records:
        for label, path, size in record["pieces"]:
            if label == "cache" and not drop_cache:
                continue
            try:
                if os.path.isdir(path):
                    shutil.rmtree(path)
                else:
                    os.remove(path)
                removed.append((record["name"], label, path, size))
            except OSError as exc:
                failed.append((record["name"], label, path, str(exc)))
    return removed, failed


def _purge_ask(msg):
    """input() where EOF means 'answered nothing'. Deliberately not _prompt: that
    one exits with advice about --map / --package, which is the wrong advice here
    and, worse, exits mid-command. A purge that cannot ask must cancel, never
    fall through to a default - the next step deletes."""
    try:
        return input(msg).strip()
    except EOFError:
        return ""


def parse_selection(answer, count):
    """0-based indices named by a picker answer: '2', '1,3,4', '2-5', 'all'. None
    when the answer does not parse or names nothing - the caller re-asks or
    cancels rather than acting on a guess, because what follows is a deletion.

    Public, and shared with run_profile's setup picker. Two lists a user deletes
    from should not accept two different syntaxes, and the only way to be sure they
    do not drift apart is for there to be one of them."""
    answer = (answer or "").strip().lower()
    if answer in ("a", "all", "*"):
        return list(range(count))
    chosen = set()
    for part in answer.replace(" ", "").split(","):
        if not part:
            continue
        lo, dash, hi = part.partition("-")
        if dash:
            if not (lo.isdigit() and hi.isdigit()):
                return None
            lo, hi = int(lo), int(hi)
            if not (1 <= lo <= hi <= count):
                return None
            chosen.update(range(lo - 1, hi))
        elif part.isdigit() and 1 <= int(part) <= count:
            chosen.add(int(part) - 1)
        else:
            return None
    return sorted(chosen) or None


def _purge_table(records, indent="   "):
    """The inventory as a numbered table: one row per map, one column per place a
    map occupies. Three columns rather than one total, because they are not
    equally expensive to rebuild and whoever is choosing needs to see that before
    they choose."""
    print(f"{indent}{'#':>3}  {'map':<22}{'cooked':>10}{'staged':>10}{'cache':>10}")
    for i, record in enumerate(records, 1):
        by = {}
        for label, _path, size in record["pieces"]:
            by[label] = by.get(label, 0) + size
        note = "" if record["cooked_umap"] else "   (staging only, never cooked)"
        print(f"{indent}{i:>3}) {record['name']:<22}"
              f"{human_bytes(by.get('cooked')):>10}"
              f"{human_bytes(by.get('staged')):>10}"
              f"{human_bytes(by.get('cache')):>10}{note}")


def purge(carla_root, mode=None, named="", drop_cache=None, interactive=False,
          carla_busy=None, carla_kill=None):
    """Delete imported maps from this machine. Returns a shell exit code.

    A map occupies three places that age independently, so all three are shown
    and the cache is chosen separately rather than swept along with the rest:

      Content/<name>/       what the cook produced. Rebuilding it costs a re-cook.
      Import/<name>/        the staging that cook read. Rebuilding costs a re-stage.
      ~/.fixs/maps/<name>/  the downloaded bundle. Rebuilding it costs a DOWNLOAD,
                            and this one is additionally read at RUN time - the
                            .sumocfg a co-sim runs comes out of its sumo/ half. So
                            deleting it does not merely make the next import
                            slower, it stops the next RUN until something
                            re-fetches it. Different blast radius, its own yes.

    `named` selects without asking: "" opens the picker, "all" takes everything,
    otherwise a comma-separated list of map names.

    `interactive` is the caller's answer to "may this stop and ask?", the same
    contract choose_imported_map takes and for the same reason: a --serve host has
    a terminal and nobody sitting at it, and isatty() cannot tell the difference.

    `carla_kill` is an optional callable taking that pid and ending the process
    tree; when both it and a terminal are present, a running CARLA is offered up to
    be closed instead of the purge simply refusing.

    `carla_busy` is an optional zero-argument callable returning the pid of a
    CARLA holding this machine's Content/ open, or None. Injected rather than
    detected here because process inspection belongs to the caller - the same way
    doctor.run takes who_has_port.

    Nothing here re-resolves a name after the listing: the records shown are the
    records deleted, so what is printed and what goes cannot drift apart."""
    if carla_root is None:
        print("[purge] no CARLA configured here; looking at the bundle cache only.")

    records = purge_candidates(carla_root, mode)
    if not records:
        print("[purge] nothing to purge: no imported maps or cached bundles found.")
        return 0

    named = (named or "").strip()
    grand = sum(record_bytes(r, True) for r in records)

    print()
    print(f"[purge] maps on this machine ({human_bytes(grand)} in total):")
    _purge_table(records)
    print()

    if named:
        if named.lower() in ("a", "all", "*"):
            chosen = list(records)
        else:
            by_name = {r["name"].lower(): r for r in records}
            chosen, unknown = [], []
            for want in named.split(","):
                want = want.strip()
                if not want:
                    continue
                record = by_name.get(want.lower())
                if record:
                    chosen.append(record)
                else:
                    unknown.append(want)
            if unknown:
                sys.exit(f"[purge] not on this machine: {', '.join(unknown)}. "
                         f"Run --purge-map with no name to see the list.")
    elif not interactive:
        sys.exit("[purge] non-interactive session: name what to purge "
                 "(--purge-map NAME[,NAME] or --purge-map all).")
    else:
        picked = parse_selection(
            _purge_ask(f"[purge] Purge which? [1-{len(records)}, a list like "
                       f"1,3,4, or 'all'; Enter to cancel]: "), len(records))
        if picked is None:
            print("[purge] cancelled; nothing was deleted.")
            return 0
        chosen = [records[i] for i in picked]

    # The cache question: asked once for the whole selection, and only when there
    # is a cache to lose. drop_cache answers it up front for a caller that knows.
    cache_bytes = sum(size for r in chosen for label, _p, size in r["pieces"]
                      if label == "cache")
    if drop_cache is None:
        if cache_bytes and interactive:
            print()
            print(f"[purge] {human_bytes(cache_bytes)} of that is the downloaded "
                  f"bundle under ~/.fixs/maps.")
            print("[purge] Re-creating it means a download - and a co-sim reads "
                  "its scenario")
            print("[purge] (.sumocfg) from there, so the next run needs it back.")
            drop_cache = _purge_ask(
                "[purge] Drop the cached bundles too? [y/N]: ").lower().startswith("y")
        else:
            drop_cache = False

    total = sum(record_bytes(r, drop_cache) for r in chosen)
    print()
    print(f"[purge] will delete ({human_bytes(total)}):")
    for record in chosen:
        for label, path, size in record["pieces"]:
            verb = "keep  " if (label == "cache" and not drop_cache) else "delete"
            print(f"   {verb}  {path}   ({human_bytes(size)})")
    print()

    # CARLA keeps every package under Content/ open, and Windows will not unlink a
    # mapped file - so a purge with CARLA up deletes half a map and then raises a
    # PermissionError from somewhere in the middle of it. Refuse before touching
    # anything, rather than leaving the wreckage that a half-delete makes.
    busy = carla_busy() if (carla_busy and carla_root) else None
    if busy:
        print(f"[purge] CARLA is running (pid {busy}) and holds files under "
              f"Content/ open, so a delete would fail part-way through.")
        # Offer rather than refuse. The delete cannot proceed either way, so the
        # only question is who closes CARLA - and sending the user away to do it by
        # hand costs a whole round trip to reach the identical state. Only offered
        # when there is someone to ask AND a way to do it; `carla_kill` is passed in
        # by the caller for the same reason carla_busy is - this module knows what
        # holds the files, not how to end a process tree on this platform.
        if interactive and carla_kill:
            if not _purge_ask(f"[purge] Close CARLA (pid {busy}) now and continue? "
                              f"[y/N]: ").lower().startswith("y"):
                sys.exit("[purge] cancelled; nothing was deleted and CARLA is "
                         "still running.")
            print(f"[purge] closing CARLA (pid {busy}) ...")
            carla_kill(busy)
            # Confirm rather than assume: taskkill returns before the process tree
            # is actually gone, and deleting while a handle survives is the
            # half-delete this guard exists to prevent. A few seconds is generous
            # for a kill that landed and short enough to be worth waiting out.
            for _ in range(20):
                time.sleep(0.5)
                busy = carla_busy()
                if not busy:
                    break
            if busy:
                sys.exit(f"[purge] CARLA (pid {busy}) is still holding files after "
                         f"the close request; nothing was deleted. Close it by hand "
                         f"and run this again.")
            print("[purge] CARLA closed.")
        else:
            sys.exit(f"[purge] close it and run this again"
                     + ("" if interactive else " (non-interactive: not closing it "
                        "for you)") + ".")

    if interactive and not _purge_ask(
            "[purge] Proceed? [y/N]: ").lower().startswith("y"):
        print("[purge] cancelled; nothing was deleted.")
        return 0

    removed, failed = purge_maps(chosen, drop_cache=drop_cache)
    freed = sum(size for _n, _l, _p, size in removed)
    print(f"[purge] removed {len(removed)} item(s), {human_bytes(freed)} freed.")
    if failed:
        print(f"[purge] {len(failed)} could NOT be removed:")
        for name, label, path, err in failed:
            print(f"   {name} ({label}): {path}")
            print(f"      {err}")
        print("[purge] something still holding a file open (CARLA, the Unreal "
              "editor, an explorer window) is the usual cause.")
        return 1
    return 0


DEFAULT_MAP_REPO = "ORNL-Real-Sim/FIXS_Applications"
DEFAULT_MAP_TAG_PREFIX = "map-"


def _app_root():
    """The application repo root that holds fixs_sources.txt: two levels up from
    this file (<repo>/FIXS/Carla/import_map.py -> <repo>)."""
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


def resolve_map_source(repo=None, tag_prefix=None):
    """Resolve (repo, tag_prefix) for the map picker. Precedence: an explicit value
    (CLI) wins, else the root fixs_sources.txt (map_repo / map_tag_prefix), else the
    built-in default. Lets the hosting repo change in one config line, no wrappers
    to edit."""
    cfg = {}
    src = os.path.join(_app_root(), "fixs_sources.txt")
    if os.path.isfile(src):
        cfg = read_map_config(src)
    repo = repo or cfg.get("map_repo") or DEFAULT_MAP_REPO
    if tag_prefix is None:
        tag_prefix = cfg.get("map_tag_prefix", DEFAULT_MAP_TAG_PREFIX)
    return repo, tag_prefix


def main():
    # First act: get onto the interpreter carla.json names. This is started as
    # `python Carla/import_map.py` from whatever shell the user has open, so without
    # this the cook runs under whatever python is on PATH - a different env than the
    # one run_cosim uses, from the same machine and the same config.
    env.reexec_under_configured(__file__, tag="import")

    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--package", default=None,
                    help="map/package name (matches Import/<name>.json)")
    ap.add_argument("--map-config", default=None,
                    help="text file declaring the map (package= and url= lines)")
    ap.add_argument("--package-url", default=None,
                    help="URL of the package zip (e.g. a GitHub release asset)")
    ap.add_argument("--package-dir", default=None,
                    help="local folder (or .zip) holding the package files")
    ap.add_argument("--package-pick", action="store_true",
                    help="always open a file picker to select a hand-downloaded package "
                         "(skips the gh auto-download)")
    ap.add_argument("--pick-release", action="store_true",
                    help="list the repo's map releases and prompt which version to import, "
                         "then download + cook it (needs gh). Repo/prefix come from "
                         "--repo/--tag-prefix or from repo=/tag_prefix= in --map-config")
    ap.add_argument("--repo", default=None,
                    help="owner/repo whose releases to list for --pick-release")
    ap.add_argument("--tag-prefix", default=None,
                    help="for --pick-release, only offer releases whose tag starts with "
                         "this (e.g. 'map-'); the package name is the tag minus this prefix")
    ap.add_argument("--carla-root", default=None, help="override the saved carla_root")
    ap.add_argument("--ue4-root", default=None, help="override the saved ue4_root")
    ap.add_argument("--force", action="store_true",
                    help="re-import even if already present (no prompt)")
    args = ap.parse_args()

    mc = read_map_config(args.map_config) if args.map_config else {}

    # Default action is the release/local picker; naming a specific --package (or a
    # package= in --map-config) opts into the direct, non-interactive import path.
    if args.pick_release or not (args.package or args.package_dir or args.package_url or mc.get("package")):
        repo, tag_prefix = resolve_map_source(
            args.repo or mc.get("repo"),
            args.tag_prefix if args.tag_prefix is not None else mc.get("tag_prefix"))
        return pick_and_import(repo, tag_prefix, args.carla_root, args.ue4_root, args.force)

    package = args.package or mc.get("package")
    url = args.package_url or mc.get("url")

    # run standalone -> offer to re-import if the map already exists
    return import_named(package, args.carla_root, args.ue4_root,
                        url, args.package_dir, args.force,
                        prompt_if_exists=True, package_pick=args.package_pick,
                        repo=args.repo or mc.get("repo"),
                        tag_prefix=(args.tag_prefix if args.tag_prefix is not None
                                    else mc.get("tag_prefix")))


if __name__ == "__main__":
    sys.exit(main())
