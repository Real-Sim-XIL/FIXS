#!/usr/bin/env python
"""Run CARLA's BasicAgent against a FIXS record instead of a live CARLA world.

CARLA's agent is already a pure function of (ego state, world) -> VehicleControl:
PythonAPI/examples/automatic_control.py calls `agent.run_step()` and then
`player.apply_control(control)` itself, so the agent never touches the vehicle.
That is what makes this possible -- both ends can be replaced without touching
the control law.

    automatic_control.py                    a FIXS client
    ------------------------------------    -----------------------------------
    world.player (a live carla.Vehicle) ->  the ego's FIXS record, via EgoAdapter
    agent.set_destination(loc)          ->  EgoRoutePoints -> set_global_plan
    agent.run_step()                    ->  IDENTICAL, from agents/ next door
    player.apply_control(control)       ->  3 fields on the wire; VirCarlaEnv
                                            calls apply_control on the physics ego

Everything in agents/ is vendored BYTE-IDENTICAL from CARLA (see VERSION.txt).
This file holds every FIXS-specific line there is, and there are three kinds:

  EgoAdapter      stands in for `world.player`, and converts frames
  FixsBasicAgent  BasicAgent with its two DETECTORS overridden -- nothing else
  densify         resample a decimated route to CARLA's own waypoint spacing

FRAMES. CARLA/UE4 is LEFT-handed, so its yaw increases clockwise and
VehicleControl.steer is positive-right consistently with that. SUMO/FIXS is
right-handed with y north. Converting once, here, is what lets CARLA's code run
in the frame it was written for:

    carla_x = fixs_x      carla_y = -fixs_y      carla_yaw = fixs_heading - 90

Checked against the MLK scenario's own data: the ego enters at FIXS heading
113.36 deg -> yaw 23.36, and its first motion (dx=+0.33, dy=-0.15) gives atan2
24.44 deg. The steer that comes back is already CARLA's positive-right, which is
what VirCarlaEnv's applyEgoActuation expects -- so there is no sign correction
anywhere here, and there cannot be one. Getting this wrong is not subtle: taking
the probe client's convention on faith span the car through 180 deg and drove it
off the map in 40 s.

Covered by tests/Python/unit/test_carla_agent_template.py.
"""
from __future__ import annotations

import json
import math
import os
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))

MAX_STEER_RAD = 0.7     # must match mainVirCarla.cpp kMaxSteerRad


# ---------------------------------------------------------------------------
# importing the agent code
# ---------------------------------------------------------------------------

def vendored_version():
    """The CARLA version agents/ was copied from, per VERSION.txt."""
    try:
        for line in open(os.path.join(_HERE, "VERSION.txt"), encoding="utf-8"):
            if line.startswith("source"):
                for tok in line.split():
                    if tok[:1].isdigit():
                        return tok
    except OSError:
        pass
    return None


def _installed_carla_version(carla_mod):
    for attr in ("__version__", "VERSION"):
        v = getattr(carla_mod, attr, None)
        if v:
            return str(v)
    try:                                    # the wheel name is the fallback
        root = json.load(open(os.path.expanduser("~/.fixs/carla.json"),
                              encoding="utf-8")).get("carla_wheel", "")
        base = os.path.basename(root)
        if base.startswith("carla-"):
            return base.split("-")[1]
    except Exception:
        pass
    return None


def import_carla_agents(explicit_root=None, warn_on_drift=True):
    """-> (carla, BasicAgent, RoadOption, BehaviorAgent).

    Uses the VENDORED agents/ next to this file. `carla` itself still comes from
    the installed package, because it is a compiled extension and cannot be
    vendored as source.

    DRIFT is the cost of vendoring: agents/ is pinned to one CARLA version while
    the simulator is whatever carla.json launches. A mismatch means the control
    code and the physics come from different releases, which is the kind of
    divergence you only notice as results that moved. So it is reported, not
    silently tolerated.

    explicit_root overrides the vendored copy with a live CARLA install's
    PythonAPI/carla -- useful for testing against a different CARLA without
    re-vendoring.
    """
    if explicit_root:
        if not os.path.isdir(os.path.join(explicit_root, "agents", "navigation")):
            raise SystemExit(f"[adapter] no agents/navigation under {explicit_root}")
        sys.path.insert(0, explicit_root)
    else:
        sys.path.insert(0, _HERE)           # agents/ lives here

    try:
        import carla                                              # noqa: F401
        from agents.navigation.basic_agent import BasicAgent
        from agents.navigation.behavior_agent import BehaviorAgent
        from agents.navigation.local_planner import RoadOption
    except ImportError as exc:
        raise SystemExit(
            f"[adapter] cannot import the agent code ({exc}).\n"
            f"[adapter]   `carla` itself must be installed (pip/conda); agents/ is\n"
            f"[adapter]   vendored next to this file. basic_agent also needs\n"
            f"[adapter]   networkx and shapely.")

    if warn_on_drift and not explicit_root:
        want, have = vendored_version(), _installed_carla_version(carla)
        if want and have and not have.startswith(want):
            print(f"[adapter] WARNING: agents/ is vendored from CARLA {want} but the "
                  f"installed CARLA is {have}.\n"
                  f"[adapter]   The control code and the simulator are different "
                  f"releases. Re-vendor (see VERSION.txt) or expect drift.",
                  file=sys.stderr)
    return carla, BasicAgent, RoadOption, BehaviorAgent


# ---------------------------------------------------------------------------
# the CARLA objects the agent touches
# ---------------------------------------------------------------------------





# ---------------------------------------------------------------------------
# BasicAgent, with two methods replaced
# ---------------------------------------------------------------------------


# ---------------------------------------------------------------------------
# route helpers
# ---------------------------------------------------------------------------


def densify(points, spacing=2.0):
    """Resample a polyline to ~spacing metres.

    CARLA's own plans are 2 m apart and LocalPlanner's purge distance
    (3.0 + 0.5*v) is tuned for that. EgoRoutePoints is decimated to ~14 m on
    MLK, which would leave the lateral controller aiming at a point whose
    distance depends on how the geometry happened to be sampled.
    """
    if len(points) < 2:
        return list(points)
    out = []
    for i in range(len(points) - 1):
        ax, ay = points[i]
        bx, by = points[i + 1]
        n = max(1, int(math.hypot(bx - ax, by - ay) / spacing))
        for k in range(n):
            out.append((ax + (bx - ax) * k / n, ay + (by - ay) * k / n))
    out.append(points[-1])
    return out






