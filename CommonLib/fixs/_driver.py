"""A ready-made driver for the FIXS ego (#24, FIXS#325).

    import fixs
    Controller = fixs.driver()

That is a working controller. The scenario names the file it lives in, FIXS
finds ``Controller`` in it, and calls ``control(ego, dt)`` every step.

WITH A DYNAMOMETER, you write the one function FIXS cannot::

    import fixs
    import my_dyno_rig                      # yours, and no business of FIXS

    def exchange(vref, dt):
        return my_dyno_rig.run_point(vref)  # a speed in, the speed reached out

    Controller = fixs.driver(exchange)

Your rig keeps its own API, its own protocol and its own rate; ``exchange``
is the glue, and it is the whole of what this module asks for.

Pass nothing and nothing is in the loop. This module never goes looking for a
dyno -- deciding one is there belongs to the caller, so passing a function IS
the declaration. For the simulated dyno, pass the one FIXS already has; its
exchange takes (vref, aref, dt), and the driver supplies aref::

    dyno = fixs.xil.dynosim()
    Controller = fixs.driver(dyno.exchange)

which is a function like any other and holds no privilege over yours.

WHERE IT GOES IN THE LOOP -- after every decision, before any command::

    want = target(ego)        the advisory, capped by the signal and the leader
    want = exchange(want)     what a real vehicle did with that
    drive(want)               pedals, or a speed for the plant to close on

Next step the driver reads ``ego.speed`` back, so the dyno is in the loop and
not beside it. ``target`` and ``drive`` are public for anyone who wants that
ordering in their own hands.

HOW IT DECIDES A SPEED. Every input that can demand less is turned into a SPEED
-- the fastest the ego may go and still stop comfortably for the thing that
input names -- and the command is their minimum::

    target = min(eco advisory, next signal, vehicle ahead)

One unit removes the arbitration, and puts the signal and the leader on a
horizon that matches the stopping distance: FIXS reports both from ~200 m,
where a stock CARLA agent finds a red light within 5 m and a leader within
about 13 m, neither of which is a stopping distance at 10-14 m/s.

CARLA-BOUND, deliberately. Steering comes from CARLA's own BehaviorAgent on
CARLA's own vehicle -- ``carla.bind`` hands it FIXS's corrected world and the
route the ego already has, ``carla.refresh`` brings it up to this tick. The
longitudinal half touches only wire fields and ``ego.set``, so it would move to
another backend; the lateral half would not. A non-CARLA driver is a second
module, not a refactor of this one.

IN-PROCESS is not a style choice. On the 0.1 s feed path the record's ``speed``
carries the eco advisory rather than the measured speed, so a controller
closing a speed loop there reads back its own setpoint (FIXS#305).
"""
from __future__ import annotations

import argparse
import inspect
import math

from . import FixsError, speedFromAcceleration   # noqa: E402
from .. import fixs as _fixs            # noqa: E402
from . import carla as carla            # noqa: E402
# CARLA's vendored agents: importing the package puts them on sys.path.
from agents.navigation.behavior_agent import BehaviorAgent   # noqa: E402
from agents.navigation.global_route_planner import GlobalRoutePlanner  # noqa: E402


#: 'speed'  -- the vehicle closes the speed loop; we write a speed and CARLA's
#:             own Ackermann controller tracks it.
#: 'pedals' -- this controller closes it and writes throttle and brake instead.
#:
#: The DEFAULT, for a scenario that says nothing. A scenario overrides it after
#: the controller's path:
#:     Controller: apps/.../ego_agent_controller.py --command-shape pedals
COMMAND_SHAPE = 'speed'

#: Stiffen CARLA's Ackermann speed loop (kp 50, ki 5, kd 0) until tracking is
#: near-exact. Only bites with COMMAND_SHAPE == 'speed', since those are the
#: gains standing between a commanded speed and the speed reached.
#:
#: On by default, which is a JUDGEMENT and not a neutral setting. CARLA's
#: lateral controller still needs tuning here, so the longitudinal plant is
#: deliberately taken out of the picture while that work happens: a run then
#: shows what the agent DECIDED rather than what the vehicle could deliver, and
#: a lateral result is not confounded by a speed the vehicle failed to hold.
#: The cost is that a vehicle tracking like this is not a vehicle -- the stiff
#: gains ring at about 1 Hz on every speed plateau -- so energy and comfort
#: numbers measured under it are a bound, not a prediction. Set False to put the
#: real plant back.
IDEAL_SPEED_TRACKING = True

#: Speed error to pedal, for COMMAND_SHAPE 'pedals'. Two laws; see the PR for
#: the measurements behind the choice.
#:
#:   'speed'  pedal = PEDAL_KV*e + I,  I += PEDAL_KI*e*dt
#:            Closes on the speed the bench (or the planner) hands us. The
#:            integral IS the road-load trim, so zero error still holds pedal.
#:
#:   'accel'  aDemand = SPEED_KP*e;  pedal += PEDAL_K*(aDemand - aMeasured)
#:            Keeps an explicit acceleration target, which is what an
#:            acceleration or jerk limit would need. NOT the default: aMeasured
#:            is differenced speed and lags the pedal by 0.25 s, and a pure
#:            integrator around that lag limit-cycles at 1.6 s. No gain removes
#:            it -- measured, amplitude moves with the gains, period does not.
#:            Fix the lag (lead term) before selecting this.
PEDAL_LOOP = 'speed'

#: What the upstream controller sends, and so what the driver reads: 'speed'
#: (speedDesired), 'accel' (accelerationDesired) or 'both'. The one not sent is
#: rebuilt from the feed's speed; 'both' checks that the two agree.
REFERENCE_MODE = 'speed'

#: The eco controller's step [s]: speedDesired = feedSpeed + accelerationDesired * FEED_STEP.
FEED_STEP = 0.1

#: 'both': how far [m/s^2] accelerationDesired may sit from (speedDesired - feedSpeed) / FEED_STEP.
BOTH_TOLERANCE = 1e-3


class Tuning(object):
    """The driver's gains, as ONE value.

    Six loose constants made a gain ladder six edits to the source, and a
    teardown line in a scratch script once left one of them changed: a whole
    batch of runs then reported gains it had not used. One value is one thing
    to pass, one thing to override, and one thing to write into the log --
    which is what makes a run able to say what it was.

        Tuning.parse('kv=0.3,ki=0.6')        # --tune, and the ladder
        tuning.replace(kv=0.3)               # in code
        str(tuning)                          # -> the log header, verbatim
    """

    __slots__ = ('kv', 'ki', 'kp', 'k', 'maxAccel', 'fullStop')

    #: What each is for, and where it came from. See the PR for the runs.
    _DOC = {
        'kv': 'speed loop, proportional [pedal per m/s]',
        'ki': 'speed loop, integral -- this is the road-load trim',
        'kp': "accel loop's outer gain [m/s2 per m/s], PEDAL_LOOP='accel' only",
        'k': "accel loop's inner gain, set by the 0.65 s dead time, not by plant gain",
        'maxAccel': 'ceiling on the acceleration the outer stage may ask [m/s2]',
        'fullStop': 'below this speed, with a target this low, hold the brake [m/s]',
    }

    def __init__(self, kv=0.25, ki=0.5, kp=5.0, k=0.01,
                 maxAccel=1.5, fullStop=0.1):
        self.kv, self.ki, self.kp, self.k = kv, ki, kp, k
        self.maxAccel, self.fullStop = maxAccel, fullStop

    def replace(self, **kw):
        """A copy with some gains changed. Unknown names are refused, not
        ignored: a typo that sets nothing is a run that silently used the
        defaults."""
        bad = set(kw) - set(self.__slots__)
        if bad:
            raise TypeError('unknown gain(s) %s -- known: %s'
                            % (', '.join(sorted(bad)), ', '.join(self.__slots__)))
        return Tuning(**dict({n: getattr(self, n) for n in self.__slots__}, **kw))

    @classmethod
    def parse(cls, text, base=None):
        """'kv=0.3,ki=0.6' -> Tuning, on top of base (or the defaults)."""
        out = {}
        for part in (text or '').split(','):
            part = part.strip()
            if not part:
                continue
            name, sep, value = part.partition('=')
            if not sep:
                raise ValueError('--tune %r: expected name=value' % part)
            try:
                out[name.strip()] = float(value)
            except ValueError:
                raise ValueError('--tune %s: %r is not a number'
                                 % (name.strip(), value.strip()))
        return (base or cls()).replace(**out)

    def __str__(self):
        return ','.join('%s=%g' % (n, getattr(self, n)) for n in self.__slots__)

    def __repr__(self):
        return 'Tuning(%s)' % self

#: 'speed' law.
PEDAL_KV, PEDAL_KI = 0.25, 0.5

#: 'accel' law. PEDAL_K is set by DEAD TIME, not plant gain: the ego reads
#: exactly 0.000 m/s for 13 ticks after the pedal first moves, and 13*K*A must
#: stay under half travel.
SPEED_KP = 5.0
PEDAL_K = 0.01
PEDAL_MAX_ACCEL = 1.5

#: Below this, with a target this low, hold the brake and carry no state
#: out of the stop. CARLA's FullStopEpsilon, same value.
FULL_STOP_MPS = 0.1


#: 'fixs'  -- drive the corridor the traffic simulator assigned.
#: 'stock' -- plan to a map spawn point with CARLA's own set_destination, the
#:            way a stock CARLA script does. For comparison only: the route is
#:            then the agent's choice, and the traffic around the ego is still
#:            reacting to the route the traffic simulator gave it.
ROUTE_SOURCE = 'fixs'

#: Use the eco advisory (ego.speedDesired) as the agent's speed target. False
#: drives EgoTargetSpeed instead, which takes the advisory out of a run and
#: leaves route following as the only thing under test.
USE_ADVISORY = True

#: Where the agent's traffic-signal information comes from.
#:
#: 'fixs'  -- the traffic simulator's own answer for the movement THIS ego is
#:            about to make: ego.signalLightColor and ego.signalLightDistance,
#:            reported from ~200 m. CARLA's own light check is then switched
#:            off, because here it can only be a worse copy of the same fact --
#:            the CARLA light actors are frozen and written from the traffic
#:            simulator every feed, so reading them back means recovering, by
#:            snapping stop-bar poses to lanes, what FIXS already knows exactly.
#:            There is no perception to simulate in this scenario.
#: 'carla' -- stock BehaviorAgent: it locates the governing signal itself among
#:            the heads fixs.carla presents at their stop bars. What a
#:            controller with a sensor model in front of it would use, and what
#:            this template shipped with. Its whole horizon is CARLA's
#:            BasicAgent._base_tlight_threshold -- a fixed 5.0 m, which
#:            BehaviorAgent.traffic_light_manager does not scale with speed the
#:            way BasicAgent.run_step does -- so the brake arrives about 3 m
#:            before the bar, while a full-brake stop from 10.5 m/s measures
#:            9.8-11.0 m on this corridor.
SIGNAL_SOURCE = 'fixs'

class Limits(object):
    """How close the driver lets the ego come to a bar or a leader.

    Policy, not tuning -- so it is set in code where a diff shows it, never on
    a yaml line where a change nobody reviews can move how hard the ego brakes.

        fixs.driver(limits=Limits(comfortDecel=1.5))
    """

    __slots__ = ('comfortDecel', 'stopMargin', 'commitDecel', 'leaderMargin')

    def __init__(self, comfortDecel=2.0, stopMargin=2.0,
                 commitDecel=4.0, leaderMargin=2.0):
        self.comfortDecel, self.stopMargin = comfortDecel, stopMargin
        self.commitDecel, self.leaderMargin = commitDecel, leaderMargin

    def __str__(self):
        return ','.join('%s=%g' % (n, getattr(self, n)) for n in self.__slots__)

    def __repr__(self):
        return 'Limits(%s)' % self


#: Deceleration used to turn a distance-to-stop into a speed ceiling, m/s^2.
#: Not a braking authority: the point of an envelope is that it is met early and
#: gently, so the emergency stop is never the thing that has to work.

#: Stop this far short of the stop bar, m.

#: Deceleration assumed when deciding whether a YELLOW can still be stopped for,
#: m/s^2. Firmer than COMFORT_DECEL on purpose: the question here is not "is this
#: pleasant" but "is stopping still possible", and answering it with the comfort
#: figure commits the ego to crossing from twice as far out as it needs to.

#: Kept behind the leader on TOP of the traffic simulator's own minGap: the
#: wire's precedingVehicleDistance is measured from the ego's front bumper plus
#: minGap to the leader's rear bumper, so 0.0 here would already leave minGap.

#: signalLightColor on the wire (CommonLib/TrafficHelper.cpp tlsStateToColor).
_RED, _YELLOW, _RED_YELLOW = 1, 2, 4

#: Stands for "this constraint binds nothing", in m/s.
_NO_LIMIT = 1e6

#: precedingVehicleDistance when the traffic simulator has NO leader in range.
#: It must be told apart from any other negative value, which means the opposite:
#: the leader's rear bumper is behind the ego's nose, i.e. the two overlap.
_NO_LEADER = -1.0



#: The options fixs.driver(**options) and fixs.driver.set(**options) take.
_OPTION_NAMES = frozenset({'shape', 'loop', 'tuning', 'limits', 'idealSpeedTracking',
                           'startInGear', 'referenceMode', 'feedStep'})
#: Set by fixs.driver.set(); every driver built afterwards starts from these.
_DEFAULTS = {}


def setDefaults(**defaults):
    """fixs.driver.set(name=value): options for every driver built after this call."""
    unknown = set(defaults) - _OPTION_NAMES
    if unknown:
        raise TypeError('fixs.driver.set(): unknown option(s) %s -- known: %s'
                        % (', '.join(sorted(unknown)), ', '.join(sorted(_OPTION_NAMES))))
    _DEFAULTS.update(defaults)


def _options(config, overrides=None):
    """Three layers, nearest the run wins: the module constants below are the
    defaults, fixs.driver(**options) overrides them, and what the SCENARIO
    wrote after the controller's path overrides both -- because the scenario
    is the thing an operator edits without touching code."""
    over = dict(overrides or {})
    unknown = set(over) - _OPTION_NAMES
    if unknown:
        raise TypeError('fixs.driver(): unknown option(s) %s -- known: %s'
                        % (', '.join(sorted(unknown)), ', '.join(sorted(_OPTION_NAMES))))

    p = argparse.ArgumentParser(prog='fixs.driver', add_help=False)
    p.add_argument('--command-shape', choices=('speed', 'pedals'),
                   default=over.get('shape', COMMAND_SHAPE))
    p.add_argument('--pedal-loop', choices=('speed', 'accel'),
                   default=over.get('loop', PEDAL_LOOP))
    p.add_argument('--reference-mode', choices=('speed', 'accel', 'both'),
                   default=over.get('referenceMode', REFERENCE_MODE))
    #: One option, not six: a gain ladder is then one token per rung, and the
    #: whole tuning state is one string that lands in the log verbatim.
    p.add_argument('--tune', default=None, metavar='kv=0.3,ki=0.6')
    opt = p.parse_args(config.get('EgoControllerArgs') or [])

    base = over.get('tuning') or Tuning()
    if not isinstance(base, Tuning):
        raise TypeError('fixs.driver(tuning=): expected a Tuning, got %s'
                        % type(base).__name__)
    opt.tuning = Tuning.parse(opt.tune, base) if opt.tune else base

    #: Policy, and the stiffened Ackermann gains: code, not the command line.
    #: A margin changed from a yaml line is a change nobody reviews.
    opt.limits = over.get('limits') or Limits()
    if not isinstance(opt.limits, Limits):
        raise TypeError('fixs.driver(limits=): expected a Limits, got %s'
                        % type(opt.limits).__name__)
    opt.idealSpeedTracking = bool(
        over.get('idealSpeedTracking', IDEAL_SPEED_TRACKING))
    opt.startInGear = bool(over.get('startInGear', False))
    opt.feedStep = float(over.get('feedStep', FEED_STEP))
    return opt


class Controller:
    """__init__ once, control() every CARLA step."""

    #: Both set by :func:`driver`. None means "ask the scenario".
    _EXCHANGE = None
    #: True for an exchange(vref, aref, dt), False for exchange(vref, dt).
    _EXCHANGE_TAKES_AREF = False
    _OPTIONS = None
    _USERCONTROL = None

    def __init__(self, config, egoId):
        opt = _options(config, getattr(self, '_OPTIONS', None))
        self.shape = opt.command_shape
        self.loop = opt.pedal_loop
        #: Every gain in force, as one value -- and written into the log, so a
        #: run can say what it was without anyone regressing it out of the data.
        self.tuning = opt.tuning
        #: How close it comes to a bar or a leader. Policy, not tuning.
        self.limits = opt.limits
        self.idealSpeedTracking = opt.idealSpeedTracking
        #: Put the ego in first gear on the first step: CARLA's autobox leaves a
        #: spawned car in neutral below ~15% throttle, so a creeping pedal never moves it.
        self.startInGear = opt.startInGear
        self._inGear = False
        self.referenceMode = opt.reference_mode
        self.feedStep = opt.feedStep
        #: This feed's speed and acceleration commands. None until a feed is read.
        self.feedVref, self.feedAref = None, 0.0
        self.dt = float(config.get('CarlaTimeStep') or 0.1)
        self.fallbackSpeed = float(config.get('EgoTargetSpeed') or 8.33)
        self.useAdvisory = USE_ADVISORY
        self.agent = None
        #: TELEPORT. EgoSetup.Dynamics: traffic -- the traffic simulator
        #: integrates the ego and no CARLA actor is spawned, so there is nothing
        #: for an agent to drive. Asked of the SCENARIO, not of whether
        #: carla.ego exists: on a virenv rung with a deferred spawn there is no
        #: actor either for the first few hundred ticks, and 'not yet' must not
        #: read as 'never'.
        from CommonLib.VirEnv.EgoControllerHost import currentDynamics
        self.passive = currentDynamics() == 'traffic'
        self.log = _openLog(
            config.get('EgoControllerLog', '_datalog/agent_embedded.csv'),
            'fixs.driver shape=%s loop=%s reference=%s %s %s exchange=%s'
            % (self.shape, self.loop, self.referenceMode, self.tuning, self.limits,
               getattr(self._EXCHANGE, '__name__', 'none')))
        self.steps, self.elapsed = 0, 0.0
        #: Held between feeds -- see _advisoryOf.
        self.advisory = None
        self.vSignal = self.vLeader = _NO_LIMIT
        #: Who answers "you asked for this speed -- what did you reach?".
        #: Whatever was handed to fixs.driver(), and nothing else: this module
        #: never goes looking for a cell. Deciding one is in the loop is the
        #: caller's, so passing a function IS the declaration and there is no
        #: flag here to disagree with it. A wrapper around fixs.xil.dynosim() is
        #: one such function, for the simulated cell; a rig's own is another.
        self._exchange = self._EXCHANGE
        self.benchInLoop = self._exchange is not None
        #: Ticks the cell did not answer usefully. Counted here, not asked of
        #: the cell: a plain function cannot be expected to carry state FIXS
        #: wants to read back.
        self.misses = 0
        #: What the bench last achieved. None until it has answered once.
        self.vDyno = None

        # Map and route planner are built at start-up, not on the first
        # controlled tick (FIXS#373). Skipped without a bridge (unit tests) and
        # on a passive rung, which has no agent to use them.
        self._map, self._grp = None, None
        if not self.passive and carla.available():
            self._map = carla.map      # FIXS caches it; the agent reuses it
            #: 2.0 m is BasicAgent's own _sampling_resolution. Passing the
            #: planner in only skips rebuilding it; it is not a different one.
            self._grp = GlobalRoutePlanner(self._map, 2.0)
            if ROUTE_SOURCE == 'fixs':
                # The plan is EgoRoutePoints snapped to the map -- neither waits
                # for the ego. Verified: seeded at the ego's z against the no-ego
                # fallback, the plans agree on road and lane at 2560 of 2560.
                carla.precompute_route()
        #: State of the speed-to-pedal law: the outer integral, the pedal
        #: itself (which holds the resistance trim), and the filtered
        #: acceleration the inner stage closes on.
        self._dbg = None
        self.speedInteg = 0.0
        self.vRef = None
        self.pedal = 0.0
        self._vPrev = 0.0
        self._aMeas = 0.0
        if self.benchInLoop:
            print('[driver] exchange in the loop (%s)'
                  % getattr(self._EXCHANGE, '__name__', 'exchange'), flush=True)

    def exchange(self, vRef):
        """(mps) -> mps -- what the dyno did with the speed we asked for.

        A 3-argument exchange also gets the acceleration that goes with vRef.

        An exchange that cannot answer this tick returns None, and the
        REFERENCE goes through untouched: it is the only value that cannot
        invent motion, so the run behaves as though nothing were attached.
        Those ticks are counted, because a run that ends with many of them did
        not test what it claims to have tested.
        """
        if self._EXCHANGE_TAKES_AREF:
            reached = self._exchange(vRef, self.arefFor(vRef), self.dt)
        else:
            reached = self._exchange(vRef, self.dt)
        if reached is None:
            self.misses += 1
            return vRef
        self.vDyno = float(reached)
        return self.vDyno

    def _readFeed(self, ego):
        """The feed's vref, per referenceMode. Keeps vref and aref for arefFor; the one not read is rebuilt."""
        v = getattr(ego, 'feedSpeed', None)
        v = float(ego.speed if v is None else v)
        declared = _fixs._declaredFields
        if (self.referenceMode != 'speed' and declared is not None
                and 'accelerationDesired' not in declared):
            raise FixsError("referenceMode=%r reads accelerationDesired, which is not in "
                            "VehicleMessageField" % self.referenceMode)
        if self.referenceMode == 'accel':
            aref = float(ego.accelerationDesired)
            vref = speedFromAcceleration(v, aref, self.feedStep)
        else:
            vref = float(getattr(ego, 'speedDesired', 0.0) or 0.0)
            aref = (vref - v) / self.feedStep
            if self.referenceMode == 'both':
                sent = float(ego.accelerationDesired)
                # A field the upstream controller left unset holds TrafficLayer's default, so the pair disagrees.
                if abs(sent - aref) > BOTH_TOLERANCE:
                    raise FixsError(
                        "referenceMode='both': accelerationDesired %.4f disagrees with "
                        "(speedDesired - feedSpeed) / feedStep = %.4f m/s^2. Does the upstream "
                        "controller send both, with a %.3f s step?" % (sent, aref, self.feedStep))
                aref = sent
        self.feedVref, self.feedAref = vref, aref
        return vref

    def arefFor(self, vRef):
        """The acceleration that goes with vRef: the feed's, plus whatever this driver changed the speed by."""
        if self.feedVref is None:
            return 0.0
        return self.feedAref + (vRef - self.feedVref) / self.feedStep

    def _build(self):
        """Built on the first controlled tick, not in __init__: the traffic
        simulator inserts the ego, so `carla.ego` does not exist until the
        bridge adopts it."""
        # BehaviorAgent, not BasicAgent: BasicAgent has no longitudinal
        # following model at all -- it applies full brake inside a threshold
        # and nothing else. car_following_manager bands on time-to-collision.
        # Measurements for both in ORNL-Real-Sim/FIXS#305.
        #
        # The PID dicts are passed in full, not as opt_dict={'dt': ...}:
        # LocalPlanner builds its gain dicts BEFORE it reads opt_dict, so 'dt'
        # alone is a no-op and both loops silently run at CARLA's 20 Hz
        # default. Gains are CARLA's own; only the step changes.
        gains = {'lateral_control_dict':
                 {'K_P': 1.95, 'K_I': 0.05, 'K_D': 0.2, 'dt': self.dt},
                 'longitudinal_control_dict':
                 {'K_P': 1.0, 'K_I': 0.05, 'K_D': 0.0, 'dt': self.dt}}
        # map_inst / grp_inst are CARLA's own escape hatches for exactly this:
        # its constructor calls them 'the expensive call of getting it'.
        self.agent = BehaviorAgent(carla.ego, behavior='normal', opt_dict=gains,
                                   map_inst=self._map, grp_inst=self._grp)
        # bind swaps what the agent READS -- the world view either way; the
        # route only when this controller wants the scenario's corridor.
        carla.bind(self.agent, route=(ROUTE_SOURCE == 'fixs'))
        if ROUTE_SOURCE == 'stock':
            import random
            self.agent.set_destination(
                random.choice(carla.map.get_spawn_points()).location)
        if self.idealSpeedTracking:
            st = carla.ego.get_ackermann_controller_settings()
            st.speed_kp, st.speed_ki, st.speed_kd = 50.0, 5.0, 0.0
            carla.ego.apply_ackermann_controller_settings(st)
        # Every run_step branch takes min(max_speed, speed_limit -
        # speed_lim_dist). The eco advisory arrives as max_speed each tick, so
        # the standard margin below the limit goes: a target is not a ceiling.
        self.agent._behavior.speed_lim_dist = 0.0
        # -1 disables the tailgating lane change, the way CARLA's own Aggressive
        # profile does. It must never fire here: _tailgating answers a car
        # closing from behind by calling set_destination, which would replace the
        # traffic simulator's route with one the rest of the simulation does not
        # know about. It has never fired in a measured run, but only because its
        # detection range is the same small constant widened below.
        self.agent._behavior.tailgate_counter = -1
        if SIGNAL_SOURCE == 'fixs':
            # Off, not overridden. Left on, a run carries two signal opinions --
            # CARLA's geometric one and the wire's -- and no log can say which
            # of them braked.
            self.agent.ignore_traffic_lights(True)

    def control(self, ego, dt):
        if dt > 0:
            self.dt = dt
        self.elapsed += dt

        # YOUR driving, if you handed some to fixs.driver(). It is called with
        # the same (ego, dt) FIXS calls us with, and writes its command the
        # same way -- ego.set(...). Nothing of ours runs: no ceilings, no
        # pedal law, no agent. You wanted your logic, so you get your logic.
        if self._USERCONTROL is not None:
            self._USERCONTROL(ego, dt)
            self.steps += 1
            return

        if self.passive:
            self._controlPassive(ego)
            return

        if self.startInGear and not self._inGear:
            # No pedal command this step, so the host does not overwrite the gear.
            carla.ego.apply_control(carla.VehicleControl(manual_gear_shift=True, gear=1))
            self._inGear = True
            self.steps += 1
            return

        if self.agent is None:
            self._build()

        advisory = self._advisoryOf(ego)
        wanted = advisory if advisory is not None else self.fallbackSpeed

        # EVERY CONSTRAINT IS A SPEED, so combining them is a min() and there is
        # no arbitration left to get wrong. Each is an envelope: the fastest this
        # controller may go and still stop comfortably for the thing it names.
        # The agent's own following model and obstacle sweep still run beneath
        # this ceiling -- they see what the wire cannot.
        self.vSignal = self._signalCeiling(ego)
        self.vLeader = self._leaderCeiling(ego)
        target = max(0.0, min(wanted, self.vSignal, self.vLeader))
        self.agent._behavior.max_speed = target * 3.6

        # The agent's OWN obstacle sweep reaches a constant too:
        # collision_and_car_avoid_manager takes max(min_proximity_threshold,
        # speed_limit/3), which is about 13 m here, centre to centre. Measured on
        # this corridor, a hazard entered that sweep at 5.6 m while the ego held
        # 10.1 m/s and contact followed 0.05 s later; a full-brake stop from
        # 10.5 m/s takes 9.8-11.0 m. Give the sweep the same stopping distance
        # the envelopes use, so the agent's own following model is engaged while
        # there is still room for it to act. It still decides.
        self.agent._behavior.min_proximity_threshold = max(
            10.0, ego.speed * ego.speed / (2.0 * self.limits.comfortDecel)
            + self.limits.leaderMargin)

        # NOT a ceiling from the bench. Capping the agent's target with the
        # bench's MEASURED speed latches: the bench starts at zero, so the
        # ceiling is zero, so the reference handed to the bench is zero, and it
        # never moves. Measured -- the ego sat at the spawn point for the whole
        # 800 s. The agent plans against the signal and the leader; the bench's
        # speed is what the pedal law chases, not a limit on intent.

        carla.refresh(ego)
        control = self.agent.run_step()
        if _isEmergencyStop(control, self.agent._max_brake):
            # BehaviorAgent.run_step RETURNS from emergency_stop before the local
            # planner runs, so the plan does not advance while the ego brakes --
            # and the ego is still moving. Measured on this corridor: a stop
            # beginning at 7.4 m/s carried the ego over the head of its own
            # waypoint queue (0.21 m at the closest) and 8.5 m past it. The
            # planner purges by distance, so a waypoint 8.5 m BEHIND is never
            # purged; when the brake lifted, the lateral controller went to full
            # lock to go back for it and drove the ego in a circle 9.8 m off the
            # corridor, into another vehicle.
            #
            # Running the planner and discarding its command restores exactly
            # what the early return skipped -- the purge, on CARLA's own rule --
            # and invents no geometry of its own.
            self.agent.get_local_planner().run_step()

        # THE COMMAND. A FIXS controller commands by writing to the ego record;
        # control()'s return value is ignored. steerAngleDesired is an ANGLE in
        # radians where the agent's steer is normalised [-1, 1], and the plant
        # divides by the same constant.
        vRef = self._reference(control)
        #: What the loop was actually told to chase. lp._target_speed is
        #: NOT this: it goes stale at the route limit whenever the agent
        #: panic-brakes, so a figure drawn from it shows the ego being
        #: asked for 53 km/h at the exact moment it was asked for zero.
        self.vRef = vRef

        # THE BENCH, when there is one. The agent has decided a speed; the bench
        # says what a vehicle with mass and a torque delay actually reaches, and
        # THAT is what CARLA is told to hold. Next tick the agent reads back
        # ego.speed, so the bench is in the loop rather than beside it.
        if self.benchInLoop:
            vRef = self.vRef = self.exchange(vRef)

        # THE COMMAND, in CARLA's own two shapes. fixs.carla relays them onto
        # the ego record and converts the normalised steer to the wire's radians
        # in the one place that conversion belongs.
        if self.shape == 'speed':
            carla.apply_ackermann_control(carla.VehicleAckermannControl(
                speed=max(0.0, vRef), steer=control.steer))
        else:
            # The SHAPE decides who closes the speed loop; the bench only
            # decides what vRef is. Gating this on the bench made 'pedals'
            # silently fall through to the agent's own pedals whenever
            # EnableXil was off, so the law under test never ran.
            thr, brk = self._speedToPedal(vRef, ego.speed)
            carla.apply_control(carla.VehicleControl(
                throttle=thr, brake=brk, steer=control.steer))
        self._logStep(ego, control, target, advisory)

    def _controlPassive(self, ego):
        """TELEPORT: the cell is the whole driver.

        No CARLA actor, so no agent, no steering, no obstacle sweep -- the
        traffic simulator owns all of that. What is left is the longitudinal
        intent, which is exactly what a bench answers: the advisory comes off
        the wire, the cell says what a vehicle with mass and a torque delay
        actually reached, and that speed is commanded back for the traffic
        simulator to integrate.

        THE CEILINGS DO NOT APPLY HERE, and that is the whole of the difference
        from the virenv path.

        There, the advisory is a TARGET a CARLA agent tracks, and _signalCeiling
        and _leaderCeiling are that agent's envelope -- it plans in a world the
        wire cannot see, so it is given room to be wrong in. Here the advisory
        is nobody's target: the eco controller writes speedDesired and the
        traffic simulator integrates it. That IS the rung below. Applying the
        envelopes on top double-counts a stop the eco controller has already
        planned -- _signalCeiling alone commands a 2 m/s^2 deceleration to the
        bar from as far as 200 m out -- and the ego slows long before a bar it
        was never going to overrun.

        Nor is safety lost by dropping them. SumoSetup.SpeedMode leaves the
        traffic simulator's own checks on, so what is commanded here is still
        clamped to its safe speed, its leader gap and its red lights. There are
        not two opinions about the stop bar on this rung; there is one, and it
        is not this file's.

        The invariant that buys: with no cell -- or a cell that returns its
        reference unchanged -- this rung reproduces the rung below it EXACTLY.
        Anything else and the cell is not the only thing being measured.
        """
        # The eco controller's command, as received, and nothing else done to
        # it. Not _advisoryOf: that drops anything <= 0.01 as 'no advisory', and
        # a command of ZERO is exactly what the eco controller writes AT a stop
        # bar -- read as absent, it was replaced by fallbackSpeed and the ego
        # was told to accelerate away from the bar it had just been asked to
        # stop at. There is no fallback on this rung either: nothing here is
        # entitled to invent a speed the eco controller did not ask for.
        #
        # Safe to read the field directly: it is dual-use, but runController
        # restores the feed's value before every call, so what is here is the
        # eco controller's, never this driver's own last command (FIXS#305).
        advisory = self._readFeed(ego)
        self.advisory = advisory
        #: Neither computed nor applied. Logged as such, so a passive trace
        #: cannot be read as though an envelope had bound.
        self.vSignal = self.vLeader = _NO_LIMIT
        target = advisory

        vRef = self.vRef = target
        if self.benchInLoop:
            vRef = self.vRef = self.exchange(vRef)

        # THE COMMAND. speedDesired only: steer belongs to whoever owns the
        # lateral, and here that is the traffic simulator.
        #
        # The command SHAPE is not consulted and does not need to be. It picks
        # who closes the speed loop against a plant; there is no plant on this
        # rung, so both shapes arrive at the same place -- the speed the traffic
        # simulator is asked to hold. A scenario therefore carries the same
        # Controller line as its virenv sibling, and the pair differs by
        # Dynamics alone.
        ego.set(speedDesired=max(0.0, vRef))
        self._logStep(ego, None, target, advisory)

    def _advisoryOf(self, ego):
        """The eco controller's advisory, read only on the tick it is new.

        `speedDesired` is one field used in both directions: the eco controller
        writes its advisory into it on the 0.1 s feed, and THIS controller writes
        its command into it every 0.05 s step. Read unconditionally, the second
        half of every feed therefore reads back the command written half a step
        earlier -- measured on a 300 s run, 1299 of 2554 ticks had `speedDesired`
        equal to the previous tick's own target to within 1e-3, which is every
        sub-tick. `feedAge` is 0 exactly on the tick a feed lands, so that is when
        the field still holds the advisory; between feeds the last one stands.
        """
        if float(getattr(ego, 'feedAge', 0.0) or 0.0) <= 1e-9:
            v = self._readFeed(ego)
            self.advisory = v if v > 0.01 else None
        if not self.useAdvisory:
            return None
        return self.advisory

    def _signalCeiling(self, ego):
        """The speed the next signal allows.

        The traffic simulator answers for the movement THIS ego will make -- its
        current lane and its next route edge pick the head, so no geometry is
        guessed -- and it answers from as far as 200 m. A red is therefore a
        smooth deceleration to the bar rather than a brake that arrives too late
        to be one.

        A yellow counts as a red unless the ego is already inside the distance
        it needs to stop at COMMIT_DECEL -- there, braking puts it in the
        junction anyway and slower, which is worse than clearing it.
        """
        if SIGNAL_SOURCE != 'fixs':
            return _NO_LIMIT
        colour = int(getattr(ego, 'signalLightColor', 0) or 0)
        dist = float(getattr(ego, 'signalLightDistance', -1.0) or -1.0)
        if dist < 0.0 or colour not in (_RED, _YELLOW, _RED_YELLOW):
            return _NO_LIMIT
        v = float(getattr(ego, 'speed', 0.0) or 0.0)
        lim = self.limits
        if colour == _YELLOW and dist < v * v / (2.0 * lim.commitDecel):
            return _NO_LIMIT
        return _stopBy(dist - lim.stopMargin, lim.comfortDecel)

    def _leaderCeiling(self, ego):
        """The speed the vehicle ahead allows.

        This does NOT replace the agent's own obstacle sweep. The wire's leader
        is the vehicle on the ego's lane along its route, which is not always the
        one it is about to hit -- measured once at a lap seam, the wire reported
        its leader 175 m away while the agent's sweep had the real obstacle at
        5.6 m. Two sources, both kept: this one sees far, the sweep sees sideways.
        """
        gap = float(getattr(ego, 'precedingVehicleDistance', _NO_LEADER)
                    or _NO_LEADER)
        if abs(gap - _NO_LEADER) < 1e-6:
            return _NO_LIMIT
        if gap < 0.0:
            # OVERLAPPING a vehicle, not an empty road. Measured at a green on
            # this corridor: the car queued in the lane to the right changed into
            # the ego's lane while the ego stood at the bar, landing with its rear
            # bumper about 3 m behind the ego's nose, and the wire reported
            # -6.66 m. Reading that as "nothing ahead" let the fallback speed
            # through, the ego was commanded into a body it was already inside,
            # and CARLA -- where a mirrored vehicle is physics-off and immovable
            # -- ejected it at 584 m/s^2 into a 34 m fall off the map.
            return 0.0
        lead = max(0.0, float(getattr(ego, 'precedingVehicleSpeed', 0.0) or 0.0))
        return math.sqrt(lead * lead
                         + 2.0 * self.limits.comfortDecel
                         * max(0.0, gap - self.limits.leaderMargin))

    def _speedToPedal(self, vTarget, vEgo):
        """(float, float) -> (throttle, brake) that puts CARLA on vTarget.

        PEDAL_LOOP picks which error the loop closes on. Either way one
        integrator carries the road-load trim, so a zero speed error still
        holds a non-zero pedal.
        """
        # Measured acceleration, lightly filtered. It is differenced speed, so
        # unfiltered it is mostly quantisation noise; CARLA filters the same
        # signal at 4/5, which costs it 0.2 s of phase. 0.1 s here.
        g = self.tuning
        a = (vEgo - self._vPrev) / self.dt if self.dt > 0 else 0.0
        self._vPrev = vEgo
        alpha = self.dt / (0.1 + self.dt)
        self._aMeas += alpha * (a - self._aMeas)

        # FULL STOP, and no state carried out of it -- a loop left running at
        # a red light acquires a demand the stopped car cannot answer. CARLA
        # bypasses its own loops on the same condition (RunControlFullStop).
        if abs(vTarget) < g.fullStop and abs(vEgo) < g.fullStop:
            self.speedInteg = 0.0
            self.pedal = -1.0
            self._dbg = (0.0, 0.0, self._aMeas, self.pedal)
            return (0.0, 1.0)

        error = vTarget - vEgo
        if self.loop == 'speed':
            # The integral is the trim, in pedal units. Held off when it would
            # push further into a stop it is already against -- an integral
            # that cannot shed what it collects is what seized the old law.
            saturated = not -1.0 < self.pedal < 1.0
            if not (saturated and error * self.pedal > 0):
                self.speedInteg += g.ki * error * self.dt
            self.speedInteg = max(-1.0, min(1.0, self.speedInteg))
            demand = float('nan')
            self.pedal = max(-1.0, min(1.0, g.kv * error + self.speedInteg))
        else:
            demand = g.kp * error
            demand = max(-g.maxAccel, min(g.maxAccel, demand))
            self.pedal = max(-1.0, min(1.0, self.pedal
                                       + g.k * (demand - self._aMeas)))
        # The loop's own signals, for the log. Without them a pedal run can
        # only be diagnosed by inference from speed.
        self._dbg = (error, demand, self._aMeas, self.pedal)
        return (max(0.0, self.pedal), max(0.0, -self.pedal))

    def _reference(self, control):
        """The speed the agent is asking for, in m/s.

        The agent's own longitudinal target, except when it panic-brakes: an
        emergency stop leaves that target untouched, so reading it alone would
        command the ego to keep driving through the thing the agent stopped for.
        A full-brake command IS the target speed being zero.
        """
        if _isEmergencyStop(control, self.agent._max_brake):
            return 0.0
        return max(0.0, self.agent.get_local_planner()._target_speed / 3.6)

    def shutdown(self):
        if self.log is not None:
            self.log.close()
            self.log = None
        if self.benchInLoop:
            # A miss is a tick the cell did not answer, and the reference went
            # through untouched. Say how many, because a run with many of them
            # did not test what it claims to have tested.
            print('[driver] exchange: %d of %d steps unanswered'
                  % (self.misses, self.steps), flush=True)
        print('[driver] %d control steps' % self.steps, flush=True)

    def _logStep(self, ego, cmd, target, advisory):
        """This application's instrumentation. It records what FIXS supplied
        beside what the agent concluded -- vehicles seen, the wire's limit, the
        speed the agent settled on -- because two defects that stopped the ego
        dead were invisible without those three and obvious with them
        (ORNL-Real-Sim/FIXS#355)."""
        self.steps += 1
        if self.log is None:
            return
        # Agent-side columns are blank on a passive run: there is no planner,
        # no CARLA command and no world to count actors in. The columns stay so
        # the two rungs' logs load with one reader.
        lp = None if self.agent is None else self.agent.get_local_planner()
        thr, brk, ste = ((0.0, 0.0, 0.0) if cmd is None
                         else (cmd.throttle, cmd.brake, cmd.steer))
        wpLeft = 0 if lp is None else len(lp.get_plan())
        nSeen = (0 if self.agent is None
                 else len(self.agent._world.get_actors().filter('*vehicle*')))
        agentTarget = 0.0 if lp is None else lp._target_speed
        self.log.write(
            '%.3f,%.3f,%.3f,%.3f,%.3f,%s,%.3f,%.4f,%.4f,%.4f,%d,%s,%s,%s,'
            '%d,%.3f,%.3f,%s,%s,%s'
            % (self.elapsed, getattr(ego, 'feedAge', 0.0), ego.positionX,
               ego.positionY, ego.speed,
               '' if advisory is None else '%.3f' % advisory, target,
               thr, brk, ste, wpLeft,
               ego.precedingVehicleDistance, ego.signalLightColor,
               ego.signalLightDistance,
               nSeen,
               float(getattr(ego, 'speedLimit', 0.0) or 0.0), agentTarget,
               _col(self.vSignal), _col(self.vLeader),
               '' if self.vDyno is None else '%.4f' % self.vDyno)
            + (',' if self.vRef is None else ',%.4f' % self.vRef)
            + (',,,,\n' if self._dbg is None else
               ',%.4f,%.4f,%.4f,%.4f\n' % self._dbg))


def _takesAref(exchange):
    """True for exchange(vref, aref, dt), False for exchange(vref, dt); anything else is refused."""
    params = inspect.signature(exchange).parameters.values()
    n = sum(1 for p in params if p.kind in (p.POSITIONAL_ONLY, p.POSITIONAL_OR_KEYWORD)
            and p.default is p.empty)
    if n not in (2, 3):
        raise TypeError('fixs.driver(exchange): takes (vref, dt) or (vref, aref, dt), '
                        'got %d required arguments' % n)
    return n == 3


def driver(exchange=None, usercontrol=None, **options):
    """(callable) -> class -- the controller the scenario should name.

    ``exchange(vref, dt) -> mps`` is yours: a speed goes in, the speed your
    dyno reached comes back. ``exchange(vref, aref, dt)`` also gets the
    acceleration that goes with vref. Return None on a tick it could not answer and the
    reference passes through untouched; those are counted and reported.

    Omit it and nothing is in the loop. For the simulated cell, pass
    ``fixs.xil.dynosim().exchange``; for yours, pass yours::

        Controller = fixs.driver(exchange)

    Returns a CLASS, and ALSO records it here, so the name you give it is
    yours::

        Driver = fixs.driver(exchange)      # any name
        fixs.driver(exchange)               # or none at all

    The loader asks :func:`built` before it scans the module for a name. The
    scenario still says which FILE -- FIXS has nothing to import otherwise --
    but nothing inside it has to be spelled a particular way.

    ``options`` override the module defaults and anything given to
    ``fixs.driver.set()`` before this call; the scenario's own
    ``--command-shape`` still wins over all of them, being nearer the run.
    """
    for name, fn in (('exchange', exchange), ('usercontrol', usercontrol)):
        if fn is not None and not callable(fn):
            raise TypeError('fixs.driver(%s): %r is not callable' % (name, fn))
    if usercontrol is not None and exchange is not None:
        # Your control() decides when to ask a cell and what to do with the
        # answer. Taking an exchange too would mean we called it somewhere
        # inside logic we are not running.
        raise TypeError(
            'fixs.driver(): usercontrol replaces the driving, so an exchange '
            'here would never be called -- ask your cell inside your own '
            'control(ego, dt).')
    cls = type('Controller', (Controller,),
               dict(_EXCHANGE=staticmethod(exchange) if exchange else None,
                    _EXCHANGE_TAKES_AREF=_takesAref(exchange) if exchange else False,
                    _USERCONTROL=staticmethod(usercontrol) if usercontrol
                    else None,
                    _OPTIONS=dict(_DEFAULTS, **options)))
    _BUILT.append(cls)
    return cls


#: Every class :func:`driver` has built. The loader clears this before it
#: imports a controller file and reads it after, so "how many" is asked of one
#: FILE rather than of the process -- a test, or a second controller, may build
#: as many as it likes.
_BUILT = []


def builtSince(mark):
    """The drivers built since `mark`, which is what :func:`mark` returned."""
    return _BUILT[mark:]


def mark():
    """Where the list stands now. Take one before importing a controller."""
    return len(_BUILT)


def _isEmergencyStop(control, maxBrake):
    """Is this command BehaviorAgent's emergency_stop rather than a planned one?

    It matters twice: an emergency stop IS a target speed of zero, and it is also
    the branch that returns before the local planner runs.
    """
    return control.throttle <= 1e-6 and control.brake >= maxBrake - 1e-6


def _stopBy(metres, decel):
    """The fastest we may go and still stop in `metres` at `decel`."""
    return math.sqrt(2.0 * decel * max(0.0, metres))


def _col(v):
    """A ceiling for the csv: blank when it binds nothing."""
    return '' if v >= _NO_LIMIT else '%.3f' % v


def _openLog(path, provenance=''):
    """Open the per-step csv, with a comment line saying what drove the run.

    The header is not decoration. A batch of 800 s runs once reported gains it
    had not used, and recovering the truth meant regressing them back out of
    pidDemand/pidError. A run has to be able to say what it was.
    """
    if not path:
        return None
    import os
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
    f = open(path, 'w', encoding='utf-8', buffering=1)
    if provenance:
        f.write('# %s' % provenance + chr(10))
    f.write('t,feedAge,x,y,speed,advisory,target,throttle,brake,steer,'
            'wpLeft,leaderGap,signalColor,signalDist,nSeen,'
            'wireLimit,agentTarget,vSignal,vLeader,vDyno,vRef,'
            'pidError,pidDemand,pidAMeas,pidPedal\n')
    return f
