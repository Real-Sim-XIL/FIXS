"""The HUD on ViewSetup camera frames: what is drawn, where, and from which values.

    python -m pytest tests/Python/unit/test_carla_hud.py
"""
from __future__ import annotations

import time

import numpy as np
import pytest

pytest.importorskip("PIL", reason="the HUD draws with Pillow")
from Carla.VirEnv.CarlaHud import (dialFraction, drawHud, litLamps, panelBox,   # noqa: E402
                                   unit01, visibleElements)

ALL = ("signal", "speed", "pedals", "advisory")
FULL = dict(speed=12.4, advisory=11.2, throttle=0.35, brake=0.0, signalColor=3, signalDistance=85.0)


@pytest.mark.parametrize("sample,overlay,shown", [
    (FULL, ALL, {"signal", "speed", "pedals", "advisory"}),
    (FULL, (), set()),
    (None, ALL, set()),
    (FULL, ("speed",), {"speed"}),
    (dict(FULL, signalColor=-1, signalDistance=-1.0), ALL, {"speed", "pedals", "advisory"}),  # no signal ahead
    (dict(FULL, signalDistance=None), ALL, {"speed", "pedals", "advisory"}),                    # field not on the wire
    (dict(FULL, throttle=None, brake=None), ALL, {"signal", "speed", "advisory"}),             # mirrored ego
    (dict(FULL, advisory=-1.0), ALL, {"signal", "speed", "pedals"}),                           # no advisory sent
    (dict(FULL, advisory=None), ALL, {"signal", "speed", "pedals"}),
    (dict(FULL, speed=None), ALL, {"signal", "pedals"}),                                       # no dial, no mark on it
    (FULL, ("advisory",), set()),
])
def test_an_element_is_drawn_only_when_listed_and_available(sample, overlay, shown):
    assert visibleElements(sample, overlay) == shown


@pytest.mark.parametrize("color,lit", [
    (1, ("red",)), (2, ("amber",)), (3, ("green",)), (4, ("red", "amber")), (7, ("red",)),
    (0, ()), (5, ()), (6, ()), (-1, ()),
])
def test_signal_color_code_lights_the_right_lamps(color, lit):
    assert litLamps(color) == lit


def test_values_are_clamped():
    assert dialFraction(-3, 40) == 0.0
    assert dialFraction(20, 40) == 0.5
    assert dialFraction(55, 40) == 1.0
    assert dialFraction(10, 0) == 0.0
    assert (unit01(-0.2), unit01(0.4), unit01(1.7)) == (0.0, 0.4, 1.0)


@pytest.mark.parametrize("w,h", [(960, 540), (1280, 720), (1920, 1080), (640, 360), (800, 600), (400, 300)])
def test_the_hud_stays_inside_the_top_right_of_the_frame(w, h):
    x0, y0, x1, y1, s = panelBox(w, h)
    assert 0 <= x0 < x1 < w and 0 <= y0 < y1 < h
    assert x1 > w * 0.6 and y1 < h * 0.5, "top-right"
    frame = np.full((h, w, 3), 100, np.uint8)
    drawn = drawHud(frame, dict(FULL, speed=80.0, advisory=90.0, throttle=1.5, brake=-1), ALL)
    assert drawn == set(ALL)
    changed = np.argwhere((frame != 100).any(axis=2))
    assert changed[:, 0].min() >= y0 and changed[:, 0].max() <= y1
    assert changed[:, 1].min() >= x0 and changed[:, 1].max() <= x1


def test_the_panel_matches_the_design_at_960x540():
    assert panelBox(960, 540)[:4] == (666, 8, 952, 186)


def test_nothing_listed_or_nothing_available_leaves_the_frame_alone():
    frame = np.full((540, 960, 3), 100, np.uint8)
    assert drawHud(frame, FULL, ()) == set()
    assert drawHud(frame, {}, ALL) == set()
    assert (frame == 100).all()


def test_lit_lamp_color_is_in_the_drawn_pixels():
    frame = np.zeros((540, 960, 3), np.uint8)
    drawHud(frame, dict(FULL, signalColor=1), ("signal",))
    x0, y0, _, _, _ = panelBox(960, 540)
    b, g, r = frame[y0 + 10 + 22, x0 + 14 + 23]          # centre of the top (red) lamp
    assert r > 200 and g < 80 and b < 80


def test_draw_cost_at_960x540():
    frame = np.full((540, 960, 3), 100, np.uint8)
    drawHud(frame, FULL, ALL)                            # fonts load once
    t = time.perf_counter()
    for _ in range(50):
        drawHud(frame, FULL, ALL)
    ms = (time.perf_counter() - t) * 1000 / 50
    assert ms < 15, "HUD draw %.1f ms per frame" % ms
