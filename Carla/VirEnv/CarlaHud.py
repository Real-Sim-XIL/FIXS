"""The HUD drawn onto a ViewSetup camera frame: traffic light, pedals and speed dial.

Geometry is defined at 960x540 and scaled to the view's Size. The panel sits in
the top-right of the frame. Only the listed elements with data for that world
frame are drawn; with none, nothing is.
"""

import functools
import math
import os

import numpy as np

ELEMENTS = ('signal', 'speed', 'pedals', 'advisory')
UNITS = {'mph': 2.2369362920544, 'kmh': 3.6}
UNIT_LABEL = {'mph': 'mph', 'kmh': 'km/h'}
DEFAULT_SPEED_MAX = {'mph': 40.0, 'kmh': 60.0}

#: Panel at 960x540: x0, y0, x1, y1 of the rounded rectangle.
_PANEL = (666, 8, 952, 186)
_REF_W, _REF_H = 960.0, 540.0
_LAMPS = (('red', (235, 45, 40)), ('amber', (250, 175, 15)), ('green', (35, 215, 90)))

#: FIXS signalLightColor (TrafficHelper.cpp tlsStateToColor) -> the lamps lit.
#: 1 r, 2 y, 3 g/G, 4 u (red+amber), 7 s (stop); 0 unknown, 5 o / 6 O off: housing, no lamp.
_LIT = {1: ('red',), 2: ('amber',), 3: ('green',), 4: ('red', 'amber'), 7: ('red',)}


def litLamps(color):
    """FIXS signalLightColor -> names of the lamps to light."""
    return _LIT.get(color, ())


def visibleElements(sample, overlay):
    """The overlay elements that are listed and have data in ``sample`` (a dict, or None)."""
    if not sample or not overlay:
        return set()
    out = set()
    color, dist = sample.get('signalColor'), sample.get('signalDistance')
    if 'signal' in overlay and color is not None and dist is not None and color >= 0 and dist >= 0:
        out.add('signal')
    if 'speed' in overlay and sample.get('speed') is not None:
        out.add('speed')
    adv = sample.get('advisory')
    if 'advisory' in overlay and 'speed' in out and adv is not None and adv > 0:
        out.add('advisory')
    if 'pedals' in overlay and sample.get('throttle') is not None and sample.get('brake') is not None:
        out.add('pedals')
    return out


def dialFraction(value, vmax):
    """Share of the 270-degree sweep for ``value`` on a 0..vmax dial."""
    if vmax <= 0:
        return 0.0
    return min(1.0, max(0.0, value / vmax))


def unit01(value):
    return min(1.0, max(0.0, float(value)))


def panelBox(width, height):
    """(x0, y0, x1, y1, scale) of the HUD panel in a width x height frame."""
    s = min(width / _REF_W, height / _REF_H)
    x1 = width - (_REF_W - _PANEL[2]) * s
    return (int(round(x1 - (_PANEL[2] - _PANEL[0]) * s)), int(round(_PANEL[1] * s)),
            int(round(x1)), int(round(_PANEL[3] * s)), s)


@functools.lru_cache(maxsize=32)
def _font(bold, size):
    from PIL import ImageFont
    size = max(6, int(round(size)))
    names = ('segoeuib.ttf', 'arialbd.ttf', 'DejaVuSans-Bold.ttf') if bold else \
            ('segoeui.ttf', 'arial.ttf', 'DejaVuSans.ttf')
    dirs = [os.path.join(os.environ.get('WINDIR', r'C:\Windows'), 'Fonts'),
            '/usr/share/fonts/truetype/dejavu', '']
    for name in names:
        for d in dirs:
            try:
                return ImageFont.truetype(os.path.join(d, name) if d else name, size)
            except OSError:
                continue
    return ImageFont.load_default(size)


def drawHud(bgr, sample, overlay, units='mph', vmax=None):
    """Draw the HUD in place on an (h, w, 3) BGR frame. -> the elements drawn."""
    shown = visibleElements(sample, overlay)
    if not shown:
        return shown
    from PIL import Image, ImageDraw
    h, w = bgr.shape[:2]
    X0, Y0, X1, Y1, s = panelBox(w, h)
    vmax = float(vmax or DEFAULT_SPEED_MAX[units])
    region = np.ascontiguousarray(bgr[Y0:Y1 + 1, X0:X1 + 1, ::-1])
    img = Image.fromarray(region, 'RGB')
    d = ImageDraw.Draw(img, 'RGBA')

    # Panel-local coordinates of the 960x540 design, scaled.
    def P(x, y):
        return (x - _PANEL[0]) * s, (y - _PANEL[1]) * s

    x0, y0, x1, y1 = _PANEL
    d.rounded_rectangle([0, 0, (x1 - x0) * s, (y1 - y0) * s], 14 * s, fill=(12, 14, 18, 185))

    if 'signal' in shown:
        hx, hy, hw, hh = x0 + 14, y0 + 10, 46, 126
        d.rounded_rectangle([*P(hx, hy), *P(hx + hw, hy + hh)], 10 * s, fill=(30, 32, 37, 255),
                            outline=(85, 89, 97, 255), width=max(1, int(round(2 * s))))
        lit = litLamps(int(sample['signalColor']))
        for i, (name, col) in enumerate(_LAMPS):
            cy = hy + 22 + i * 41
            if name in lit:
                d.ellipse([*P(hx + 2, cy - 21), *P(hx + hw - 2, cy + 21)], fill=col + (80,))
                d.ellipse([*P(hx + 8, cy - 15), *P(hx + hw - 8, cy + 15)], fill=col + (255,))
            else:
                d.ellipse([*P(hx + 8, cy - 15), *P(hx + hw - 8, cy + 15)],
                          fill=tuple(int(v * 0.22) for v in col) + (255,))
        d.text(P(hx + hw / 2, hy + hh + 22), '%d m' % int(round(sample['signalDistance'])),
               font=_font(True, 26 * s), fill=(255, 255, 255, 255), anchor='mm')

    cx, cy, r = x1 - 80, y0 + 90, 66
    if 'pedals' in shown:
        bx = cx - r - 34
        top, bottom = y0 + 30, y1 - 30
        for k, (val, col) in enumerate(((sample['throttle'], (70, 200, 110)), (sample['brake'], (235, 70, 60)))):
            x = bx + k * 13
            d.rectangle([*P(x, top), *P(x + 9, bottom)], fill=(55, 60, 68, 255))
            v = unit01(val)
            if v > 0:
                d.rectangle([*P(x, bottom - (bottom - top) * v), *P(x + 9, bottom)], fill=col + (255,))

    if 'speed' in shown:
        speed = sample['speed'] * UNITS[units]
        a0 = 135
        C = P(cx, cy)
        R = r * s

        def at(radius, deg):
            a = math.radians(deg)
            return C[0] + radius * s * math.cos(a), C[1] + radius * s * math.sin(a)

        d.ellipse([C[0] - R - 6 * s, C[1] - R - 6 * s, C[0] + R + 6 * s, C[1] + R + 6 * s], fill=(22, 24, 29, 255))
        arcW = max(1, int(round(7 * s)))
        d.arc([C[0] - R, C[1] - R, C[0] + R, C[1] + R], a0, a0 + 270, fill=(80, 85, 95, 255), width=arcW)
        frac = dialFraction(speed, vmax)
        if frac > 0:
            d.arc([C[0] - R, C[1] - R, C[0] + R, C[1] + R], a0, a0 + 270 * frac,
                  fill=(235, 238, 242, 255), width=arcW)
        for v in range(0, int(vmax) + 1, 10):
            deg = a0 + 270 * v / vmax
            d.line([at(r - 12, deg), at(r - 22, deg)], fill=(200, 205, 212, 255), width=max(1, int(round(2 * s))))
        if 'advisory' in shown:
            deg = a0 + 270 * dialFraction(sample['advisory'] * UNITS[units], vmax)
            d.polygon([at(r - 4, deg), at(r + 12, deg - math.degrees(0.12)), at(r + 12, deg + math.degrees(0.12))],
                      fill=(255, 190, 0, 255))
        deg = a0 + 270 * frac
        d.line([C, at(r - 16, deg)], fill=(255, 75, 60, 255), width=max(1, int(round(4 * s))))
        d.ellipse([C[0] - 6 * s, C[1] - 6 * s, C[0] + 6 * s, C[1] + 6 * s], fill=(255, 75, 60, 255))
        d.text((C[0], C[1] + 30 * s), '%d' % int(round(max(0.0, speed))), font=_font(True, 30 * s),
               fill=(255, 255, 255, 255), anchor='mm')
        d.text((C[0], C[1] + 54 * s), UNIT_LABEL[units], font=_font(False, 14 * s),
               fill=(190, 195, 205, 255), anchor='mm')

    bgr[Y0:Y1 + 1, X0:X1 + 1] = np.asarray(img)[:, :, ::-1]
    return shown
