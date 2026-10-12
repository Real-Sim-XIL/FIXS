"""The robot driver on the bench (#323).

A bench does not drive itself. Somebody works the pedal: on a real emissions cell
that is a driver robot chasing a speed trace, and here it is this.

The integral term is not optional. Under road resistance a steady speed needs a
steady pedal, and a proportional-only law can only produce one from a standing
error -- so it sits permanently below the reference by however much error makes
the pedal it needs. Measured on the deployed rig's gains: 0.44 m/s slow and 40 m
behind over 90 s.

A PI is a placeholder and will flatter a real vehicle's controller, which has
rate limits, an acceleration envelope and regen blending that this does not. That
matters when the bench is asked to PREDICT hardware, not when it is used to build
a coupling.
"""

from collections import deque


class RobotDriver(object):
    """PI on speed error (``mode='speed'``) or on acceleration error
    (``mode='accel'``), producing one pedal split into throttle and brake.

    One axis, not two: positive is throttle, negative is brake, and they are
    never both non-zero. That is what a driver does and what a VehicleControl
    expects.
    """

    def __init__(self, kp=0.45, ki=0.25, max_throttle=1.0, max_brake=1.0,
                 standstill_ref_mps=0.05, standstill_brake=0.3,
                 max_accel_mps2=None, max_decel_mps2=None,
                 stop_speed_mps=0.0, stop_accel_mps2=0.0, delay_s=0.0,
                 actuator=None, mode='speed', kp_accel=0.05, ki_accel=0.5,
                 accel_filter_s=0.1):
        if mode not in ('speed', 'accel'):
            raise ValueError("mode must be 'speed' or 'accel', got %r" % (mode,))
        #: 'speed' chases v_ref; 'accel' chases a_ref and ignores v_ref.
        self.mode = mode
        self.kp = kp
        self.ki = ki
        self.max_throttle = max_throttle
        self.max_brake = max_brake
        #: THE ENVELOPE, in the units an envelope is actually quoted in.
        #:
        #: A pedal cap is the wrong instrument for this. It bounds TORQUE, and
        #: the acceleration that buys falls away with speed as road load and
        #: the power limit take their share -- so a cap chosen to give 2 m/s^2
        #: off the line gives less further up, and the envelope is exact only
        #: at the one point it was measured.
        #:
        #: Capping the reference slew states the envelope directly, holds it at
        #: any speed, and leaves the vehicle its full torque to track within
        #: that envelope -- which is the honest arrangement, because the car on
        #: the rollers really does have that torque.
        #:
        #: None is no envelope, the bench's own limits, and the default. Set
        #: them when something else also has an opinion about what the ego can
        #: do -- a traffic simulator's vType, say -- and the two must describe
        #: the same vehicle.
        self.max_accel_mps2 = max_accel_mps2
        self.max_decel_mps2 = max_decel_mps2
        self._ramp = None
        self.standstill_ref_mps = standstill_ref_mps
        self.standstill_brake = standstill_brake
        #: openpilot's hold: brake while slower than stop_speed_mps and asked for
        #: less than stop_accel_mps2 (openpilot: 0.3 and 0.1). 0.0 is off.
        self.stop_speed_mps = stop_speed_mps
        self.stop_accel_mps2 = stop_accel_mps2
        #: Transport delay between the robot's command and the pedal, in s; 0.0 is
        #: off. Held per step, so it resolves to whole steps of the caller's dt.
        self.delay_s = delay_s
        #: Your pedal dynamics, after the delay: actuator(throttle, brake, dt)
        #: -> (throttle, brake). Its reset(), if it has one, runs with ours.
        self.actuator = actuator
        #: Accel mode: PI gains on a_ref - a_meas, no feedforward.
        self.kp_accel = kp_accel
        self.ki_accel = ki_accel
        #: Time constant of the first-order filter on dv/dt that gives a_meas.
        self.accel_filter_s = accel_filter_s
        self.a_meas = 0.0
        self._v_prev = None
        self.integral = 0.0
        self.pedal = 0.0
        self._clock = 0.0
        self._sent = deque()
        self._applied = (0.0, 0.0)

    #: How far the ramp may sit from the measured speed. Anti-windup for the
    #: setpoint itself: without it, a ramp advancing past a vehicle that cannot
    #: keep up accumulates a lead it later spends all at once.
    _WINDUP_BAND = 0.2

    def reset(self):
        self.integral = 0.0
        self.pedal = 0.0
        self._ramp = None
        self._clock = 0.0
        self._sent.clear()
        self._applied = (0.0, 0.0)
        self.a_meas = 0.0
        self._v_prev = None
        if hasattr(self.actuator, 'reset'):
            self.actuator.reset()

    def step(self, v_ref, v_measured, dt, a_ref=None):
        """Returns (throttle, brake), each in [0, 1], never both positive,
        unless an ``actuator`` says otherwise.

        ``a_ref`` is the acceleration command: the reference in accel mode,
        otherwise read only by the hold. With ``delay_s`` set, this is the command of ``delay_s`` ago, and
        (0, 0) until there is one; ``pedal`` stays the command just made.
        """
        if dt <= 0.0:
            raise ValueError('dt must be positive')
        command = self._command(v_ref, v_measured, dt, a_ref)
        if self.delay_s > 0.0:
            self._sent.append((self._clock, command))
            due = self._clock - self.delay_s + 1e-9
            self._clock += dt
            while self._sent and self._sent[0][0] <= due:
                self._applied = self._sent.popleft()[1]
            command = self._applied
        if self.actuator is not None:
            command = self.actuator(command[0], command[1], dt)
        return command

    def _command(self, v_ref, v_measured, dt, a_ref):
        if self.mode == 'accel' and a_ref is None:
            raise ValueError("mode 'accel' needs a_ref")
        if self._v_prev is not None:
            self.a_meas += ((v_measured - self._v_prev) / dt - self.a_meas)                 * dt / (self.accel_filter_s + dt)
        self._v_prev = v_measured

        if a_ref is not None and v_measured < self.stop_speed_mps \
                and a_ref < self.stop_accel_mps2:
            self.integral = 0.0
            self.pedal = -self.standstill_brake
            return 0.0, min(self.max_brake, self.standstill_brake)

        if self.mode == 'accel':
            return self._accel(a_ref, v_measured, dt)

        # THE ENVELOPE. A RAMPED setpoint, advanced from its own last value --
        # not the incoming reference clamped to the measured speed.
        #
        # Clamping to measured looks equivalent and is not: it parks the
        # setpoint permanently just ahead of actual, the integrator winds on
        # that standing error, and the pedal grows until the vehicle exceeds
        # the very rate the clamp was meant to impose. Measured on this cell,
        # a 2.0 clamp delivered 2.92 accel and 4.06 decel.
        #
        # Ramping gives the loop a target it can actually reach, so the error
        # stays small and the rate is the ramp's. The ramp is held within
        # _WINDUP_BAND of the measured speed so it cannot run away from a
        # vehicle that has fallen behind -- an envelope is a limit on what the
        # robot asks for, not a promise the car can deliver it.
        if self.max_accel_mps2 is not None or self.max_decel_mps2 is not None:
            if self._ramp is None:
                self._ramp = v_measured
            base = min(max(self._ramp, v_measured - self._WINDUP_BAND),
                       v_measured + self._WINDUP_BAND)
            hi = (base + self.max_accel_mps2 * dt
                  if self.max_accel_mps2 is not None else v_ref)
            lo = (base - self.max_decel_mps2 * dt
                  if self.max_decel_mps2 is not None else v_ref)
            self._ramp = min(max(v_ref, lo), hi)
            v_ref = self._ramp

        if v_ref <= self.standstill_ref_mps \
                and v_measured <= self.standstill_ref_mps:
            # Asked to stand still and standing still. Hold the brake and stop
            # integrating, or the integral winds on an error that cannot close.
            self.integral = 0.0
            self.pedal = -self.standstill_brake
            return 0.0, min(self.max_brake, self.standstill_brake)

        return self._pi(v_ref - v_measured, self.kp, self.ki, dt)

    def _accel(self, a_ref, v_measured, dt):
        if self.max_accel_mps2 is not None:
            a_ref = min(a_ref, self.max_accel_mps2)
        if self.max_decel_mps2 is not None:
            a_ref = max(a_ref, -self.max_decel_mps2)
        if a_ref <= 0.0 and v_measured <= self.standstill_ref_mps:
            self.integral = 0.0
            self.pedal = -self.standstill_brake
            return 0.0, min(self.max_brake, self.standstill_brake)
        return self._pi(a_ref - self.a_meas, self.kp_accel, self.ki_accel, dt)

    def _pi(self, err, kp, ki, dt):
        raw = kp * err + ki * self.integral
        if -self.max_brake < raw < self.max_throttle:
            self.integral += err * dt           # anti-windup: freeze at the rail
        self.pedal = max(-self.max_brake,
                         min(self.max_throttle, kp * err + ki * self.integral))
        return (self.pedal, 0.0) if self.pedal >= 0.0 else (0.0, -self.pedal)
