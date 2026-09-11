"""FIXS client API -- one import, no hand-written socket code (#316).

A Python application talks to TrafficLayer like this::

    from CommonLib import fixs          # or `import fixs` once it is on the path

    fixs.connect('config.yaml')
    try:
        while True:
            fixs.recv()                           # data arrives here

            ego = fixs.vehicle.get('ego')
            if ego is not None:
                ego.set(speedDesired=plan(ego.speed))

            fixs.send()                           # goes on the wire here
    except fixs.Shutdown:
        pass

Everything the co-simulation protocol requires -- connecting and retrying,
framing, answering exactly once per tick, answering even when there is nothing
to say, honouring the shutdown signal, clearing state between ticks -- is
handled here. None of it is the application's to remember.

This module is a facade over the existing helpers, which are unchanged:
ConfigHelper reads the yaml, MsgHelper packs and unpacks, SocketHelper frames.
Scripts that use those directly keep working exactly as before.


recv() is the only thing that touches the network on the read side
------------------------------------------------------------------
``recv()`` reads one message, decodes it, and rebinds the views. Everything
after that is local: ``get()`` / ``getAll()`` / ``getIDList()`` are dictionary
lookups, and ``set()`` writes into a record held in memory. Nothing leaves the
process until ``send()``.

That is the difference from TraCI, where ``getSpeed(id)`` is a request that
costs a round trip. It is also why there are no per-field getters here: they
would imply you can ask for anything at any time, when in fact the answer set
was fixed by the config before the loop started -- the vehicles the
subscription selected, carrying the fields ``VehicleMessageField`` declared.

The closest TraCI analogue is its SUBSCRIPTIONS -- ``subscribe()`` plus
``getAllSubscriptionResults()`` -- not its getters. There is deliberately no
``simulationStep`` either: TraCI's advances the simulator's clock, and nothing
here does, because TrafficLayer owns the clock.

Three attributes are rebound on every ``recv()``:

    fixs.sim            this moment's scalars -- fixs.sim.time, fixs.sim.state
    fixs.vehicle        every vehicle in this moment's feed
    fixs.trafficlight   this moment's signal states

``fixs.sim`` is a record for the same reason vehicles are: the header's values
arrive together, so they come back together. Growth is a field on it rather than
another module-level function, which is why there is no ``getTime()``.

On a namespace, **attributes are declared and constant; methods query this
moment**. ``fixs.vehicle.fields`` is fixed for the connection;
``fixs.vehicle.getIDList()`` changes on every ``recv()``. That is also why the
declared field list lives on the namespace it describes rather than in a
connection-wide bag: MsgHelper already anticipates per-type field lists
(set_traffic_light_message_field, set_detector_message_field -- both currently
dead, with no config key driving them), so ``fixs.trafficlight.fields`` can
appear later without anything moving.

Holding a record across ticks is safe: every tick decodes fresh objects, so
``previous = fixs.vehicle.getAll()`` keeps that tick's values rather than
aliasing something about to be overwritten.
"""

import atexit
import dataclasses
import math
import os
import socket
import sys
import time as _time
import typing

from CommonLib.ConfigHelper import ConfigHelper
from CommonLib.MsgHelper import MsgHelper
from CommonLib.SocketHelper import SocketHelper
from CommonLib.VehDataMsgDefs import VehData

__all__ = [
    'connect', 'recv', 'send', 'close',
    'sim', 'vehicle', 'trafficlight',
    'emit', 'transport', 'commandKind',
    'Vehicle', 'EgoVehicle', 'MAX_STEER_RAD',
    'Shutdown', 'FixsError', 'NotConnected', 'ProtocolError',
]


#: Full-lock front road-wheel angle [rad]. `steerAngleDesired` is an ANGLE on
#: the wire, where a CARLA agent's VehicleControl.steer is normalised [-1, 1];
#: the plant divides by this same constant, so multiplying by it here makes the
#: round trip the agent's own number. Must match mainVirCarla's kMaxSteerRad.
MAX_STEER_RAD = 0.7


def _addCarlaAgentsToPath():
    """Make the vendored CARLA agents importable off `import fixs`.

    A controller that brings a CARLA-shaped agent writes

        import fixs
        from agents.navigation.behavior_agent import BehaviorAgent

    so agents/ has to be on the path by the time the second line runs. Appended,
    not inserted: an installed CARLA PythonAPI, or the user's own copy, wins.
    """
    # __file__ is CommonLib/fixs/__init__.py, so the FIXS root is three up.
    root = os.path.join(
        os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
        'Carla', 'carla_agents')
    if os.path.isdir(os.path.join(root, 'agents')) and root not in sys.path:
        sys.path.append(root)


_addCarlaAgentsToPath()

#: The roles a connection can be opened in. A CONTROLLER decides -- it may write
#: only the command fields, and may return only records that arrived, which is
#: what stops a controller fabricating a record and teleporting a vehicle it
#: never saw (ORNL-Real-Sim/FIXS_Applications#25). A VIRENV bridge MEASURES: its
#: whole job is to report the pose and speed its backend produced, for ids the
#: traffic simulator may not have yet. Those are opposite contracts, so the role
#: is named at connect() rather than inferred -- no client acquires bridge powers
#: by accident, and the controller contract does not weaken by one field.
_ROLES = ('controller', 'virenv')


class Shutdown(Exception):
    """Raised by recv() when TrafficLayer signals the end of the run.

    Not a FixsError: the run ending is not a failure, and a client with a broad
    ``except fixs.FixsError`` around its loop should not swallow it. This is the
    same shape as StopIteration -- an expected end of sequence, reported as an
    exception because the only sane response is to unwind.
    """


class FixsError(Exception):
    """Base class for every error this module raises."""


class NotConnected(FixsError):
    """Raised when the API is used before connect() or after close()."""


class ProtocolError(FixsError):
    """Raised when recv/send are called out of order, or a command is incomplete.

    Two situations:

    * recv/send out of order. TrafficLayer does not advance until every
      subscriber has answered, so a tick received and never answered stalls the
      whole co-simulation; rather than let that surface as an unexplained hang
      on the next recv(), it is reported here.
    * reading tick data when no tick is held -- before the first recv(), after
      shutdown, or after close(). Answering "no vehicles" there would read as a
      quiet tick rather than as no tick at all.
    """


# ---------------------------------------------------------------------------
# Records
# ---------------------------------------------------------------------------

_WIRE_FIELDS = frozenset(f.name for f in dataclasses.fields(VehData))


class Vehicle(VehData):
    """One vehicle, for one tick.

    Read any field directly (``veh.speed``). Write only through :meth:`set`,
    which takes wire field names as keywords::

        veh.set(speedDesired=9.0)
        veh.set(steerAngleDesired=s, acceleratorPedalDesired=a, brakePedalDesired=b)

    Assignment raises. Two failure modes close because of it:

    * **A local calculation cannot become a command.** Converting units in place
      (``veh.speed *= 2.23694`` -- the eco controller does exactly this
      conversion on its dataframe) would otherwise be indistinguishable from an
      instruction to SUMO.

    * **A reply record cannot be fabricated.** A fresh ``VehData`` leaves every
      unset field at its dataclass default -- position (0,0,0), heading 0 -- and
      TrafficLayer publishes a client's return to later clients by REPLACING the
      whole record for that id, not by merging fields. That is how a controller
      on a low port put the ego at the world origin in the CARLA bridge's copy
      (ORNL-Real-Sim/FIXS_Applications#25). Here the only record that can be
      returned is the one that arrived.

    Calling ``set()`` also marks this record for the reply, so ``send()`` with no
    argument returns exactly what was commanded.

    This subclasses ``VehData`` rather than redefining it, so
    ``isinstance(rec, VehData)`` holds and a field added to ``VehDataMsgDefs.py``
    is available here with no change to this module.
    """

    #: Longitudinal control. TrafficLayer consumes one of these, never both:
    #: ConfigHelper.cpp:256 requires exactly one in VehicleMessageField, and
    #: TrafficHelper.cpp:801 reports that SUMO does not implement the
    #: acceleration form.
    LONGITUDINAL_FIELDS = frozenset({'speedDesired', 'accelerationDesired'})

    #: The pedals. CarlaBackend::applyEgoActuation reads throttle, brake AND
    #: steer every tick (mainVirCarla.cpp:335), so a partial write ships
    #: whatever arrived for the rest -- hence they travel together.
    PEDAL_FIELDS = frozenset({'acceleratorPedalDesired', 'brakePedalDesired'})

    #: Steer is a physical angle in rad. It is deliberately NOT part of
    #: PEDAL_FIELDS: it belongs to both command shapes. Pedals + steer is the
    #: actuation command; speed + steer is the speed-and-steer command, which a
    #: plant closes the loop on itself (Carla: apply_ackermann_control). Folding
    #: steer into the pedal set is what used to make the second shape
    #: unrepresentable -- set(speedDesired=..., steerAngleDesired=...) was
    #: rejected for 'missing' pedals it was never going to send.
    STEER_FIELD = frozenset({'steerAngleDesired'})

    #: L4 actuation: pedals + steer, one command.
    ACTUATION_FIELDS = PEDAL_FIELDS | STEER_FIELD

    #: Everything a client may write.
    COMMAND_FIELDS = LONGITUDINAL_FIELDS | ACTUATION_FIELDS

    _written = frozenset()

    def set(self, **fields):
        """(**fields) -> None -- command this vehicle.

        Keywords are wire field names. Raises on an unknown name, and on a
        measured field, which is data from the traffic simulator rather than a
        command channel.
        """
        if not fields:
            raise TypeError(
                'set() needs at least one field, e.g. set(speedDesired=9.0)')
        for name in fields:
            if name in self.COMMAND_FIELDS:
                continue
            if name in _WIRE_FIELDS:
                raise AttributeError(
                    f"'{name}' is measured data from the traffic simulator and "
                    f"cannot be commanded. Writable: "
                    f"{', '.join(sorted(self.COMMAND_FIELDS))}."
                )
            raise AttributeError(
                f"'{name}' is not a vehicle field. Writable: "
                f"{', '.join(sorted(self.COMMAND_FIELDS))}."
            )
        if _declaredFields is not None:
            absent = [f for f in fields if f not in _declaredFields]
            if absent:
                # pack_veh_data only serialises fields in VehicleMessageField, so
                # this command would be written into the record, never leave the
                # process, and the vehicle would simply not respond -- with no
                # symptom anywhere in the stack.
                raise ProtocolError(
                    f"{', '.join(sorted(absent))} cannot be commanded: not in "
                    f"this config's SimulationSetup.VehicleMessageField, so it "
                    f"would not be put on the wire. On the wire: "
                    f"{', '.join(sorted(_declaredFields & self.COMMAND_FIELDS))}."
                )
        for name, value in fields.items():
            if isinstance(value, bool) or not isinstance(value, (int, float)):
                raise TypeError(
                    f"{name} must be a number, got {type(value).__name__}")
            if not math.isfinite(value):
                # A diverged controller otherwise ships NaN into SUMO, where it
                # surfaces as strange vehicle behaviour far from the cause.
                raise ValueError(f'{name} must be finite, got {value!r}')
        for name, value in fields.items():
            object.__setattr__(self, name, value)
        object.__setattr__(self, '_written', self._written | frozenset(fields))

    def __setattr__(self, name, value):
        if name.startswith('_'):
            object.__setattr__(self, name, value)
            return
        if name in self.COMMAND_FIELDS:
            raise AttributeError(
                f'records are read-only; use veh.set({name}=...) to command it')
        raise AttributeError(
            f"'{name}' is measured data from the traffic simulator and cannot "
            f"be assigned. Records are read-only."
        )


# ---------------------------------------------------------------------------
# Views -- this tick's records of one kind, addressed by id
# ---------------------------------------------------------------------------

class _View:
    """Records of one kind for the current tick.

    Every method here is a local lookup -- the data arrived with the last
    recv(). ``getAll()`` is the FIXS equivalent of TraCI's
    ``getAllSubscriptionResults()``.
    """

    _KIND = 'record'

    def __init__(self, records=(), key=lambda r: r.id.strip(), unavailable=None,
                 fields=None):
        self._byId = {key(r): r for r in records}
        #: Declared for the connection, so it outlives any one moment and is
        #: readable even when no tick is held.
        self._fields = fields
        # Set when there is no tick to report. Answering "empty" in that state
        # would read as a quiet tick rather than as no tick at all.
        self._unavailable = unavailable

    def _require(self):
        if self._unavailable is not None:
            raise ProtocolError(self._unavailable)

    def get(self, objectID):
        """(string) -> record | None

        ``None`` rather than an error: a subscribed vehicle that has not
        departed yet is a normal state, checked every tick.
        """
        self._require()
        return self._byId.get(objectID)

    def getAll(self):
        """() -> dict[string, record] -- this tick's records, keyed by id.

        Iterate it with ``.items()``; iterating a dict directly yields its keys.
        """
        self._require()
        return dict(self._byId)

    def getIDList(self):
        """() -> list[string]"""
        self._require()
        return list(self._byId)

    def __repr__(self):
        if self._unavailable is not None:
            return f'<no {self._KIND} data: {self._unavailable}>' 
        return (f'<{len(self._byId)} {self._KIND}(s): '
                f'{", ".join(sorted(self._byId)) or "none"}>')


class _Sim:
    """This moment's scalar status, as ``fixs.sim``.

    A record rather than a set of getters, for the same reason vehicle data is:
    the values arrive together in the message header, so exposing them one
    function at a time would make the function count track the status count.
    Anything added later is a field here.
    """

    __slots__ = ('_time', '_state', '_unavailable')

    def __init__(self, time=0.0, state=0, unavailable=None):
        self._time = time
        self._state = state
        self._unavailable = unavailable

    def _require(self):
        if self._unavailable is not None:
            raise ProtocolError(self._unavailable)

    @property
    def time(self):
        """double -- simulation time of this moment."""
        self._require()
        return self._time

    @property
    def state(self):
        """integer -- the simulation state word TrafficLayer sent."""
        self._require()
        return self._state

    def __repr__(self):
        if self._unavailable is not None:
            return f'<no sim data: {self._unavailable}>'
        return f'<sim t={self._time:.2f} state={self._state}>'


class _VehicleView(_View):
    _KIND = 'vehicle'

    def __init__(self, records=(), key=lambda r: r.id.strip(), unavailable=None,
                 fields=None, egoIDs=None):
        super().__init__(records, key, unavailable, fields)
        self._egoIDs = egoIDs

    @property
    def fields(self):
        """list[string] -- SimulationSetup.VehicleMessageField for this connection.

        Read-only. Both ends decode by the same list, so changing it mid-run
        would desync the stream on the next record rather than reconfigure
        anything; a runtime change needs a renegotiation the protocol does not
        have.
        """
        if self._fields is None:
            raise NotConnected('call fixs.connect(...) first')
        return list(self._fields)

    @property
    def egoIDs(self):
        """list[string] -- the ids this client declared.

        The subscription's ``attribute.id`` list, so it is configuration rather
        than data and does not change between ticks. Whether one is in the feed
        right now is ``fixs.vehicle.get(vehID) is not None``.
        """
        if self._egoIDs is None:
            raise NotConnected('call fixs.connect(...) first')
        return list(self._egoIDs)


class _TrafficLightView(_View):
    _KIND = 'traffic light'


# ---------------------------------------------------------------------------
# Module state
# ---------------------------------------------------------------------------

_NOT_CONNECTED = 'no tick data: not connected -- call fixs.connect() first'
_NO_TICK_YET = 'no tick data: nothing received yet -- call fixs.recv() first'
_SHUTDOWN = 'no tick data: TrafficLayer has shut down'

sim: _Sim = _Sim(unavailable=_NOT_CONNECTED)
vehicle: _VehicleView = _VehicleView(unavailable=_NOT_CONNECTED)
trafficlight: _TrafficLightView = _TrafficLightView(unavailable=_NOT_CONNECTED)

#: VehicleMessageField for this connection; None until connect().
_declaredFields: typing.Optional[frozenset] = None
#: Why tick data is unavailable, or None while a tick is held.
_noTick: typing.Optional[str] = _NOT_CONNECTED

_helper: typing.Optional[SocketHelper] = None
_sock: typing.Optional[socket.socket] = None
_egoIds: typing.List[str] = []
#: This connection's role; see _ROLES. 'controller' until connect() says otherwise.
_role: str = 'controller'
#: Records a virenv bridge has staged for this tick's reply via emit().
_emitted: typing.List[VehData] = []

# The tick currently held awaiting its answer.
_armed: bool = False
_received: typing.List[Vehicle] = []
_simState: int = 0
_simTime: float = 0.0


def _requireConnection():
    if _helper is None or _sock is None:
        raise NotConnected('call fixs.connect(...) first')


# ---------------------------------------------------------------------------
# Connecting
# ---------------------------------------------------------------------------

def connect(configPath=None, *, port=None, host=None, ego=None,
            connectTimeout=None, recvTimeout=None, role='controller'):
    """(string, ...) -> (string, integer) -- connect and return the endpoint.

    :param configPath: the config yaml TrafficLayer is running. Defaults to
        ``$FIXS_CONFIG_YAML``, which run_cosim sets for exactly this purpose:
        the wire format IS ``SimulationSetup.VehicleMessageField``, so two yamls
        that disagree decode the same bytes differently.
    :param port: which ``ApplicationSetup.VehicleSubscription`` entry is this
        client's. Required only when the config declares more than one, and then
        required rather than guessed -- silently taking entry 0 is how two
        clients end up sharing one endpoint.
    :param host: override the address from the config.
    :param ego: id, or list of ids, this client controls, reported by
        :func:`getEgoIDList`. Defaults to the ``attribute.id`` list of the
        selected subscription, so the yaml is not restated in code.
    :param connectTimeout: seconds to keep retrying the connect; ``None``
        retries forever, which is right under a supervisor (run_cosim) that
        stops the stack itself when something upstream dies.
    :param recvTimeout: seconds to wait for the next message; ``None`` waits
        indefinitely. TrafficLayer advances only once EVERY subscriber has
        answered, so a deadline here fires when some OTHER client stalls.
    :param role: ``'controller'`` (default) or ``'virenv'``. See :data:`_ROLES`.
        ``'virenv'`` unlocks :func:`emit` and :func:`transport`, which a
        controller must not have.
    """
    global _helper, _sock, _egoIds, _declaredFields, _role
    global sim, vehicle, trafficlight, _noTick

    if role not in _ROLES:
        raise FixsError(
            f'unknown role {role!r}. Use one of: {", ".join(_ROLES)}.')

    if _sock is not None:
        close()

    configPath = configPath or os.environ.get('FIXS_CONFIG_YAML') or 'config.yaml'
    if not os.path.isfile(configPath):
        raise FixsError(f'config not found: {configPath}')

    config = ConfigHelper()
    config.getConfig(configPath)

    declared = config.simulation_setup.get('VehicleMessageField') or ['id', 'speed']

    subscription = _selectSubscription(config, configPath, port)
    if host is None:
        host = subscription['ip'][0]
    if port is None:
        port = subscription['port'][0]

    if ego is None:
        ego = (subscription.get('attribute') or {}).get('id') or []
    _egoIds = [ego] if isinstance(ego, str) else list(ego)

    msgHelper = MsgHelper()
    msgHelper.set_vehicle_message_field(declared)

    _declaredFields = frozenset(declared)
    _role = role
    _helper = SocketHelper(config_helper=config, msg_helper=msgHelper)
    _sock = _openSocket(host, int(port), connectTimeout, recvTimeout)
    _noTick = _NO_TICK_YET
    sim = _Sim(unavailable=_NO_TICK_YET)
    vehicle = _VehicleView(unavailable=_NO_TICK_YET,
                           fields=_declaredFields, egoIDs=_egoIds)
    trafficlight = _TrafficLightView(unavailable=_NO_TICK_YET)
    atexit.register(close)          # an unanswered tick must still go out
    return host, int(port)


def _selectSubscription(config, configPath, port):
    subscriptions = config.application_setup.get('VehicleSubscription') or []
    if not subscriptions:
        raise FixsError(
            f'{configPath} has no ApplicationSetup.VehicleSubscription, so '
            f'there is no endpoint to connect to.'
        )
    if port is not None:
        for entry in subscriptions:
            if port in entry.get('port', []):
                return entry
        declaredPorts = [p for e in subscriptions for p in e.get('port', [])]
        raise FixsError(
            f'{configPath} declares no subscription on port {port}. '
            f'It declares: {declaredPorts}.'
        )
    if len(subscriptions) > 1:
        declaredPorts = [p for e in subscriptions for p in e.get('port', [])]
        raise FixsError(
            f'{configPath} declares {len(subscriptions)} vehicle subscriptions '
            f'(ports {declaredPorts}); pass port= to say which is this client.'
        )
    return subscriptions[0]


def _openSocket(host, port, connectTimeout, recvTimeout):
    """Connect, retrying until TrafficLayer is actually listening.

    A client is often started BEFORE TrafficLayer -- which may itself be waiting
    on a map cook or a simulator launch -- so a single attempt fails on a
    perfectly healthy stack.
    """
    deadline = None if connectTimeout is None else _time.time() + connectTimeout
    announced = False
    while True:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
        # TCP_NODELAY: disable Nagle, as SocketHelper.cpp does on every socket it
        # opens. The FIXS exchange is strict request/response with small messages --
        # a render-only bridge answers with a bare 9-byte header -- which is the
        # exact shape Nagle penalises: it holds a small write until the previous
        # segment is acknowledged, and the peer's delayed ACK does not arrive until
        # its timer fires. Measured on the MLK corridor, the Python bridge waited
        # 77.41 ms per tick in the FIXS exchange against the C++ bridge's 42.66 ms
        # on the same stack and window, while doing LESS work of its own (33.13 vs
        # 37.41 ms). The 34.75 ms difference is a delayed-ACK interval, not
        # computation, and it applied to every Python FIXS client -- the eco
        # controller included, so it was a cost on the whole co-simulation.
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        try:
            sock.connect((host, port))
        except OSError as exc:
            sock.close()
            if deadline is not None and _time.time() >= deadline:
                raise FixsError(
                    f'could not reach TrafficLayer at {host}:{port} within '
                    f'{connectTimeout}s: {exc}'
                ) from exc
            if not announced:
                print(f'[fixs] waiting for TrafficLayer on {host}:{port} ...')
                announced = True
            _time.sleep(0.5)
            continue

        sock.settimeout(recvTimeout)
        print(f'[fixs] connected to TrafficLayer on {host}:{port}')
        return sock


def close():
    """Close the connection. Safe to call more than once.

    A tick that was received and never answered is answered here as a safety
    net -- TrafficLayer is blocked waiting for it. Applications should not rely
    on this: call send() in the loop body, where it is visible.
    """
    global _sock, _helper, _egoIds, _armed, _received, _role, _emitted
    global sim, vehicle, trafficlight, _noTick, _declaredFields
    if _armed and _sock is not None:
        try:
            send()
        except (OSError, FixsError):
            pass                # peer already gone, or an incomplete command
    if _sock is not None:
        try:
            _sock.close()
        except OSError:
            pass
    _sock = None
    _helper = None
    _egoIds = []
    _armed = False
    _received = []
    _role = 'controller'
    _emitted = []
    _declaredFields = None
    _noTick = _NOT_CONNECTED
    sim = _Sim(unavailable=_NOT_CONNECTED)
    vehicle = _VehicleView(unavailable=_NOT_CONNECTED)
    trafficlight = _TrafficLightView(unavailable=_NOT_CONNECTED)


# ---------------------------------------------------------------------------
# The tick
# ---------------------------------------------------------------------------

def recv():
    """() -> None -- receive the next tick.

    Receiving is all it does. The moment's data is read through ``fixs.sim`` and
    the views, so nothing is encoded in a return value that would have to change
    shape as more status is exposed::

        try:
            while True:
                fixs.recv()
                ...
                fixs.send()
        except fixs.Shutdown:
            pass

    :raises Shutdown: TrafficLayer has ended the run.
    :raises ProtocolError: the previous tick was never answered. TrafficLayer
        does not advance until every subscriber has replied, so a missing send()
        would otherwise surface as an unexplained hang here.
    """
    _requireConnection()
    global sim, vehicle, trafficlight, _armed, _received, _simState, _simTime
    global _noTick

    if _armed:
        raise ProtocolError(
            f'tick {_simTime:.2f} was received but never answered. Call '
            f'fixs.send() before fixs.recv().'
        )

    _helper.clear_data()
    simState, simTime = _helper.recv_data(_sock)
    if simState == 0:
        # The views refuse after shutdown -- reporting vehicles when there is no
        # tick would be a lie. fixs.sim keeps answering, because the time the run
        # reached is a fact, and reporting it is the natural thing to do in the
        # `except fixs.Shutdown:` block.
        _noTick = _SHUTDOWN
        vehicle = _VehicleView(unavailable=_SHUTDOWN,
                               fields=_declaredFields, egoIDs=_egoIds)
        trafficlight = _TrafficLightView(unavailable=_SHUTDOWN)
        raise Shutdown('TrafficLayer has ended the run')

    _received = _helper.vehicle_data_receive_list
    for record in _received:
        record.__class__ = Vehicle      # adopt in place: no copy, no re-decode
        object.__setattr__(record, '_written', frozenset())
    vehicle = _VehicleView(_received, fields=_declaredFields, egoIDs=_egoIds)
    trafficlight = _TrafficLightView(
        _helper.traffic_light_data_receive_list,
        key=lambda r: (r.name or '').strip())

    _simState, _simTime = simState, simTime
    sim = _Sim(time=simTime, state=simState)
    _noTick = None
    _armed = True


def send(vehIDs=None):
    """(list) -> None -- answer the tick now.

    Without an argument the reply carries exactly the records commanded with
    ``veh.set(...)``, which is what a controller wants: returning ~200 untouched
    records every tick doubles upstream volume to say nothing.

    ``vehIDs`` adds records to return UNCHANGED, on top of whatever was
    commanded -- additive, so it cannot silently drop a command. That is what an
    echo client or protocol probe needs; an application generally does not,
    because TrafficLayer overlays a returned record onto later clients' feed and
    an unchanged one replaces a record with itself.

    A tick with nothing to say still sends: a header and no records. That is a
    real message -- it is what tells TrafficLayer this client is done and the
    tick may advance.
    """
    _requireConnection()
    global _armed, _emitted
    if not _armed:
        raise ProtocolError(
            'there is no tick to answer. Call fixs.recv() first, and call '
            'fixs.send() exactly once per tick received.'
        )

    commanded = set()
    for record in _received:
        if record._written:
            _validateCommand(record)
            commanded.add(record.id.strip())

    if vehIDs is None:
        wanted = commanded
    else:
        if isinstance(vehIDs, str):
            raise TypeError(
                f'send() takes a list of ids, not a single string. '
                f'Use send([{vehIDs!r}]).'
            )
        wanted = commanded | set(vehIDs)

    # Filter the received list rather than the id-keyed view, so records go back
    # in the order they arrived and a repeated id is not collapsed.
    sendList = [r for r in _received if r.id.strip() in wanted]

    # Do NOT substitute a placeholder record when sendList is empty: it would
    # put a vehicle with an empty id and zeroed fields on the wire every idle
    # tick, which over a long run is most of the traffic.
    # A virenv bridge's measured records go out ahead of any echoed ones, so a
    # bridge that both emits an id and lists it in vehIDs reports the measured
    # value -- the last write for an id is the one TrafficLayer keeps, and the
    # echo would otherwise overwrite the measurement with the stale input.
    _helper.vehicle_data_send_list.extend(_emitted)
    _helper.vehicle_data_send_list.extend(
        r for r in sendList if r.id.strip() not in {e.id.strip() for e in _emitted})
    _emitted = []
    _helper.sendData(_simState, _simTime, _sock)
    _armed = False


def emit(record):
    """(VehData) -> None -- report a MEASURED record on this tick's reply.

    The bridge counterpart of :meth:`Vehicle.set`. A virtual-environment bridge
    does not command vehicles; it reports what its backend measured -- the ego
    pose and speed CARLA's physics produced, for an id the traffic simulator may
    not have yet (a deferred ego spawn). Both of those are things
    :meth:`Vehicle.set` and :func:`send` deliberately forbid a controller, so
    this is a separate verb gated on ``role='virenv'`` rather than a relaxation
    of the controller contract.

    ``record`` is a plain :class:`~CommonLib.VehDataMsgDefs.VehData` the caller
    built -- fields are written directly, not through ``set()``. Only fields in
    ``SimulationSetup.VehicleMessageField`` reach the wire, exactly as for any
    other record.

    :raises NotConnected: before connect() or after close().
    :raises ProtocolError: on a controller connection, or with no tick held.
    """
    _requireConnection()
    if _role != 'virenv':
        raise ProtocolError(
            "emit() reports measured state and is only available to a bridge. "
            "This connection is a controller; command a vehicle with "
            "veh.set(speedDesired=...), or open the connection with "
            "fixs.connect(..., role='virenv')."
        )
    if not _armed:
        raise ProtocolError(
            'there is no tick to answer. Call fixs.recv() before fixs.emit().')
    if not isinstance(record, VehData):
        raise TypeError(
            f'emit() takes a VehData, got {type(record).__name__}')
    _emitted.append(record)


def transport():
    """() -> (SocketHelper, MsgHelper) -- the codec objects behind this connection.

    For a bridge that mirrors the C++ ``VirEnvCore``, which owns ``Sock_c`` and
    ``Msg_c`` directly and walks ``Msg_c.VehDataRecv_um`` in its step body. A
    controller has no reason to reach past the views and is refused, so this does
    not become a back door around the read-only records.

    :raises ProtocolError: on a controller connection.
    """
    _requireConnection()
    if _role != 'virenv':
        raise ProtocolError(
            "transport() exposes the raw codec and is only available to a "
            "bridge. Read this tick through fixs.vehicle / fixs.trafficlight / "
            "fixs.sim instead."
        )
    return _helper, _helper.msg_helper


def commandKind(record):
    """(Vehicle) -> 'actuation' | 'speedsteer' | None -- the shape that was written.

    Which fields a controller wrote IS which interface it is commanding through,
    so the shape is read off the record rather than declared anywhere. The two
    map onto the two interfaces a vehicle plant offers:

        'actuation'   pedals + steer   -> the caller closes the loop
                                          (Carla: apply_control)
        'speedsteer'  speed  + steer   -> the plant closes it
                                          (Carla: apply_ackermann_control)

    ``None`` means nothing was commanded this tick, which is a real answer: the
    last command persists, and a bridge should leave it alone rather than
    substitute a zero.
    """
    written = record._written
    if written & Vehicle.PEDAL_FIELDS:
        return 'actuation'
    if written & Vehicle.LONGITUDINAL_FIELDS:
        return 'speedsteer'
    return None


def _validateCommand(record):
    """Check that what was written forms a command a plant can act on."""
    written = record._written
    kind = commandKind(record)
    who = record.id.strip()

    if kind == 'actuation':
        missing = Vehicle.ACTUATION_FIELDS - written
        if missing:
            raise ProtocolError(
                f"'{who}': actuation is one command -- applyEgoActuation reads "
                f"throttle, brake and steer every tick, so the ones left out "
                f"would ship whatever arrived. Missing: "
                f"{', '.join(sorted(missing))}."
            )

    elif kind == 'speedsteer':
        if written >= Vehicle.LONGITUDINAL_FIELDS:
            raise ProtocolError(
                f"'{who}': set speedDesired or accelerationDesired, not both -- "
                f"TrafficLayer accepts exactly one longitudinal command "
                f"(ConfigHelper.cpp:256)."
            )

    elif written & Vehicle.STEER_FIELD:
        # Steer alone became representable when it left PEDAL_FIELDS. It is not
        # a command: nothing downstream knows what to do with a steer angle and
        # no longitudinal intent, so say so rather than apply half a command.
        raise ProtocolError(
            f"'{who}': steerAngleDesired alone is not a command. Pair it with "
            f"the pedals (acceleratorPedalDesired, brakePedalDesired) to close "
            f"the loop yourself, or with speedDesired to let the plant close it."
        )


def __getattr__(name):
    # Imported on demand: it pulls in the CARLA-side impersonation, which a
    # client that is not driving an ego has no reason to load.
    if name == 'EgoVehicle':
        from CommonLib.CarlaAgent import EgoVehicle
        return EgoVehicle
    # Detector records are received but not decoded -- SocketHelper.recv_data
    # drops them. Say so rather than handing back an empty view, which would
    # read as "no detectors this tick".
    if name == 'detector':
        raise NotImplementedError(
            'detector data is not decoded yet -- SocketHelper.recv_data drops '
            'DetectorData records (see the TODO in CommonLib/SocketHelper.py)'
        )
    raise AttributeError(f'module {__name__!r} has no attribute {name!r}')
