"""DataLogger -- CSV time-series logging of the FIXS vehicle-data stream.

Python peer of ``CommonLib/DataLogger.{h,cpp}``: same method names, same header
line, same ``%.3f`` / ``%.5f`` cell formatting, so a CSV written by the Python
bridge and one written by the C++ bridge are diffable line for line.

IMPORTANT -- the logged values are in the FIXS WIRE convention, which is the
SUMO / VISSIM convention:

* position = front-of-vehicle anchor, metres
* heading  = navigational degrees, 0 = North, clockwise
* speed    = m/s

The traffic simulators share that convention; the vehicle-dynamics / virtual-
environment backends (CARLA, CarMaker) each have their own native frames and
diverge more. Those are normalised to the wire convention by the bridge BEFORE a
record reaches this logger -- which is the whole reason to log at the FIXS-message
layer rather than at a backend-native transform: logs from ANY backend are then
directly comparable.

Analysis and plotting of a logger's output is TEST-SPECIFIC and lives under the
relevant ``tests/`` folder, never in this class.
"""

import os

__all__ = ['DataLogger']

#: Written when DataLogSetup names no fields. Same list as the C++ default.
_DEFAULT_FIELDS = ('positionX', 'positionY', 'positionZ', 'heading', 'speed')

#: Fields rendered as text rather than as a number.
_STRING_FIELDS = frozenset({'type', 'vehicleClass', 'linkId'})

#: Fields this logger knows how to render. An unknown name yields an empty cell
#: rather than an error, so a config naming a field this build does not have
#: still produces a usable log -- exactly as the C++ ``cell()`` does.
_NUMERIC_FIELDS = frozenset({
    'positionX', 'positionY', 'positionZ', 'heading', 'speed', 'speedDesired',
    'acceleration', 'grade', 'length', 'width', 'height',
})


class DataLogger:
    """A generic, config-driven CSV logger for FIXS vehicle records."""

    def __init__(self):
        self._f = None
        self._path = ''
        self._fields = list(_DEFAULT_FIELDS)

    def open(self, path, fields):
        """(string, list) -> bool -- open the CSV; False if it cannot be written.

        Empty ``fields`` selects the default core set. Parent directories are
        created as needed. A False return leaves the logger a no-op, so callers
        need no extra guard around :meth:`logVehicle`.
        """
        self._path = path
        self._fields = list(fields) if fields else list(_DEFAULT_FIELDS)

        parent = os.path.dirname(os.path.abspath(path))
        try:
            os.makedirs(parent, exist_ok=True)
            self._f = open(path, 'w', encoding='utf-8', newline='')
        except OSError:
            self._f = None
            return False

        # Self-documenting convention line; CSV readers skip lines starting '#'.
        self._f.write('# FIXS DataLogger -- FIXS wire convention (== SUMO/VISSIM): '
                      'position = front-of-vehicle anchor [m], heading = navigational '
                      'degrees (0=N, CW), speed [m/s]. Backend-native frames '
                      '(Carla/CarMaker) normalized upstream.\n')
        self._f.write('simTime,id' + ''.join(',' + f for f in self._fields) + '\n')
        self._f.flush()
        return True

    def logVehicle(self, simTime, v, idOverride=None):
        """(float, VehData[, string]) -> None -- append one record.

        The record's values must already be in the FIXS wire convention documented
        above.

        ``idOverride`` labels the row with a different id than the record
        carries, for logging two VIEWS of one vehicle on the same clock --
        the CARLA ego and the traffic simulator's copy of it, as 'ego' and
        'ego_sumo'. It belongs here rather than at the call site because a
        fixs record refuses assignment to `id` (it is measured data), and a
        copy carries the same guard.
        """
        if self._f is None:
            return
        self._f.write('%.3f,%s%s\n'
                      % (simTime, idOverride or v.id,
                         ''.join(',' + self._cell(f, v) for f in self._fields)))
        # Flushed per record: at a 10 Hz feed that is cheap, and it means the log
        # survives a hard kill of the demo -- which is when it is most wanted.
        self._f.flush()

    def isOpen(self):
        return self._f is not None

    def path(self):
        return self._path

    def close(self):
        if self._f is not None:
            self._f.close()
            self._f = None

    @staticmethod
    def _cell(f, v):
        """Format one field by name. Unknown -> an empty cell, as the C++ does."""
        if f in _NUMERIC_FIELDS:
            return '%.5f' % getattr(v, f, 0.0)
        if f in _STRING_FIELDS:
            return str(getattr(v, f, ''))
        return ''
