"""IVirEnvBackend -- the verb interface a virtual-environment backend implements.

Python peer of ``CommonLib/IVirEnvBackend.h``. Same type names, same verb names,
same argument order and units, so a reader moving between the two implementations
never has to translate. Read that header for the full design rationale; what is
restated here is only what a Python caller needs at the call site.

Design rules carried over verbatim from #174:

* **This module is SDK-FREE**: no ``carla``, no CarMaker binding. The core that
  consumes it therefore imports on a machine with neither installed, which is
  what lets ``tests/VirEnv/replay_core.py`` run with no simulator at all.
* Poses cross this boundary in the **canonical FIXS wire frame** (front-of-vehicle
  reference point, z at ground; heading in navigational degrees, north = 0,
  clockwise). The BACKEND applies its own pose anchor and convention conversion --
  the anchor never lives in the core. Interpolation stays in the core, BEFORE this
  boundary, so verb-boundary comparisons diff canonical decisions rather than
  backend coordinate output.
* Handles are opaque ints: a CarMaker pre-placed slot index, or a Carla actor id.
  ``>= 0`` is valid; :meth:`IVirEnvBackend.spawnVehicle` returns :data:`kNoHandle`
  when the backend cannot place the vehicle.
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import IntEnum

__all__ = ['VehClass', 'VehHandle', 'kNoHandle', 'Pose', 'EgoState', 'IVirEnvBackend']


class VehClass(IntEnum):
    """Vehicle classes the bridge distinguishes.

    CarMaker keeps a separate pre-placed slot pool per class; Carla maps them to
    blueprint categories. Values match the C++ ``enum class VehClass`` order, which
    ``MockVirEnvBackend`` encodes into its handles -- so they are visible in a
    transcript and must not be reordered.
    """
    Car = 0
    Truck = 1
    Bus = 2


#: Opaque backend handle for a spawned/mapped vehicle. CarMaker: traffic slot id.
#: Carla: actor id. Returned by :meth:`IVirEnvBackend.spawnVehicle`, passed back to
#: the pose/lights/despawn verbs. An alias for ``int``, as in the C++ ``using``.
VehHandle = int

#: Backend full / nothing to spawn into. The core skips that vehicle this step.
kNoHandle = -1


@dataclass
class Pose:
    """A pose in the RAW FIXS wire frame (pre-anchor, pre-convention).

    ``x``/``y``/``z`` is the vehicle's FRONT reference point at GROUND height;
    ``headingDeg`` and ``gradeRad`` are the FIXS wire conventions verbatim. The
    BACKEND does all of: convention conversion (heading -> CarMaker yaw / Carla
    rotation) AND its pose anchor. Keeping the raw values here -- not a
    pre-converted yaw -- is what lets each backend reproduce its original math
    bit-for-bit, since the core never round-trips the angle.

    :param headingDeg: FIXS wire heading, north = 0, clockwise (degrees).
    :param gradeRad: FIXS wire grade, positive = climbing (radians).
    """
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0
    headingDeg: float = 0.0
    gradeRad: float = 0.0


@dataclass
class EgoState:
    """Ego state read back from a backend that owns ego dynamics (mode A).

    Returned in the CANONICAL FIXS frame so the core can pack it straight onto the
    wire; the backend removes its own anchor before returning.

    :param speed: m/s.
    :param heading: degrees, north = 0, clockwise.
    :param grade: radians, positive = climbing (0 if the backend cannot supply it).
    """
    speed: float = 0.0
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0
    heading: float = 0.0
    grade: float = 0.0
    brake: bool = False
    indL: bool = False
    indR: bool = False


class IVirEnvBackend(ABC):
    """Host-supplied verbs. A backend implements these against its SDK.

    The core never sees a CarMaker or Carla symbol. All methods are called from
    the core's :meth:`~CommonLib.VirEnv.VirEnvCore.VirEnvCore.processStep` in the
    order of the seven-step skeleton.

    Only the verbs the C++ header declares pure-virtual are ``@abstractmethod``
    here; the rest keep their C++ default (a no-op or an empty string), so a
    minimal backend -- a signal-free scenario, a Carla backend with no spare pool --
    implements exactly what the C++ one has to.
    """

    # --- logging -----------------------------------------------------------
    @abstractmethod
    def log(self, msg):
        """(string) -> None -- informational message, routed to the host's log."""

    @abstractmethod
    def logError(self, msg):
        """(string) -> None -- error message, routed to the host's error log."""

    def loadSignalTable(self, path):
        """(string) -> None -- load whatever the backend needs to dispatch signals.

        CarMaker reads ``RSsignalTable.csv`` (controller -> head id + TrfLight
        index); Carla matches its ``traffic.traffic_light`` actors to the junction
        map. Default no-op -- a signal-free scenario never calls it. Called once in
        :meth:`~CommonLib.VirEnv.VirEnvCore.VirEnvCore.initialization`.
        """

    # --- traffic pool lifecycle -------------------------------------------
    @abstractmethod
    def initTrafficPool(self):
        """() -> None -- prepare the pool; called once at the first step.

        CarMaker enumerates its pre-placed RS_C/RS_T slots and parks them off-road;
        Carla may no-op and spawn lazily. After this, :meth:`spawnVehicle` must be
        serviceable.
        """

    @abstractmethod
    def spawnVehicle(self, vType, vClass, spawnPose):
        """(string, string, Pose) -> VehHandle -- acquire a handle for a new vehicle.

        :param vType: the FIXS vehicle type.
        :param vClass: the SUMO/VISSIM class string. The backend does its OWN
            classification (CarMaker: ``vClass`` -> car/truck slot pool; Carla:
            ``vType`` or ``vClass`` -> blueprint).
        :param spawnPose: the raw FIXS pose to place the vehicle at. Carla's
            ``try_spawn_actor`` needs a transform; CarMaker's pre-placed slots
            ignore it.
        :returns: :data:`kNoHandle` if the vehicle cannot be placed (e.g. the
            CarMaker pool is exhausted); the core then skips it this step.
        """

    @abstractmethod
    def despawnVehicle(self, h):
        """(VehHandle) -> None -- release a handle whose vehicle left the sim.

        CarMaker parks the slot at z = -5000 and returns it to the pool; Carla
        destroys the actor.
        """

    # --- per-step actuation (raw FIXS pose in; backend converts + anchors) -
    @abstractmethod
    def setVehiclePose(self, h, p):
        """(VehHandle, Pose) -> None -- place a mapped vehicle."""

    @abstractmethod
    def setVehicleLights(self, h, brake, indL, indR):
        """(VehHandle, bool, bool, bool) -> None -- set brake / indicator lights."""

    def parkSpares(self):
        """() -> None -- re-park UNMAPPED spare slots; once per refresh slot.

        Called before the mapped-vehicle pose loop. CarMaker must re-park its spare
        RS_C/RS_T slots at z = -5000 EVERY refresh (FreeMotion slots drift at low
        UpdRate -- #168); Carla has no spare pool, so this is a no-op there. Lets
        the core stay ignorant of the backend's spare set.
        """

    def syncTrafficLight(self, junctionId, stateStr):
        """(string, string) -> None -- dispatch one junction's signal state.

        :param junctionId: the FIXS/SUMO TLS id.
        :param stateStr: its per-link SUMO state string (``rGyu...``).

        The backend owns the per-link mapping and the char -> native-state
        conversion. NOTE: this stays a SUMO state string until #156 defines a
        FIXS-canonical signal type. Default no-op.
        """

    # --- ego coupling ------------------------------------------------------
    @abstractmethod
    def readEgoState(self, egoId, out):
        """(string, EgoState) -> bool -- mode A "backend owns ego": read ego back.

        Fills ``out`` IN PLACE and returns True, or returns False if the ego is
        unavailable this step. The out-parameter shape is the C++ signature kept
        deliberately: the core calls it identically in both languages, so the
        seven-step body reads the same.

        "Unavailable" includes "exists but has no pose yet". A backend that has
        just created the ego may not be able to say where it is until its
        simulator advances, and returning a placeholder pose there is how a
        controller ends up commanding from somewhere the ego has never been.
        """

    def noteWorldTicked(self):
        """The simulator advanced a step, so any actor created since the last
        one now has a state to read.

        Optional: the default does nothing, for backends whose readback never
        lags creation. CARLA's does -- see CarlaBackend.readEgoState.
        """

    def setEgoPose(self, egoId, p):
        """(string, Pose) -> None -- mode B "external owns ego": teleport ego in.

        The ego is driven elsewhere (Simulink/dSPACE) and the backend provides
        render + sensors only. Default no-op, so mode-A-only backends need not
        implement it (CarMaker mode B is unwired today).
        """

    def applyEgoControl(self, egoId, desiredSpeed):
        """(string, float) -> None -- optional L2 hook: advise a desired speed.

        e.g. Carla TM ``set_desired_speed``. Default no-op.
        """

    # --- the two command shapes -------------------------------------------
    # A vehicle plant offers two interfaces, and which one a controller is using
    # is read off the fields it wrote (fixs.commandKind). They differ in WHO
    # CLOSES THE LOOP, which is a real architectural difference and not a
    # convenience: with pedals the caller is the controller and its rate is the
    # rate the loop closes at; with speed the plant closes it internally, at the
    # plant's own rate, using the plant's own gains.
    #
    # Both default to no-ops so a backend can support one, the other, or neither.

    def applyEgoActuation(self, throttle, brake, steerNorm):
        """(float, float, float) -> None -- pedals + steer; the CALLER closes the loop.

        throttle/brake in [0, 1], steerNorm in [-1, 1]. Carla: ``apply_control``.
        All three arrive together (fixs._validateCommand enforces it) because a
        partial write would leave the omitted ones at whatever arrived last.
        """

    def applyEgoSpeedSteer(self, speed, steerNorm, accel=None, jerk=None):
        """(float, float, float, float) -> None -- speed + steer; the PLANT closes it.

        speed in m/s, steerNorm in [-1, 1]; accel and jerk are optional limits.
        Carla: ``apply_ackermann_control``, whose controller then tracks the speed
        at the CARLA step rate rather than the rate commands arrive at -- which is
        why this shape gives a fast inner loop without an in-process controller.

        The cost is that the tracking gains become the plant's
        (``AckermannControllerSettings`` in Carla), not the controller's. A
        backend whose plant has no such loop should leave this a no-op rather
        than fake one.
        """

    # --- unified debug diagnostics -----------------------------------------
    def debugHeader(self):
        """() -> string -- extra CSV column names, comma-led, written once.

        When the core runs with debug rows enabled it writes ONE canonical
        per-vehicle CSV (simTime, id, the FIXS-canonical pose, the pose handed to
        the backend). A backend may append its OWN columns to that SAME row so
        every simulator shares one log instead of N divergent CSVs --
        e.g. ``,carla_x,carla_y,carla_yaw``. Default empty, so the core log has
        identical columns regardless of backend.
        """
        return ''

    def debugFields(self, h):
        """(VehHandle) -> string -- the values matching :meth:`debugHeader`."""
        return ''
