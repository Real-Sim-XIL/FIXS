"""CarlaViews attaches each camera to its target, re-attaches it, and cleans up.

A fake world and fake actors stand in for CARLA, and the Tk display is
replaced, so this opens no window and needs no server.

    python -m pytest tests/Python/unit/test_carla_views.py
"""
from __future__ import annotations

import threading

import numpy as np

import pytest

pytest.importorskip("carla", reason="needs the CARLA PythonAPI")
import carla                                                     # noqa: E402
from Carla.VirEnv import CarlaViews as cv                        # noqa: E402


class _Display:
    def __init__(self, slots):
        self.ready = threading.Event()
        self.error = None
        self.stopped = False
        self.cost = {}

    def start(self):
        self.ready.set()

    def stop(self):
        self.stopped = True


class _Bp:
    def __init__(self):
        self.attrs = {}

    def set_attribute(self, k, v):
        self.attrs[k] = v

    def has_attribute(self, k):
        return k == 'role_name'


class _Camera:
    def __init__(self, parent, tf, attrs):
        self.parent, self.tf, self.attrs = parent, tf, dict(attrs)
        self.callback = None
        self.destroyed = False

    def listen(self, cb):
        self.callback = cb

    def stop(self):
        pass

    def destroy(self):
        self.destroyed = True


class _World:
    def __init__(self):
        self.bp = _Bp()
        self.spawned = []
        self.ticks = 0

    def get_blueprint_library(self):
        world = self

        class _Lib:
            def find(self, _id):
                return world.bp
        return _Lib()

    def spawn_actor(self, bp, tf, attach_to=None, attachment_type=None):
        assert attachment_type == carla.AttachmentType.Rigid
        cam = _Camera(attach_to, tf, bp.attrs)
        self.spawned.append(cam)
        return cam

    def tick(self):
        self.ticks += 1


class _Actor:
    def __init__(self, i):
        self.id = i


VIEW = {"Name": "pov", "Simulator": "carla", "Type": "chase", "Target": "ego", "Enable": True,
        "Implied": False, "Size": [640, 360], "Fov": 100.0, "SensorTick": 0.0,
        "Offset": [-6.5, 0.0, 2.8], "Pitch": -12.0, "Height": 50.0, "AlignYaw": False,
        "Zoom": 600.0, "Boundary": [], "Display": True, "Record": ""}


@pytest.fixture
def views(monkeypatch):
    monkeypatch.setattr(cv, "_Display", _Display)
    world = _World()
    return world, cv.CarlaViews(world, [VIEW, dict(VIEW, Name="off", Enable=False)], 0.05)


def test_camera_follows_its_target_through_respawn_and_absence(views):
    world, v = views
    assert v.active()
    present = {"ego": _Actor(7)}
    v.update(present.get, 1.0)
    v.update(present.get, 1.1)
    assert len(world.spawned) == 1, "one camera, spawned once for an unchanged target"
    cam = world.spawned[0]
    assert cam.parent.id == 7
    assert cam.attrs["image_size_x"] == "640" and cam.attrs["fov"] == "100.0"
    assert cam.attrs["role_name"] == cv.ROLE_PREFIX + "pov"
    loc, rot = cam.tf.location, cam.tf.rotation
    assert (loc.x, loc.y, loc.z, rot.pitch) == pytest.approx((-6.5, 0.0, 2.8, -12.0))

    present["ego"] = _Actor(9)                  # respawned as a new actor
    v.update(present.get, 1.2)
    assert cam.destroyed and world.spawned[-1].parent.id == 9

    del present["ego"]                          # left the network
    v.update(present.get, 1.3)
    assert world.spawned[-1].destroyed and len(world.spawned) == 2

    present["ego"] = _Actor(11)
    v.update(present.get, 1.4)
    assert world.spawned[-1].parent.id == 11
    assert world.ticks == 0, "views must never tick the world"

    v.close()
    assert world.spawned[-1].destroyed


def test_closing_the_window_removes_the_camera(views):
    world, v = views
    v.update({"ego": _Actor(7)}.get, 1.0)
    v._views[0].slot.closed = True
    v.update({"ego": _Actor(7)}.get, 1.1)
    v.update({"ego": _Actor(7)}.get, 1.2)
    assert world.spawned[0].destroyed and len(world.spawned) == 1


def test_no_camera_views_start_nothing(monkeypatch):
    started = []
    monkeypatch.setattr(cv, "_Display", lambda slots: started.append(slots))
    v = cv.CarlaViews(_World(), [dict(VIEW, Type="spectator"), dict(VIEW, Name="x", Display=False)], 0.05)
    assert not v.active() and started == []
    v.update(lambda _t: None, 0.0)
    v.close()


class _Image:
    def __init__(self, frame, w, h, value):
        px = np.zeros((h, w, 4), np.uint8)
        px[..., :3] = value
        px[..., 3] = 255
        self.raw_data, self.width, self.height, self.frame = px.tobytes(), w, h, frame


def test_a_recording_has_one_frame_per_world_tick(monkeypatch, tmp_path):
    """Frames 10, 11, 13 (one missing): four video frames, the video at 1/step fps."""
    cv2 = pytest.importorskip("cv2")
    monkeypatch.setattr(cv, "_Display", _Display)
    path = tmp_path / "out" / "chase.avi"
    world = _World()
    v = cv.CarlaViews(world, [dict(VIEW, Display=False, Record=str(path))], 0.05)
    v.update({"ego": _Actor(7)}.get, 1.0)
    cam = world.spawned[0]
    for frame in (10, 11, 13):
        cam.callback(_Image(frame, 64, 48, 40 * (frame - 9)))
    v.close()
    cap = cv2.VideoCapture(str(path))
    assert cap.get(cv2.CAP_PROP_FPS) == pytest.approx(20.0)
    frames = []
    while True:
        ok, img = cap.read()
        if not ok:
            break
        frames.append(int(img[24, 32, 0]))
    cap.release()
    assert len(frames) == 4
    assert frames[0] == pytest.approx(40, abs=8) and frames[-1] == pytest.approx(160, abs=8)
