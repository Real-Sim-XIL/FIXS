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
    EgoSetup:
      ActuationSource: user
      Controller:      apps/<app>/my_controller.py


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

    ActuationSource: user, no Controller   own process, over FIXS -> feed rate
    ActuationSource: user +  Controller    in the bridge          -> CarlaTimeStep

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
                         feedSpeed         (the feed's speed, which the eco advisory was planned from)
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

    def __init__(self, spec, obj, setupFn=None, shutdownFn=None, isClass=False,
                 argv=()):
        self.spec = spec
        #: What the scenario wrote after the path. FIXS does not read it.
        self.argv = list(argv)
        self._obj = obj
        self._setup = setupFn
        self._shutdown = shutdownFn
        self._isClass = isClass
        self._state = None
        self._instance = None

    def setup(self, config, egoId, backend=None, core=None, dynamics=None):
        # Registered before the controller is built, because a CARLA-shaped
        # agent asks its map road questions inside its own constructor. The
        # controller's own signature is unchanged: it does not take a backend,
        # it asks FIXS -- see currentBackend.
        global _backend, _core, _config, _dynamics
        # The scenario's own words for its controller, verbatim. FIXS resolves
        # the path and carries the rest; what the options MEAN is the
        # controller's business, so no option of its ever reaches this schema.
        config['EgoControllerArgs'] = self.argv
        _backend, _core, _config = backend, core, config
        _dynamics = dynamics
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


def _driverMark():
    """Where fixs.driver()'s registry stands, or None if there is no driver.

    Swallows everything: a FIXS build without the driver, or one whose import
    failed for its own reasons, must still load a hand-written controller.
    """
    try:
        from CommonLib.fixs import _driver
        return _driver.mark()
    except Exception:
        return None


def _driverBuilt(mark, where):
    """What fixs.driver() built while `where` was importing."""
    if mark is None:
        return None
    from CommonLib.fixs import _driver
    made = _driver.builtSince(mark)
    if len(made) > 1:
        raise ControllerError(
            f'EgoController: {where} calls fixs.driver() {len(made)} times. '
            f'FIXS drives the ego with one thing -- name the one you mean '
            f'(EgoController: {where}:TheOneIMeant).')
    return made[0] if made else None


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
_config = None
_dynamics = None


def currentConfig():
    """The scenario this bridge is running, for the parts of FIXS that answer a
    controller from it -- the ego's route, above all. None outside a run."""
    return _config


def currentCore():
    """The VirEnvCore this bridge is running, for a controller that must map a
    wire id to the CARLA actor mirroring it. None outside a run."""
    return _core


def currentDynamics():
    """EgoSetup.Dynamics for this run: 'virenv', 'traffic', or None outside one.

    A controller asks this rather than whether a CARLA ego exists, because the
    two are not the same question. On a virenv rung with a deferred spawn there
    is no ego actor either, for the first few hundred ticks -- 'not yet' and
    'never' would be indistinguishable. Dynamics is the declared, permanent
    answer, so a driver can decide once what kind of run it is in.
    """
    return _dynamics


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
    ``package.module:attribute``, each optionally followed by the controller's
    own options::

        apps/mlk_eco_driving/ego_agent_controller.py --command-shape pedals

    Split on the first ``' --'``, never on whitespace: a path may contain
    spaces, and one that did used to resolve. The tail arrives as
    ``config['EgoControllerArgs']`` and is not interpreted here -- a controller
    option must never need a key in this schema.

    The path form takes no sys.path arrangement, which is the point:
    applications are not installed packages.

    Nothing is discovered. The scenario names the controller the way it names
    the map, and if the name is wrong you find out here rather than 400 ticks in.
    """
    if not spec or not spec.strip():
        raise ControllerError('EgoController is empty')
    spec = spec.strip()
    spec, sep, rest = spec.partition(' --')
    argv = ('--' + rest).split() if sep else []
    spec = spec.strip()
    _letControllerImportFixs()
    driverMark = _driverMark()      # before the module runs

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
        # A driver built by fixs.driver() says so itself, so the name the
        # user gave it -- or did not give it -- is theirs. Asked before the
        # name scan, which stays for controllers written from scratch.
        obj = _driverBuilt(driverMark, where)
        found = ('fixs.driver()' if obj is not None else
                 next((n for n in ENTRY_POINTS if hasattr(module, n)), None))
        if found is not None and obj is None:
            obj = getattr(module, found)
        if found is None:
            raise ControllerError(
                f'EgoController: {where} defines none of {", ".join(ENTRY_POINTS)}.\n'
                f'  Define control(ego, dt), or a class Controller with '
                f'__init__(config, egoId) and control(ego, dt).')

    isClass = isinstance(obj, type)
    if isClass:
        if not callable(getattr(obj, 'control', None)):
            raise ControllerError(
                f'EgoController: {where}:{found} is a class with no control(ego, dt).')
        return LoadedController(spec, obj, isClass=True, argv=argv)

    if not callable(obj):
        raise ControllerError(
            f'EgoController: {where}:{found} is {type(obj).__name__}, not callable.')

    setupFn = getattr(module, 'setup', None)
    shutdownFn = getattr(module, 'shutdown', None)
    if setupFn is not None and not callable(setupFn):
        raise ControllerError(f'EgoController: {where}:setup is not callable')
    return LoadedController(spec, obj, setupFn, shutdownFn, argv=argv)


# ---------------------------------------------------------------------------
# running one
# ---------------------------------------------------------------------------

#: Seconds since the last FIXS feed. Module state because the host calls
#: runController as a free function, and the age belongs to the connection, not
#: to any one controller.
_feedAge = [0.0]

#: What the feed brought in each DUAL-USE field, kept across the sub-steps that
#: follow it. Beside _feedAge for the same reason, and cleared with it.
_heldInputs = {}

#: The ego speed the last feed carried, before the live state overwrites it.
_feedSpeed = [None]

#: Fields the traffic simulator OWNS and a controller also WRITES.
#:
#: The record is one object used in both directions, so a controller's command
#: lands in the same slot the feed's value arrived in. With CarlaTimeStep 0.05
#: against a 0.1 s feed that value is read back half a step later as though it
#: were still input -- which contradicts what this module's own docstring
#: promises about speedDesired, and closes a speed controller's loop onto
#: itself on every second step.
#:
#: speedDesired is the only one: acceleratorPedalDesired, brakePedalDesired and
#: steerAngleDesired are commands the traffic simulator never fills in.
_DUAL_USE = ('speedDesired',)


#: The ego record this step's controller call is holding. Published so
#: ``fixs.carla.apply_control`` can write a CARLA-shaped command onto the same
#: record ``ego.set`` writes to, without the controller having to pass it.
_egoRecord = [None]


def currentEgoRecord():
    """The ego's fixs.Vehicle for the call in progress, or None outside one."""
    return _egoRecord[0]


def resetFeedAge():
    """Call when a new feed arrives, before the sub-steps that follow it."""
    _feedAge[0] = 0.0
    _heldInputs.clear()


def runController(backend, controller, ego, dt, onFeed, maxSteerRad):
    """One step: hand the controller state, apply whatever shape it commanded.

    ``backend`` may be None. That is the case where the TRAFFIC SIMULATOR owns
    the ego (EgoSetup.Dynamics: traffic) and the controller is a cell in the
    speed loop rather than the driver of a physics actor: there is no ego actor
    to read a state from or to apply a command to. The record IS the state --
    it already carries the traffic simulator's pose and speed -- and the command
    written onto it is forwarded to TrafficLayer by the caller instead of being
    applied here. Everything between those two ends is identical, which is what
    lets one controller file serve both.

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
        Those fields are RESTORED before each call, because the record is also
        where the controller writes -- see _DUAL_USE.
    :returns: the command shape applied -- 'actuation', 'speedsteer', or None.
    """
    from CommonLib import fixs
    from CommonLib.VirEnv.IVirEnvBackend import EgoState

    if ego is None:
        return None
    # No backend -> no physics ego: the record already holds the traffic
    # simulator's view of it, which is the only state there is on that rung.
    es = None
    if backend is not None:
        es = EgoState()
        if not backend.readEgoState(ego.id.strip(), es):
            return None

    if onFeed:
        resetFeedAge()
        for name in _DUAL_USE:
            _heldInputs[name] = getattr(ego, name, None)
        _feedSpeed[0] = getattr(ego, 'speed', None)
    else:
        _feedAge[0] += dt
        # Put the feed's value back before asking the controller for a new
        # command. Without this the controller reads its own last command out of
        # a field the docstring above promises holds the traffic simulator's --
        # and a controller that closes a speed loop on it is closing it on
        # itself. Restored BEFORE control(), so the command it writes is still
        # the one applied below.
        for name, held in _heldInputs.items():
            if held is not None:
                object.__setattr__(ego, name, held)

    # EgoState is flat and already in the canonical FIXS wire frame -- the
    # backend removed its own anchor before returning, so nothing is converted
    # here (IVirEnvBackend.EgoState).
    if es is not None:
        object.__setattr__(ego, 'positionX', es.x)
        object.__setattr__(ego, 'positionY', es.y)
        object.__setattr__(ego, 'positionZ', es.z)
        object.__setattr__(ego, 'heading', es.heading)
        object.__setattr__(ego, 'speed', es.speed)
    object.__setattr__(ego, 'feedAge', _feedAge[0])
    object.__setattr__(ego, 'feedSpeed', _feedSpeed[0])

    # Clear what the LAST step wrote before asking for this one. The record
    # survives every sub-step of a feed, so without this _written only ever
    # grows: a controller that wrote pedals once would still look like it was
    # commanding them ten steps later, and "commanded nothing this step" -- a
    # real and useful answer -- could never be observed again.
    object.__setattr__(ego, '_written', frozenset())

    _egoRecord[0] = ego
    try:
        controller.control(ego, dt)
    finally:
        _egoRecord[0] = None

    kind = fixs.commandKind(ego)
    if backend is None:
        # Nothing to apply here: the caller forwards ego.speedDesired to
        # TrafficLayer and the traffic simulator integrates it. Pedals cannot be
        # forwarded -- there is no plant on this rung to turn one into a speed --
        # so refuse rather than drop them, which would leave the LOWER-port
        # controller's command as the last write and hand the run quietly back
        # to it while this one looked like it was driving.
        if kind == 'actuation':
            raise ControllerError(
                "the controller commanded pedals, but EgoSetup.Dynamics is "
                "'traffic': the traffic simulator integrates the ego and there "
                "is no plant to turn a pedal into a speed. Command a speed "
                "instead -- ego.set(speedDesired=...), or --command-shape speed "
                "if this is fixs.driver.")
        return kind
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
