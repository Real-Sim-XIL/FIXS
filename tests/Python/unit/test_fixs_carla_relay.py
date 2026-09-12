"""Does `import fixs.carla as carla` leave a user's CARLA code working?

That is the whole adoption claim, and it is worth pinning because the failure
is quiet: a value type that does NOT come through reads as an AttributeError
deep inside someone's agent, and a session call that DOES come through reads as
a second client silently racing the bridge for the world's clock.

Measured on a real 2656-line controller: 38 of its 40 `carla.` sites are value
types, which is why those must pass through untouched and why the two that are
not are the only ones FIXS has to answer.

    python -m pytest tests/Python/unit/test_fixs_carla_relay.py
"""
from __future__ import annotations

import pytest

from CommonLib.fixs import carla as relay

realCarla = pytest.importorskip("carla", reason="needs the CARLA PythonAPI")


@pytest.fixture(autouse=True)
def noBackend():
    """A bare unit test has no bridge behind it, which is also how a user finds
    out they ran their controller outside one."""
    from CommonLib.VirEnv import EgoControllerHost as host
    saved = host._backend
    host._backend = None
    relay._reset()
    yield host
    host._backend = saved
    relay._reset()


# --------------------------------------------------------------------------
# value types pass straight through
# --------------------------------------------------------------------------

@pytest.mark.parametrize("name", [
    "Location", "Rotation", "Transform", "Vector3D",
    "VehicleControl", "LaneType", "LaneChange", "TrafficLightState",
])
def test_a_value_type_is_carlas_own_class(name):
    """Not a wrapper -- the same class object. Anything else means a user's
    `isinstance` check, or a value handed to a real CARLA call, can fail."""
    assert getattr(relay, name) is getattr(realCarla, name)


def test_a_value_type_still_constructs_and_carries_its_numbers():
    """The shape a controller actually writes (this is verbatim from a real
    one's sensor mount)."""
    tf = relay.Transform(relay.Location(x=2.5, z=0.85),
                         relay.Rotation(pitch=-2.0))
    assert tf.location.x == pytest.approx(2.5)
    assert tf.location.z == pytest.approx(0.85)
    assert tf.rotation.pitch == pytest.approx(-2.0)


def test_an_unlisted_symbol_still_resolves():
    """The module forwards by fallback rather than by an allow-list, so a
    symbol nobody anticipated is not a new adoption blocker."""
    assert relay.Color is realCarla.Color


def test_a_name_carla_does_not_have_raises_attribute_error():
    with pytest.raises(AttributeError):
        relay.NotAThingInCarla


# --------------------------------------------------------------------------
# session calls are FIXS's
# --------------------------------------------------------------------------

def test_opening_a_second_client_is_refused_with_the_alternative():
    """A synchronous world may be advanced by one party. Two clients is how a
    run loses its clock, and it would look like a hang, not an error."""
    with pytest.raises(RuntimeError) as exc:
        relay.Client("127.0.0.1", 2000)
    assert "carla.world" in str(exc.value)


def test_world_without_a_backend_says_why(noBackend):
    with pytest.raises(RuntimeError) as exc:
        relay.world
    assert "backend" in str(exc.value)


def test_world_and_client_come_from_the_backend(noBackend):
    class _Stub:
        carlaWorld = "the-world"
        carlaClient = "the-client"

    noBackend._backend = _Stub()
    assert relay.world == "the-world"
    assert relay.client == "the-client"
    assert relay.available() is True


def test_the_map_is_fetched_once(noBackend):
    """get_map() serialises the whole OpenDRIVE; a controller asking per tick
    would pay that on every step."""
    calls = []

    class _World:
        def get_map(self):
            calls.append(1)
            return "the-map"

    class _Stub:
        carlaWorld = _World()

    noBackend._backend = _Stub()
    assert relay.map == "the-map"
    assert relay.map == "the-map"
    assert len(calls) == 1
