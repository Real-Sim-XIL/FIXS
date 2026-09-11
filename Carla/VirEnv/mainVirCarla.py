"""mainVirCarla -- the thin CARLA tick-driver over VirEnvCore.

Python peer of ``VirCarlaEnv/VirCarlaEnv/mainVirCarla.cpp``, and the entry point
of the Python bridge. It does what the C++ driver does and nothing more: bring up
the CARLA world and its tick, construct :class:`CarlaBackend` + :class:`VirEnvCore`,
and per tick call ``core.runStep``, flush the transform batch, tick the world, and
(for interested / external ids) read back POST-tick and send to FIXS.

Run it::

    python Carla/VirEnv/mainVirCarla.py -f <config.yaml> [-t <tls_table.csv>]
    python -m Carla.VirEnv.mainVirCarla  -f <config.yaml> [-t <tls_table.csv>]

Selected by ``CarlaSetup.EnablePythonBackend: true``; ``false`` runs the C++
``VirCarlaEnv.exe`` instead. Both read the SAME scenario yaml and drive the same
FIXS exchange, which is what makes a run with one directly comparable to a run
with the other.
"""

import argparse
import math
import os
import sys
import time

import carla

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..')))

from CommonLib.ConfigHelper import ConfigHelper                          # noqa: E402
from CommonLib.VehDataMsgDefs import VehData                             # noqa: E402
from CommonLib.VirEnv.DataLogger import DataLogger                       # noqa: E402
from CommonLib.VirEnv.EgoDriver import EgoDriver                         # noqa: E402
from CommonLib.VirEnv.FixsProtocol import kFeedPeriodS, onFeedBoundary   # noqa: E402
from CommonLib.VirEnv.EgoControllerHost import (                        # noqa: E402
    loadController, runController)
from CommonLib.VirEnv.IVirEnvBackend import EgoState, Pose, kNoHandle    # noqa: E402
from CommonLib.VirEnv.VirEnvCore import VirEnvCore                       # noqa: E402

if __package__:
    from .BridgeHelper import BridgeHelper
    from .CarlaBackend import CarlaBackend
else:
    # Run as a plain script (python Carla/VirEnv/mainVirCarla.py). Put the PARENT
    # of this package on sys.path and import through the package, rather than
    # importing the siblings flat: CarlaBackend imports BridgeHelper relatively,
    # and a flat import gives it no parent package to resolve that against.
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from VirEnv.BridgeHelper import BridgeHelper
    from VirEnv.CarlaBackend import CarlaBackend

#: Must match the EgoDriver client's DriveCommand steer scaling.
kMaxSteerRad = 0.7


def resolveCadence(cs):
    """(dict) -> (feed, carlaStep, poseRefresh) -- three distinct things, one each.

    ``feed`` is ``fixs.kFeedPeriodS``: the FIXS exchange period, which is also the
    traffic simulator's step, since TrafficLayer steps it once per exchange. A
    protocol constant, not a knob.

    ``carlaStep`` is ``CarlaSetup.CarlaTimeStep``: the CARLA world step
    (``fixed_delta_seconds``), the analogue of CarMaker's solver dt. Absent or 0
    means the feed -- tick 1:1 and do not interpolate.

    ``poseRefresh`` is ``CarlaSetup.TrafficRefreshRate``: how often the core
    re-applies (interpolated) traffic poses. Exactly the meaning the key has on the
    CarMaker side: a visual / RPC-cost knob, independent of the world step. Absent
    or 0 means every tick.

    These used to be entangled -- TrafficRefreshRate doubled as the feed period AND
    as the tick when CarlaTimeStep was unset, so setting it to 0.05 silently turned
    interpolation OFF and every pose was held for two ticks. Nothing falls back to
    anything else now.

    :raises SystemExit: with the same message the C++ driver prints, if a value
        would leave the bridge unable to trade messages at all.
    """
    feed = kFeedPeriodS
    carlaStep = cs['CarlaTimeStep'] if cs['CarlaTimeStep'] > 1e-9 else feed
    poseRefresh = cs['TrafficRefreshRate'] if cs['TrafficRefreshRate'] > 1e-9 else carlaStep

    # The exchange boundary is tested on the feed grid, so the world clock has to
    # LAND on it: carlaStep must divide the feed exactly. With 0.03 the clock steps
    # 0.09 -> 0.12 and no tick is ever a feed boundary, so the bridge would trade no
    # messages at all and simply hang.
    slots = feed / carlaStep
    if carlaStep > feed + 1e-9 or abs(slots - round(slots)) > 1e-6:
        raise SystemExit(
            'CarlaSetup.CarlaTimeStep (%g s) must be the FIXS feed period (%g s) or '
            'an exact divisor of it (0.05, 0.025, 0.02, 0.01), else no Carla tick '
            'ever lands on an exchange boundary.' % (carlaStep, feed))

    # The core gates re-application with int(1.0 / poseRefresh), so a value whose
    # reciprocal is not whole silently snaps to a different grid (0.03 -> 1/33 s).
    if poseRefresh < carlaStep - 1e-9 or abs(1.0 / poseRefresh - round(1.0 / poseRefresh)) > 1e-6:
        raise SystemExit(
            'CarlaSetup.TrafficRefreshRate (%g s) must be >= CarlaTimeStep (%g s) and '
            'have a whole reciprocal (0.1, 0.05, 0.025, 0.02, 0.01). It is the pose '
            're-apply cadence, not the feed period - to tick Carla faster, set '
            'CarlaTimeStep.' % (poseRefresh, carlaStep))

    return feed, carlaStep, poseRefresh


def clearStaleActors(world, verbose):
    """Destroy vehicle actors left by a crashed run.

    This bridge owns EVERY vehicle in the world, so any pre-existing one is a
    zombie: it would block the ego spawn point or duplicate background traffic.

    NOTE: in synchronous mode the client's episode snapshot is EMPTY until a tick,
    so ``get_actors()`` would miss the zombies without the tick this does first.
    """
    world.tick()
    stale = list(world.get_actors().filter('vehicle.*'))
    for a in stale:
        a.destroy()
    if stale:
        print('Cleared %d stale vehicle actor(s) from a prior run.' % len(stale))


def applySyncSettings(world, carlaStep, verbose):
    """Force BOTH synchronous mode and the tick delta.

    The world-load script may have set sync with a different delta, and a physics
    ego (virenv dynamics) steps PhysX and the traffic manager by
    ``fixed_delta_seconds`` -- it must equal the bridge's carlaStep, or the ego's
    dynamics run on a different clock from the traffic it is driving through.
    """
    settings = world.get_settings()
    if (not settings.synchronous_mode
            or settings.fixed_delta_seconds is None
            or abs(settings.fixed_delta_seconds - carlaStep) > 1e-9):
        settings.synchronous_mode = True
        settings.fixed_delta_seconds = carlaStep
        world.apply_settings(settings)
        if verbose:
            print('Synchronous mode enabled (delta %g s).' % carlaStep)


def _spectatorTransform(tf, height, alignYaw):
    """A top-down BEV pose above ``tf``. Rigid snap -- no low-pass, no oscillation."""
    loc = carla.Location(tf.location.x, tf.location.y, tf.location.z + height)
    yaw = (tf.rotation.yaw - 90.0) if alignYaw else -90.0
    return carla.Transform(loc, carla.Rotation(-90.0, yaw, 0.0))


def main(argv=None):
    ap = argparse.ArgumentParser(
        prog='mainVirCarla',
        description='FIXS virtual-environment bridge for CARLA (Python VirEnvCore).')
    ap.add_argument('-f', '--file', dest='configPath', required=True,
                    help='scenario config.yaml (the SAME one TrafficLayer is given)')
    ap.add_argument('-t', '--tls', dest='tlsPath', default='',
                    help='traffic-light table csv (junctionId,linkId,x,y,z,heading)')
    args = ap.parse_args(argv)

    config = ConfigHelper()
    config.getConfig(args.configPath)
    cs = config.Carla_setup
    egoCfg = config.Ego_setup          # the ego, once (FIXS#305)
    verbose = cs['EnableVerboseLog']

    feed, carlaStep, poseRefresh = resolveCadence(cs)

    simEndTime = config.simulation_setup['SimulationEndTime']
    centeredViewId = cs['CenteredViewId']
    spectatorFollow = cs['EnableSpectatorFollow'] and bool(centeredViewId)
    spectatorHeight = cs['SpectatorHeight']
    spectatorAlignYaw = cs['SpectatorAlignYaw']
    realtimePacing = cs['RealtimePacing']
    enableTlsSync = True
    egoId = egoCfg['Id']

    # The ego, from the one place that describes it. Canonical values, so these
    # are string compares and nothing re-derives a mode (FIXS#305).
    virEnvOwnsEgo = egoCfg['Dynamics'] == 'virenv'
    useFixsDriver = egoCfg['ActuationSource'] == 'fixs'
    # "user" is one value; the Controller key decides where it runs. In-process
    # inherits the CARLA step rate and reads the plant, not the feed (#305).
    useEmbedded = egoCfg['ActuationSource'] == 'user' and bool(egoCfg['Controller'])
    useWireActuation = egoCfg['ActuationSource'] == 'user' and not egoCfg['Controller']

    # WHO CREATES THE EGO -- decided by whether EgoSpawnPose is configured, the
    # same predicate mainVirCarla.cpp:188 uses:
    #   given  -- Carla creates it before the loop, and TrafficLayer injects the
    #             result into the traffic simulator on a single-edge dummy route
    #   absent -- the traffic simulator creates it, on its own route with its own
    #             depart time, and we spawn a physics actor at the pose it reports
    #             the first tick its id arrives
    # The deferred direction is what keeps the ego's next traffic signal: a
    # single-edge dummy route has none.
    deferEgoSpawn = virEnvOwnsEgo and len(cs['EgoSpawnPose']) < 4

    interestedIds = set(cs['InterestedIds'] or [])

    client = carla.Client(cs['CarlaServerIP'], cs['CarlaServerPort'])
    client.set_timeout(10.0)
    print('Carla client %s / server %s'
          % (client.get_client_version(), client.get_server_version()))
    world = client.get_world()
    spectator = world.get_spectator()

    applySyncSettings(world, carlaStep, verbose)
    clearStaleActors(world, verbose)

    backend = CarlaBackend(world, client, cs['UseVehicleTypeAsBlueprint'], verbose)
    core = VirEnvCore()
    core.setBackend(backend)
    core.interpolateTraffic = (carlaStep < feed - 1e-9)   # sub-step -> interpolate
    core.sendEgoFromCore = False    # this driver owns the send (post-tick)
    core.openSignalPort = False     # Carla: vehicles + signals on ONE port
    core.ENABLE_REALSIM = cs['EnableCosimulation']
    core.SYNCHRONIZE_TRAFFIC_SIGNAL = enableTlsSync
    # EgoMode 0: Carla renders every vehicle including the traffic-sim-driven ego.
    # EgoMode >= 1: Carla OWNS the ego -- the core must never spawn or teleport the
    # traffic simulator's echo of it, which is the injected shadow of this actor.
    core.egoId_ = egoId if virEnvOwnsEgo else ''
    core.egoType_ = ''
    core.trafficLayerIP_ = cs['CarlaClientIP']
    core.vehDataPort_ = cs['CarlaClientPort']
    core.trafficRefreshRate_ = poseRefresh

    # One line that states the whole cadence, so a run never has to be reverse
    # engineered from three keys again.
    print('Cadence: FIXS feed %g s (= the traffic simulator step) | Carla tick %g s '
          '(%s) | pose refresh %g s | pacing %s'
          % (feed, carlaStep,
             ('interpolated %dx' % round(feed / carlaStep)) if core.interpolateTraffic
             else '1:1 with the feed',
             poseRefresh, 'realtime' if realtimePacing else 'as fast as possible'))

    rc, err = core.initialization(args.configPath, args.tlsPath)
    if rc < 0:
        raise SystemExit('VirEnvCore init failed: %s' % (err or '?'))
    if enableTlsSync:
        backend.freezeAndMatchTrafficLights()

    # Does this run's reply to TrafficLayer ever carry records? Only a bridge
    # that REPORTS state does. When it does not, answer BEFORE the CARLA tick so
    # SUMO and the controller are not held behind the render (#329).
    replyCarriesNothing = not virEnvOwnsEgo
    if replyCarriesNothing:
        print("Reply carries no records (the traffic simulator owns the ego): "
              "answering TrafficLayer before the CARLA tick, so SUMO and the "
              "controller are not held behind the render.")

    egoDriver = EgoDriver()
    embedded = None
    if useEmbedded:
        spec = egoCfg['Controller']
        if not spec:
            raise SystemExit("EgoSetup.ActuationSource: user needs a Controller: "
                             "<path to a .py defining control(ego, dt)>")
        embedded = loadController(spec, appRoot=os.getcwd())
        embedded.setup(cs, egoId, backend, core)
        print("Ego controller: %s (called every CARLA step, not every feed)"
              % embedded.spec)
    lastAdvisory = cs['EgoTargetSpeed']

    try:
        # Checked here rather than inside the bring-up, so a missing route fails
        # at start-up and not 400 ticks in when a deferred ego finally arrives.
        if virEnvOwnsEgo and useFixsDriver and not cs['EgoRoutePoints']:
            raise SystemExit('ActuationSource fixs needs EgoRoutePoints')

        egoIsUp = False
        haveTick = False       # has a feed actually been received yet
        if virEnvOwnsEgo and not deferEgoSpawn:
            sp = Pose(x=cs['EgoSpawnPose'][0], y=cs['EgoSpawnPose'][1],
                      z=cs['EgoSpawnPose'][2], headingDeg=cs['EgoSpawnPose'][3])
            if not _bringUpEgo(cs, backend, egoDriver, world, virEnvOwnsEgo,
                               useFixsDriver, useWireActuation, useEmbedded, sp, True):
                return -1
            egoIsUp = True
        elif deferEgoSpawn:
            print("No EgoSpawnPose -- waiting for '%s' to enter the traffic "
                  "simulator, then taking it over." % egoId)

        dataLog = _openDataLog(config)
        logWanted = _logWantedPredicate(config)
        poseLog = _openPoseLog()

        stepCount = 0
        simTime = 0.0
        wallStart = time.monotonic()
        loopStart = time.monotonic()   # rate summary at the end
        feedCount = 0
        # Per-phase tic-toc: separates "this bridge is slower" from "this bridge
        # waits longer for everyone else", which look identical in a tick total.
        tickMs = []
        tickT0 = time.monotonic()
        phase = {k: 0.0 for k in
                 ("fixs recv", "orchestrate", "flush batch", "world.tick",
                  "z audit", "readback+send", "pacing sleep")}

        while simTime < simEndTime:
            # ---- core: recv (only on the feed boundary) -> spawn / pose (batch)
            #      / despawn; the refresh interpolates EVERY sub-step ------------
            _t0 = time.monotonic()
            rc, err = core.runStep(simTime)
            phase["fixs recv"] += core.lastRecvSeconds
            phase["orchestrate"] += (time.monotonic() - _t0) - core.lastRecvSeconds
            if rc < 0:
                print('co-sim recv/step ended: %s' % (err or '?'), file=sys.stderr)
                break

            # ---- deferred ego: the traffic simulator inserted it, take it over --
            # Spawn at the pose it just reported, read out of the FIXS record
            # rather than off a Carla actor, so it does not wait on the pose
            # batch. Everything downstream already tolerates a missing ego.
            if deferEgoSpawn and not egoIsUp:
                rec = core.Msg_c.VehDataRecv_um.get(egoId)
                if rec is not None:
                    sp = Pose(x=rec.positionX, y=rec.positionY, z=rec.positionZ,
                              headingDeg=rec.heading)
                    print("Ego '%s' entered at t=%.2f (%.2f, %.2f, %.2f, %.1f deg)"
                          " -- spawning the physics ego."
                          % (egoId, simTime, sp.x, sp.y, sp.z, sp.headingDeg))
                    if not _bringUpEgo(cs, backend, egoDriver, world, virEnvOwnsEgo,
                                       useFixsDriver, useWireActuation,
                                       useEmbedded, sp, False):
                        break
                    egoIsUp = True
            # #266/#267: the batch is NOT flushed here. It is flushed just before
            # world.tick(), AFTER the spectator has been queued into it, so the
            # camera and the vehicles it follows are applied by ONE acknowledged
            # call and cannot land in different ticks. Nothing between here and
            # there reads back a CARLA transform: the ego-control calls drive TM /
            # pedals, and the pose log reads lastAppliedPose, our own copy.

            # An exchange boundary AND a tick that actually carried one. The core
            # skips the recv at simTime 0, so answering there would reply to a tick
            # that was never received -- and fixs.py refuses it outright ("there is
            # no tick to answer"), which is how this was found.
            #
            # mainVirCarla.cpp deliberately does NOT carry this term. Measured, the
            # C++ path BREAKS without that leading message, while this path is fine
            # with the pairing strict -- 6501 exchanges of mlk_eco_driving,
            # reproducing the reference exactly. The two transports are therefore
            # not symmetric here, and neither should be made to match the other
            # until that is explained. The bisect is on the #325 thread, finding A.
            onFeed = simTime > 1e-5 and onFeedBoundary(simTime, 1e-6)
            haveTick = haveTick or onFeed

            if replyCarriesNothing and onFeed and core.ENABLE_REALSIM:
                _t0 = time.monotonic()
                rc, err = core.sendData(simTime)
                phase["readback+send"] += time.monotonic() - _t0
                if rc < 0:
                    print("send to traffic layer failed: %s" % (err or "?"),
                          file=sys.stderr)
                    break

            # ---- L2: apply the external speed advisory at each feed -----------
            # Set the driver target BEFORE it runs this tick. applyEgoControl routes
            # it to native TM (set_desired_speed) or the EgoDriver fallback; it
            # persists across sub-steps until the next feed refreshes it.
            if virEnvOwnsEgo and not useWireActuation and onFeed:
                adv = core.Msg_c.VehDataRecv_um.get(egoId)
                if adv is not None and adv.speedDesired > 0.0:
                    lastAdvisory = adv.speedDesired
                # else keep the last advisory (controller not up yet / no update)
                backend.applyEgoControl(egoId, lastAdvisory)

            # ---- unified EgoDriver: apply the external ACTUATION command ------
            # One apply-path for L0/L2 (EgoDriver client) and L4 (a real external
            # controller); no TM and no route on the Carla side.
            if useWireActuation and onFeed:
                cmd = core.Msg_c.VehDataRecv_um.get(egoId)
                if cmd is not None:
                    backend.applyEgoActuation(
                        cmd.acceleratorPedalDesired, cmd.brakePedalDesired,
                        cmd.steerAngleDesired / kMaxSteerRad)   # rad -> normalised

            # the fallback module drives per-tick; native TM drives inside world.tick
            if virEnvOwnsEgo and useFixsDriver:
                _stepEgoDriver(backend, egoDriver, cs['EgoTargetSpeed'], lastAdvisory)
            # #325 embedded controller: same slot, same per-step rate. The
            # step itself lives in CommonLib so this file stays a faithful peer
            # of mainVirCarla.cpp, which has no such hook.
            # Both needed: nothing to control before the ego exists, and
            # fixs.vehicle.get RAISES rather than answering empty with no tick
            # received -- and the core skips the recv at simTime 0.
            if virEnvOwnsEgo and embedded is not None and egoIsUp and haveTick:
                from CommonLib import fixs as _fixs
                runController(backend, embedded, _fixs.vehicle.get(egoId),
                              carlaStep, onFeed, kMaxSteerRad)

            if poseLog is not None:      # A/B: the applied Carla pose per SUMO id
                for vid, h in core.mappedVehicles().items():
                    tf = backend.lastAppliedPose(h)
                    if tf is not None:
                        poseLog.write('%s,%s,%s,%s,%s\n'
                                      % (simTime, vid, tf.location.x, tf.location.y,
                                         tf.rotation.yaw))

            # ---- PRE-tick: spectator follow (#254) ---------------------------
            # The camera is placed BEFORE the tick that renders the frame, or it
            # trails by one step. Mirrored vehicles only -- a physics ego has no
            # pre-tick pose, so it keeps the post-tick snap below.
            if (spectatorFollow and not (virEnvOwnsEgo and centeredViewId == egoId)
                    and centeredViewId in interestedIds):
                h = core.mappedVehicles().get(centeredViewId)
                if h is not None:
                    tf = backend.lastAppliedPose(h)
                    if tf is not None:
                        backend.queueTransform(
                            spectator.id,
                            _spectatorTransform(tf, spectatorHeight, spectatorAlignYaw))

            _t0 = time.monotonic()
            backend.flushBatch()   # one acknowledged apply: vehicles + camera
            phase["flush batch"] += time.monotonic() - _t0
            _t0 = time.monotonic()
            world.tick()               # advance Carla one sub-step
            backend.noteWorldTicked()  # every actor now has a snapshot
            phase["world.tick"] += time.monotonic() - _t0

            # SUMO <-> CARLA elevation audit, once per exchange. Here rather than
            # inside setVehiclePose because it asks whether the two MAPS agree,
            # which no interpolated sub-step can change.
            if onFeed:
                _t0 = time.monotonic()
                backend.auditZAlignment()
                phase["z audit"] += time.monotonic() - _t0

            _t0 = time.monotonic()
            # ---- POST-tick L0+: the Carla-driven ego -> FIXS -----------------
            if virEnvOwnsEgo:
                es = EgoState()
                if backend.readEgoState(egoId, es):
                    if onFeed and core.ENABLE_REALSIM:
                        d = VehData()
                        d.id = egoId
                        d.type = egoCfg['Type']
                        # L2: report the COMMANDED advisory as speedDesired
                        # (measured speed stays in `speed`) so the DataLogger
                        # captures both and the ego's tracking of the external
                        # target is verifiable.
                        d.speed = es.speed
                        d.speedDesired = lastAdvisory
                        d.positionX, d.positionY, d.positionZ = es.x, es.y, es.z
                        d.heading = es.heading
                        d.grade = es.grade
                        core.Msg_c.VehDataSend_um.setdefault(0, []).append(d)
                        if dataLog.isOpen() and logWanted(d.id):
                            dataLog.logVehicle(simTime, d)
                        # Also log the TRAFFIC-SIM view of the ego on the SAME clock
                        # as the Carla view, so a plot compares them directly. It is
                        # the ego ~2 ticks stale (Carla is a step ahead and SUMO's
                        # getPosition is n-1); logged as id "ego_sumo".
                        if dataLog.isOpen():
                            fromSumo = core.Msg_c.VehDataRecv_um.get(egoId)
                            if fromSumo is not None:
                                # Records refuse assignment ('id' is measured
                                # data), so the relabel goes through the logger.
                                dataLog.logVehicle(simTime, fromSumo, idOverride='ego_sumo')
                    if spectatorFollow and egoId == centeredViewId and backend.egoActor():
                        spectator.set_transform(_spectatorTransform(
                            backend.egoActor().get_transform(),
                            spectatorHeight, spectatorAlignYaw))

            # ---- POST-tick: interested-id readback (feed) --------------------
            # The spectator used to be snapped here too; it now runs pre-tick, see
            # the PRE-tick block above (#254).
            mapped = core.mappedVehicles()
            for iid in interestedIds:
                if virEnvOwnsEgo and iid == egoId:
                    continue                    # ego handled above (never mapped)
                h = mapped.get(iid)
                if h is None:
                    continue
                actor = backend.actorOf(h)
                if actor is None:
                    continue
                if virEnvOwnsEgo and onFeed:
                    cTf = actor.get_transform()
                    ext = actor.bounding_box.extent
                    vel = actor.get_velocity()
                    sTf = BridgeHelper.map_transfrom_Carla_to_Sumo(cTf, ext)
                    d = VehData()
                    d.id = iid
                    d.type = 'ego'
                    d.speedDesired = math.sqrt(vel.x * vel.x + vel.y * vel.y)
                    d.positionX = sTf.location.x
                    d.positionY = sTf.location.y
                    d.positionZ = sTf.location.z
                    d.heading = sTf.rotation.yaw
                    d.grade = sTf.rotation.pitch * math.pi / 180.0
                    core.Msg_c.VehDataSend_um.setdefault(0, []).append(d)
                    if dataLog.isOpen() and logWanted(d.id):
                        dataLog.logVehicle(simTime, d)

            # ---- the driver owns the send: once per feed, pairing with the recv
            if not replyCarriesNothing and onFeed and core.ENABLE_REALSIM:
                rc, err = core.sendData(simTime)
                if rc < 0:
                    print('send to traffic layer failed: %s' % (err or '?'),
                          file=sys.stderr)
                    break

            if onFeed:
                feedCount += 1
            phase["readback+send"] += time.monotonic() - _t0
            _now = time.monotonic()
            if feedCount > 2:      # past the connect / warm-up wait
                tickMs.append(1000.0 * (_now - tickT0))
            tickT0 = _now
            stepCount += 1
            simTime = stepCount * carlaStep   # step counter avoids fp drift

            # Realtime pacing (viz): sleep so each sub-tick lands at its wall-clock
            # sim time, spreading them evenly instead of bursting. Off by default.
            _t0 = time.monotonic()
            if realtimePacing:
                target = wallStart + simTime
                now = time.monotonic()
                if now < target:
                    time.sleep(target - now)
                elif now - target > 0.25:
                    wallStart = now - simTime
            phase["pacing sleep"] += time.monotonic() - _t0

        # How fast the bridge actually ran. Printed always, because "is the
        # Python bridge slower than the C++ one" is otherwise answered by timing a
        # whole stack -- SUMO, TrafficLayer, the controller and a warm-up -- and
        # attributing the difference to the bridge. This is the loop itself.
        loopElapsed = time.monotonic() - loopStart
        if loopElapsed > 0 and stepCount:
            print("Bridge loop: %d exchanges, %d ticks in %.1f s "
                  "(%.1f exchanges/s, %.2f ms/tick)"
                  % (feedCount, stepCount, loopElapsed, feedCount / loopElapsed,
                     1000.0 * loopElapsed / stepCount))
            for name, sec in sorted(phase.items(), key=lambda kv: -kv[1]):
                print("             %-14s %7.2f ms/tick  %4.1f%%"
                      % (name, 1000.0 * sec / stepCount, 100.0 * sec / loopElapsed))
            _acct = sum(phase.values())
            print("             %-14s %7.2f ms/tick  %4.1f%%  (loop overhead)"
                  % ("unaccounted", 1000.0 * (loopElapsed - _acct) / stepCount,
                     100.0 * (loopElapsed - _acct) / loopElapsed))
            steady = tickMs
            if len(steady) > 20:
                _s = sorted(steady)
                _n = len(_s)
                _mean = sum(_s) / _n
                _sd = (sum((x - _mean) ** 2 for x in _s) / _n) ** 0.5
                print("             steady state:  mean %.1f  p50 %.1f  p95 %.1f  "
                      "p99 %.1f  max %.1f  sd %.1f ms  (%.1f exchanges/s)"
                      % (_mean, _s[_n // 2], _s[int(_n * 0.95)], _s[int(_n * 0.99)],
                         _s[-1], _sd, 1000.0 / _mean))
        if dataLog.isOpen():
            print('DataLogger closed: %s' % dataLog.path())
            dataLog.close()
        if poseLog is not None:
            poseLog.close()
        if virEnvOwnsEgo:
            backend.destroyEgo()
    finally:
        core.shutdown()
        settings = world.get_settings()
        if settings.synchronous_mode:
            settings.synchronous_mode = False
            world.apply_settings(settings)
    return 0


def _bringUpEgo(cs, backend, egoDriver, world, virEnvOwnsEgo, useFixsDriver,
                useWireActuation, useEmbedded, sp, settleOutOfBand):
    """Spawn the ego at `sp` and wire whichever driver owns it. -> bool

    Peer of the ``bringUpEgo`` lambda in mainVirCarla.cpp:282, and callable from
    the same two places: before the loop at a configured EgoSpawnPose, or from
    inside it at the pose the traffic simulator just reported.

    Order matters and mirrors the proven sequence: spawn + physics first, THEN the
    traffic manager, sync and autopilot. TM must be synchronous in a synchronous
    world; ``world.tick()`` then drives its synchronous tick automatically.

    ``settleOutOfBand`` is the difference between those two call sites. Before the
    loop the extra ticks settle the ego onto its tyres and pay TM's one-time map
    build up front. Inside the loop they must NOT happen: an out-of-band tick
    there advances Carla past the feed and desyncs every mirrored vehicle. A
    deferred ego therefore settles inside the co-sim ticks instead.
    """
    if backend.spawnEgo(cs['EgoBlueprint'], sp, cs['TrafficManagerPort']) == kNoHandle:
        print('ego spawn failed', file=sys.stderr)
        return False

    if useFixsDriver:
        # The only Carla-specific step is the frame conversion (FIXS -> Carla is a
        # Y flip); the module then owns densification and the pursuit control law.
        egoDriver.setRoute([(x, -y) for x, y in cs['EgoRoutePoints']], True)
        print('L0 ego route: %d waypoints -> %d path points (EgoDriver fallback module)'
              % (len(cs['EgoRoutePoints']), egoDriver.routeSize()))

    if settleOutOfBand:
        for _ in range(10):
            world.tick()

    if useWireActuation:
        print('ActuationSource user (over the feed): ego driven by the wire '
              'actuation command (no TM, no route).')
    elif useEmbedded:
        # The controller in the driver slot owns the ego. Autopilot here would
        # fight it: TM would steer to its own plan while the controller's pedals
        # are applied on top, and the ego would track neither.
        print('ActuationSource user (in-process): ego driven by the Controller, '
              'called every Carla step (no TM, no route).')
    elif not useFixsDriver:
        backend.enableEgoTM(cs['TrafficManagerPort'], cs['EgoTargetSpeed'])
        # TM builds its InMemoryMap on the FIRST tick after autopilot, which can
        # take 15-30 s on a generated map. Absorb that ONE-TIME cost HERE, before
        # the co-sim loop couples with TrafficLayer, so the loop's tight tick never
        # stalls past its timeout and drops the connection.
        if settleOutOfBand:
            print('Pre-building TM InMemoryMap (one-time, may take ~30 s)...')
            for _ in range(5):
                world.tick()
            print('TM InMemoryMap ready; entering co-sim loop.')
        else:
            print('WARNING: TM autopilot on a deferred ego -- its one-time '
                  'InMemoryMap build happens now, inside the co-sim loop. '
                  'Configure EgoSpawnPose to pay it up front.', file=sys.stderr)

    if virEnvOwnsEgo:
        print('L2: external speed advisory via FIXS (ego.speedDesired) -- driver: %s'
              % ('EgoDriver (in-bridge pure pursuit)' if useFixsDriver
                 else 'external (pedals over FIXS)' if useWireActuation
                 else 'EgoController, in-process per Carla step' if useEmbedded
                 else 'TM (Carla Traffic Manager)'))
    return True


def _stepEgoDriver(backend, egoDriver, targetSpeed, lastAdvisory):
    """Per-tick fallback driver: pose -> EgoDriver -> apply through full PhysX.

    Peer of ``CarlaBackend::driveEgoFallback``, kept in the driver here because the
    Python EgoDriver lives in CommonLib and the backend need not own an instance of
    it to satisfy the verb interface.
    """
    ego = backend.egoActor()
    if ego is None or not egoDriver.hasRoute():
        return
    tf = ego.get_transform()
    vel = ego.get_velocity()
    v = math.sqrt(vel.x * vel.x + vel.y * vel.y)
    yawRad = tf.rotation.yaw * math.pi / 180.0
    # L2: an external advisory supersedes the static cruise target.
    tgt = lastAdvisory if lastAdvisory else targetSpeed
    dc = egoDriver.computeControl(tf.location.x, tf.location.y, yawRad, v, tgt)
    ego.apply_control(carla.VehicleControl(throttle=float(dc.throttle),
                                           brake=float(dc.brake),
                                           steer=float(dc.steer)))


def _openDataLog(config):
    """Generic FIXS data logging (config: DataLogSetup).

    Records the vehicle-data records this bridge reports to FIXS, in the
    SUMO/VISSIM wire convention. Same code path for every ActuationSource, so the CSVs
    are directly comparable -- and, because it is the same DataLogger the C++ side
    writes, comparable across bridges too.
    """
    dataLog = DataLogger()
    dls = config.DataLog_setup
    if not dls['EnableDataLog']:
        return dataLog
    path = dls['DataLogPath']
    if not path or path == 'auto':
        path = os.path.join('_datalog', 'vircarla.csv')
    if dataLog.open(path, dls['DataLogFields']):
        print('DataLogger -> %s (FIXS/SUMO-VISSIM wire convention)' % dataLog.path())
    else:
        print('DataLogger: could not open %s' % path, file=sys.stderr)
    return dataLog


def _logWantedPredicate(config):
    who = config.DataLog_setup['DataLogWho'] or []
    if not who:
        return lambda _id: True
    wanted = set(who)
    return lambda _id: _id in wanted


def _openPoseLog():
    """Optional applied-pose log keyed by SUMO id: set ``RS_POSE_LOG=<path>``.

    The A/B instrument for bridge-vs-bridge comparison: the C++ driver writes the
    same columns under the same environment variable, so a Python run and a C++ run
    of one scenario diff per (simTime, id) with no CARLA-readback confound.
    """
    path = os.environ.get('RS_POSE_LOG')
    if not path:
        return None
    parent = os.path.dirname(os.path.abspath(path))
    os.makedirs(parent, exist_ok=True)
    f = open(path, 'w', encoding='utf-8')
    f.write('simTime,id,x,y,yaw\n')
    return f


if __name__ == '__main__':
    sys.exit(main())
