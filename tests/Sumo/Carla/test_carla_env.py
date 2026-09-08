"""
Tier-1 tests for the CARLA env setup/config layer (carla_env_setup.py) and the
launch-command resolution in run_cosim.py.

These are pure-stdlib: NO CARLA server, NO GPU, NO map asset, and NO interactive
prompt or GUI (the folder picker is never invoked here). They use a temp config
path and fake CARLA/UE4 trees, so they run on any computer. Run with:

    pytest test_carla_env.py
"""
import os
import platform
import shutil
import sys

import pytest

HERE = os.path.dirname(os.path.abspath(__file__))
# the co-sim runtime lives at the repo root: FIXS_root/Carla
CARLA = os.path.normpath(os.path.join(HERE, "..", "..", "..", "Carla"))
sys.path.insert(0, CARLA)

import carla_env_setup as env  # noqa: E402
import run_cosim  # noqa: E402  (imports carla_env_setup; does NOT import the carla wheel)
import import_map  # noqa: E402  (stdlib + carla_env_setup; no carla wheel)
import place_tls  # noqa: E402  (stdlib + carla_env_setup; no carla wheel)

WIN = platform.system() == "Windows"


# ---------------------------------------------------------------- config io

def test_config_roundtrip(tmp_path, monkeypatch):
    """save_config then load_config returns the same dict."""
    cfg_path = tmp_path / ".fixs" / "carla.json"
    monkeypatch.setattr(env, "CONFIG_DIR", str(cfg_path.parent))
    monkeypatch.setattr(env, "CONFIG_PATH", str(cfg_path))

    cfg = {"mode": "packaged", "carla_root": str(tmp_path / "carla")}
    env.save_config(cfg)
    assert cfg_path.is_file()
    assert env.load_config() == cfg


def test_load_config_missing(tmp_path, monkeypatch):
    """No file -> None (the first-run signal run_cosim keys off)."""
    monkeypatch.setattr(env, "CONFIG_PATH", str(tmp_path / "nope.json"))
    assert env.load_config() is None


def test_load_config_rejects_garbage(tmp_path, monkeypatch):
    """A malformed / incomplete config is treated as 'not configured'."""
    bad = tmp_path / "carla.json"
    bad.write_text('{"mode": "weird"}', encoding="utf-8")
    monkeypatch.setattr(env, "CONFIG_PATH", str(bad))
    assert env.load_config() is None


# --------------------------------------------------------- path resolving

def _make_packaged(root):
    """Create a fake packaged CARLA tree for this OS; return the launcher path."""
    name = "CarlaUE4.exe" if WIN else "CarlaUE4.sh"
    os.makedirs(root, exist_ok=True)
    exe = os.path.join(root, name)
    open(exe, "w").close()
    return exe


def _make_source(tmp_path):
    """Create a fake source CARLA + UE4 tree; return (carla_root, ue4_root, uproject, editor)."""
    carla_root = str(tmp_path / "carla")
    ue4_root = str(tmp_path / "ue4")
    uproject, editor = env.source_paths(carla_root, ue4_root)
    for path in (uproject, editor):
        os.makedirs(os.path.dirname(path), exist_ok=True)
        open(path, "w").close()
    return carla_root, ue4_root, uproject, editor


def test_packaged_exe_found(tmp_path):
    root = str(tmp_path / "carla")
    exe = _make_packaged(root)
    assert env.packaged_exe(root) == exe


def test_packaged_exe_missing(tmp_path):
    assert env.packaged_exe(str(tmp_path / "empty")) is None


def test_source_paths_shape(tmp_path):
    uproject, editor = env.source_paths(str(tmp_path / "carla"), str(tmp_path / "ue4"))
    assert uproject.endswith(os.path.join("Unreal", "CarlaUE4", "CarlaUE4.uproject"))
    assert editor.endswith("UE4Editor.exe" if WIN else "UE4Editor")


# --------------------------------------------- run_cosim launch resolution

def test_carla_command_packaged(tmp_path):
    """Packaged mode resolves to the launcher + rpc-port flag."""
    root = str(tmp_path / "carla")
    exe = _make_packaged(root)
    cfg = {"mode": "packaged", "carla_root": root}
    cmd = run_cosim._carla_command(cfg, 2000, render_offscreen=False)
    assert cmd[0] == exe
    assert "-carla-rpc-port=2000" in cmd


def test_carla_command_offscreen_flag(tmp_path):
    root = str(tmp_path / "carla")
    _make_packaged(root)
    cfg = {"mode": "packaged", "carla_root": root}
    cmd = run_cosim._carla_command(cfg, 2000, render_offscreen=True)
    assert "-RenderOffScreen" in cmd


def test_carla_command_source(tmp_path):
    """Source mode resolves to UE4Editor <uproject> -game."""
    carla_root, ue4_root, uproject, editor = _make_source(tmp_path)

    cfg = {"mode": "source", "carla_root": carla_root, "ue4_root": ue4_root}
    cmd = run_cosim._carla_command(cfg, 2000, render_offscreen=False)
    assert cmd[0] == editor
    assert cmd[1] == uproject
    assert "-game" in cmd


def test_carla_command_source_disables_renderdoc_prompt(tmp_path):
    """Source launches suppress UE4's RenderDoc plugin (#311).

    CARLA's uproject enables RenderDocPlugin, whose loader asks for a renderdoc.dll
    it cannot find by opening a modal file dialog at startup - which blocks the game
    thread until a human cancels it. -DisableFrameTraceCapture returns before the
    search, so the dialog is never reachable.
    """
    carla_root, ue4_root, _, _ = _make_source(tmp_path)

    cfg = {"mode": "source", "carla_root": carla_root, "ue4_root": ue4_root}
    cmd = run_cosim._carla_command(cfg, 2000, render_offscreen=False)
    assert "-DisableFrameTraceCapture" in cmd


def test_carla_command_packaged_carries_no_editor_flags(tmp_path):
    """A packaged build never loads the plugin (UncookedOnly), so it needs no flag."""
    root = str(tmp_path / "carla")
    _make_packaged(root)
    cfg = {"mode": "packaged", "carla_root": root}
    cmd = run_cosim._carla_command(cfg, 2000, render_offscreen=False)
    assert "-DisableFrameTraceCapture" not in cmd


def test_carla_command_packaged_missing_raises(tmp_path):
    cfg = {"mode": "packaged", "carla_root": str(tmp_path / "empty")}
    with pytest.raises(FileNotFoundError):
        run_cosim._carla_command(cfg, 2000, render_offscreen=False)


# ------------------------------------- generic python / wheel resolution

def test_python_can_import_self():
    """The running interpreter can import os (sanity of the subprocess probe)."""
    assert env._python_can_import(sys.executable, ("os", "sys"))
    assert not env._python_can_import(sys.executable, ("a_module_that_does_not_exist_xyz",))


def test_python_candidates_includes_current():
    """Candidate discovery always includes the current interpreter, all real."""
    cands = env._python_candidates()
    assert os.path.normcase(sys.executable) in {os.path.normcase(c) for c in cands}
    assert all(os.path.isfile(c) for c in cands)


def test_python_candidates_deduped_by_real_path():
    """One binary reached under several names (conda's bin/python -> bin/python3)
    is offered once, not once per name."""
    reals = [os.path.normcase(os.path.realpath(c)) for c in env._python_candidates()]
    assert len(reals) == len(set(reals))


def test_python_candidates_offer_system_python():
    """A system interpreter on PATH is a candidate: on Linux it is frequently the
    only one that can import traci/sumolib (apt puts them in dist-packages)."""
    sys_py = shutil.which("python3") or shutil.which("python")
    if not sys_py:
        pytest.skip("no python/python3 on PATH")
    reals = {os.path.normcase(os.path.realpath(c)) for c in env._python_candidates()}
    assert os.path.normcase(os.path.realpath(sys_py)) in reals


def test_interpreter_kind_env_private_base_and_system_shared(tmp_path, monkeypatch):
    """A named conda env is FIXS-private; the base env under the same root and
    anything outside conda are shared (so installs into them are gated)."""
    root = tmp_path / "miniconda3"
    named = env._env_python(str(root / "envs" / "realsim"))
    base = env._env_python(str(root))
    for p in (named, base):
        os.makedirs(os.path.dirname(p), exist_ok=True)
        open(p, "w").close()
    monkeypatch.setattr(env, "_conda_roots", lambda: [str(root)])

    label, shared = env._interpreter_kind(named)
    assert "realsim" in label and shared is False
    label, shared = env._interpreter_kind(base)
    assert "BASE" in label and shared is True
    label, shared = env._interpreter_kind(str(tmp_path / "usr" / "bin" / "python3"))
    assert label == "SYSTEM python" and shared is True


def test_confirm_install_gates_shared_interpreter(monkeypatch, capsys):
    """A private env installs unasked. A shared one must be confirmed, warns why,
    and declines when there is no console to answer on."""
    monkeypatch.setattr(env, "_interpreter_kind", lambda py: ("conda env 'realsim'", False))
    assert env._confirm_install("py", "carla==0.9.15") is True

    monkeypatch.setattr(env, "_interpreter_kind", lambda py: ("SYSTEM python", True))
    monkeypatch.setattr("builtins.input", lambda *_: "y")
    assert env._confirm_install("py", "carla==0.9.15") is True
    monkeypatch.setattr("builtins.input", lambda *_: "")
    assert env._confirm_install("py", "carla==0.9.15") is False
    assert "WARNING" in capsys.readouterr().out

    def _no_console(*_):
        raise EOFError
    monkeypatch.setattr("builtins.input", _no_console)
    assert env._confirm_install("py", "carla==0.9.15") is False


def test_confirm_install_question_always_asks(monkeypatch):
    """An explicit question is put even to a private env - that is how the source
    build's client/server-match reinstall stays opt-in."""
    monkeypatch.setattr(env, "_interpreter_kind", lambda py: ("conda env 'realsim'", False))
    monkeypatch.setattr("builtins.input", lambda *_: "n")
    assert env._confirm_install("py", "wheel", "reinstall? [y/N]: ") is False


def test_find_source_wheel_prefers_tag(tmp_path):
    """Wheel auto-resolution picks one matching the interpreter's cpXY tag."""
    dist = tmp_path / "PythonAPI" / "carla" / "dist"
    dist.mkdir(parents=True)
    tag = env._python_tag(sys.executable)  # e.g. cp310
    (dist / f"carla-0.9.15-{tag}-{tag}-win_amd64.whl").write_text("", encoding="utf-8")
    (dist / "carla-0.9.15-cp38-cp38-win_amd64.whl").write_text("", encoding="utf-8")
    picked = env.find_source_wheel(str(tmp_path), sys.executable)
    assert picked is not None and tag in os.path.basename(picked)


def test_find_source_wheel_absent(tmp_path):
    assert env.find_source_wheel(str(tmp_path / "nope"), sys.executable) is None


def test_reexec_noop_same_interpreter():
    """No re-exec when already on the configured python, or when it is missing /
    unset (must simply return, never SystemExit).

    Lives on carla_env_setup now, not run_cosim: every entry point -- import_map,
    place_tls, load_opendrive_world -- needs the same relaunch, so run_cosim keeping
    a private copy meant which script you started decided which interpreter you got
    (see the note at run_cosim.py:1126). This test still named the old private copy.
    """
    env.reexec_under_configured(__file__, {"python": sys.executable})
    env.reexec_under_configured(__file__,
                                {"python": os.path.join(os.sep, "no", "such", "python")})
    env.reexec_under_configured(__file__, {})


def test_ensure_runtime_noop_when_python_valid(monkeypatch):
    """A config whose python can import carla is returned unchanged - no setup."""
    monkeypatch.setattr(env, "_python_can_import", lambda py, mods: True)
    called = {"resolve": False}
    monkeypatch.setattr(env, "resolve_python",
                        lambda: called.__setitem__("resolve", True) or sys.executable)
    cfg = {"mode": "source", "carla_root": "x", "python": sys.executable}
    out = env.ensure_runtime(dict(cfg))
    assert out == cfg and called["resolve"] is False


def test_frame_from_table_centroid_and_span(tmp_path):
    """TL-table framing: centroid + span, with --no-net-offset mapping y -> -y."""
    csv_path = tmp_path / "tl.csv"
    csv_path.write_text(
        "junction_id,link_id,x,y,z,heading\n"
        "j,0,100,200,10,0\n"
        "j,1,300,400,30,0\n", encoding="utf-8")
    cx, cy, cz, span, anchor = run_cosim._frame_from_table(str(csv_path), no_net_offset=True)
    assert cx == 200.0 and cy == -300.0 and cz == 20.0   # y negated, averaged
    assert span == 200.0                                  # max(200, 200)
    # without no_net_offset, y is kept as-is
    _, cy2, _, _, _ = run_cosim._frame_from_table(str(csv_path), no_net_offset=False)
    assert cy2 == 300.0


def test_frame_from_table_missing_file():
    assert run_cosim._frame_from_table("nope.csv", no_net_offset=True) is None


class _FakeWorld:
    """Only what _frame_from_map touches: world.get_map().get_spawn_points(),
    each spawn exposing .location.x/.y/.z. No carla server involved."""

    class _Loc:
        def __init__(self, x, y, z):
            self.x, self.y, self.z = x, y, z

    class _Spawn:
        def __init__(self, loc):
            self.location = loc

    class _Map:
        def __init__(self, spawns):
            self._spawns = spawns

        def get_spawn_points(self):
            return self._spawns

    def __init__(self, points=(), raises=False):
        self._spawns = [self._Spawn(self._Loc(*p)) for p in points]
        self._raises = raises

    def get_map(self):
        if self._raises:
            raise RuntimeError("map not queryable yet")
        return self._Map(self._spawns)


def test_frame_from_map_centroid_and_span():
    """No-signal fallback: centroid + span of the map's spawn points, so a map with
    no traffic lights still gets framed instead of leaving the camera at the origin."""
    cx, cy, cz, span, anchor = run_cosim._frame_from_map(
        _FakeWorld([(0, 0, 0), (100, 40, 10)]))
    assert (cx, cy, cz) == (50.0, 20.0, 5.0)
    assert span == 100.0                       # max(x-range 100, y-range 40)
    assert "map centre" in anchor and "2 spawn points" in anchor


def test_frame_from_map_no_spawn_points():
    assert run_cosim._frame_from_map(_FakeWorld([])) is None


def test_frame_from_map_unqueryable_map():
    """A server that cannot answer get_map() degrades to 'no framing', not a crash."""
    assert run_cosim._frame_from_map(_FakeWorld(raises=True)) is None


# ---------------------------------------------- map import (no real cook)

def test_map_is_imported_detects_umap(tmp_path):
    """map_is_imported keys off the cooked .umap under the source Content tree."""
    root = str(tmp_path / "carla")
    assert not import_map.map_is_imported(root, "RP_Ver0529")
    umap = import_map.cooked_map_path(root, "RP_Ver0529")
    os.makedirs(os.path.dirname(umap), exist_ok=True)
    open(umap, "w").close()
    assert import_map.map_is_imported(root, "RP_Ver0529")


def test_stage_package_from_local_dir(tmp_path):
    """A local package dir is copied into <carla_root>/Import (descriptor + assets)."""
    carla_root = tmp_path / "carla"
    (carla_root / "Import").mkdir(parents=True)
    pkg = tmp_path / "pkg"
    (pkg / "Assets").mkdir(parents=True)
    (pkg / "RP_Ver0529.json").write_text('{"maps":[]}', encoding="utf-8")
    (pkg / "Assets" / "RP_Ver0529.xodr").write_text("<x/>", encoding="utf-8")
    import_map.stage_package(str(carla_root), "RP_Ver0529", package_dir=str(pkg))
    assert (carla_root / "Import" / "RP_Ver0529.json").is_file()
    assert (carla_root / "Import" / "Assets" / "RP_Ver0529.xodr").is_file()


def test_gh_release_ref_parsing():
    """github release-asset URLs parse to (repo, tag, asset); others -> None."""
    ref = import_map._gh_release_ref(
        "https://github.com/ORNL-Real-Sim/FIXS_Applications/releases/download/"
        "map-RP_Ver0529/RP_Ver0529_carla_import.zip")
    assert ref == ("ORNL-Real-Sim/FIXS_Applications", "map-RP_Ver0529",
                   "RP_Ver0529_carla_import.zip")
    assert import_map._gh_release_ref("https://example.com/foo.zip") is None


def test_stage_from_path_accepts_zip(tmp_path):
    """A hand-downloaded .zip is extracted into Import/ (the manual ORNL path)."""
    src = tmp_path / "pkg"
    (src / "Assets").mkdir(parents=True)
    (src / "RP_Ver0529.json").write_text("{}", encoding="utf-8")
    (src / "Assets" / "RP_Ver0529.xodr").write_text("<x/>", encoding="utf-8")
    zpath = tmp_path / "RP_Ver0529_carla_import.zip"
    with __import__("zipfile").ZipFile(zpath, "w") as z:
        z.write(src / "RP_Ver0529.json", "RP_Ver0529.json")
        z.write(src / "Assets" / "RP_Ver0529.xodr", "Assets/RP_Ver0529.xodr")
    import_dir = tmp_path / "carla" / "Import"
    import_dir.mkdir(parents=True)
    import_map._stage_from_path(str(zpath), str(import_dir))
    assert (import_dir / "RP_Ver0529.json").is_file()
    assert (import_dir / "Assets" / "RP_Ver0529.xodr").is_file()


def test_stage_package_pick_uses_selector_not_gh(monkeypatch, tmp_path):
    """--package-pick forces the manual file selector and never calls gh."""
    carla_root = tmp_path / "carla"
    (carla_root / "Import").mkdir(parents=True)
    pkg = tmp_path / "pkg"
    pkg.mkdir()
    (pkg / "RP_Ver0529.json").write_text("{}", encoding="utf-8")
    monkeypatch.setattr(import_map, "_select_package", lambda name, url: str(pkg))
    monkeypatch.setattr(import_map, "_try_gh_download",
                        lambda url: (_ for _ in ()).throw(AssertionError("gh used!")))
    import_map.stage_package(str(carla_root), "RP_Ver0529",
                             package_url="https://x/y.zip", package_pick=True)
    assert (carla_root / "Import" / "RP_Ver0529.json").is_file()


def test_stage_package_noop_when_already_staged(tmp_path, capsys):
    """If the descriptor is already present and no source is given, it's a no-op."""
    carla_root = tmp_path / "carla"
    (carla_root / "Import").mkdir(parents=True)
    (carla_root / "Import" / "RP_Ver0529.json").write_text("{}", encoding="utf-8")
    import_map.stage_package(str(carla_root), "RP_Ver0529")
    assert "already staged" in capsys.readouterr().out


def test_ensure_map_rejects_packaged(monkeypatch, tmp_path):
    """Importing requires a source build - packaged config is refused."""
    monkeypatch.setattr(import_map.env, "load_config",
                        lambda: {"mode": "packaged", "carla_root": str(tmp_path)})
    with pytest.raises(SystemExit):
        import_map.ensure_map("RP_Ver0529")


def test_ensure_map_noop_when_present(monkeypatch, tmp_path, capsys):
    """Already-imported map short-circuits without cooking."""
    root = tmp_path / "carla"
    umap = import_map.cooked_map_path(str(root), "RP_Ver0529")
    os.makedirs(os.path.dirname(umap), exist_ok=True)
    open(umap, "w").close()
    monkeypatch.setattr(import_map.env, "load_config",
                        lambda: {"mode": "source", "carla_root": str(root),
                                 "ue4_root": str(tmp_path / "ue4")})
    # run_import must NOT be called
    monkeypatch.setattr(import_map, "run_import",
                        lambda *a, **k: (_ for _ in ()).throw(AssertionError("cooked!")))
    assert import_map.ensure_map("RP_Ver0529") == 0
    assert "already imported" in capsys.readouterr().out


def test_ensure_map_force_reimports_when_present(monkeypatch, tmp_path):
    """force=True re-cooks even an already-imported map: it moves the old content
    aside, imports fresh, and reports success when the .umap is produced."""
    root = tmp_path / "carla"
    umap = import_map.cooked_map_path(str(root), "RP_Ver0529")
    os.makedirs(os.path.dirname(umap), exist_ok=True)
    open(umap, "w").close()
    monkeypatch.setattr(import_map.env, "load_config",
                        lambda: {"mode": "source", "carla_root": str(root),
                                 "ue4_root": str(tmp_path / "ue4")})
    monkeypatch.setattr(import_map, "stage_package", lambda *a, **k: None)
    called = {"import": False}

    def fake_import(cr, ue, nm):  # simulate a successful cook re-creating the umap
        called["import"] = True
        os.makedirs(os.path.dirname(umap), exist_ok=True)
        with open(umap, "w") as f:
            f.write("fresh")
        return 0

    monkeypatch.setattr(import_map, "run_import", fake_import)
    assert import_map.ensure_map("RP_Ver0529", force=True) == 0
    assert called["import"] is True
    assert os.path.isfile(umap)
    assert not os.path.isdir(import_map.cooked_content_dir(str(root), "RP_Ver0529") + ".bak_reimport")


def test_ensure_map_restores_backup_on_failed_reimport(monkeypatch, tmp_path):
    """If the re-cook fails to produce the umap, the previous map is restored."""
    root = tmp_path / "carla"
    umap = import_map.cooked_map_path(str(root), "RP_Ver0529")
    os.makedirs(os.path.dirname(umap), exist_ok=True)
    open(umap, "w").close()
    monkeypatch.setattr(import_map.env, "load_config",
                        lambda: {"mode": "source", "carla_root": str(root),
                                 "ue4_root": str(tmp_path / "ue4")})
    monkeypatch.setattr(import_map, "stage_package", lambda *a, **k: None)
    monkeypatch.setattr(import_map, "run_import", lambda *a, **k: 1)  # cook fails, no umap
    with pytest.raises(SystemExit):
        import_map.ensure_map("RP_Ver0529", force=True)
    assert os.path.isfile(umap)  # restored


def test_read_map_config(tmp_path):
    """A map.txt declares the package + url for the wrappers."""
    p = tmp_path / "map.txt"
    p.write_text("# the roosevelt map\npackage=RP_Ver0529\n"
                 "url=https://x/y.zip\n\n", encoding="utf-8")
    mc = import_map.read_map_config(str(p))
    assert mc["package"] == "RP_Ver0529" and mc["url"] == "https://x/y.zip"


# ----------------------------------------------- traffic-light placement

def test_tls_content_path_and_marker(tmp_path):
    """Content path + placement marker resolve under the map's cooked content."""
    assert place_tls.content_map_path("RP_Ver0529") == "/Game/RP_Ver0529/Maps/RP_Ver0529/RP_Ver0529"
    root = str(tmp_path / "carla")
    assert not place_tls.tls_placed(root, "RP_Ver0529")
    marker = place_tls.tls_marker(root, "RP_Ver0529")
    os.makedirs(os.path.dirname(marker), exist_ok=True)
    open(marker, "w").close()
    assert place_tls.tls_placed(root, "RP_Ver0529")


def test_place_tls_noop_when_marker_present(monkeypatch, tmp_path, capsys):
    """Already-placed (marker) short-circuits without launching the editor."""
    root = tmp_path / "carla"
    # map present
    umap = import_map.cooked_map_path(str(root), "RP_Ver0529")
    os.makedirs(os.path.dirname(umap), exist_ok=True)
    open(umap, "w").close()
    # marker present
    open(place_tls.tls_marker(str(root), "RP_Ver0529"), "w").close()
    table = tmp_path / "tl.csv"
    table.write_text("junction_id,link_id,x,y,z,heading\n", encoding="utf-8")
    monkeypatch.setattr(place_tls.env, "load_config",
                        lambda: {"mode": "source", "carla_root": str(root),
                                 "ue4_root": str(tmp_path / "ue4")})
    monkeypatch.setattr(place_tls.subprocess, "call",
                        lambda *a, **k: (_ for _ in ()).throw(AssertionError("editor launched!")))
    assert place_tls.place_tls("RP_Ver0529", str(table)) == 0
    assert "already placed" in capsys.readouterr().out


def test_place_tls_rejects_packaged(monkeypatch, tmp_path):
    """Placement needs a source build (it saves the umap via the editor)."""
    table = tmp_path / "tl.csv"
    table.write_text("x\n", encoding="utf-8")
    monkeypatch.setattr(place_tls.env, "load_config",
                        lambda: {"mode": "packaged", "carla_root": str(tmp_path)})
    with pytest.raises(SystemExit):
        place_tls.place_tls("RP_Ver0529", str(table))


def test_frame_from_table_picks_busiest_junction(tmp_path):
    """Default framing zooms to the junction with the most signal heads."""
    csv_path = tmp_path / "tl.csv"
    csv_path.write_text(
        "junction_id,link_id,x,y,z,heading\n"
        "A,0,0,0,0,0\n"                       # junction A: 1 head, far away
        "B,0,1000,1000,5,0\n"                 # junction B: 3 heads (busiest)
        "B,1,1010,1000,5,0\n"
        "B,2,1020,1000,5,0\n", encoding="utf-8")
    cx, cy, cz, span, anchor = run_cosim._frame_from_table(str(csv_path), no_net_offset=True)
    assert cx == 1010.0 and cy == -1000.0   # centred on B, not the A/B centroid
    assert span == 20.0 and "B" in anchor    # tight span -> close view

    # whole=True frames everything instead
    _, _, _, span_all, anchor_all = run_cosim._frame_from_table(
        str(csv_path), no_net_offset=True, whole=True)
    assert span_all == 1020.0 and "network" in anchor_all


def test_ensure_runtime_repairs_missing_python(monkeypatch, tmp_path):
    """A stale config without a usable python is repaired via resolve_python +
    ensure_carla, and the result is persisted (CARLA paths preserved)."""
    monkeypatch.setattr(env, "_python_can_import", lambda py, mods: False)
    monkeypatch.setattr(env, "resolve_python", lambda: sys.executable)
    monkeypatch.setattr(env, "ensure_carla", lambda py, mode, root: None)
    saved = {}
    monkeypatch.setattr(env, "save_config", lambda c: saved.update(c))
    cfg = {"mode": "source", "carla_root": "C:/src_ext/Carla", "ue4_root": "C:/ue4"}
    out = env.ensure_runtime(dict(cfg))
    assert out["python"] == sys.executable
    assert out["carla_root"] == "C:/src_ext/Carla" and out["ue4_root"] == "C:/ue4"
    assert saved.get("python") == sys.executable  # persisted


# --------------------------------------------------------------- --purge-map

def _fake_map(carla_root, name, cooked=True, umap=True, staged=True, cache_root=None,
              cached=False):
    """Lay down the on-disk traces one imported map leaves, each half optional, so
    a test can build the half-states a purge exists to clean up."""
    if cooked:
        cfg = import_map.package_descriptor(carla_root, name)
        os.makedirs(os.path.dirname(cfg), exist_ok=True)
        with open(cfg, "w", encoding="utf-8") as f:
            f.write("{}")
        if umap:
            path = import_map.cooked_map_path(carla_root, name)
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "w", encoding="utf-8") as f:
                f.write("umap")
    if staged:
        stage = os.path.join(carla_root, "Import", name)
        os.makedirs(stage, exist_ok=True)
        with open(os.path.join(stage, name + ".fbx"), "w", encoding="utf-8") as f:
            f.write("fbx")
        with open(os.path.join(carla_root, "Import", name + ".json"),
                  "w", encoding="utf-8") as f:
            f.write("{}")
    if cached and cache_root:
        sumo = os.path.join(cache_root, name, "sumo")
        os.makedirs(sumo, exist_ok=True)
        with open(os.path.join(sumo, name + ".sumocfg"), "w", encoding="utf-8") as f:
            f.write("<configuration/>")


def _labels(record):
    return sorted(label for label, _p, _b in record["pieces"])


def test_purge_candidates_skips_carlas_own_content(tmp_path, monkeypatch):
    """CARLA's own Content/Carla/ ships a Config/Carla.Package.json exactly like an
    imported package does, so the descriptor alone cannot be the test - the engine's
    own content must never be offered for deletion."""
    monkeypatch.setenv("FIXS_MAP_CACHE", str(tmp_path / "cache"))
    root = str(tmp_path / "carla")
    _fake_map(root, "Carla", umap=False, staged=False)       # the engine's own
    _fake_map(root, "mlk_no_signal", cache_root=str(tmp_path / "cache"))
    names = [r["name"] for r in import_map.purge_candidates(root, mode="source")]
    assert names == ["mlk_no_signal"]


def test_purge_candidates_does_not_create_the_cache_dir(tmp_path, monkeypatch):
    """Taking an inventory must not bring into being the thing it reports on:
    _map_cache_dir(name) CREATES its folder, so using it here would invent an empty
    cache for every map and then offer it up."""
    cache = tmp_path / "cache"
    monkeypatch.setenv("FIXS_MAP_CACHE", str(cache))
    root = str(tmp_path / "carla")
    _fake_map(root, "mlk_no_signal")
    records = import_map.purge_candidates(root, mode="source")
    assert not (cache / "mlk_no_signal").exists()
    assert "cache" not in _labels(records[0])


def test_purge_candidates_offers_a_cook_that_died_partway(tmp_path, monkeypatch):
    """Content/<name>/ with a descriptor but no .umap is a crashed cook - exactly
    the wreckage this command exists to clear - so it is listed, and flagged."""
    monkeypatch.setenv("FIXS_MAP_CACHE", str(tmp_path / "cache"))
    root = str(tmp_path / "carla")
    _fake_map(root, "half_cooked", umap=False)
    records = import_map.purge_candidates(root, mode="source")
    assert [r["name"] for r in records] == ["half_cooked"]
    assert records[0]["cooked_umap"] is False


def test_purge_candidates_ignores_a_lone_descriptor(tmp_path, monkeypatch):
    """CARLA's Import/ holds descriptors that name no map (roadpainter_decals.json).
    A .json with no folder beside it must not conjure a purge candidate."""
    monkeypatch.setenv("FIXS_MAP_CACHE", str(tmp_path / "cache"))
    root = str(tmp_path / "carla")
    os.makedirs(os.path.join(root, "Import"), exist_ok=True)
    with open(os.path.join(root, "Import", "roadpainter_decals.json"),
              "w", encoding="utf-8") as f:
        f.write("{}")
    assert import_map.purge_candidates(root, mode="source") == []


def test_purge_candidates_finds_a_cache_with_no_carla(tmp_path, monkeypatch):
    """carla_root=None is a valid call: a client-mode machine has no local CARLA but
    still has a bundle cache, and reclaiming it is the point of asking."""
    cache = tmp_path / "cache"
    monkeypatch.setenv("FIXS_MAP_CACHE", str(cache))
    _fake_map(None, "x", cooked=False, staged=False)   # no CARLA halves at all
    os.makedirs(str(cache / "roosevelt_full" / "sumo"), exist_ok=True)
    with open(str(cache / "roosevelt_full" / "sumo" / "r.sumocfg"),
              "w", encoding="utf-8") as f:
        f.write("<configuration/>")
    records = import_map.purge_candidates(None, mode="client")
    assert [r["name"] for r in records] == ["roosevelt_full"]
    assert _labels(records[0]) == ["cache"]


def test_purge_sweeps_the_reimport_backup(tmp_path, monkeypatch):
    """A failed re-import leaves Content/<name>.bak_reimport - a full second copy of
    the map. Purging the map without it would strand that copy forever."""
    monkeypatch.setenv("FIXS_MAP_CACHE", str(tmp_path / "cache"))
    root = str(tmp_path / "carla")
    _fake_map(root, "mlk_no_signal")
    backup = import_map.cooked_content_dir(root, "mlk_no_signal") + ".bak_reimport"
    os.makedirs(backup, exist_ok=True)
    with open(os.path.join(backup, "old.uasset"), "w", encoding="utf-8") as f:
        f.write("old")
    records = import_map.purge_candidates(root, mode="source")
    assert backup in [p for _l, p, _b in records[0]["pieces"]]
    import_map.purge_maps(records)
    assert not os.path.exists(backup)


def test_purge_keeps_the_cache_unless_asked(tmp_path, monkeypatch):
    """The default deletes the CARLA halves only. The bundle cache costs a download
    to rebuild AND is read at run time (the .sumocfg lives there), so it takes its
    own yes."""
    cache = tmp_path / "cache"
    monkeypatch.setenv("FIXS_MAP_CACHE", str(cache))
    root = str(tmp_path / "carla")
    _fake_map(root, "mlk_no_signal", cache_root=str(cache), cached=True)
    records = import_map.purge_candidates(root, mode="source")
    assert _labels(records[0]) == ["cache", "cooked", "staged", "staged"]

    removed, failed = import_map.purge_maps(records)
    assert failed == []
    assert not os.path.isdir(import_map.cooked_content_dir(root, "mlk_no_signal"))
    assert not os.path.exists(os.path.join(root, "Import", "mlk_no_signal"))
    assert not os.path.exists(os.path.join(root, "Import", "mlk_no_signal.json"))
    assert (cache / "mlk_no_signal" / "sumo" / "mlk_no_signal.sumocfg").exists()
    assert all(label != "cache" for _n, label, _p, _b in removed)


def test_purge_drops_the_cache_when_asked(tmp_path, monkeypatch):
    cache = tmp_path / "cache"
    monkeypatch.setenv("FIXS_MAP_CACHE", str(cache))
    root = str(tmp_path / "carla")
    _fake_map(root, "mlk_no_signal", cache_root=str(cache), cached=True)
    records = import_map.purge_candidates(root, mode="source")
    _removed, failed = import_map.purge_maps(records, drop_cache=True)
    assert failed == []
    assert not (cache / "mlk_no_signal").exists()


def test_purge_reports_a_failure_instead_of_raising(tmp_path, monkeypatch):
    """The inventory is taken before the deletion, so a path can vanish in between.
    That must be reported per item, not abort the whole purge."""
    monkeypatch.setenv("FIXS_MAP_CACHE", str(tmp_path / "cache"))
    root = str(tmp_path / "carla")
    _fake_map(root, "a")
    _fake_map(root, "b")
    records = import_map.purge_candidates(root, mode="source")
    shutil.rmtree(import_map.cooked_content_dir(root, "a"))   # vanishes underneath
    removed, failed = import_map.purge_maps(records)
    assert [name for name, _l, _p, _e in failed] == ["a"]
    assert not os.path.isdir(import_map.cooked_content_dir(root, "b"))   # b still done


def test_record_bytes_excludes_the_cache_by_default(tmp_path, monkeypatch):
    cache = tmp_path / "cache"
    monkeypatch.setenv("FIXS_MAP_CACHE", str(cache))
    root = str(tmp_path / "carla")
    _fake_map(root, "m", cache_root=str(cache), cached=True)
    record = import_map.purge_candidates(root, mode="source")[0]
    assert import_map.record_bytes(record) < import_map.record_bytes(record, True)


@pytest.mark.parametrize("answer,expected", [
    ("2", [1]),
    ("1,3,4", [0, 2, 3]),
    (" 1 , 3 ", [0, 2]),
    ("2-4", [1, 2, 3]),
    ("1,3-5", [0, 2, 3, 4]),
    ("all", [0, 1, 2, 3, 4]),
    ("a", [0, 1, 2, 3, 4]),
    ("3,3", [2]),
    ("1,", [0]),        # a trailing comma has one reading; do not re-ask over it
])
def test_parse_selection_accepts(answer, expected):
    assert import_map._parse_selection(answer, 5) == expected


@pytest.mark.parametrize("answer", ["", "0", "6", "4-2", "1-9", "x", "1,x", "-"])
def test_parse_selection_rejects(answer):
    """None means re-ask. Nothing here may be guessed at: the next step deletes."""
    assert import_map._parse_selection(answer, 5) is None
