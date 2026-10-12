"""Simulator-free tests for the fixs client API (#316).

These cover the rules the API exists to enforce -- what may be written, what
gets returned, and what happens when recv/send are used out of order -- using a
fake socket in place of TrafficLayer, so they run anywhere.
"""
import os

import pytest

from CommonLib import fixs
from CommonLib.MsgHelper import MsgHelper
from CommonLib.VehDataMsgDefs import VehData

SIMPLE_ECHO_CONFIG = os.path.join(
    os.path.dirname(__file__), '..', 'SimpleEchoClient', 'config.yaml'
)


def _adopted(**kwargs):
    record = VehData(**kwargs)
    record.__class__ = fixs.Vehicle
    return record


# ---------------------------------------------------------------------------
# Records are read-only
# ---------------------------------------------------------------------------

def test_adopted_record_is_still_a_vehdata():
    """Adoption must not break code that type-checks against the wire struct."""
    assert isinstance(_adopted(id='ego'), VehData)


def test_measured_fields_cannot_be_assigned():
    """Unit conversion in place must not silently become a command."""
    veh = _adopted(id='ego', speed=10.0)
    with pytest.raises(AttributeError):
        veh.speed = veh.speed * 2.23694
    assert veh.speed == 10.0


@pytest.mark.parametrize('field', sorted(fixs.Vehicle.COMMAND_FIELDS))
def test_command_fields_cannot_be_assigned_either(field):
    """One way to command: the setter. Assignment points at it."""
    veh = _adopted(id='ego')
    with pytest.raises(AttributeError) as excinfo:
        setattr(veh, field, 1.0)
    assert 'veh.set(' in str(excinfo.value)


# ---------------------------------------------------------------------------
# Views
# ---------------------------------------------------------------------------

def test_getall_and_getidlist():
    a, b = _adopted(id='ego  '), _adopted(id='veh1 ')
    view = fixs._VehicleView([a, b])
    assert view.getAll() == {'ego': a, 'veh1': b}
    assert sorted(view.getIDList()) == ['ego', 'veh1']


def test_get_returns_the_record_or_none():
    a = _adopted(id='ego')
    view = fixs._VehicleView([a])
    assert view.get('ego') is a
    # Absent is a normal state -- a subscribed ego before its departure time.
    assert view.get('nope') is None


def test_declared_values_are_read_only_and_copied():
    """fields cannot be changed mid-run: both ends decode by the same list."""
    view = fixs._VehicleView(fields=['id', 'speed'], egoIDs=['ego'])
    with pytest.raises(AttributeError):
        view.fields = ['id']
    view.fields.clear()
    assert view.fields == ['id', 'speed']
    assert view.egoIDs == ['ego']


def test_getall_returns_a_copy():
    view = fixs._VehicleView([_adopted(id='ego')])
    view.getAll().clear()
    assert view.getIDList() == ['ego']


# ---------------------------------------------------------------------------
# Connect-time contracts
# ---------------------------------------------------------------------------

def test_missing_config_is_reported_by_path():
    with pytest.raises(fixs.FixsError) as excinfo:
        fixs.connect('no_such_config.yaml')
    assert 'no_such_config.yaml' in str(excinfo.value)


def test_recv_before_connect_raises():
    fixs.close()
    with pytest.raises(fixs.NotConnected):
        fixs.recv()


def test_send_before_connect_raises():
    fixs.close()
    with pytest.raises(fixs.NotConnected):
        fixs.send()


def test_detector_says_it_is_not_decoded():
    """An empty view would read as 'no detectors this tick', which is a lie."""
    with pytest.raises(NotImplementedError):
        fixs.detector


class _Cfg:
    """Minimal ConfigHelper stand-in for subscription-selection tests."""

    def __init__(self, subscriptions, raw=None):
        self.simulation_setup = {'VehicleMessageField': ['id', 'speed']}
        self.application_setup = {'VehicleSubscription': subscriptions}
        # _selectSubscription reads the document as written to tell the bridge's
        # subscription from the caller's; ConfigHelper always sets it.
        self.raw = dict(raw or {})


def _sub(port, ids=('ego',)):
    return {'type': 'ego', 'attribute': {'id': list(ids), 'radius': [0]},
            'ip': ['127.0.0.1'], 'port': [port]}


def test_single_subscription_is_selected_without_a_port():
    entry = fixs._selectSubscription(_Cfg([_sub(2444)]), 'c.yaml', None)
    assert entry['port'] == [2444]


def test_several_subscriptions_require_a_port():
    """Silently taking entry 0 is how two clients end up sharing one endpoint."""
    with pytest.raises(fixs.FixsError) as excinfo:
        fixs._selectSubscription(_Cfg([_sub(430), _sub(440)]), 'c.yaml', None)
    assert '430' in str(excinfo.value) and '440' in str(excinfo.value)


def test_carla_client_port_names_the_bridge_so_the_rest_is_mine():
    """A CARLA scenario subscribes the BRIDGE on ApplicationSetup too, and says
    which one that is in CarlaSetup.CarlaClientPort. The application should not
    have to answer a question its own scenario yaml already answers."""
    cfg = _Cfg([_sub(430), _sub(2444)], raw={'CarlaSetup': {'CarlaClientPort': 430}})
    assert fixs._selectSubscription(cfg, 'c.yaml', None)['port'] == [2444]


def test_two_left_over_after_the_bridge_still_needs_a_port():
    """Naming the bridge removes ONE candidate. Two still ambiguous is still an
    error -- silently taking entry 0 is how two clients share one endpoint."""
    cfg = _Cfg([_sub(430), _sub(2444), _sub(2445)],
               raw={'CarlaSetup': {'CarlaClientPort': 430}})
    with pytest.raises(fixs.FixsError) as excinfo:
        fixs._selectSubscription(cfg, 'c.yaml', None)
    assert '2444' in str(excinfo.value) and '2445' in str(excinfo.value)


def test_port_selects_its_own_subscription():
    entry = fixs._selectSubscription(_Cfg([_sub(430), _sub(440)]), 'c.yaml', 440)
    assert entry['port'] == [440]


def test_unknown_port_lists_what_is_declared():
    with pytest.raises(fixs.FixsError) as excinfo:
        fixs._selectSubscription(_Cfg([_sub(430)]), 'c.yaml', 999)
    assert '430' in str(excinfo.value)


# ---------------------------------------------------------------------------
# recv / send, against a fake TrafficLayer
# ---------------------------------------------------------------------------

FIELDS = ['id', 'speed', 'speedDesired']
# A config that also carries the command fields the actuation tests use.
FULL_FIELDS = FIELDS + ['accelerationDesired', 'steerAngleDesired',
                        'acceleratorPedalDesired', 'brakePedalDesired']


class FakeSocket:
    """Serves a canned sequence of ticks and records everything sent back."""

    def __init__(self, ticks, fields):
        helper = MsgHelper()
        helper.set_vehicle_message_field(fields)
        self._stream = b''.join(self._encode(helper, *tick) for tick in ticks)
        self._read = 0
        self.sent = []

    @staticmethod
    def _encode(helper, simState, simTime, vehicles):
        body = bytearray(65536)
        index = 0
        total = helper.msg_header_size
        for veh in vehicles:
            _, size, index = helper.pack_veh_data(body, index, veh)
            total += size
        header = bytearray(81728)
        header, headerLen = helper.pack_msg_header(header, simState, simTime, total)
        return bytes(header[:headerLen]) + bytes(body[:index])

    def recv(self, n):
        chunk = self._stream[self._read:self._read + n]
        self._read += len(chunk)
        if not chunk:
            raise ConnectionError('fake stream exhausted')
        return chunk

    def sendall(self, data):
        self.sent.append(bytes(data))

    def settimeout(self, _):
        pass

    def close(self):
        pass


class _ConfigStub:
    simulation_setup = {'EnableVerboseLog': False}
    application_setup = {}


def _install(ticks, egoIds=('ego',), shutdown=True, fields=None):
    """Point the module at a fake TrafficLayer without touching a real socket."""
    from CommonLib.SocketHelper import SocketHelper
    if shutdown:
        ticks = list(ticks) + [(0, 99.0, [])]
    fixs.close()
    fields = list(fields or FIELDS)
    msgHelper = MsgHelper()
    msgHelper.set_vehicle_message_field(fields)
    fixs._helper = SocketHelper(config_helper=_ConfigStub(), msg_helper=msgHelper)
    fixs._sock = FakeSocket(ticks, fields)
    fixs._egoIds = list(egoIds)
    fixs._declaredFields = frozenset(fields)
    fixs._armed = False
    return fixs._sock


def _veh(vehID, speed=1.0):
    return VehData(id=vehID, speed=speed)


def _drain(vehIDs=None):
    """Run the loop the way a client does, returning the tick times."""
    times = []
    try:
        while True:
            fixs.recv()
            times.append(fixs.sim.time)
            fixs.send(vehIDs() if callable(vehIDs) else vehIDs)
    except fixs.Shutdown:
        return times


def _recordCount(message, fields=None):
    """Count vehicle records in one encoded FIXS message."""
    helper = MsgHelper()
    helper.set_vehicle_message_field(list(fields or FIELDS))
    _, _, total = helper.depack_msg_header(message[:helper.msg_header_size])
    index = helper.msg_header_size
    count = 0
    while index < total:
        size, _kind = helper.depack_msg_type(
            message[index:index + helper.msg_each_header_size])
        index += size
        count += 1
    return count


def test_every_tick_is_answered():
    sock = _install([(1, 0.1, [_veh('ego')]), (1, 0.2, [_veh('ego')])])
    assert _drain() == [pytest.approx(0.1), pytest.approx(0.2)]
    assert len(sock.sent) == 2


def test_reply_carries_only_commanded_records_by_default():
    sock = _install([(1, 0.1, [_veh('ego', 5.0), _veh('other', 6.0)])])
    try:
        while True:
            fixs.recv()
            fixs.vehicle.get('ego').set(speedDesired=9.0)
            fixs.send()
    except fixs.Shutdown:
        pass
    assert len(sock.sent) == 1
    # One record on the wire, not two: the untouched vehicle is not returned.
    assert _recordCount(sock.sent[0]) == 1


def test_setter_writes_the_value_that_goes_out():
    _install([(1, 0.1, [_veh('ego', 5.0)])])
    try:
        while True:
            fixs.recv()
            fixs.vehicle.get('ego').set(speedDesired=9.0)
            assert fixs.vehicle.get('ego').speedDesired == 9.0
            fixs.send()
    except fixs.Shutdown:
        pass


def test_absent_id_reads_as_none():
    _install([(1, 0.1, [_veh('ego')])])
    fixs.recv()
    assert fixs.vehicle.get('egoo') is None
    fixs.close()


def test_set_rejects_a_measured_field():
    _install([(1, 0.1, [_veh('ego')])])
    fixs.recv()
    with pytest.raises(AttributeError) as excinfo:
        fixs.vehicle.get('ego').set(speed=12.0)
    assert 'measured' in str(excinfo.value)
    fixs.close()


def test_set_rejects_a_field_not_on_the_wire():
    """A command that cannot be serialised would vanish with no symptom."""
    _install([(1, 0.1, [_veh('ego')])])          # declares speedDesired only
    fixs.recv()
    with pytest.raises(fixs.ProtocolError) as excinfo:
        fixs.vehicle.get('ego').set(steerAngleDesired=0.1)
    assert 'VehicleMessageField' in str(excinfo.value)
    fixs.close()


def test_set_rejects_non_finite_values():
    """NaN otherwise reaches SUMO and surfaces far from the cause."""
    _install([(1, 0.1, [_veh('ego')])])
    fixs.recv()
    with pytest.raises(ValueError):
        fixs.vehicle.get('ego').set(speedDesired=float('nan'))
    fixs.close()


def test_views_refuse_to_answer_when_no_tick_is_held():
    """Answering '{}' would read as a quiet tick rather than as no tick."""
    fixs.close()
    for call in (lambda: fixs.vehicle.getAll(),
                 lambda: fixs.vehicle.get('ego'),
                 lambda: fixs.vehicle.getIDList(),
                 lambda: fixs.sim.time):
        with pytest.raises(fixs.ProtocolError):
            call()


def test_views_refuse_to_answer_after_shutdown():
    _install([(1, 0.1, [_veh('ego')])])
    fixs.recv()
    assert fixs.vehicle.getIDList() == ['ego']
    fixs.send()
    with pytest.raises(fixs.Shutdown):
        fixs.recv()
    with pytest.raises(fixs.ProtocolError) as excinfo:
        fixs.vehicle.getAll()
    assert 'shut down' in str(excinfo.value)


def test_set_rejects_an_unknown_field():
    """A typo in a field name fails immediately, listing what is writable."""
    _install([(1, 0.1, [_veh('ego')])])
    fixs.recv()
    with pytest.raises(AttributeError) as excinfo:
        fixs.vehicle.get('ego').set(spedDesired=9.0)
    assert 'speedDesired' in str(excinfo.value)
    fixs.close()


def test_partial_actuation_is_rejected_at_send():
    """applyEgoActuation reads all three every tick, so two is not a command."""
    _install([(1, 0.1, [_veh('ego')])], fields=FULL_FIELDS)
    fixs.recv()
    fixs.vehicle.get('ego').set(steerAngleDesired=0.1, brakePedalDesired=0.0)
    with pytest.raises(fixs.ProtocolError) as excinfo:
        fixs.send()
    assert 'acceleratorPedalDesired' in str(excinfo.value)
    fixs.close()


def test_complete_actuation_is_accepted():
    sock = _install([(1, 0.1, [_veh('ego')])], fields=FULL_FIELDS)
    fixs.recv()
    fixs.vehicle.get('ego').set(steerAngleDesired=0.1,
                                acceleratorPedalDesired=0.3,
                                brakePedalDesired=0.0)
    fixs.send()
    assert _recordCount(sock.sent[0], FULL_FIELDS) == 1
    fixs.close()


def test_both_longitudinal_commands_are_sent():
    """speedDesired for SUMO, accelerationDesired beside it for a downstream controller."""
    sock = _install([(1, 0.1, [_veh('ego')])], fields=FULL_FIELDS)
    fixs.recv()
    fixs.vehicle.get('ego').set(speedDesired=9.0, accelerationDesired=1.0)
    fixs.send()
    assert _recordCount(sock.sent[0], FULL_FIELDS) == 1
    fixs.close()


def test_declared_ids_are_configuration_not_this_tick():
    """getEgoIDList is the subscription's list, whether or not they are present."""
    _install([(1, 0.1, [_veh('other')])], egoIds=('ego',))
    fixs.recv()
    assert fixs.vehicle.egoIDs == ['ego']
    assert fixs.vehicle.getIDList() == ['other']
    assert fixs.vehicle.get('ego') is None
    fixs.close()


def test_multiple_egos_are_supported():
    _install([(1, 0.1, [_veh('e1'), _veh('e2'), _veh('bg')])], egoIds=('e1', 'e2'))
    fixs.recv()
    assert sorted(fixs.vehicle.egoIDs) == ['e1', 'e2']
    fixs.close()


def test_send_with_ids_returns_those_records_unchanged():
    sock = _install([(1, 0.1, [_veh('ego', 5.0), _veh('other', 6.0)])])
    _drain(lambda: fixs.vehicle.getIDList())
    assert _recordCount(sock.sent[0]) == 2


def test_send_with_a_slice_caps_the_reply():
    sock = _install([(1, 0.1, [_veh('ego'), _veh('a'), _veh('b')])])
    _drain(lambda: fixs.vehicle.getIDList()[:1])
    assert _recordCount(sock.sent[0]) == 1


def test_send_ids_are_additive_to_commands():
    """Listing ids must not drop what the setters marked."""
    sock = _install([(1, 0.1, [_veh('ego'), _veh('a'), _veh('b')])])
    try:
        while True:
            fixs.recv()
            fixs.vehicle.get('b').set(speedDesired=3.0)
            fixs.send(['ego'])
    except fixs.Shutdown:
        pass
    assert _recordCount(sock.sent[0]) == 2


def test_send_rejects_a_bare_string():
    sock = _install([(1, 0.1, [_veh('ego')])])
    fixs.recv()
    with pytest.raises(TypeError):
        fixs.send('ego')
    fixs.close()
    assert len(sock.sent) == 1          # close() still answered the tick


def test_empty_tick_replies_with_no_records():
    """A quiet tick sends a header and nothing else.

    Not a placeholder record: that would put a vehicle with an empty id and
    zeroed fields on the wire every idle tick. The reference echo clients have
    always replied this way, so it is also what byte-identity requires.
    """
    sock = _install([(1, 0.1, [])])
    _drain()
    assert len(sock.sent) == 1
    assert _recordCount(sock.sent[0]) == 0


def test_shutdown_raises_and_the_tick_is_not_answered():
    sock = _install([(1, 0.1, [_veh('ego')]), (0, 0.2, [])], shutdown=False)
    assert _drain() == [pytest.approx(0.1)]
    assert len(sock.sent) == 1          # the shutdown tick is not answered


def test_sim_still_answers_after_shutdown():
    """The time the run reached is a fact; the vehicle list would be a lie."""
    _install([(1, 0.1, [_veh('ego')])])
    fixs.recv()
    fixs.send()
    with pytest.raises(fixs.Shutdown):
        fixs.recv()
    assert fixs.sim.time == pytest.approx(0.1)     # last value, still available
    with pytest.raises(fixs.ProtocolError):
        fixs.vehicle.getAll()


def test_shutdown_is_not_a_fixserror():
    """A broad `except fixs.FixsError` must not swallow the end of a run."""
    assert not issubclass(fixs.Shutdown, fixs.FixsError)


def test_recv_without_send_raises_naming_the_tick():
    """A missed send stalls TrafficLayer; say so instead of hanging."""
    _install([(1, 0.1, [_veh('ego')]), (1, 0.2, [_veh('ego')])])
    fixs.recv()
    with pytest.raises(fixs.ProtocolError) as excinfo:
        fixs.recv()
    assert '0.10' in str(excinfo.value)
    fixs.close()


def test_send_without_recv_raises():
    _install([(1, 0.1, [_veh('ego')])])
    with pytest.raises(fixs.ProtocolError):
        fixs.send()


def test_close_answers_an_unsent_tick_as_a_safety_net():
    """Applications should call send() themselves; close() covers a crash."""
    sock = _install([(1, 0.1, [_veh('ego')]), (1, 0.2, [_veh('ego')])])
    fixs.recv()
    assert len(sock.sent) == 0
    fixs.close()
    assert len(sock.sent) == 1


def test_close_is_idempotent():
    _install([(1, 0.1, [])])
    fixs.recv()
    fixs.close()
    fixs.close()
