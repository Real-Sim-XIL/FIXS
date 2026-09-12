"""A route is laid on lanes that go the way the route goes.

`get_waypoint(project_to_road=True)` snaps to the NEAREST driving lane and says
nothing about direction. On an out-and-back corridor the two carriageways are
metres apart, so the return leg snaps onto the OUTBOUND lane and the plan leads
the ego up the wrong side of the road.

Measured, before this: the ego drove the wrong way along lane E3_0 until it met
an oncoming vehicle 3.09 m ahead -- both at a standstill, facing 109.44 and
290.61 degrees -- and the agent held an emergency stop for the remaining 630 s.
The agent was right. The route was not.

    python -m pytest tests/Python/unit/test_route_direction.py
"""
from __future__ import annotations

import pytest

from CommonLib.fixs import carla as relay

realCarla = pytest.importorskip("carla", reason="needs the CARLA PythonAPI")


class _Lane:
    """A CARLA waypoint, as far as the direction check is concerned."""

    def __init__(self, yaw, left=None, right=None, driving=True):
        self.transform = realCarla.Transform(realCarla.Location(0, 0, 0),
                                             realCarla.Rotation(yaw=yaw))
        self.lane_type = (realCarla.LaneType.Driving if driving
                          else realCarla.LaneType.Sidewalk)
        self._left, self._right = left, right

    def get_left_lane(self):
        return self._left

    def get_right_lane(self):
        return self._right


def test_a_lane_already_going_the_right_way_is_kept():
    wp = _Lane(yaw=90.0)
    assert relay._facingLane(wp, 90.0, realCarla) is wp


def test_the_opposing_carriageway_is_crossed_to_the_right_one():
    """The snap landed on the outbound lane; the route is coming back."""
    correct = _Lane(yaw=-90.0)
    snapped = _Lane(yaw=90.0, left=correct)
    assert relay._facingLane(snapped, -90.0, realCarla) is correct


def test_a_lane_is_never_invented_when_none_agrees():
    """A dead end in the map is not a licence to put the ego on a sidewalk."""
    snapped = _Lane(yaw=90.0, left=_Lane(yaw=85.0, driving=False))
    assert relay._facingLane(snapped, -90.0, realCarla) is snapped


def test_the_heading_comes_from_the_route_itself():
    east = relay._headingAt([(0.0, 0.0), (10.0, 0.0)], 0)
    assert east == pytest.approx(0.0)
    # FIXS y is north-positive, CARLA's is flipped: going north is -90 in CARLA.
    north = relay._headingAt([(0.0, 0.0), (0.0, 10.0)], 0)
    assert north == pytest.approx(-90.0)


def test_the_last_point_keeps_the_headings_of_the_one_before():
    pts = [(0.0, 0.0), (10.0, 0.0), (20.0, 0.0)]
    assert relay._headingAt(pts, 2) == pytest.approx(relay._headingAt(pts, 1))


class _Map:
    """A map whose two carriageways are separate roads, as this corridor's is:
    no lane link joins them, so only a sideways probe can find the other one."""

    def __init__(self, outboundYaw, returnYaw, gap):
        self.outbound, self.back, self.gap = outboundYaw, returnYaw, gap
        self.asked = []

    junction = False

    def get_waypoint(self, loc, project_to_road=True):
        self.asked.append((loc.x, loc.y))
        away = (loc.x ** 2 + loc.y ** 2) ** 0.5
        lane = _Lane(yaw=self.back if away >= self.gap else self.outbound)
        lane.is_junction = self.junction
        return lane


def test_a_separate_opposing_road_is_found_abeam():
    """184 of 2558 waypoints snapped against the route here and NOT ONE could
    reach an agreeing lane by crossing lane links -- the directions are separate
    roads. Probing sideways is what finds them."""
    cmap = _Map(outboundYaw=90.0, returnYaw=-90.0, gap=6.0)
    snapped = _Lane(yaw=90.0)
    got = relay._facingLane(snapped, -90.0, realCarla, cmap, x=0.0, y=0.0)
    assert got is not snapped
    assert got.transform.rotation.yaw == pytest.approx(-90.0)


def test_nothing_within_reach_leaves_carlas_own_answer():
    cmap = _Map(outboundYaw=90.0, returnYaw=90.0, gap=6.0)   # no opposing road
    snapped = _Lane(yaw=90.0)
    assert relay._facingLane(snapped, -90.0, realCarla, cmap, 0.0, 0.0) is snapped


def test_the_probe_walks_outwards_and_stops():
    cmap = _Map(outboundYaw=90.0, returnYaw=90.0, gap=6.0)
    relay._facingLane(_Lane(yaw=90.0), -90.0, realCarla, cmap, 0.0, 0.0, reach=8.0)
    assert max((p[0] ** 2 + p[1] ** 2) ** 0.5 for p in cmap.asked) <= 8.0


def test_a_turning_lane_is_not_the_same_road():
    """A junction turning lane 54.8 degrees off the route passed the old
    right-angle test, and the sideways probe put the plan 10 m onto it: the ego
    turned into the junction at 10.7 m/s and stopped there for the rest of the
    run. Within a right angle means 'not opposed'; it does not mean 'this is my
    lane'."""
    route_east = 0.0
    turning = _Lane(yaw=54.8)
    assert relay._agrees(turning, route_east)        # not opposed
    assert not relay._accepts(turning, route_east)   # but not the same road


def test_a_junction_lane_is_never_crossed_to():
    """Inside a junction the turning lanes fan out and one is always abeam, so
    lateral position says nothing about which movement the route takes."""
    j = _Lane(yaw=2.0)
    j.lane_type = realCarla.LaneType.Driving
    j.is_junction = True
    assert not relay._accepts(j, 0.0)


def test_a_junction_snap_is_left_as_carla_gave_it():
    """Letting junction lanes compete -- even scored by heading -- measured
    WORSE: the worst excursion grew from 13.85 m to 88.60 m, because a
    well-aligned internal lane can still belong to a different movement. A
    junction is crossed by connectivity, not by looking sideways."""
    snapped = _Lane(yaw=175.0)
    snapped.is_junction = True
    cmap = _Map(outboundYaw=175.0, returnYaw=0.0, gap=4.0)
    assert relay._facingLane(snapped, 0.0, realCarla, cmap, 0.0, 0.0) is snapped


def test_the_opposing_carriageway_is_still_crossed_to():
    """The fix must not undo what it was added for: a genuinely opposed lane on
    a separate parallel road is still corrected."""
    cmap = _Map(outboundYaw=90.0, returnYaw=-90.0, gap=6.0)
    snapped = _Lane(yaw=90.0)
    got = relay._facingLane(snapped, -90.0, realCarla, cmap, x=0.0, y=0.0)
    assert got is not snapped
    assert got.transform.rotation.yaw == pytest.approx(-90.0)
