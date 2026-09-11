"""EgoControllerHost -- load a user-written ego controller, and run it (#325).

A controller is a Python file the scenario names. FIXS imports it and calls one
function per step; the function reads the ego's state and writes a command onto
it. There is no base class to inherit, no registry, and no socket::

    # apps/<app>/my_controller.py
    def control(ego, dt):
        ego.set(acceleratorPedalDesired=0.30,
                brakePedalDesired=0.00,
                steerAngleDesired=0.10)

    # scenario yaml
    EgoActuationSource: embedded
    EgoController:      apps/<app>/my_controller.py


WHY THIS IS A HOOK AND NOT A CLIENT
-----------------------------------
A controller can already run as an ordinary FIXS client, and for most work it
should: its own process, killable, isolated, and a crash cannot take the bridge
down with it. The one thing it cannot do there is beat the feed. TrafficLayer
exchanges at 0.1 s, so a client sees state once per feed and its command is held
across every world step in between -- with CarlaTimeStep 0.05 that is one whole
step of a 50 ms-stale command, every feed, forever.

This hook exists for exactly that gap. An embedded controller is called from
inside the bridge's step loop, so it sees the pose the backend just measured and
its command is applied before the next tick. Nothing is serialised; it is a
function call.

The rate is therefore not configurable and never has been: it is a consequence
of where the controller lives.

    EgoActuationSource: external   own process, over FIXS   -> feed rate
    EgoActuationSource: embedded   in the bridge            -> CarlaTimeStep

Because both deployments write through ``ego.set``, the same file runs either
way -- which makes "does the fast loop actually change the result?" a one-
variable experiment instead of an argument.


THE TWO COMMAND SHAPES
----------------------
Which fields you write IS which plant interface you are commanding through
(see fixs.commandKind):

    ego.set(acceleratorPedalDesired=, brakePedalDesired=, steerAngleDesired=)
        pedals + steer. YOU close the loop; your rate is the loop rate.
        Carla: apply_control.

    ego.set(speedDesired=, steerAngleDesired=)
        speed + steer. THE PLANT closes the loop, at its own rate with its own
        gains. Carla: apply_ackermann_control.

The second is worth trying before reaching for this hook at all: it gives a fast
inner loop on the plant's side without an in-process controller. It also moves
the tuning out of your reach, which is the trade.


WHAT `ego` CARRIES, AND HOW FRESH IT IS
---------------------------------------
Two clocks meet in one record, and pretending otherwise is how a controller
computes a time-to-collision from two different instants:

    LIVE, this call      positionX/Y/Z, heading, speed, acceleration
    HELD since the feed  speedDesired      (the eco advisory -- genuinely 10 Hz)
                         signalLightColor  (the traffic simulator owns the phase)
    ego.feedAge          seconds since the last feed: how old the held ones are

A controller that ignores ``feedAge`` is fine. One that differentiates a held
field, or divides a live field by a held one, needs to know.


LIFECYCLE
---------
``setup`` and ``shutdown`` are optional; ``control`` is not.

    setup(config, egoId) -> state        once, before the run
    control(ego, dt, state=None)         every step
    shutdown(state)                      once, after

A class named ``Controller`` is accepted in place of the functions, with
``__init__(config, egoId)`` and ``control(ego, dt)``. Neither form inherits
anything: FIXS checks for the method, never for a base class.


WHAT A CONTROLLER MUST NOT DO
-----------------------------
* **Touch the backend or the simulator.** State arrives as an argument. A
  controller that grabs a ``carla`` handle stops working under an XIL plant and
  in ``tests/VirEnv/replay_core.py``, where there is no simulator at all.
* **Call fixs.recv() or fixs.send().** Embedded, the bridge owns the tick; a
  controller calling them corrupts the protocol. That is also why a controller
  is a function rather than a script with its own loop -- the deployment
  supplies the loop.

An exception from ``control`` stops the run and names the file. It does not fall
back to EgoDriver: a silent fallback produces a run that looks fine and is not,
which is the failure mode this codebase has paid for most.


NOTE ON THE MIRROR
------------------
CommonLib/VirEnv/*.py and Carla/VirEnv/*.py are peers of C++ files, name for name
(#335). This module deliberately has NO C++ twin: importing a user's .py at
runtime is a Python capability, and the C++ bridge will not grow one shaped like
this. Keeping it in its own module is what lets the mirrored files stay mirrored
-- an earlier cut of this put the run half inside Carla/VirEnv/mainVirCarla.py,
which quietly made that file stop being a faithful peer of mainVirCarla.cpp.

For the same reason EgoDriver is left exactly as it is. It has a C++ twin, and
_driveEgoFallback applies through the actor rather than through a backend verb,
so routing it through this contract would change the verb transcript
tests/VirEnv/test_core_parity.py compares. EgoDriver is not "the default
implementation of this hook"; it is the built-in driver, and this is a second,
Python-only slot beside it. The cost of that honesty is that EgoDriver cannot
emit the speed+steer shape -- a missing feature, not a reason to restructure.
"""

import importlib.util
import os
import sys

__all__ = ['ControllerError', 'loadController', 'LoadedController', 'runController']


class ControllerError(Exception):
    """A controller could not be loaded, or refused to behave at load time."""


#: Tried in order when the config names a file with no ``:attribute``.
ENTRY_POINTS = ('Controller', 'control')


class LoadedController:
    """What :func:`loadController` returns: a uniform call surface.

    Normalises the accepted shapes -- a class, or a module-level function with
    or without ``setup``/``shutdown`` -- so the bridge has one thing to call and
    does not branch on how the user chose to write it.
    """

    def __init__(self, spec, obj, setupFn=None, shutdownFn=None, isClass=False):
        self.spec = spec
        self._obj = obj
        self._setup = setupFn
        self._shutdown = shutdownFn
        self._isClass = isClass
        self._state = None
        self._instance = None

    def setup(self, config, egoId, backend=None, core=None):
        # Registered before the controller is built, because a CARLA-shaped
        # agent asks its map road questions inside its own constructor. The
        # controller's own signature is unchanged: it does not take a backend,
        # it asks FIXS -- see currentBackend.
        global _backend, _core
        _backend, _core = backend, core
        if self._isClass:
            self._instance = self._obj(config, egoId)
        elif self._setup is not None:
            self._state = self._setup(config, egoId)

    def control(self, ego, dt):
        if self._isClass:
            return self._instance.control(ego, dt)
        if self._setup is not None:
            return self._obj(ego, dt, self._state)
        return self._obj(ego, dt)

    def shutdown(self):
        if self._isClass:
            fn = getattr(self._instance, 'shutdown', None)
            if fn is not None:
                fn()
        elif self._shutdown is not None:
            self._shutdown(self._state)

    def __repr__(self):
        return f'<LoadedController {self.spec}>'


def _importFromPath(path):
    name = os.path.splitext(os.path.basename(path))[0]
    spec = importlib.util.spec_from_file_location(name, path)
    if spec is None or spec.loader is None:
        raise ControllerError(f'EgoController: cannot import {path}')
    module = importlib.util.module_from_spec(spec)
    # Registered before exec so the module's own relative imports resolve, and
    # so a controller that imports itself indirectly does not load twice.
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


_backend = None
_core = None


def currentCore():
    """The VirEnvCore this bridge is running, for a controller that must map a
    wire id to the CARLA actor mirroring it. None outside a run."""
    return _core


def currentBackend():
    """The backend this bridge is running, for a controller that must reach
    past the record.

    A CARLA-shaped agent asks its map road questions in its own constructor, so
    something has to answer them. FIXS holds the backend client, so the answer
    is FORWARDED to the real map rather than reconstructed -- reconstructing it
    is what produced a road network invented from the ego's own route
    (ORNL-Real-Sim/FIXS#305).

    None when a controller is driven without one, which is how the tests run.
    """
    return _backend


def _letControllerImportFixs():
    """Make `import fixs` inside a controller reach THIS process's fixs.

    A controller is a loose .py, not an installed package, so it has no sys.path
    of its own -- and asking every one to reconstruct FIXS's layout before its
    first import is the boilerplate this hook exists to remove.

    The sys.modules aliases are the part that matters. The engine has already
    imported this package as `CommonLib.fixs`, and its records live in module
    globals; a bare `import fixs` off sys.path would find the same FILE and
    execute it AGAIN, giving the controller a second module object whose feed is
    never advanced. Every read then returns nothing -- silently, since a
    controller cannot tell an empty tick from an unconnected one. Aliasing makes
    the two names one module, which is what a caller already assumes.
    """
    d = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))   # CommonLib
    if d not in sys.path:
        sys.path.append(d)
    import CommonLib.fixs
    import CommonLib.fixs.carla
    sys.modules['fixs'] = CommonLib.fixs
    sys.modules['fixs.carla'] = CommonLib.fixs.carla


def loadController(spec, appRoot=None):
    """(string) -> LoadedController -- resolve what the scenario named.

    ``spec`` is ``path/to/file.py``, ``path/to/file.py:attribute``, or
    ``package.module:attribute``. The path form takes no sys.path arrangement,
    which is the point: applications are not installed packages.

    Nothing is discovered. The scenario names the controller the way it names
    the map, and if the name is wrong you find out here rather than 400 ticks in.
    """
    if not spec or not spec.strip():
        raise ControllerError('EgoController is empty')
    spec = spec.strip()
    _letControllerImportFixs()

    modPart, sep, attr = spec.rpartition(':')
    # A Windows drive letter is not a separator: 'C:/x/y.py' has no attribute.
    if not sep or len(modPart) <= 1:
        modPart, attr = spec, None

    isPath = modPart.endswith('.py') or '/' in modPart or os.sep in modPart
    if isPath:
        path = modPart if os.path.isabs(modPart) else os.path.join(appRoot or '.', modPart)
        path = os.path.normpath(path)
        if not os.path.isfile(path):
            raise ControllerError(
                f'EgoController: no such file: {path}\n'
                f'  (from {spec!r}; relative paths resolve against {appRoot or os.getcwd()!r})')
        module = _importFromPath(path)
        where = path
    else:
        try:
            import importlib
            module = importlib.import_module(modPart)
        except ImportError as exc:
            raise ControllerError(f'EgoController: cannot import {modPart!r}: {exc}')
        where = modPart

    if attr:
        obj = getattr(module, attr, None)
        if obj is None:
            raise ControllerError(f'EgoController: {where} has no {attr!r}')
        found = attr
    else:
        found = next((n for n in ENTRY_POINTS if hasattr(module, n)), None)
        if found is None:
            raise ControllerError(
                f'EgoController: {where} defines none of {", ".join(ENTRY_POINTS)}.\n'
                f'  Define control(ego, dt), or a class Controller with '
                f'__init__(config, egoId) and control(ego, dt).')
        obj = getattr(module, found)

    isClass = isinstance(obj, type)
    if isClass:
        if not callable(getattr(obj, 'control', None)):
            raise ControllerError(
                f'EgoController: {where}:{found} is a class with no control(ego, dt).')
        return LoadedController(spec, obj, isClass=True)

    if not callable(obj):
        raise ControllerError(
            f'EgoController: {where}:{found} is {type(obj).__name__}, not callable.')

    setupFn = getattr(module, 'setup', None)
    shutdownFn = getattr(module, 'shutdown', None)
    if setupFn is not None and not callable(setupFn):
        raise ControllerError(f'EgoController: {where}:setup is not callable')
    return LoadedController(spec, obj, setupFn, shutdownFn)


# ---------------------------------------------------------------------------
# running one
# ---------------------------------------------------------------------------

#: Seconds since the last FIXS feed. Module state because the host calls
#: runController as a free function, and the age belongs to the connection, not
#: to any one controller.
_feedAge = [0.0]


def resetFeedAge():
    """Call when a new feed arrives, before the sub-steps that follow it."""
    _feedAge[0] = 0.0


def runController(backend, controller, ego, dt, onFeed, maxSteerRad):
    """One step: hand the controller state, apply whatever shape it commanded.

    Backend-agnostic on purpose -- it touches only ``readEgoState``,
    ``applyEgoActuation`` and ``applyEgoSpeedSteer``, all IVirEnvBackend verbs.
    That is what lets tests/VirEnv drive a real controller against
    MockVirEnvBackend with no simulator, dispatch included, and what would let a
    non-CARLA host reuse this unchanged.

    ``maxSteerRad`` is passed rather than imported: it is mirrored in the C++
    bridge (mainVirCarla.cpp:131) and belongs to the host that owns the wire
    scaling, not to this module.

    :param ego: the ego's fixs.Vehicle for this feed. Its pose fields are
        refreshed here every step; the fields the traffic simulator owns last
        changed at the feed, and ``ego.feedAge`` says how long ago that was.
    :returns: the command shape applied -- 'actuation', 'speedsteer', or None.
    """
    from CommonLib import fixs
    from CommonLib.VirEnv.IVirEnvBackend import EgoState

    if ego is None:
        return None
    es = EgoState()
    if not backend.readEgoState(ego.id.strip(), es):
        return None

    if onFeed:
        resetFeedAge()
    else:
        _feedAge[0] += dt

    # EgoState is flat and already in the canonical FIXS wire frame -- the
    # backend removed its own anchor before returning, so nothing is converted
    # here (IVirEnvBackend.EgoState).
    object.__setattr__(ego, 'positionX', es.x)
    object.__setattr__(ego, 'positionY', es.y)
    object.__setattr__(ego, 'positionZ', es.z)
    object.__setattr__(ego, 'heading', es.heading)
    object.__setattr__(ego, 'speed', es.speed)
    object.__setattr__(ego, 'feedAge', _feedAge[0])

    # Clear what the LAST step wrote before asking for this one. The record
    # survives every sub-step of a feed, so without this _written only ever
    # grows: a controller that wrote pedals once would still look like it was
    # commanding them ten steps later, and "commanded nothing this step" -- a
    # real and useful answer -- could never be observed again.
    object.__setattr__(ego, '_written', frozenset())

    controller.control(ego, dt)

    kind = fixs.commandKind(ego)
    if kind == 'actuation':
        backend.applyEgoActuation(ego.acceleratorPedalDesired,
                                  ego.brakePedalDesired,
                                  ego.steerAngleDesired / maxSteerRad)
    elif kind == 'speedsteer':
        backend.applyEgoSpeedSteer(ego.speedDesired,
                                   ego.steerAngleDesired / maxSteerRad)
    # kind is None: the controller commanded nothing this step. The last command
    # persists in the plant, which is the honest reading -- substituting a zero
    # would brake a car whose controller simply had nothing new to say.
    return kind
