"""SumoSetup.LaneChangeMode -- SUMO's lane-change mode, set through FIXS.

TrafficLayer owns SUMO through libsumo. An application is a FIXS CLIENT holding
a port, so it has no TraCI connection of its own: calling
`traci.vehicle.setLaneChangeMode` from one fails with "Not connected". A
per-vehicle SUMO mode therefore travels the same road SpeedMode already does --
config -> ConfigHelper -> TrafficHelper, applied by the process that owns SUMO.

    -1   leave SUMO's own default (597, every motive) alone
    512  strategic only: still reaches the lane its next turn needs, but never
         changes lane by choice -- for comparing against a driver that holds one
         lane, such as a CARLA agent steering a fixed route polyline

    python -m pytest tests/Python/unit/test_lane_change_mode.py
"""
from __future__ import annotations

import io
import textwrap

import pytest

from CommonLib.ConfigHelper import ConfigHelper


def _config(tmp_path, sumo_block):
    p = tmp_path / "scenario.yaml"
    p.write_text(textwrap.dedent(sumo_block), encoding="utf-8")
    cfg = ConfigHelper()
    cfg.getConfig(str(p))
    return cfg


def test_absent_means_leave_sumos_default_alone(tmp_path):
    """Every scenario written before this key existed must keep behaving as it
    did, so 'not set' cannot mean 'mode 0' -- that would silently forbid the
    strategic changes a vehicle needs to follow its route."""
    cfg = _config(tmp_path, """
        SumoSetup:
          SpeedMode: 31
    """)
    assert cfg.Sumo_setup["LaneChangeMode"] == -1


def test_the_configured_mode_is_carried(tmp_path):
    cfg = _config(tmp_path, """
        SumoSetup:
          SpeedMode: 31
          LaneChangeMode: 512
    """)
    assert cfg.Sumo_setup["LaneChangeMode"] == 512


def test_it_sits_beside_speed_mode(tmp_path):
    """Same section, same shape, same owner -- the point of the key is that it
    is not special."""
    cfg = _config(tmp_path, """
        SumoSetup:
          SpeedMode: 32
          LaneChangeMode: 0
    """)
    assert cfg.Sumo_setup["SpeedMode"] == 32
    assert cfg.Sumo_setup["LaneChangeMode"] == 0
