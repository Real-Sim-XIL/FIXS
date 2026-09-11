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

Selftest (no CARLA server, no FIXS needed):
    python fixs_adapter.py
"""
from __future__ import annotations

import json
import math
import os
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))

SIGNAL_RED = 1          # CommonLib/TrafficHelper.cpp tlsStateToColor()
MAX_STEER_RAD = 0.7     # must match mainVirCarla.cpp kMaxSteerRad

#: A pose step beyond this is a teleport, not motion -- adoption, a lap wrap, a
#: SUMO re-insertion. 5 m in one CARLA step is 100 m/s at 0.05 s.
_kTeleportStep = 5.0


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

class _Control:
    """get_control() -- LocalPlanner reads .steer once, at construction."""
    steer = 0.0


class _LaneMarking:
    def __init__(self, carla_mod):
        self.lane_change = carla_mod.LaneChange.NONE


class Waypoint:
    """A point on the ego's route, standing in for carla.Waypoint.

    Takes FIXS coordinates and stores CARLA ones, so callers never do the y-flip.

    The lane-graph attributes below are inert on purpose: this adapter has the
    ego's route as a polyline and no road network, so there is no neighbouring
    lane to answer with. BehaviorAgent's lane-change and tailgating paths read
    them and then do nothing, which is the correct outcome here -- see FIXS#305
    for why the polyline is the wrong thing to answer road questions with, and
    what replaces it.
    """
    __slots__ = ("transform", "is_junction", "lane_id", "road_id",
                 "left_lane_marking", "right_lane_marking", "lane_width")

    def __init__(self, carla_mod, fixs_x, fixs_y):
        self.transform = carla_mod.Transform(
            carla_mod.Location(x=fixs_x, y=-fixs_y, z=0.0))
        self.is_junction = False
        self.lane_id = 0
        self.road_id = 0
        self.lane_width = 3.5
        self.left_lane_marking = _LaneMarking(carla_mod)
        self.right_lane_marking = _LaneMarking(carla_mod)

    def get_left_lane(self):  return None
    def get_right_lane(self): return None


class _ActorList(list):
    """world.get_actors() -- always empty, because both sweeps over it are
    overridden in FixsBasicAgent."""

    def filter(self, _pattern):
        return []


class _Map:
    def __init__(self, carla_mod):
        self._carla = carla_mod

    def get_waypoint(self, location, *_a, **_k):
        # location is already CARLA-side; the constructor flips, so pre-flip.
        return Waypoint(self._carla, location.x, -location.y)

    def get_topology(self):
        # BasicAgent.__init__ builds a GlobalRoutePlanner unconditionally
        # (basic_agent.py:99) even though the route is supplied here. An empty
        # topology gives it an empty graph, which is safe precisely because
        # trace_route is never called. Measured: 0.1 ms, 0 nodes, never read.
        return []


class _World:
    def __init__(self, carla_mod):
        self._map = _Map(carla_mod)

    def get_map(self):
        return self._map

    def get_actors(self):
        return _ActorList()


class _OtherVehicle:
    """A detected vehicle, from its FIXS record.

    BasicAgent discards what the detector returns; BehaviorAgent measures it --
    it subtracts half-extents to turn a centre distance into a gap, and reads
    the speed to band on time-to-collision. Returning None is an AttributeError
    on the first detection (FIXS#305).
    """

    def __init__(self, carla_mod, rec=None, speed_ms=0.0, length=4.5, width=1.8):
        c = self._carla = carla_mod
        if rec is not None:
            speed_ms = float(getattr(rec, "speed", 0.0) or 0.0)
            length = float(getattr(rec, "length", 0.0) or length)
            width = float(getattr(rec, "width", 0.0) or width)
            self.id = getattr(rec, "id", "") or ""
            yaw = fixs_heading_to_carla_yaw(getattr(rec, "heading", 0.0) or 0.0)
            x, y = getattr(rec, "positionX", 0.0), -getattr(rec, "positionY", 0.0)
        else:
            self.id, yaw, x, y = "", 0.0, 0.0, 0.0
        self._tf = c.Transform(c.Location(x=x, y=y, z=0.0), c.Rotation(yaw=yaw))
        r = math.radians(yaw)
        self._vel = c.Vector3D(x=speed_ms * math.cos(r),
                               y=speed_ms * math.sin(r), z=0.0)
        self.bounding_box = _BoundingBox(c, length, width)

    def _copyTf(self):
        """A FRESH transform each call, as carla.Actor.get_transform gives.

        Stock aliases then mutates it -- `ego_front_transform = ego_transform;
        ego_front_transform.location += ...` (basic_agent.py:367, :410) -- which
        is harmless against CARLA's copy and, against a stored one, walks the
        vehicle's own pose forward by half its length on every detector call.
        Measured: the planner then never purged a waypoint, the lateral
        controller saturated, and the ego never moved (FIXS#305).
        """
        c = self._carla
        return c.Transform(c.Location(x=self._tf.location.x, y=self._tf.location.y,
                                      z=self._tf.location.z),
                           c.Rotation(yaw=self._tf.rotation.yaw))

    def get_transform(self): return self._copyTf()
    def get_location(self):  return self._copyTf().location
    def get_velocity(self):  return self._vel


class _BoundingBox:
    def __init__(self, carla_mod, length, width):
        self._carla = carla_mod
        self.extent = carla_mod.Vector3D(x=length / 2.0, y=width / 2.0, z=0.75)

    def get_world_vertices(self, transform):
        """The body's corners in world space, for stock's junction test.

        FOUR corners, not CARLA's eight. Stock feeds them straight to
        `Polygon([[v.x, v.y, v.z] for v in vertices])`, and the test is purely
        planar -- so four give a clean rectangle where eight give the same
        footprint twice over and a self-touching ring.

        The box is laid BACKWARDS from the transform, because a FIXS record's
        position is the FRONT of the vehicle where a CARLA actor's origin is
        its centre. The same offset _bodyPolygon applies.
        """
        loc = transform.location
        yaw = math.radians(transform.rotation.yaw)
        fx, fy = math.cos(yaw), math.sin(yaw)      # forward
        rx, ry = -fy, fx                           # right
        ex, ey = self.extent.x, self.extent.y
        cx, cy = loc.x - fx * ex, loc.y - fy * ex  # centre, half a body back
        L = self._carla.Location
        return [L(x=cx + fx * ex + rx * ey, y=cy + fy * ex + ry * ey, z=loc.z),
                L(x=cx + fx * ex - rx * ey, y=cy + fy * ex - ry * ey, z=loc.z),
                L(x=cx - fx * ex - rx * ey, y=cy - fy * ex - ry * ey, z=loc.z),
                L(x=cx - fx * ex + rx * ey, y=cy - fy * ex + ry * ey, z=loc.z)]


class EgoAdapter:
    """Stands in for `world.player`.

    Quacks like carla.Vehicle for exactly the calls LocalPlanner and
    VehiclePIDController make -- get_world, get_control, get_transform,
    get_location, get_velocity, get_speed_limit, bounding_box.

    get_speed_limit is the ROAD's limit, never the eco advisory:
    collision_and_car_avoid_manager searches max(min_proximity_threshold,
    speed_limit / 3) metres, so an advisory of 5-8 m/s collapses the search to
    the 10 m floor and the first thing seen is already inside braking distance.
    The advisory belongs in behavior.max_speed (FIXS#305).
    """

    def __init__(self, carla_mod, length=4.5, width=1.8):
        self._carla = carla_mod
        self._world = _World(carla_mod)
        self._yaw = 0.0
        self._prev_xy = None
        self._speed_limit_kmh = 0.0
        self.id = ""
        self.bounding_box = _BoundingBox(carla_mod, length, width)
        self.set_state(0.0, 0.0, 0.0, 0.0)

    # --- the carla.Vehicle interface ---
    def get_world(self):     return self._world
    def get_control(self):   return _Control()
    def _copyTf(self):
        """A FRESH transform each call, as carla.Actor.get_transform gives.

        Stock aliases then mutates it -- `ego_front_transform = ego_transform;
        ego_front_transform.location += ...` (basic_agent.py:367, :410) -- which
        is harmless against CARLA's copy and, against a stored one, walks the
        vehicle's own pose forward by half its length on every detector call.
        Measured: the planner then never purged a waypoint, the lateral
        controller saturated, and the ego never moved (FIXS#305).
        """
        c = self._carla
        return c.Transform(c.Location(x=self._tf.location.x, y=self._tf.location.y,
                                      z=self._tf.location.z),
                           c.Rotation(yaw=self._tf.rotation.yaw))

    def get_transform(self): return self._copyTf()
    def get_location(self):  return self._copyTf().location
    def get_velocity(self):  return self._vel
    def get_speed_limit(self):        return self._speed_limit_kmh
    def set_speed_limit(self, ms):    self._speed_limit_kmh = max(0.0, float(ms)) * 3.6

    # --- the FIXS side ---
    def set_state(self, fixs_x, fixs_y, carla_yaw_deg, speed_ms):
        c = self._carla
        self._tf = c.Transform(c.Location(x=fixs_x, y=-fixs_y, z=0.0),
                               c.Rotation(yaw=carla_yaw_deg))
        r = math.radians(carla_yaw_deg)
        self._vel = c.Vector3D(x=speed_ms * math.cos(r),
                               y=speed_ms * math.sin(r), z=0.0)

    def update_from_record(self, ego):
        """One call per tick: take the ego's FIXS record and become it.

        Heading comes from MOTION once the car is moving, which is convention-
        free: no assumption about whether the wire's `heading` is navigational
        or mathematical, or which way it turns. Seeded from the wire on the
        first tick because the ego is inserted at ~0 m/s and sits there.
        """
        dx = dy = 0.0
        if self._prev_xy is not None:
            dx = ego.positionX - self._prev_xy[0]
            dy = ego.positionY - self._prev_xy[1]
        step = math.hypot(dx, dy)
        if self._prev_xy is None or step > _kTeleportStep:
            # A JUMP is not motion. The ego's first record is whatever the
            # bridge had before it was adopted -- near the origin -- so the step
            # to its real spawn is a kilometre, and reading a heading off that
            # points the car ~100 deg wrong. Survivable for a detector that
            # works off the wire; fatal for stock's, whose cone is 30 deg wide,
            # so it sees no leader, drives, and the lateral PID saturates.
            self._yaw = fixs_heading_to_carla_yaw(ego.heading or 0.0)
        elif step > 0.05:
            self._yaw = motion_heading_to_carla_yaw(dx, dy)
        self._prev_xy = (ego.positionX, ego.positionY)
        self.set_state(ego.positionX, ego.positionY, self._yaw, ego.speed)
        return self._yaw


def fixs_heading_to_carla_yaw(fixs_heading_deg):
    """FIXS navigational degrees (0 = N, clockwise) -> CARLA yaw."""
    return fixs_heading_deg - 90.0


def motion_heading_to_carla_yaw(dx_fixs, dy_fixs):
    """CARLA yaw from a FIXS displacement, independent of any wire convention."""
    return math.degrees(math.atan2(-dy_fixs, dx_fixs))


# ---------------------------------------------------------------------------
# BasicAgent, with two methods replaced
# ---------------------------------------------------------------------------

def _bodyPolygon(Polygon, rec):
    """A vehicle record -> its footprint in the FIXS frame.

    `positionX/Y` is the FRONT-of-vehicle anchor (the wire convention the
    DataLogger header states), so the body runs BACKWARDS from it along the
    heading. Getting this wrong puts every car half a length ahead of where it
    is, which on a queue is the difference between clear and blocked.
    """
    length = float(getattr(rec, "length", 0.0) or 0.0) or 4.5
    width = float(getattr(rec, "width", 0.0) or 0.0) or 1.8
    # FIXS heading is navigational: 0 = +y (north), clockwise.
    h = math.radians(float(rec.heading or 0.0))
    fx, fy = math.sin(h), math.cos(h)          # unit vector along travel
    rx, ry = fy, -fx                           # unit vector to its right
    x, y = rec.positionX, rec.positionY
    hw = width / 2.0
    return Polygon([
        (x + rx * hw, y + ry * hw),
        (x - rx * hw, y - ry * hw),
        (x - fx * length - rx * hw, y - fy * length - ry * hw),
        (x - fx * length + rx * hw, y - fy * length + ry * hw),
    ])


def make_agent_class(BasicAgent):
    """CARLA's BasicAgent with SUMO's answers behind its detectors.

    A factory rather than a module-scope class because BasicAgent is only
    importable after import_carla_agents() has resolved the path.

    The overrides keep the STOCK SIGNATURE and are handed CARLA's own
    max_distance -- base_threshold + speed_ratio * speed, computed inside
    run_step -- so the thresholds stay stock too. Only the source of the answer
    changes.

    The SIGNAL override stays either way: stock has no notion of "my next
    signal", so it sweeps every light actor and keeps those sharing its road_id
    and facing the same way (basic_agent.py:296-309), where SUMO answers the
    question route-aware and publishes it on the wire. That is better
    information, not a reconstruction.

    The OBSTACLE override is on borrowed time. It exists because the adapter
    had no road network, so stock's primary filter -- same road_id and lane_id
    -- was unavailable and only its junction fallback remained, run everywhere.
    With the map forwarded it should go, and the leader distance SUMO publishes
    should be read by the controller rather than substituted behind its back.
    What blocks that is measured: CARLA's mirrored vehicles report a velocity
    of exactly 0.000 (182 of 182 while 119 were moving), so a following model
    fed world.get_actors() reads every leader as stationary. See FIXS#305.
    """

    class FixsBasicAgent(BasicAgent):
        ego_record = None       # set from the tick loop before each run_step

        #: Every vehicle on the wire this tick, set from the control loop next to
        #: ego_record. A list of FIXS records, not CARLA actors.
        fixs_vehicles = ()

        def _vehicle_obstacle_detected(self, vehicle_list=None, max_distance=None,
                                       up_angle_th=90, low_angle_th=0, lane_offset=0):
            """Two answers to two different questions, and the nearer wins.

            SUMO's precedingVehicle* answers "what is ahead of me ON MY ROUTE",
            which is the right answer for car-following and the wrong one for
            "is anything in my way". It is route-topological: a vehicle on any
            other edge is not the leader, so an oncoming left-turner cutting the
            corner, cross traffic in a junction, and anything drifting over the
            line all read as clear. Measured on the U-turn corridor with only
            this answer wired in: three impacts at 45-70 m/s2, one of them an
            opposing left-turner encroaching into the ego's lane.

            So the geometric sweep comes back -- stock BasicAgent's own test,
            against the FIXS vehicle list instead of a CARLA actor list, since
            this controller is handed records and not a world.
            """
            if self._ignore_vehicles:
                return (False, None, -1)
            md = max_distance or self._base_vehicle_threshold

            gaps = []
            e = self.ego_record
            if e is not None and getattr(e, "hasPrecedingVehicle", 0):
                gap = e.precedingVehicleDistance or 0.0
                if 0.0 < gap < md:
                    gaps.append((gap, None))   # SUMO's leader: distance only

            swept = self._sweptObstacle(md)
            if swept is not None:
                gaps.append(swept)

            if not gaps:
                return (False, None, -1)
            gap, rec = min(gaps, key=lambda g: g[0])
            return (True, self._standIn(rec, gap), gap)

        def _standIn(self, rec, gap):
            """A vehicle object for the following model to measure."""
            carla_mod = self._vehicle._carla
            if rec is not None:
                return _OtherVehicle(carla_mod, rec)
            # SUMO's leader gives a distance and no record: place a default body
            # at that gap ahead, closing at the ego's own speed.
            v = _OtherVehicle(carla_mod, None,
                              speed_ms=float(getattr(self.ego_record, "speed", 0.0) or 0.0))
            return v

        def _sweptObstacle(self, maxDistance):
            """(distance, record) for the nearest body in the corridor, or None.

            The corridor is the planned path buffered by the ego's half-width --
            the same shape stock builds by offsetting each waypoint left and
            right, expressed as a buffer because these waypoints carry a location
            and no rotation to take a right-vector from.

            Records are in the FIXS frame and give the FRONT of the vehicle, so
            each body is laid backwards from its reported point.
            """
            ego = self.ego_record
            if ego is None or not self.fixs_vehicles:
                return None
            try:
                from shapely.geometry import LineString, Polygon
            except ImportError:
                return None

            ex, ey = ego.positionX, ego.positionY
            path = [(ex, ey)]
            for wp, _ in self._local_planner.get_plan():
                # Waypoints are stored CARLA-side; bring them back to the FIXS
                # frame so everything here is in one frame.
                wx, wy = wp.transform.location.x, -wp.transform.location.y
                if math.hypot(wx - ex, wy - ey) > maxDistance:
                    break
                path.append((wx, wy))
            if len(path) < 2:
                return None

            halfWidth = max(float(getattr(ego, "width", 0.0) or 0.0), 1.8) / 2.0
            corridor = LineString(path).buffer(halfWidth)

            nearest = None
            nearestRec = None
            egoId = (ego.id or "").strip()
            for v in self.fixs_vehicles:
                if (v.id or "").strip() == egoId:
                    continue
                d = math.hypot(v.positionX - ex, v.positionY - ey)
                if d > maxDistance:
                    continue
                if corridor.intersects(_bodyPolygon(Polygon, v)):
                    if nearest is None or d < nearest:
                        nearest, nearestRec = d, v
            return None if nearest is None else (nearest, nearestRec)

        def _affected_by_traffic_light(self, lights_list=None, max_distance=None):
            e = self.ego_record
            if e is None or e.signalLightColor != SIGNAL_RED:
                return (False, None)
            dist = e.signalLightDistance or 0.0
            if dist <= 0.0:
                return (False, None)
            return (dist < (max_distance or self._base_tlight_threshold), None)

    return FixsBasicAgent


# ---------------------------------------------------------------------------
# route helpers
# ---------------------------------------------------------------------------

def planner_options(dt, sampling_resolution=2.0):
    """opt_dict for BasicAgent/LocalPlanner with `dt` ACTUALLY applied.

    LocalPlanner.__init__ captures self._dt into the two gain dicts by value
    (local_planner.py:81-82) and only then reads opt_dict, where 'dt' rebinds
    the attribute -- which nothing reads afterwards; line 94 is its last
    occurrence in the file. So opt_dict={'dt': 0.1} is a no-op and both PIDs
    keep CARLA's 20 Hz default, even though the docstring lists dt as a
    supported parameter. The dicts are only replaced wholesale, by
    lateral_control_dict / longitudinal_control_dict.

    Left uncorrected against the 10 Hz FIXS feed that silently doubles the
    derivative term and halves the integral one:

        _de = (e[-1] - e[-2]) / dt        dt too small -> D too strong
        _ie = sum(buffer) * dt            dt too small -> I too weak

    Measured before this helper existed: lateral PID _dt = 0.05 while the loop
    ran at 0.1. Gains below are CARLA's own defaults, untouched; only dt is
    carried through. Lives here rather than in a controller script so the next
    controller does not have to rediscover it.
    """
    return {
        "dt": dt,
        "sampling_radius": sampling_resolution,
        "lateral_control_dict":      {"K_P": 1.95, "K_I": 0.05, "K_D": 0.2, "dt": dt},
        "longitudinal_control_dict": {"K_P": 1.0,  "K_I": 0.05, "K_D": 0.0, "dt": dt},
    }


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






def _selftest():
    """Everything that can be checked without a CARLA server or a FIXS feed."""
    carla, BasicAgent, RoadOption = import_carla_agents()
    import agents.navigation.basic_agent as ba
    print(f"vendored from : {vendored_version()}")
    print(f"agents/       : {os.path.relpath(ba.__file__, _HERE)}")

    Agent = make_agent_class(BasicAgent)
    overrides = sorted(k for k in Agent.__dict__ if not k.startswith("__"))
    print(f"overrides     : {overrides}")
    assert overrides == ["_affected_by_traffic_light", "_vehicle_obstacle_detected",
                         "ego_record"], overrides

    # frames, against the MLK scenario's measured entry state
    assert abs(fixs_heading_to_carla_yaw(113.36) - 23.36) < 1e-6
    assert abs(motion_heading_to_carla_yaw(0.33, -0.15) - 24.44) < 0.01
    print("frames        : 113.36 deg -> yaw 23.36; motion -> 24.44  OK")

    class _Rec:
        hasPrecedingVehicle = 0
        precedingVehicleDistance = 0.0
        signalLightColor = -1
        signalLightDistance = 0.0
        heading = 90.0
    rec = _Rec()

    # closed loop on a straight line, both signs, through CARLA's real planner
    for y0 in (1.5, -1.5):
        ego = EgoAdapter(carla)
        ag = Agent(ego, target_speed=8.0 * 3.6,
                   opt_dict={"dt": 0.1, "sampling_radius": 2.0})
        ag.set_global_plan([(Waypoint(carla, float(x), 0.0), RoadOption.LANEFOLLOW)
                            for x in range(0, 600, 2)])
        ag.ego_record = rec
        x, y, yaw, v = 0.0, y0, 0.0, 8.0
        for _ in range(200):
            ego.set_state(x, y, yaw, v)
            c = ag.run_step()
            yaw += math.degrees((v / 2.9) * math.tan(c.steer * 1.22) * 0.1)
            x += v * math.cos(math.radians(yaw)) * 0.1
            y -= v * math.sin(math.radians(yaw)) * 0.1
        assert abs(y) < 0.2, f"offset {y0} did not converge: {y}"
        print(f"lateral       : {y0:+.1f} m -> {y:+.3f} m after 20 s  OK")

    # hazards, with CARLA's own thresholds (5 + 1.0*v)
    ego = EgoAdapter(carla)
    ag = Agent(ego, target_speed=8.0 * 3.6, opt_dict={"dt": 0.1})
    ag.set_global_plan([(Waypoint(carla, float(x), 0.0), RoadOption.LANEFOLLOW)
                        for x in range(0, 600, 2)])
    ego.set_state(0.0, 0.0, 0.0, 8.0)
    ag.ego_record = rec
    rec.hasPrecedingVehicle, rec.precedingVehicleDistance = 1, 8.0
    c = ag.run_step()
    assert c.brake > 0.4 and c.throttle == 0.0, (c.throttle, c.brake)
    print(f"leader  8 m   : throttle {c.throttle:.2f} brake {c.brake:.2f}  OK")
    rec.precedingVehicleDistance = 40.0
    c = ag.run_step()
    assert c.brake == 0.0, c.brake
    print(f"leader 40 m   : throttle {c.throttle:.2f} brake {c.brake:.2f}  OK")
    rec.hasPrecedingVehicle = 0
    rec.signalLightColor, rec.signalLightDistance = SIGNAL_RED, 6.0
    c = ag.run_step()
    assert c.brake > 0.4, c.brake
    print(f"red     6 m   : throttle {c.throttle:.2f} brake {c.brake:.2f}  OK")

    print("\nselftest passed")
    return 0


if __name__ == "__main__":
    sys.exit(_selftest())
