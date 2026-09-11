"""CARLA, reached through FIXS.

A user bringing a CARLA-shaped controller changes its import and keeps the rest
of its code::

    -import carla
    +import fixs.carla as carla

Every ``carla.XX`` site in their file then goes on working. What changes is WHO
EXECUTES the calls that touch the simulator: FIXS does, against the session the
bridge already owns. The user never opens a client, never ticks the world, and
cannot end up with a second connection racing the one driving the run.

Two kinds of name live here, and the split is the whole design.

VALUE TYPES -- ``Location``, ``Transform``, ``Rotation``, ``Vector3D``,
``LaneType``, ``VehicleControl`` and the rest -- are re-exported unchanged.
Constructing one is arithmetic: three numbers in a struct, no connection
involved, nothing to mediate. Relaying is about who performs calls, and a
``Location(x=2.5)`` is not a call. Measured on a real 2656-line controller: 38
of its 40 ``carla.`` sites are these, and all 38 need no FIXS involvement at all.

SESSION OBJECTS -- ``client``, ``world``, ``map`` -- are answered by FIXS from
the backend. These are the two remaining sites, and they are where the relay
earns its keep: one owner of the CARLA session, and a seam a non-CARLA backend
can answer later.

WHAT IS REFUSED, and why it is refused rather than quietly allowed:

    carla.Client(host, port)   a second connection to the same server, racing
                              the bridge's. Use `carla.client`.
    world.tick()              only one client may advance a synchronous world,
                              and the bridge is it. Return from control(ego, dt)
                              instead -- that IS the tick.

Actuation is the third thing FIXS keeps, and it is not refused here because it
never reaches here: the bridge applies the FIXS record to the physics ego, so a
controller commands with ``ego.set(acceleratorPedalDesired=...)`` rather than
``actor.apply_control()``. Two writers would fight over the actuator.

See ORNL-Real-Sim/FIXS#305.
"""
from __future__ import annotations

import importlib

__all__ = ['client', 'world', 'map', 'available']

_carla = None
_mapCache = None


def _real():
    """The installed CARLA package. Value types come from here verbatim."""
    global _carla
    if _carla is None:
        _carla = importlib.import_module('carla')
    return _carla


def _backend():
    from CommonLib.VirEnv.EgoControllerHost import currentBackend
    return currentBackend()


def available():
    """Is a FIXS backend behind this module? False under a bare unit test."""
    return _backend() is not None


class _Refused(RuntimeError):
    """A call FIXS holds rather than forwards."""


def Client(*_args, **_kwargs):                              # noqa: N802
    """Refused: FIXS owns the connection.

    Opening a second client to the same server gives a synchronous run two
    parties who each think they may advance it. The bridge is already connected;
    ask for that one.
    """
    raise _Refused(
        "carla.Client(...) is not available through FIXS -- the bridge already "
        "holds the connection, and a second client racing it is how a "
        "synchronous run loses its clock.\n"
        "    use:  world = carla.world      (or carla.client for the client)")


def __getattr__(name):
    """Session names from FIXS; everything else from CARLA itself."""
    if name == 'client':
        b = _backend()
        c = getattr(b, 'carlaClient', None) if b is not None else None
        if c is None:
            raise _Refused(
                "carla.client needs a running FIXS backend, and none is "
                "registered. This module is only live inside a controller the "
                "bridge loaded (EgoSetup.Controller).")
        return c
    if name == 'world':
        b = _backend()
        w = getattr(b, 'carlaWorld', None) if b is not None else None
        if w is None:
            raise _Refused(
                "carla.world needs a running FIXS backend, and none is "
                "registered. This module is only live inside a controller the "
                "bridge loaded (EgoSetup.Controller).")
        return w
    if name == 'map':
        global _mapCache
        if _mapCache is None:
            # Cached because get_map() serialises the whole OpenDRIVE; a
            # controller asking per tick would pay that on every step.
            _mapCache = __getattr__('world').get_map()
        return _mapCache
    return getattr(_real(), name)


def _reset():
    """Drop the cached map -- for tests, and for a backend swapped mid-process."""
    global _mapCache
    _mapCache = None
