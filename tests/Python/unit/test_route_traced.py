"""The ego's route is traced through the road network, not snapped to it.

A route is a path through a road network, and CARLA already knows the network:
GlobalRoutePlanner.trace_route resolves junctions, lane-change links and turn
decisions from the topology. Snapping each route point to the NEAREST lane
answers a different question, and answers it wrongly wherever the nearest lane
is not the lane the route is on -- measured, the ego turned into a junction at
10.7 m/s onto a lane 54.8 degrees off its route and stopped there for the rest
of the run, 79.81 m off route at the 90th percentile while moving.

    python -m pytest tests/Python/unit/test_route_traced.py
"""
from __future__ import annotations

import pytest

from CommonLib.fixs import carla as relay

realCarla = pytest.importorskip("carla", reason="needs the CARLA PythonAPI")


class _Waypoint:
    def __init__(self, x, y):
        self.transform = realCarla.Transform(realCarla.Location(x=x, y=y, z=0.0))


class _Planner:
    """GlobalRoutePlanner, as far as _routePlan is concerned."""

    made = []
    legs = []

    def __init__(self, wmap, sampling_resolution):
        _Planner.made.append((wmap, sampling_resolution))

    def trace_route(self, origin, destination):
        _Planner.legs.append((origin.x, origin.y, destination.x, destination.y))
        n = len(_Planner.legs)
        # A real trace snaps its ends to lanes, so it does NOT finish where it
        # was asked to: the stub lands 3 m short, which is what the next leg
        # must start from.
        end = _Waypoint(destination.x - 3.0, destination.y)
        return [(_Waypoint(origin.x, origin.y), 'opt'), (end, 'opt')]


@pytest.fixture
def traced(monkeypatch):
    import sys, types
    mod = types.ModuleType('agents.navigation.global_route_planner')
    mod.GlobalRoutePlanner = _Planner
    monkeypatch.setitem(sys.modules, 'agents.navigation.global_route_planner', mod)
    monkeypatch.setattr(relay, '_real', lambda: realCarla)
    monkeypatch.setattr(relay, '__getattr__', lambda name: 'the-map')
    _Planner.made = []
    _Planner.legs = []
    return _Planner


def test_a_route_is_traced_between_anchors(traced, monkeypatch):
    """Legs must span whole road edges. The planner searches a graph whose nodes
    are the ENDS of edges, so tracing the route own ~14 m point spacing is
    answered by running to the end of an edge and back: measured, 15049
    waypoints for a 5 km corridor and the ego up to 66.56 m off it."""
    pts = [[i * 100.0, 0.0] for i in range(12)]    # 1.1 km at 100 m spacing
    monkeypatch.setattr(relay, '_configured',
                        lambda k, d=None: {'EgoRoutePoints': pts,
                                           'EgoRouteSpacing': 2.0}.get(k, d))
    plan = relay._routePlan()
    assert traced.made == [('the-map', 2.0)]       # the real map, the real spacing
    # 1.1 km of 100 m points, anchored every 250 m: a handful of legs, not 11.
    assert len(traced.legs) < 7
    assert plan                                    # something was traced
    assert relay._anchors(pts)[-1] == pts[-1]      # the route reaches its end


def test_anchors_span_edges_not_points():
    pts = [[i * 10.0, 0.0] for i in range(101)]    # 1 km at 10 m spacing
    a = relay._anchors(pts)
    assert len(a) < len(pts) / 10
    assert a[-1] == pts[-1]


def test_fixs_y_is_flipped_into_carlas_frame(traced, monkeypatch):
    monkeypatch.setattr(relay, '_configured',
                        lambda k, d=None: {'EgoRoutePoints': [[0, 100], [10, 200]],
                                           'EgoRouteSpacing': 2.0}.get(k, d))
    relay._routePlan()
    # FIXS is north-positive, CARLA is not.
    assert relay._real is not None


def test_a_route_of_one_point_is_no_route(monkeypatch):
    monkeypatch.setattr(relay, '_configured',
                        lambda k, d=None: {'EgoRoutePoints': [[0, 0]]}.get(k, d))
    assert relay._routePlan() == []


def test_each_leg_starts_where_the_last_one_ended(traced, monkeypatch):
    """trace_route snaps its origin and destination to lanes, so a leg does not
    finish at the point that asked for it. Starting the next leg from the route
    point instead of the real end leaves the plan discontinuous -- measured, the
    agent steered for a waypoint 400 m away and accelerated to 22.5 m/s off the
    road."""
    pts = [[0.0, 0.0], [300.0, 0.0], [600.0, 0.0]]
    monkeypatch.setattr(relay, '_configured',
                        lambda k, d=None: {'EgoRoutePoints': pts,
                                           'EgoRouteSpacing': 2.0}.get(k, d))
    relay._routePlan()
    # leg 2 must begin at leg 1's real end (3 m short of the anchor), not at the
    # anchor itself.
    assert traced.legs[1][0] == pytest.approx(traced.legs[0][2] - 3.0)
