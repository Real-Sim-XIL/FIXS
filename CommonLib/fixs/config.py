"""fixs.config -- the scenario yaml this run is using, read-only.

    xil = fixs.config.get('xil')
    xil['transport']            # 'inprocess' | 'udp' | 'tcp'
    xil['ip']                   # the section's address, see get()

Names are the yaml's, lowercased: a section drops its ``Setup`` suffix and
every name goes to snake_case, so ``XilSetup.EnableXil`` is
``get('xil')['enable_xil']`` and ``CarlaSetup.CarlaTimeStep`` is
``get('carla')['carla_time_step']``.
"""

import copy
import os
import re
import types

from CommonLib.ConfigHelper import ConfigHelper

from . import FixsError

__all__ = ['get']

#: yaml section -> the ConfigHelper attribute holding its parsed values.
_PARSED = {
    'SimulationSetup': 'simulation_setup',
    'SumoSetup': 'Sumo_setup',
    'ApplicationSetup': 'application_setup',
    'XilSetup': 'Xil_setup',
    'EgoSetup': 'Ego_setup',
    'CarlaSetup': 'Carla_setup',
    'CarMakerSetup': 'CarMaker_setup',
    'DataLogSetup': 'DataLog_setup',
    'ViewSetup': 'View_setup',
}

_cache = {}


def _snake(name):
    """CarlaTimeStep -> carla_time_step, TrafficSimulatorIP -> traffic_simulator_ip."""
    return re.sub(r'(?<=[a-z0-9])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])', '_', name).lower()


def _section(name):
    """XilSetup -> xil, DataLogSetup -> data_log."""
    return _snake(name[:-len('Setup')] if name.endswith('Setup') else name)


def _configPath(configPath):
    from . import _connectedConfigPath       # set by fixs.connect(), read at call time
    path = (configPath or _connectedConfigPath
            or os.environ.get('FIXS_CONFIG_YAML'))
    if not path or not os.path.isfile(path):
        raise FixsError(
            'cannot read the scenario: '
            + ('$FIXS_CONFIG_YAML is not set' if not path else '%s does not exist' % path)
            + '. Pass the scenario yaml, or set $FIXS_CONFIG_YAML to the one this run is using.')
    return path


def _load(path):
    config = ConfigHelper()
    config.getConfig(path)
    sections = {}
    for yamlName in list(config.raw) + [n for n in _PARSED if n not in config.raw]:
        written = config.raw.get(yamlName) or {}
        if not isinstance(written, dict):
            continue
        # What the author wrote, then what ConfigHelper parsed over it (defaults, validation).
        merged = dict(written)
        if yamlName in _PARSED:
            merged.update(getattr(config, _PARSED[yamlName]))
        values = {_snake(k): v for k, v in merged.items()}
        subs = values.get('vehicle_subscription')
        if subs:
            # The same rule TrafficLayer uses for a section's address.
            values.setdefault('ip', (subs[0].get('ip') or [None])[0])
            values.setdefault('port', (subs[0].get('port') or [None])[0])
        sections[_section(yamlName)] = values
    return sections


def get(section, configPath=None):
    """(str) -> read-only dict -- one section of this run's scenario yaml.

    Values are parsed: defaults applied and validated the way FIXS reads them.
    A section with a ``vehicle_subscription`` also answers ``ip`` and ``port``,
    the first subscription's first ip and port -- the address TrafficLayer
    takes for that section.

    Changing the result changes nothing; each call hands back its own copy.
    """
    path = _configPath(configPath)
    if path not in _cache:
        _cache[path] = _load(path)
    sections = _cache[path]
    if section not in sections:
        raise FixsError('%s has no %r section. It has: %s'
                        % (path, section, ', '.join(sorted(sections)) or 'none'))
    return types.MappingProxyType(copy.deepcopy(sections[section]))
