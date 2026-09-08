"""
sumo_uturn.py - attach drivable U-turn loops to the ends of a SUMO corridor.

A corridor scenario that ends at a stub has no way to send a vehicle back the
other way except SUMO's *point* turnaround: a ~5 m internal link that pivots the
vehicle 180 deg on the spot. That is fine for queue statistics and unusable for
anything that has to physically drive the geometry - a CARLA/CarMaker ego
following the OpenDRIVE export cannot track a 1.6 m turning radius, so the ego
either stalls at the terminus or leaves the road.

This tool replaces that with the geometry FIXS#172 built by hand for the
SimpleTrafficLight corridor: a compact CURVED turnaround ("teardrop") past the
last intersection -

    corridor -> straight stub -> entry ramp (peels off, turn T1)
             -> loop (turns -(2*T1+180))
             -> exit ramp (turn T1) -> corridor, reversed.

The path is defined by a curvature profile and integrated, so it is
tangent-continuous everywhere and provably closes back onto the corridor
centreline heading the opposite way. T1 is solved numerically so the exit ramp
lands on the axis; with ramp_radius == loop_radius the solution is exactly
60 deg.  See `curved_uturn()`.

Where `gen_loop_net.py` (tests/Vissim/Probes/TrafficLayer_DSProxy_CMoffice_Signal)
generates a whole synthetic corridor with a loop at each end, this module takes
the same geometry core and *patches an existing network*: it works at any
heading, carries the terminus elevation, keeps every existing edge id (so route
files stay valid), and hands the result to netconvert as plain-XML patch files.

Stdlib only. netconvert (SUMO) must be on PATH or at $SUMO_HOME/bin.

Usage
-----
  # what could take a loop?
  python sumo_uturn.py --net city.net.xml --list

  # both ends of a corridor, given the ordered edge sequences
  python sumo_uturn.py --net mlk.net.xml --out mlk_uturn.net.xml \
      --corridor connected_edge_sequence_EB.csv \
      --corridor connected_edge_sequence_WB.csv

  # or name the terminus junctions directly
  python sumo_uturn.py --net mlk.net.xml --out mlk_uturn.net.xml \
      --terminal 11097239234 --terminal 1845486816 --loop-radius 40
"""
from __future__ import annotations

import argparse
import csv
import math
import os
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path

# --------------------------------------------------------------------------
# geometry: constant-curvature integration (generalised from gen_loop_net.py)
# --------------------------------------------------------------------------

DEFAULT_RAMP_RADIUS = 45.0
DEFAULT_LOOP_RADIUS = 45.0
DEFAULT_STRAIGHT_PAST = 30.0
DEFAULT_SPEED = 8.33          # m/s, ~30 km/h
DEFAULT_DS = 2.0              # polyline sample step (m)
DEFAULT_THROAT_RADIUS = 12.0  # junction radius at the ramp<->loop tangent nodes


def _arc(x, y, h, radius, turn_sign, total_ang, ds):
    """Integrate a constant-curvature arc from (x, y) at heading h [rad].

    turn_sign +1 = left/CCW, -1 = right/CW. Returns (points, x, y, h_end);
    `points` excludes the start point and ends exactly on the arc end.
    """
    n = max(2, int(round(radius * total_ang / ds)))
    dphi = turn_sign * total_ang / n
    step = radius * abs(dphi)
    pts = []
    for _ in range(n):
        h += dphi / 2.0
        x += step * math.cos(h)
        y += step * math.sin(h)
        h += dphi / 2.0
        pts.append((x, y))
    return pts, x, y, h


def _three_phase(t1_deg, ramp_r, loop_r, turn_sign, ds):
    """entry ramp +T1 -> loop -(2*T1+180) -> exit ramp +T1, starting at the
    origin heading +x. Net heading change is exactly 180 deg (a U-turn)."""
    t1 = math.radians(t1_deg)
    t2 = 2 * t1 + math.pi
    p1, x, y, h = _arc(0.0, 0.0, 0.0, ramp_r, turn_sign, t1, ds)
    p2, x, y, h = _arc(x, y, h, loop_r, -turn_sign, t2, ds)
    p3, x, y, h = _arc(x, y, h, ramp_r, turn_sign, t1, ds)
    return p1, p2, p3, x, y, h


def solve_ramp_angle(ramp_r, loop_r, turn_sign, ds=DEFAULT_DS, target_dy=0.0):
    """Ramp angle T1 [deg] such that the turnaround lands `target_dy` off the line
    it started on.

    target_dy is what keeps the two directions apart. The entry lane and the exit
    lane are different lanes, a median apart. A loop solved to come back to where
    it STARTED lands on the entry lane and has to be bridged across to the exit
    one - and both bridges are drawn to the same neck point, so the two ribbons
    converge and overlap for a third of their length. Solving for the real
    separation instead means the loop leaves the entry lane and arrives on the
    exit lane, and each stub stays on its own.

    For ramp_r == loop_r the closed form is acos(r/(r+R)) = 60 deg exactly;
    bisection is kept because it also solves the ramp_r != loop_r case, where
    the tangency condition is cos(T1) = ramp_r / (ramp_r + loop_r).
    """
    def end_offset(t1d):
        return _three_phase(t1d, ramp_r, loop_r, turn_sign, ds)[4] - target_dy

    lo, hi = 1.0, 89.0
    flo = end_offset(lo)
    for _ in range(80):
        mid = (lo + hi) / 2.0
        fm = end_offset(mid)
        if (flo < 0) == (fm < 0):
            lo, flo = mid, fm
        else:
            hi = mid
    return (lo + hi) / 2.0


def _hermite(p0, t0, p1, t1, ds):
    """Cubic Hermite from p0 with unit tangent t0 to p1 with unit tangent t1.

    This is the stub between the corridor and the neck. It has to be a blend
    rather than a straight line: the lane that feeds a U-turn is generally offset
    sideways from the axis the loop is symmetric about (at MLK's west end the
    inbound and outbound inner lanes are 7 m apart), and joining them with
    straight segments puts a kink at each end - exactly the undrivable geometry
    this tool exists to remove.
    """
    d = math.dist(p0, p1)
    if d < 1e-6:                      # no stub at all (the corridor edge ends on the neck)
        return []
    m0, m1 = (t0[0] * d, t0[1] * d), (t1[0] * d, t1[1] * d)
    n = max(2, int(round(d / ds)))
    pts = []
    for i in range(1, n + 1):
        s = i / n
        h00 = 2 * s ** 3 - 3 * s ** 2 + 1
        h10 = s ** 3 - 2 * s ** 2 + s
        h01 = -2 * s ** 3 + 3 * s ** 2
        h11 = s ** 3 - s ** 2
        pts.append((h00 * p0[0] + h10 * m0[0] + h01 * p1[0] + h11 * m1[0],
                    h00 * p0[1] + h10 * m0[1] + h01 * p1[1] + h11 * m1[1]))
    return pts


def curved_uturn(ramp_r, loop_r, straight_past, side="right", ds=DEFAULT_DS,
                 entry_lat=0.0, entry_dir=(1.0, 0.0),
                 exit_lat=0.0, exit_dir=(-1.0, 0.0)):
    """The whole turnaround in a LOCAL frame: origin at the ATTACHMENT ANCHOR
    (where the corridor lanes end), +x pointing away from the network.

    entry_lat / exit_lat are the lateral offsets of the feeding and receiving
    lane centrelines from that anchor, and entry_dir / exit_dir their travel
    directions - the loop is hung off the lanes that actually carry the traffic,
    not off the junction centroid, which can be several metres to one side.

    Returns polylines `in_pts` (corridor -> t1), `loop_pts` (t1 -> t2),
    `out_pts` (t2 -> corridor), the throat points t1/t2, the solved ramp angle
    and the driven path length.
    """
    turn_sign = -1 if side == "right" else +1
    t1_deg = solve_ramp_angle(ramp_r, loop_r, turn_sign, ds,
                              target_dy=exit_lat - entry_lat)
    p1, p2, p3, _ex, ey, eh = _three_phase(t1_deg, ramp_r, loop_r, turn_sign, ds)

    # The arcs start on the ENTRY lane and finish on the EXIT lane, so each stub
    # runs straight out along its own lane instead of both angling in to a shared
    # neck point. A single crossing where the two swap sides of the median remains,
    # and is inherent: a U-turn between the two inner lanes has to cross once.
    neck = (straight_past, entry_lat)
    shift = lambda pts: [(x + neck[0], y + neck[1]) for x, y in pts]
    p1, p2, p3 = shift(p1), shift(p2), shift(p3)
    t1_pt, t2_pt = p1[-1], p2[-1]

    p_in, p_out = (0.0, entry_lat), (0.0, exit_lat)
    stub_in = _hermite(p_in, entry_dir, neck, (1.0, 0.0), ds)
    stub_out = _hermite(p3[-1], (-1.0, 0.0), p_out, exit_dir, ds)

    t1 = math.radians(t1_deg)
    length = (ramp_r * t1 * 2 + loop_r * (2 * t1 + math.pi)
              + _polyline_length(stub_in) + _polyline_length(stub_out))
    return {
        "in_pts": [p_in] + stub_in + p1,
        "loop_pts": [t1_pt] + p2,
        "out_pts": [t2_pt] + p3 + stub_out,
        "t1": t1_pt,
        "t2": t2_pt,
        "neck": neck,
        "ramp_angle_deg": t1_deg,
        "closure_offset": ey - (exit_lat - entry_lat),
        "end_heading_deg": math.degrees(eh),
        "path_length": length,
        "min_stub_radius": min(_min_radius(stub_in), _min_radius(stub_out)),
    }


def _polyline_length(pts):
    return sum(math.dist(pts[i], pts[i + 1]) for i in range(len(pts) - 1))


def _min_radius(pts):
    """Smallest turn radius along a polyline (inf if it is straight)."""
    best = float("inf")
    for i in range(1, len(pts) - 1):
        a, b, c = pts[i - 1], pts[i], pts[i + 1]
        h0 = math.atan2(b[1] - a[1], b[0] - a[0])
        h1 = math.atan2(c[1] - b[1], c[0] - b[0])
        dh = abs((h1 - h0 + math.pi) % (2 * math.pi) - math.pi)
        ds = math.dist(b, c)
        if dh > 1e-9 and ds > 1e-9:
            best = min(best, ds / dh)
    return best


def _to_world(pts, origin, u):
    """Local (x along u, y left of u) -> world, anchored at `origin`."""
    ox, oy = origin
    ux, uy = u
    return [(ox + x * ux - y * uy, oy + x * uy + y * ux) for x, y in pts]


# --------------------------------------------------------------------------
# reading the network
# --------------------------------------------------------------------------

# connection attributes netconvert accepts back in a .con.xml. `dir`, `state`
# and `via` are derived and must NOT be echoed; `tl`/`linkIndex` must be, or a
# signalised terminus would have its tls program re-indexed underneath it.
CONN_ATTRS = ("fromLane", "toLane", "pass", "keepClear", "contPos", "visibility",
              "speed", "length", "uncontrolled", "allow", "disallow", "type",
              "changeLeft", "changeRight", "indirect", "tl", "linkIndex")


class Net:
    """The slice of a .net.xml this tool needs: normal edges, junctions,
    and the connections between normal edges."""

    def __init__(self, path):
        self.path = Path(path)
        root = ET.parse(str(path)).getroot()
        self.lefthand = root.get("lefthand") == "1"
        self.edges = {}
        for e in root.findall("edge"):
            if e.get("function") == "internal":
                continue
            lanes = e.findall("lane")
            self.edges[e.get("id")] = {
                "id": e.get("id"),
                "from": e.get("from"),
                "to": e.get("to"),
                "priority": e.get("priority"),
                "num_lanes": len(lanes),
                "speed": lanes[0].get("speed") if lanes else None,
                "disallow": lanes[-1].get("disallow") if lanes else None,
                "shape": _parse_shape(e.get("shape") or (lanes[0].get("shape") if lanes else "")),
                # per-lane centrelines: the loop has to attach to the lane that
                # actually feeds it, which is not the edge centreline
                "lane_shapes": [_parse_shape(l.get("shape")) for l in lanes],
            }
        self.junctions = {}
        for j in root.findall("junction"):
            jid = j.get("id")
            if jid.startswith(":"):
                continue
            self.junctions[jid] = {
                "id": jid,
                "type": j.get("type"),
                "x": float(j.get("x")), "y": float(j.get("y")),
                "z": float(j.get("z")) if j.get("z") is not None else None,
                # the polygon this terminus already has, to hand back to netconvert
                # verbatim - see UTurn.nodes_xml()
                "shape": j.get("shape"),
            }
        self.connections = []
        for c in root.findall("connection"):
            if c.get("from", "").startswith(":"):
                continue
            self.connections.append(dict(c.attrib))

        self.incoming, self.outgoing = {}, {}
        for e in self.edges.values():
            self.outgoing.setdefault(e["from"], []).append(e["id"])
            self.incoming.setdefault(e["to"], []).append(e["id"])

    def conns_from(self, edge_id):
        return [c for c in self.connections if c.get("from") == edge_id]

    def turn_angle(self, in_edge, out_edge):
        """Angle [deg] between arriving on in_edge and leaving on out_edge.
        180 = a straight-back U-turn, 0 = carrying straight on."""
        d_in = _seg_dir(self.edges[in_edge]["shape"], True)
        d_out = _seg_dir(self.edges[out_edge]["shape"], False)
        if not d_in or not d_out:
            return 0.0
        dot = max(-1.0, min(1.0, d_in[0] * d_out[0] + d_in[1] * d_out[1]))
        return math.degrees(math.acos(dot))

    def stub_ends(self, min_angle=150.0):
        """Corridor termini: junctions with exactly one incoming and one outgoing
        normal edge that run back along each other.

        Anti-parallel GEOMETRY is the test, not node topology. A one-way pair
        that splits at the terminus (MLK's west end: the arriving and departing
        edges continue to different nodes) is still a corridor end, and SUMO
        itself classifies the movement there as a turnaround by angle.
        """
        found = []
        for jid in self.junctions:
            inc = self.incoming.get(jid, [])
            out = self.outgoing.get(jid, [])
            if len(inc) == 1 and len(out) == 1:
                ang = self.turn_angle(inc[0], out[0])
                if ang >= min_angle:
                    found.append((jid, inc[0], out[0]))
        return sorted(found)

    def resolve_terminal(self, junction_id, min_angle=150.0):
        """(in_edge, out_edge) for a terminus junction: the one movement that
        turns the vehicle back the way it came. Anything ambiguous must be
        given explicitly with --pair."""
        if junction_id not in self.junctions:
            raise SystemExit(f"no junction '{junction_id}' in {self.path.name}")
        inc = self.incoming.get(junction_id, [])
        out = self.outgoing.get(junction_id, [])
        cands = [(i, o) for i in inc for o in out
                 if self.turn_angle(i, o) >= min_angle]
        if len(cands) != 1:
            raise SystemExit(
                f"junction '{junction_id}' has {len(inc)} in / {len(out)} out edge(s) and "
                f"{len(cands)} movement(s) turning back at >= {min_angle:g} deg - name the "
                f"pair explicitly with --pair IN_EDGE,OUT_EDGE")
        return cands[0]


def _parse_shape(text):
    pts = []
    for tok in (text or "").split():
        parts = tok.split(",")
        if len(parts) >= 2:
            pts.append((float(parts[0]), float(parts[1])))
    return pts


def _seg_dir(pts, at_end, baseline=5.0):
    """Unit direction of TRAVEL along a shape, at its end (at_end) or its start.

    Walks back from the endpoint until at least `baseline` metres have been
    covered, so a stray sub-metre vertex cannot set the heading.
    """
    if len(pts) < 2:
        return None
    seq = list(reversed(pts)) if at_end else pts   # seq[0] is the endpoint of interest
    anchor = seq[0]
    far = seq[-1]
    for p in seq[1:]:
        if math.hypot(p[0] - anchor[0], p[1] - anchor[1]) >= baseline:
            far = p
            break
    # travel points from `far` towards `anchor` at the end, and away from it at the start
    a, b = (far, anchor) if at_end else (anchor, far)
    dx, dy = b[0] - a[0], b[1] - a[1]
    n = math.hypot(dx, dy)
    return (dx / n, dy / n) if n > 1e-9 else None


def corridor_terminal(net, csv_path):
    """Terminus junction implied by an ordered corridor edge list (a CSV with an
    `edge_id` column, e.g. connected_edge_sequence_EB.csv): the far end of the
    last edge."""
    with open(csv_path, newline="", encoding="utf-8-sig") as fh:
        rows = list(csv.DictReader(fh))
    if not rows:
        raise SystemExit(f"{csv_path}: empty")
    col = next((c for c in rows[0] if c.strip().lower() in ("edge_id", "edge", "id")), None)
    if col is None:
        raise SystemExit(f"{csv_path}: no edge_id column (have {list(rows[0])})")
    last = rows[-1][col].strip()
    if last not in net.edges:
        raise SystemExit(f"{csv_path}: last edge '{last}' is not in the network")
    return net.edges[last]["to"]


# --------------------------------------------------------------------------
# building the patch
# --------------------------------------------------------------------------

def outward_axis(net, junction_id, in_edge, out_edge):
    """Unit vector pointing away from the network along the corridor.

    Averaging the arrival direction of the in-edge with the reversed departure
    direction of the out-edge cancels the lateral offset that spreadType/lane
    count puts between the two centrelines.
    """
    d_in = _seg_dir(net.edges[in_edge]["shape"], True)
    d_out = _seg_dir(net.edges[out_edge]["shape"], False)
    d_out = (-d_out[0], -d_out[1]) if d_out else None
    vecs = [v for v in (d_in, d_out) if v]
    if not vecs:
        raise SystemExit(f"junction '{junction_id}': cannot derive a corridor heading")
    ux = sum(v[0] for v in vecs) / len(vecs)
    uy = sum(v[1] for v in vecs) / len(vecs)
    n = math.hypot(ux, uy)
    if n < 1e-6:
        ux, uy, n = d_in[0], d_in[1], 1.0
    return (ux / n, uy / n)


def _lane_end(edge, lane_index, at_end):
    """(point, travel direction) where a lane centreline meets the junction."""
    shapes = edge["lane_shapes"]
    if not (0 <= lane_index < len(shapes)) or len(shapes[lane_index]) < 2:
        return None, None
    pts = shapes[lane_index]
    return (pts[-1] if at_end else pts[0]), _seg_dir(pts, at_end)


def lane_map(src_lanes, dst_lanes, lefthand=False, fan_out=False):
    """Pair source lanes to destination lanes, aligning the two groups by their
    LEFT edge - a U-turn leaves from, and returns to, the inside of the road.

        1 -> 2 gives [(0, 1)]        2 -> 1 gives [(1, 0)]

    A narrow ramp meeting a wide road leaves the outer lane(s) with no feeder at
    the terminus. That is what the real geometry looks like, and vehicles change
    into them a moment later; netconvert warns about it, but the alternative is
    worse. `fan_out=True` silences the warning by also linking the outermost
    source lane to the surplus lanes - at the cost of a connection that crabs a
    vehicle a full lane width sideways through the junction, which is exactly
    the undrivable geometry this tool exists to remove. Opt in only if the
    warning matters more than the path.
    """
    def idx(i, n):                       # i-th lane counted from the left
        return i if lefthand else n - 1 - i
    pairs = []
    for i in range(max(src_lanes, dst_lanes) if fan_out else min(src_lanes, dst_lanes)):
        s, d = idx(min(i, src_lanes - 1), src_lanes), idx(i, dst_lanes)
        if 0 <= d < dst_lanes:
            pairs.append((s, d))
    return pairs


class UTurn:
    """One terminus turned into a teardrop, ready to be written out."""

    def __init__(self, net, junction_id, in_edge, out_edge, opt):
        self.j = net.junctions[junction_id]
        self.jid = junction_id
        self.in_edge, self.out_edge = in_edge, out_edge
        self.opt = opt
        self.prefix = f"{opt.prefix}_{junction_id}"

        e_in, e_out = net.edges[in_edge], net.edges[out_edge]
        self.lanes = opt.lanes
        lh = (opt.side == "left")
        left = (lambda n: 0) if lh else (lambda n: n - 1)
        feed_lane = opt.from_lane if opt.from_lane is not None else left(e_in["num_lanes"])
        recv_lane = opt.to_lane if opt.to_lane is not None else left(e_out["num_lanes"])

        self.keep_shape = not opt.recompute_terminus_shape
        u = outward_axis(net, junction_id, in_edge, out_edge)
        # attach to the lane centrelines, not the junction centroid
        p_in, d_in = _lane_end(e_in, feed_lane, at_end=True)
        p_out, d_out = _lane_end(e_out, recv_lane, at_end=False)
        if p_in is None or p_out is None:
            p_in = p_out = (self.j["x"], self.j["y"])
            d_in, d_out = u, u
        origin = ((p_in[0] + p_out[0]) / 2.0, (p_in[1] + p_out[1]) / 2.0)
        to_local = lambda p: ((p[0] - origin[0]) * u[0] + (p[1] - origin[1]) * u[1],
                              -(p[0] - origin[0]) * u[1] + (p[1] - origin[1]) * u[0])
        dir_local = lambda d: (d[0] * u[0] + d[1] * u[1], -d[0] * u[1] + d[1] * u[0])
        self.entry_lat, self.exit_lat = to_local(p_in)[1], to_local(p_out)[1]

        geo = curved_uturn(opt.ramp_radius, opt.loop_radius, opt.straight_past,
                           opt.side, opt.ds,
                           # both tangents are directions of TRAVEL: outward on the
                           # feeding lane, back into the network on the receiving one
                           entry_lat=self.entry_lat, entry_dir=dir_local(d_in),
                           exit_lat=self.exit_lat, exit_dir=dir_local(d_out))
        self.geo = geo
        self.in_pts = _to_world(geo["in_pts"], origin, u)
        self.loop_pts = _to_world(geo["loop_pts"], origin, u)
        self.out_pts = _to_world(geo["out_pts"], origin, u)
        self.t1 = _to_world([geo["t1"]], origin, u)[0]
        self.t2 = _to_world([geo["t2"]], origin, u)[0]
        self.z = self.j["z"]

        self.speed = opt.speed
        self.priority = opt.priority if opt.priority is not None else e_in["priority"]
        self.disallow = opt.disallow if opt.disallow is not None else e_in["disallow"]
        # entry: a U-turn is taken from the leftmost lane(s) of the arriving edge
        self.entry_map = ([(opt.from_lane, 0)] if opt.from_lane is not None
                          else lane_map(e_in["num_lanes"], self.lanes, lh))
        self.exit_map = ([(0, opt.to_lane)] if opt.to_lane is not None
                         else lane_map(self.lanes, e_out["num_lanes"], lh, opt.feed_all_lanes))

    # ids
    @property
    def eid_in(self): return f"{self.prefix}_in"
    @property
    def eid_loop(self): return f"{self.prefix}_loop"
    @property
    def eid_out(self): return f"{self.prefix}_out"
    @property
    def nid_t1(self): return f"{self.prefix}_t1"
    @property
    def nid_t2(self): return f"{self.prefix}_t2"

    def nodes_xml(self):
        r = self.opt.throat_radius
        out = []
        for nid, (x, y) in ((self.nid_t1, self.t1), (self.nid_t2, self.t2)):
            z = f' z="{self.z:.2f}"' if self.z is not None else ""
            out.append(f'    <node id="{nid}" x="{x:.2f}" y="{y:.2f}"{z} '
                       f'type="priority" radius="{r}" keepClear="false"/>')
        j = self.j
        z = f' z="{j["z"]:.2f}"' if j["z"] is not None else ""
        if self.opt.neck_radius is not None:
            out.append(f'    <node id="{self.jid}" x="{j["x"]:.2f}" y="{j["y"]:.2f}"{z} '
                       f'type="{j["type"]}" radius="{self.opt.neck_radius}" keepClear="false"/>')
        elif self.keep_shape and j.get("shape"):
            # Hand the terminus its EXISTING polygon back, so netconvert does not
            # recompute one. Left to itself it would: the loop gives this node two
            # more legs, a wider polygon follows, and every edge meeting the node is
            # clipped back to it - so the corridor's first and last edges get
            # SHORTER. On MLK that moved 51066109#1 from 137.18 m to 133.19 m, and
            # since the ego enters at a fixed offset from the lane start, it began
            # 3.99 m closer to its first stop bar and every stored result diverged
            # from the first step.
            #
            # Pinning the shape keeps the rest of the network bit-identical and, as
            # a side effect, attaches the ramps BETTER: an unclipped ramp starts
            # exactly where the corridor lane ends (measured 0.00 m and 0.04 m at
            # MLK's two ends) instead of needing an internal connector to reach
            # across the widened junction (0.38 m and 8.41 m).
            out.append(f'    <node id="{self.jid}" x="{j["x"]:.2f}" y="{j["y"]:.2f}"{z} '
                       f'type="{j["type"]}" shape="{j["shape"]}"/>')
        return out

    def edges_xml(self):
        dis = f' disallow="{self.disallow}"' if self.disallow else ""
        pri = f' priority="{self.priority}"' if self.priority else ""
        specs = [(self.eid_in, self.jid, self.nid_t1, self.in_pts),
                 (self.eid_loop, self.nid_t1, self.nid_t2, self.loop_pts),
                 (self.eid_out, self.nid_t2, self.jid, self.out_pts)]
        return [
            f'    <edge id="{eid}" from="{fr}" to="{to}" numLanes="{self.lanes}" '
            f'speed="{self.speed}"{pri}{dis} spreadType="center" shape="{self._shape(pts)}"/>'
            for eid, fr, to, pts in specs
        ]

    def _shape(self, pts):
        z = "" if self.z is None else f",{self.z:.2f}"
        return " ".join(f"{x:.2f},{y:.2f}{z}" for x, y in pts)

    def connections_xml(self, net):
        """Everything that leaves in_edge, plus the links that wire the loop in.

        The existing connections are re-stated because naming an edge as `from`
        in a .con.xml replaces its whole connection set - dropping them would
        silently sever a signalised terminus. The point turnaround is removed
        with <delete>, not by omission: netconvert regenerates turnarounds after
        reading the file, so omitting it is not enough.
        """
        out = []
        for c in net.conns_from(self.in_edge):
            if c.get("to") == self.out_edge and self.opt.drop_point_turnaround:
                continue
            attrs = "".join(f' {k}="{c[k]}"' for k in CONN_ATTRS if k in c)
            out.append(f'    <connection from="{self.in_edge}" to="{c["to"]}"{attrs}/>')
        for s, d in self.entry_map:
            out.append(f'    <connection from="{self.in_edge}" to="{self.eid_in}" '
                       f'fromLane="{s}" toLane="{d}"/>')
        for s, d in self.exit_map:
            out.append(f'    <connection from="{self.eid_out}" to="{self.out_edge}" '
                       f'fromLane="{s}" toLane="{d}"/>')
        if self.opt.drop_point_turnaround:
            out.append(f'    <delete from="{self.in_edge}" to="{self.out_edge}"/>')
        return out

    def report(self):
        g = self.geo
        entry = ", ".join(f"{s}->{d}" for s, d in self.entry_map)
        exit_ = ", ".join(f"{s}->{d}" for s, d in self.exit_map)
        r_stub = g["min_stub_radius"]
        r_min = min(self.opt.ramp_radius, self.opt.loop_radius, r_stub)
        return (f"  {self.jid}: {self.in_edge} -> {self.eid_in} -> {self.eid_loop} -> "
                f"{self.eid_out} -> {self.out_edge}\n"
                f"      ramp angle {g['ramp_angle_deg']:.2f} deg, driven length "
                f"{g['path_length']:.1f} m, closure {abs(g['closure_offset']):.3f} m\n"
                f"      min radius {r_min:.1f} m (loop {self.opt.loop_radius:.1f}, "
                f"stub {r_stub:.1f}); lane attach offset "
                f"{abs(self.entry_lat):.2f}/{abs(self.exit_lat):.2f} m\n"
                f"      lanes in [{entry}]  out [{exit_}]"
                + ("" if not self.opt.drop_point_turnaround else
                   f"; point turnaround {self.in_edge}->{self.out_edge} removed"))


# --------------------------------------------------------------------------
# netconvert
# --------------------------------------------------------------------------

def check_routes(route_files, uturns):
    """Count routes that drive a point turnaround this run removes.

    Removing the pivot is a real change to an existing scenario, so it is
    reported as a number rather than left to be discovered at run time: those
    routes become disconnected and SUMO will reject (or, with
    --ignore-route-errors, silently drop) the vehicles using them.
    """
    moves = {(u.in_edge, u.out_edge): u.jid for u in uturns
             if u.opt.drop_point_turnaround}
    if not moves or not route_files:
        return
    hits = {m: 0 for m in moves}
    total = 0
    for rf in route_files:
        for _, el in ET.iterparse(str(rf)):
            if el.tag == "route":
                total += 1
                ed = (el.get("edges") or "").split()
                for a, b in zip(ed, ed[1:]):
                    if (a, b) in hits:
                        hits[(a, b)] += 1
                el.clear()
    broken = sum(hits.values())
    print(f"[sumo_uturn] route check: {total} route(s) scanned, "
          f"{broken} traverse a removed point turnaround")
    for (a, b), n in hits.items():
        if n:
            print(f"      {n} x {a} -> {b} (at {moves[(a, b)]}) - reroute these, or keep "
                  f"the pivot with --drop-point-turnaround")


def repair_routes(specs, uturns):
    """Send routes that drove a removed point turnaround round the loop instead.

    Removing the pivot disconnects every route that used it, so a scenario is
    only usable with the new network once its routes are repaired. The splice
    keys on the same in-edge/out-edge adjacency that was removed, so what gets
    rewritten is exactly what would otherwise be broken.

    Each spec is "IN[,OUT]"; without OUT the file is rewritten in place and the
    original kept as IN.bak.
    """
    loops = {(u.in_edge, u.out_edge): [u.eid_in, u.eid_loop, u.eid_out]
             for u in uturns if u.opt.drop_point_turnaround}
    if not loops:
        print("[sumo_uturn] --repair-routes: no turnaround was removed, nothing to repair")
        return

    def splice(edges):
        out, n = [], 0
        for i, e in enumerate(edges):
            out.append(e)
            nxt = edges[i + 1] if i + 1 < len(edges) else None
            if (e, nxt) in loops:
                out.extend(loops[(e, nxt)])
                n += 1
        return out, n

    for spec in specs:
        src, _, dst = spec.partition(",")
        src_path = Path(src.strip())
        dst_path = Path(dst.strip()) if dst.strip() else src_path
        if not src_path.is_file():
            raise SystemExit(f"--repair-routes: no such file {src_path}")
        tree = ET.parse(str(src_path))
        routes = changed = splices = 0
        for el in tree.getroot().iter("route"):
            edges = (el.get("edges") or "").split()
            if not edges:
                continue
            routes += 1
            new_edges, n = splice(edges)
            if n:
                el.set("edges", " ".join(new_edges))
                changed += 1
                splices += n
        if dst_path == src_path:
            backup = src_path.with_suffix(src_path.suffix + ".bak")
            if not backup.exists():
                shutil.copy2(str(src_path), str(backup))
                print(f"[sumo_uturn] kept the original as {backup.name}")
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        tree.write(str(dst_path), encoding="UTF-8", xml_declaration=True)
        print(f"[sumo_uturn] repaired {dst_path.name}: {changed} of {routes} route(s), "
              f"{splices} turnaround(s) sent round a loop")


def find_netconvert():
    exe = shutil.which("netconvert")
    if exe:
        return exe
    home = os.environ.get("SUMO_HOME")
    if home:
        for name in ("netconvert", "netconvert.exe"):
            p = Path(home) / "bin" / name
            if p.exists():
                return str(p)
    raise SystemExit("netconvert not found - put SUMO's bin on PATH or set SUMO_HOME")


def write_patch(uturns, net, outdir):
    def dump(name, tag, lines):
        p = Path(outdir) / name
        p.write_text('<?xml version="1.0" encoding="UTF-8"?>\n'
                     f"<{tag}>\n" + "\n".join(lines) + f"\n</{tag}>\n", encoding="utf-8")
        return p

    nod = dump("uturn.nod.xml", "nodes", [l for u in uturns for l in u.nodes_xml()])
    edg = dump("uturn.edg.xml", "edges", [l for u in uturns for l in u.edges_xml()])
    con = dump("uturn.con.xml", "connections",
               [l for u in uturns for l in u.connections_xml(net)])
    return nod, edg, con


def _turnarounds(net_path):
    found = set()
    for _, el in ET.iterparse(str(net_path), events=("end",)):
        if el.tag == "connection":
            if el.get("dir") == "t" and not (el.get("from") or "").startswith(":"):
                found.add((el.get("from"), el.get("to")))
            el.clear()
    return found


def surplus_turnarounds(source_net, built_net):
    """Turnarounds the build invented that the source network did not have."""
    return _turnarounds(built_net) - _turnarounds(source_net)


def keep_source_edge_order(net_in, out):
    """Reorder `out` so the source net's edges keep their positions and the new
    ones come last. Returns how many were moved.

    SUMO hands each lane one of a pre-allocated pool of random generators, chosen
    by the LANE'S POSITION in load order - and load order is the order edges
    appear in the file. netconvert writes them sorted by id, so edges added
    anywhere but the end of the alphabet land interleaved and shift every lane
    after them onto a different generator. Every vehicle then draws its
    car-following randomness from a different stream, network-wide, from the
    second step, with no causal path to the edit.

    Measured on MLK: the 16 U-turn edges land at positions 4-6, 14-16, 483-486 and
    724-729, and 16 vehicles differ on the second step, on six edges nowhere near
    either terminus, each by one dawdle quantum. With the added edges written last
    the patched network reproduces the source network exactly - and, unlike
    `--thread-rngs 1`, it does so at SUMO's own defaults, so a result recorded
    before the edit stays a valid reference after it.

    This only holds while the patch is strictly ADDITIVE. Removing or renaming a
    source edge shifts the lanes after it whatever the order, and then one
    generator on both sides is the only instrument left.
    """
    order = {e.get("id"): i
             for i, e in enumerate(ET.parse(net_in).getroot().findall("edge"))}
    tree = ET.parse(out)
    root = tree.getroot()
    edges = root.findall("edge")
    if not edges:
        return 0
    added = [e for e in edges if e.get("id") not in order]
    if not added:
        return 0
    kept = sorted((e for e in edges if e.get("id") in order),
                  key=lambda e: order[e.get("id")])
    at = list(root).index(edges[0])
    for e in edges:
        root.remove(e)
    for i, e in enumerate(kept + added):
        root.insert(at + i, e)
    tree.write(out, encoding="UTF-8", xml_declaration=True)
    return len(added)


def run_netconvert(net_in, out, nod, edg, con, extra=()):
    cmd = [find_netconvert(),
           "--sumo-net-file", str(net_in),
           "--node-files", str(nod),
           "--edge-files", str(edg),
           "--connection-files", str(con),
           "--output-file", str(out),
           # keep the network exactly where it was: no re-offset, no geometry
           # simplification, no re-guessing of things the source net already fixed
           "--offset.disable-normalization", "true",
           "--geometry.remove", "false",
           "--geometry.max-grade.fix", "false",
           # Do not GENERATE turnarounds. The ones the source network already has
           # are loaded with it and survive; without this netconvert re-derives them
           # from geometry and invents ones the source never had - MLK goes 184 ->
           # 244 on a plain round-trip with no patch at all, at stub ends this tool
           # never touches. The U-turn's own links are declared explicitly in the
           # connection file, so they are unaffected.
           "--no-turnarounds", "true",
           "--tls.guess", "false",
           "--junctions.corner-detail", "5",
           *extra]
    print("[sumo_uturn] " + " ".join(cmd))
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.stdout.strip():
        print(res.stdout.strip())
    if res.returncode != 0:
        print(res.stderr.strip(), file=sys.stderr)
        raise SystemExit(f"netconvert failed ({res.returncode})")
    moved = keep_source_edge_order(net_in, out)
    if moved:
        print(f"[sumo_uturn] {moved} added edge(s) written last, so the source net's "
              f"lanes keep their index (see keep_source_edge_order)")
    warn = [l for l in res.stderr.splitlines() if l.strip()]
    if warn:
        print("[sumo_uturn] netconvert notes:")
        for l in warn[:30]:
            print("   " + l)
        if len(warn) > 30:
            print(f"   ... {len(warn) - 30} more")


# --------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------

def build_parser():
    p = argparse.ArgumentParser(
        description="Attach drivable curved U-turn loops to SUMO corridor ends.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__.split("Usage\n-----\n")[-1])
    p.add_argument("--net", required=True, help="input .net.xml")
    p.add_argument("--out", help="output .net.xml (required unless --list/--dry-run)")

    s = p.add_argument_group("where to put loops")
    s.add_argument("--terminal", action="append", default=[], metavar="JUNCTION",
                   help="terminus junction id (repeatable)")
    s.add_argument("--pair", action="append", default=[], metavar="IN,OUT",
                   help="explicit in-edge,out-edge pair (repeatable); use when the "
                        "terminus is not a simple stub")
    s.add_argument("--corridor", action="append", default=[], metavar="CSV",
                   help="ordered corridor edge list; its last edge's far node is a "
                        "terminus (repeatable - one per direction)")
    s.add_argument("--auto", action="store_true",
                   help="every detected corridor terminus (usually far too many - --list first)")
    s.add_argument("--list", action="store_true", help="list corridor termini and exit")
    s.add_argument("--terminal-angle", type=float, default=150.0, metavar="DEG",
                   help="how far back a movement must turn to count as a terminus "
                        "(default 150; 180 = straight back)")

    g = p.add_argument_group("geometry")
    g.add_argument("--loop-radius", type=float, default=DEFAULT_LOOP_RADIUS)
    g.add_argument("--ramp-radius", type=float, default=DEFAULT_RAMP_RADIUS)
    g.add_argument("--straight-past", type=float, default=DEFAULT_STRAIGHT_PAST,
                   help="straight run past the terminus before the ramp peels off (m)")
    g.add_argument("--side", choices=("right", "left"), default="right",
                   help="which side the loop peels off to (default right = RH traffic)")
    g.add_argument("--ds", type=float, default=DEFAULT_DS, help="polyline step (m)")
    g.add_argument("--throat-radius", type=float, default=DEFAULT_THROAT_RADIUS)
    g.add_argument("--neck-radius", type=float, default=None,
                   help="re-emit the terminus node with this junction radius instead of "
                        "pinning its shape (implies --recompute-terminus-shape)")
    g.add_argument("--recompute-terminus-shape", action="store_true",
                   help="let netconvert recompute the terminus junction polygon. Default "
                        "is to pin the one it already has: two extra legs otherwise widen "
                        "it and every edge meeting it is clipped SHORTER, which moves the "
                        "corridor and invalidates stored results (see nodes_xml)")

    l = p.add_argument_group("lanes and links")
    l.add_argument("--lanes", type=int, default=1)
    l.add_argument("--speed", type=float, default=DEFAULT_SPEED)
    l.add_argument("--priority", default=None)
    l.add_argument("--disallow", default=None)
    l.add_argument("--from-lane", type=int, default=None)
    l.add_argument("--to-lane", type=int, default=None)
    l.add_argument("--feed-all-lanes", action="store_true",
                   help="also link the ramp to the receiving edge's outer lanes. Silences "
                        "netconvert's 'lane is not connected' warning, but those links "
                        "cross a lane width inside the junction - see lane_map()")
    l.add_argument("--drop-point-turnaround", action="store_true",
                   help="also DELETE the on-the-spot pivot the loop replaces. Only needed "
                        "when demand is generated by a router, which would prefer a 5 m "
                        "pivot to a 390 m loop. With explicit routes it buys nothing - a "
                        "vehicle drives the loop because its route names those edges - and "
                        "it disconnects every route that used the pivot")
    l.add_argument("--check-routes", action="append", default=[], metavar="ROU.XML",
                   help="report how many routes in this file drive a removed point "
                        "turnaround (repeatable)")
    l.add_argument("--repair-routes", action="append", default=[], metavar="IN[,OUT]",
                   help="rewrite those routes to drive the loop instead, so they stay "
                        "connected. Without OUT the file is updated in place and the "
                        "original kept as IN.bak (repeatable)")

    o = p.add_argument_group("output")
    o.add_argument("--prefix", default="ut", help="id prefix for new edges/nodes")
    o.add_argument("--keep-patch", metavar="DIR",
                   help="write the plain-XML patch files here instead of a temp dir")
    o.add_argument("--dry-run", action="store_true", help="build and report, do not run netconvert")
    o.add_argument("--netconvert-arg", action="append", default=[], metavar="ARG",
                   help="extra netconvert argument (repeatable)")
    return p


def main(argv=None):
    opt = build_parser().parse_args(argv)
    net = Net(opt.net)

    if opt.list:
        stubs = net.stub_ends(opt.terminal_angle)
        print(f"{len(stubs)} corridor terminus/termini in {Path(opt.net).name} "
              f"(turn-back >= {opt.terminal_angle:g} deg):")
        for jid, ein, eout in stubs:
            j = net.junctions[jid]
            print(f"  {jid:<16} {j['type']:<12} ({j['x']:.1f}, {j['y']:.1f})  "
                  f"{net.turn_angle(ein, eout):5.1f} deg  "
                  f"in={ein} ({net.edges[ein]['num_lanes']}L)  "
                  f"out={eout} ({net.edges[eout]['num_lanes']}L)")
        return 0

    if opt.side == "left" and not net.lefthand:
        print("[sumo_uturn] note: --side left on a right-hand-traffic network")

    targets = []  # (junction, in_edge, out_edge)
    for jid in opt.terminal:
        targets.append((jid, *net.resolve_terminal(jid, opt.terminal_angle)))
    for spec in opt.pair:
        ein, _, eout = spec.partition(",")
        ein, eout = ein.strip(), eout.strip()
        for e in (ein, eout):
            if e not in net.edges:
                raise SystemExit(f"--pair: no edge '{e}' in the network")
        if net.edges[ein]["to"] != net.edges[eout]["from"]:
            raise SystemExit(f"--pair {spec}: '{ein}' and '{eout}' do not meet at one junction")
        targets.append((net.edges[ein]["to"], ein, eout))
    for path in opt.corridor:
        jid = corridor_terminal(net, path)
        targets.append((jid, *net.resolve_terminal(jid, opt.terminal_angle)))
    if opt.auto:
        targets.extend(net.stub_ends(opt.terminal_angle))

    seen, unique = set(), []
    for t in targets:
        if t[0] not in seen:
            seen.add(t[0])
            unique.append(t)
    if not unique:
        raise SystemExit("nothing selected - pass --terminal / --pair / --corridor / --auto "
                         "(or --list to see the candidates)")
    if not opt.out and not opt.dry_run:
        raise SystemExit("--out is required (or use --dry-run)")

    uturns = [UTurn(net, jid, ein, eout, opt) for jid, ein, eout in unique]
    print(f"[sumo_uturn] {len(uturns)} U-turn loop(s):")
    for u in uturns:
        print(u.report())
    check_routes(opt.check_routes, uturns)

    outdir = opt.keep_patch or tempfile.mkdtemp(prefix="sumo_uturn_")
    Path(outdir).mkdir(parents=True, exist_ok=True)
    nod, edg, con = write_patch(uturns, net, outdir)
    print(f"[sumo_uturn] patch written to {outdir}")
    if opt.dry_run:
        return 0
    run_netconvert(opt.net, opt.out, nod, edg, con, opt.netconvert_arg)
    # The contract is "adds the loop, changes nothing else", so check it.
    surplus = surplus_turnarounds(opt.net, opt.out)
    if surplus:
        print(f"[sumo_uturn] WARN the build invented {len(surplus)} turnaround(s) the "
              f"source network does not have, e.g. {sorted(surplus)[:3]}. Expected 0.")
    else:
        print("[sumo_uturn] turnarounds match the source network exactly")
    print(f"[sumo_uturn] wrote {opt.out}")
    repair_routes(opt.repair_routes, uturns)
    return 0


if __name__ == "__main__":
    sys.exit(main())
