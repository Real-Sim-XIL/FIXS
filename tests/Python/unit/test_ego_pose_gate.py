"""A backend reports the ego's pose only once it has one.

`try_spawn_actor` returns an actor immediately, but CARLA has not snapshotted
it yet: its get_transform() reads the ORIGIN until the next tick. Measured, the
first readback of a deferred ego gave (2.396, 0) -- the front-bumper offset of a
vehicle at the map origin -- while the ego had been spawned 933 m away at
(152.16, 921.20).

Anything downstream that treats that as a pose commands from somewhere the ego
has never been. The controller used to recognise it by SIZE, which only works
while the origin happens to be far from the route; the backend knows it as a
fact, so it says so instead.

    python -m pytest tests/Python/unit/test_ego_pose_gate.py
"""
from __future__ import annotations

import pytest

from CommonLib.VirEnv.IVirEnvBackend import EgoState

pytest.importorskip("carla", reason="needs the CARLA PythonAPI")
from Carla.VirEnv.CarlaBackend import CarlaBackend            # noqa: E402


class _Actor:
    """A CARLA actor whose transform is the origin until CARLA ticks."""

    id = 375

    def __init__(self):
        import carla
        self._carla = carla
        self.placed = False
        self.bounding_box = carla.BoundingBox(
            carla.Location(0, 0, 0), carla.Vector3D(2.396, 1.0, 0.75))

    def get_transform(self):
        c = self._carla
        if not self.placed:
            return c.Transform(c.Location(0.0, 0.0, 0.0), c.Rotation())
        return c.Transform(c.Location(152.16, -921.20, 205.29),
                           c.Rotation(yaw=23.36))

    def get_velocity(self):
        import carla
        return carla.Vector3D(0.0, 0.0, 0.0)


def _backendWithSpawnedEgo():
    b = CarlaBackend.__new__(CarlaBackend)
    b._egoActor = _Actor()
    b._egoAwaitingSnapshot = True            # what spawnEgo leaves behind
    return b


def test_no_pose_is_reported_before_carla_has_one():
    b = _backendWithSpawnedEgo()
    assert b.readEgoState('ego', EgoState()) is False


def test_the_pose_is_reported_once_a_tick_has_landed():
    b = _backendWithSpawnedEgo()
    b._egoActor.placed = True
    b.noteWorldTicked()
    out = EgoState()
    assert b.readEgoState('ego', out) is True
    assert out.x == pytest.approx(152.16, abs=3.0)


def test_a_backend_with_no_ego_still_reports_nothing():
    b = CarlaBackend.__new__(CarlaBackend)
    b._egoActor = None
    b._egoAwaitingSnapshot = False
    assert b.readEgoState('ego', EgoState()) is False
