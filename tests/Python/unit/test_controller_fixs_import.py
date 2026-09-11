"""`import fixs` inside a controller must reach THIS process's fixs.

A controller is a loose .py on sys.path, so a bare `import fixs` finds the same
FILE the engine already imported as `CommonLib.fixs` and executes it a SECOND
time. The two module objects then have separate globals, and the feed is
advanced on only one of them: every read from the controller's copy returns
nothing, forever, with no error -- an agent handed that drives an empty world
for the whole run while the road is full.

    python -m pytest tests/Python/unit/test_controller_fixs_import.py
"""
from __future__ import annotations

import sys

import CommonLib.fixs
from CommonLib.VirEnv import EgoControllerHost as host


def test_fixs_resolves_to_the_module_the_engine_feeds():
    host._letControllerImportFixs()
    assert sys.modules['fixs'] is CommonLib.fixs


def test_the_relay_resolves_to_the_same_one():
    host._letControllerImportFixs()
    import CommonLib.fixs.carla
    assert sys.modules['fixs.carla'] is CommonLib.fixs.carla


def test_an_import_executed_by_a_controller_sees_the_live_records():
    """What the controller actually writes: `import fixs` then read the feed.
    The object it gets must be the one the engine mutates, not a twin."""
    host._letControllerImportFixs()
    import fixs
    marker = object()
    CommonLib.fixs._aliasProbe = marker
    try:
        assert getattr(fixs, '_aliasProbe', None) is marker
    finally:
        del CommonLib.fixs._aliasProbe
