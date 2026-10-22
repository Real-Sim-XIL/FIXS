"""CarlaViews attaches each camera to its target, re-attaches it, and cleans up.

A fake world and fake actors stand in for CARLA, and the Tk display is
replaced, so this opens no window and needs no server.

    python -m pytest tests/Python/unit/test_carla_views.py
"""
from __future__ import annotations

import threading

import pytest

pytest.importorskip("carla", reason="needs the CARLA PythonAPI")
import carla                                                     # noqa: E402
from Carla.VirEnv import CarlaViews as cv                        # noqa: E402


class _Display:
    def __init__(self, slots):
        self.ready = threading.Event()
        self.error = None
        self.stopped = False

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
        "Zoom": 600.0, "Boundary": []}


@pytest.fixture
def views(monkeypatch):
    monkeypatch.setattr(cv, "_Display", _Display)
    world = _World()
    return world, cv.CarlaViews(world, [VIEW, dict(VIEW, Name="off", Enable=False)])


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
    v = cv.CarlaViews(_World(), [dict(VIEW, Type="spectator")])
    assert not v.active() and started == []
    v.update(lambda _t: None, 0.0)
    v.close()
