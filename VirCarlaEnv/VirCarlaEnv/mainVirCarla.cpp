//============================================================================
//  mainVirCarla  (#174)  -- now a THIN Carla tick-driver over VirEnvCore.
//----------------------------------------------------------------------------
//  Previously this file WAS the whole Carla bridge (~677 lines) with the
//  seven-step orchestration inlined in main(). That orchestration moved to the
//  backend-agnostic CommonLib/VirEnvCore (shared with CarMaker); the Carla verbs
//  moved to CarlaBackend. main() now just: brings up the Carla world + tick,
//  constructs CarlaBackend + VirEnvCore, and per tick calls core.runStep then
//  flushes the transform batch, ticks the world, and (for interested/external
//  ids) reads back POST-tick + sends to FIXS -- exactly the Carla-specific shell.
//============================================================================
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <thread>

#include <carla/client/Client.h>
#include <carla/client/World.h>
#include <carla/client/Actor.h>
#include <carla/client/ActorList.h>
#include <carla/client/Vehicle.h>
#include <carla/client/TimeoutException.h>
#include <carla/geom/Transform.h>
#include <carla/Memory.h>
#include <carla/trafficmanager/TrafficManager.h>

#include "BridgeHelper.h"
#include "CarlaBackend.h"
#include "../../CommonLib/VirEnvCore.h"
#include "../../CommonLib/DataLogger.h"
#include "MsgHelper.h"
#include "ConfigHelper.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std::chrono_literals;

static void show_usage(const std::string& name) {
    std::cerr << "Usage: " << name << " -f <config.yaml> [-t <tls_table.csv>]\n";
}

int main(int argc, const char* argv[]) {
    std::cout << std::unitbuf;   // flush every << so a crash never swallows the last diagnostic line
    std::string configPath, tlsPath;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "-h" || a == "--help") { show_usage(argv[0]); return 0; }
        else if ((a == "-f" || a == "--file") && i + 1 < argc) configPath = argv[++i];
        else if ((a == "-t" || a == "--tls")  && i + 1 < argc) tlsPath = argv[++i];
        else { show_usage(argv[0]); return 0; }
    }

    ConfigHelper config;
    if (config.getConfig(configPath) < 0) { std::cerr << "Bad config: " << configPath << "\n"; return -1; }
    CarlaSetup_t cs = config.CarlaSetup;
    const bool   verbose      = cs.EnableVerboseLog;
    // ---- cadence: three distinct things, one each --------------------------
    //  feed        fixs::kFeedPeriodS -- the FIXS exchange period, which is also
    //              the traffic simulator's step (TrafficLayer steps it once per
    //              exchange). A protocol constant, not a knob: see FixsProtocol.h.
    //  carlaStep   CarlaSetup.CarlaTimeStep -- the Carla world step
    //              (fixed_delta_seconds), the analogue of CarMaker's solver dt.
    //              Absent/0 -> the feed, i.e. tick 1:1 and do not interpolate.
    //  poseRefresh CarlaSetup.TrafficRefreshRate -- how often the core re-applies
    //              (interpolated) traffic poses. EXACTLY the meaning the key has on
    //              the CarMaker side (VirEnvHelper -> core_.trafficRefreshRate_):
    //              a visual/RPC-cost knob, independent of the world step. Absent/0
    //              -> every tick. Coarser than the tick = fewer ApplyBatch calls on
    //              a heavy scene while physics/sensors still run at the tick rate.
    //
    // These used to be entangled: TrafficRefreshRate doubled as the feed period AND
    // as the tick when CarlaTimeStep was unset, so setting it to 0.05 silently
    // turned interpolation OFF (carlaStep == refreshRate) and every pose was held
    // for two ticks. Nothing falls back to anything else now.
    const double feed        = fixs::kFeedPeriodS;
    const double carlaStep   = (cs.CarlaTimeStep > 1e-9) ? cs.CarlaTimeStep : feed;
    const double poseRefresh = (cs.TrafficRefreshRate > 1e-9) ? cs.TrafficRefreshRate
                                                              : carlaStep;
    // The exchange boundary is tested on the feed grid, so the world clock has to
    // LAND on it: carlaStep must divide the feed exactly. With, say, 0.03 the clock
    // steps 0.09 -> 0.12 and no tick is ever a feed boundary, so the bridge would
    // trade no messages at all and simply hang.
    {
        const double slots = feed / carlaStep;
        const double whole = (double)(long long)(slots + 0.5);
        if (carlaStep > feed + 1e-9 || std::fabs(slots - whole) > 1e-6) {
            std::cerr << "CarlaSetup.CarlaTimeStep (" << carlaStep << " s) must be the FIXS "
                      << "feed period (" << feed << " s) or an exact divisor of it "
                      << "(0.05, 0.025, 0.02, 0.01), else no Carla tick ever lands on an "
                      << "exchange boundary.\n";
            return -1;
        }
    }
    // The core gates re-application with (int)(1.0 / poseRefresh), so a value whose
    // reciprocal is not whole silently snaps to a different grid (0.03 -> 1/33 s).
    if (poseRefresh < carlaStep - 1e-9 ||
        std::fabs(1.0 / poseRefresh - (double)(long long)(1.0 / poseRefresh + 0.5)) > 1e-6) {
        std::cerr << "CarlaSetup.TrafficRefreshRate (" << poseRefresh << " s) must be >= "
                  << "CarlaTimeStep (" << carlaStep << " s) and have a whole reciprocal "
                  << "(0.1, 0.05, 0.025, 0.02, 0.01). It is the pose re-apply cadence, "
                  << "not the feed period - to tick Carla faster, set CarlaTimeStep.\n";
        return -1;
    }
    const uint32_t simEndTime = config.SimulationSetup.SimulationEndTime;
    const std::string centeredViewId   = cs.CenteredViewId;
    const bool   spectatorFollow  = cs.EnableSpectatorFollow && !cs.CenteredViewId.empty();
    const float  spectatorHeight  = (float)cs.SpectatorHeight;
    const bool   spectatorAlignYaw = cs.SpectatorAlignYaw;
    const bool   realtimePacing = cs.RealtimePacing;
    const bool   enableTlsSync = true;
    // The ego, from the one place that describes it (#305). Canonical values, so
    // these are string compares and nothing re-derives a mode.
    const bool   virEnvOwnsEgo     = (config.EgoSetup.Dynamics == "virenv");
    const bool   useFixsDriver = (config.EgoSetup.ActuationSource == "fixs");
    // "user" over the FEED. With a Controller file it runs in-process instead,
    // which only the Python backend can load.
    const bool   useWireActuation  = (config.EgoSetup.ActuationSource == "user"
                                      && config.EgoSetup.Controller.empty());
    const double kMaxSteerRad = 0.7;   // must match the client's DriveCommand steer scaling
    const std::string egoId = config.EgoSetup.Id;

    // DOES THIS BRIDGE OWN THE EGO? Same predicate TrafficLayer uses -- both
    // derive from EgoSetup.Dynamics, so they cannot disagree (#305).
    const bool carlaOwnsEgo = virEnvOwnsEgo
        && (std::find(cs.InterestedIds.begin(), cs.InterestedIds.end(), egoId)
            != cs.InterestedIds.end());
    if (virEnvOwnsEgo && !carlaOwnsEgo) {
        std::cout << "Dynamics: virenv is inactive -- the ego id is not in "
                     "InterestedIds, so the traffic simulator keeps the ego and this "
                     "bridge mirrors it (L0 control run).\n";
    }
    // WHO CREATES THE EGO -- decided by whether EgoSpawnPose is configured.
    // Absent, the traffic simulator inserts it on its own route (#305).
    const bool deferEgoSpawn = (carlaOwnsEgo && cs.EgoSpawnPose.size() < 4);

    std::unordered_set<std::string> interestedIds(cs.InterestedIds.begin(), cs.InterestedIds.end());

    try {
        // ---- Carla world: connect + synchronous mode -----------------------
        carla::client::Client client(cs.CarlaServerIP, cs.CarlaServerPort);
        client.SetTimeout(10s);
        std::cout << "Carla client " << client.GetClientVersion()
                  << " / server " << client.GetServerVersion() << "\n";
        carla::client::World world = client.GetWorld();
        carla::SharedPtr<carla::client::Actor> spectator = world.GetSpectator();

        carla::rpc::EpisodeSettings settings = world.GetSettings();
        // Force BOTH sync mode and the tick delta: the world-load script may have
        // set sync with a different delta, and a physics ego (EgoMode >= 1) steps
        // PhysX/TM by fixed_delta_seconds -- it must equal the bridge's carlaStep.
        if (!settings.synchronous_mode ||
            !settings.fixed_delta_seconds.has_value() ||
            std::fabs(settings.fixed_delta_seconds.value_or(0.0) - carlaStep) > 1e-9) {
            settings.synchronous_mode = true;
            settings.fixed_delta_seconds = carlaStep;
            world.ApplySettings(settings, 1s);
            if (verbose) std::cout << "Synchronous mode enabled (delta " << carlaStep << " s).\n";
        }

        // Clear stale vehicle actors from prior runs: this bridge owns EVERY
        // vehicle in the world, so any pre-existing one is a crashed run's zombie
        // (it would block the ego spawn point / duplicate bg traffic).
        // NOTE: in synchronous mode the client's episode snapshot is EMPTY until a
        // tick -- GetActors() would miss the zombies without this first Tick.
        world.Tick(10s);
        {
            carla::SharedPtr<carla::client::ActorList> stale =
                world.GetActors()->Filter("vehicle.*");
            int nStale = 0;
            for (const carla::SharedPtr<carla::client::Actor>& a : *stale) {
                if (a) { a->Destroy(); nStale++; }
            }
            if (nStale > 0) std::cout << "Cleared " << nStale << " stale vehicle actor(s) from a prior run.\n";
        }

        // ---- backend + core ------------------------------------------------
        virenv::CarlaBackend backend(&world, &client, cs.UseVehicleTypeAsBlueprint, verbose);
        virenv::VirEnvCore core;
        core.setBackend(&backend);
        core.interpolateTraffic        = (carlaStep < feed - 1e-9);  // sub-step -> interpolate the feed
        core.sendEgoFromCore           = false;  // this driver owns the send (post-tick)
        core.openSignalPort            = false;  // Carla: vehicles + signals on ONE port
        core.ENABLE_REALSIM            = cs.EnableCosimulation;
        core.SYNCHRONIZE_TRAFFIC_SIGNAL = enableTlsSync;
        // EgoMode 0: Carla renders every vehicle incl. the SUMO-driven ego ("").
        // EgoMode >=1: Carla OWNS the ego -- the core must never spawn/teleport the
        // SUMO echo of it (SUMO's copy is the injected shadow of this Carla actor).
        core.egoId_                    = carlaOwnsEgo ? egoId : "";
        core.egoType_                  = "";
        core.trafficLayerIP_           = cs.CarlaClientIP;
        core.vehDataPort_              = cs.CarlaClientPort;
        core.trafficRefreshRate_       = poseRefresh;  // pose re-apply cadence (CarMaker semantics)
        core.Msg_c.getConfig(config);
        // One line that states the whole cadence, so a run never has to be reverse
        // engineered from three keys again.
        std::cout << "Cadence: FIXS feed " << feed << " s (= the traffic simulator step)"
                  << " | Carla tick " << carlaStep << " s"
                  << (core.interpolateTraffic
                        ? " (interpolated " + std::to_string((int)(feed / carlaStep + 0.5)) + "x)"
                        : " (1:1 with the feed)")
                  << " | pose refresh " << poseRefresh << " s"
                  << " | pacing " << (realtimePacing ? "realtime" : "as fast as possible") << "\n";

        const char* err = nullptr;
        if (core.initialization(&err, configPath.c_str(), tlsPath.c_str()) < 0) {
            std::cerr << "VirEnvCore init failed: " << (err ? err : "?") << "\n";
            return -1;
        }
        if (enableTlsSync) backend.freezeAndMatchTrafficLights();

        // Spawn the ego and wire its driver, before the loop or inside it.
        // settleOutOfBand: only the pre-loop caller may tick the world to settle
        // the car -- an extra Tick() inside the loop desyncs everything (#305).
        bool egoIsUp = false;
        int  egoTraceLeft = 50;   // feeds of handover trace (5 s at the 0.1 s feed)
        auto bringUpEgo = [&](const virenv::Pose& sp, bool settleOutOfBand) -> bool {
            if (backend.spawnEgo(cs.EgoBlueprint, sp, cs.TrafficManagerPort) == virenv::kNoHandle) {
                std::cerr << "ego spawn failed\n";
                return false;
            }
            // Two in-Carla drivers (EgoSetup.ActuationSource):
            //  - native Carla TM autopilot (default): server-side, needs a
            //    routable map (the simple_loop junction-id fix makes it routable);
            //  - EgoDriver fallback module: map-agnostic pure pursuit on
            //    EgoRoutePoints, through full PhysX -- needs the route.
            if (useFixsDriver)
                backend.setEgoRoute(cs.EgoRoutePoints, cs.EgoRouteRepeat, cs.TrafficManagerPort);
            if (settleOutOfBand)
                for (int i = 0; i < 10; i++) world.Tick(30s);
            if (useWireActuation) {
                // No in-Carla driver: the ego stays physics-on and MANUAL (no autopilot),
                // driven each feed by the external EgoDriver client's wire actuation.
                std::cout << "ActuationSource user (over the feed): ego driven by the "
                          << "wire actuation command (no TM, no route).\n";
            }
            // native TM must be enabled AFTER spawn + physics settle (proven order)
            if (!useFixsDriver && !useWireActuation) {
                backend.enableEgoTM(cs.TrafficManagerPort, cs.EgoTargetSpeed);
                // TM builds its InMemoryMap on the FIRST tick after autopilot -- can
                // take 15-30 s on a generated map. Absorb that ONE-TIME cost HERE
                // (generous timeout, before the co-sim loop couples with TL/SUMO), so
                // the loop's tight world.Tick() never stalls past its timeout and
                // drops the TrafficLayer/SUMO connection.
                if (settleOutOfBand) {
                    std::cout << "Pre-building TM InMemoryMap (one-time, may take ~30 s)...\n";
                    for (int i = 0; i < 5; i++) world.Tick(120s);
                    std::cout << "TM InMemoryMap ready; entering co-sim loop.\n";
                }
                else {
                    // Deferred + TM: that one-time build now lands inside the loop and
                    // can stall the feed. Configure EgoSpawnPose to pay it up front.
                    std::cout << "WARNING: TM autopilot on a deferred ego -- its one-time "
                              << "InMemoryMap build happens now, inside the co-sim loop.\n";
                }
            }
            egoIsUp = true;
            return true;
        };

        // ---- Carla drives the ego ------------------------------------------
        // Order matters: spawn + physics first, THEN TM + sync + autopilot. TM
        // must be synchronous in a synchronous world.
        if (carlaOwnsEgo && useFixsDriver && cs.EgoRoutePoints.empty()) {
            std::cerr << "ActuationSource fixs needs EgoRoutePoints\n";
            return -1;
        }
        if (carlaOwnsEgo && !deferEgoSpawn) {
            virenv::Pose sp;
            sp.x = cs.EgoSpawnPose[0]; sp.y = cs.EgoSpawnPose[1];
            sp.z = cs.EgoSpawnPose[2]; sp.headingDeg = cs.EgoSpawnPose[3];
            if (!bringUpEgo(sp, /*settleOutOfBand=*/true)) return -1;
        }
        else if (deferEgoSpawn) {
            std::cout << "No EgoSpawnPose -- waiting for '" << egoId
                      << "' to enter the traffic simulator, then taking it over.\n";
        }

        // ---- L2 (EgoMode >= 2): artificial external speed-advisory controller ----
        // Feeds the ego's L0 driver a time-varying desired speed (the advisory a
        // real external CAV controller would stream over FIXS). SDK-free module;
        // swap-in point for a separate advisory client later. Empty profile -> the
        // advisor returns EgoTargetSpeed, so EgoMode 2 degenerates to constant L0.
        // L2 (EgoMode >= 2): the ego's target speed comes from an EXTERNAL controller
        // over FIXS -- read off the ego's received record (ego.speedDesired), which an
        // advisory client (e.g. py_ego_speed_advisor.py) feeds through TrafficLayer's
        // sequential-client path. No controller attached -> falls back to EgoTargetSpeed.
        if (carlaOwnsEgo)
            // Name all three drivers, not two: this is the only line telling the
            // operator which one they got.
            std::cout << "L2: external speed advisory via FIXS (ego.speedDesired) -- driver: "
                      << (useFixsDriver ? "EgoDriver (in-bridge pure pursuit)"
                          : useWireActuation ? "external (pedals over FIXS)"
                          : "TM (Carla Traffic Manager)") << "\n";
        double lastAdvisory = cs.EgoTargetSpeed;   // most-recent commanded desired speed (for the driver/log)

        const int sock0 = 0;
        // #174 A/B: optional applied-pose log keyed by SUMO id (set RS_POSE_LOG=path)
        std::ofstream poseLog;
        if (const char* plp = std::getenv("RS_POSE_LOG")) { poseLog.open(plp); poseLog << "simTime,id,x,y,yaw\n"; }
        long stepCount = 0;
        double simTime = 0.0;
        auto wallStart = std::chrono::steady_clock::now();   // realtime-pacing reference
        auto loopStart = std::chrono::steady_clock::now();   // rate summary at the end
        long feedCount = 0;
        // Per-phase tic-toc, the SAME phases the Python bridge reports, so the two
        // breakdowns line up name for name and a difference can be attributed to a
        // part of the workflow instead of to a language.
        double phRecv = 0, phOrch = 0, phFlush = 0, phTick = 0,
               phAudit = 0, phBack = 0, phPace = 0;
        auto _clk = []{ return std::chrono::steady_clock::now(); };
        auto _el = [](const std::chrono::steady_clock::time_point& a) {
            return std::chrono::duration<double>(
                std::chrono::steady_clock::now() - a).count(); };

        // ---- generic FIXS data logging (config: DataLogSetup) --------------
        // Records the vehicle-data records this bridge reports to FIXS, in the
        // SUMO/VISSIM wire convention (see CommonLib/DataLogger). Same code path
        // for every EgoL0Driver, so the CSVs are directly comparable.
        const auto& dls = config.DataLogSetup;
        fixs::DataLogger dataLog;
        auto logWanted = [&](const std::string& id) {
            if (dls.DataLogWho.empty()) return true;
            for (const std::string& w : dls.DataLogWho) if (w == id) return true;
            return false;
        };
        if (dls.EnableDataLog) {
            std::string p = (dls.DataLogPath.empty() || dls.DataLogPath == "auto")
                            ? std::string("_datalog/vircarla.csv") : dls.DataLogPath;
            if (dataLog.open(p, dls.DataLogFields))
                std::cout << "DataLogger -> " << dataLog.path() << " (FIXS/SUMO-VISSIM wire convention)\n";
            else
                std::cerr << "DataLogger: could not open " << p << "\n";
        }

        while (simTime < simEndTime) {
            // ---- core: recv (only on the 0.1s feed boundary) -> spawn / pose
            //      (batch) / despawn; the refresh interpolates EVERY sub-step ----
            auto _t0 = _clk();
            if (core.runStep(simTime, &err) < 0) {
                if (WSAGetLastError() != WSAEINTR && WSAGetLastError() != WSAEFAULT)
                    std::cerr << "co-sim recv/step ended: " << (err ? err : "?") << "\n";
                break;
            }

            phRecv += core.lastRecvSeconds;
            phOrch += _el(_t0) - core.lastRecvSeconds;
                                                   // Python bridge reports it
            // #266: the batch is NOT flushed here. It is flushed just before
            // world.Tick(), AFTER the spectator has been queued into it, so the
            // camera and the vehicles it follows are applied by ONE ApplyBatchSync
            // and cannot land in different ticks. Nothing between here and there
            // reads back a Carla transform: the ego-control calls drive TM / pedals,
            // and the pose log reads lastAppliedPose, our own copy.
            //
            // The deferred ego spawn below is safe under that rule for the same
            // reason: it reads the pose out of the FIXS record it just received,
            // not off a Carla actor, so it does not depend on the batch having
            // been applied yet.

            // ---- deferred ego: the traffic simulator inserted it, take it over ----
            // Spawn at the pose it just reported, so the two are one from the first
            // tick. Everything downstream already tolerates a missing ego.
            if (deferEgoSpawn && !egoIsUp) {
                auto itNew = core.Msg_c.VehDataRecv_um.find(egoId);
                if (itNew != core.Msg_c.VehDataRecv_um.end()) {
                    virenv::Pose sp;
                    sp.x = itNew->second.positionX;
                    sp.y = itNew->second.positionY;
                    sp.z = itNew->second.positionZ;
                    sp.headingDeg = itNew->second.heading;
                    std::cout << "Ego '" << egoId << "' entered at t=" << simTime
                              << " (" << sp.x << ", " << sp.y << ", " << sp.z
                              << ", " << sp.headingDeg << " deg) -- spawning the physics ego.\n";
                    if (!bringUpEgo(sp, /*settleOutOfBand=*/false)) break;
                }
            }


            // ---- L2: apply the external speed advisory at each 0.1s FIXS feed ----
            // Set the driver target BEFORE it runs this tick. applyEgoControl routes
            // it to native TM (SetDesiredSpeed) or the EgoDriver fallback (override);
            // it persists across sub-steps until the next feed refreshes it.
            if (carlaOwnsEgo && !useWireActuation) {
                const bool onFeedNow = fixs::onFeedBoundary(simTime, 1e-6);
                if (onFeedNow) {
                    // the external controller's advisory rides on the ego's received
                    // FIXS record (TrafficLayer's sequential overlay merged it in).
                    auto itAdv = core.Msg_c.VehDataRecv_um.find(egoId);
                    if (itAdv != core.Msg_c.VehDataRecv_um.end() && itAdv->second.speedDesired > 0.0f)
                        lastAdvisory = itAdv->second.speedDesired;
                    // else keep the last advisory (controller not up yet / no update)
                    backend.applyEgoControl(egoId, lastAdvisory);
                }
            }
            // #174 unified EgoDriver: apply the external client's ACTUATION command off
            // the ego's wire record (throttle/brake pedals + steer angle). One apply-path
            // for L0/L2 (EgoDriver client) and L4 (real controller); no TM/route here.
            if (useWireActuation) {
                const bool onFeedNow = fixs::onFeedBoundary(simTime, 1e-6);
                if (onFeedNow) {
                    auto itAct = core.Msg_c.VehDataRecv_um.find(egoId);
                    if (itAct != core.Msg_c.VehDataRecv_um.end()) {
                        const VehFullData_t& cmd = itAct->second;
                        backend.applyEgoActuation(cmd.acceleratorPedalDesired, cmd.brakePedalDesired,
                                                  cmd.steerAngleDesired / kMaxSteerRad);  // rad -> normalized
                        // The advisory rides on the SAME record; track it here too or
                        // the datalog reports EgoTargetSpeed forever (#305).
                        if (cmd.speedDesired > 0.0f) lastAdvisory = cmd.speedDesired;
                    }
                }
            }
            // fallback module drives per-tick; native TM drives inside world.Tick()
            if (carlaOwnsEgo && useFixsDriver) backend.stepEgoDriver(cs.EgoTargetSpeed);
            if (poseLog.is_open()) {       // A/B: log the applied Carla pose per SUMO id
                for (const auto& kv : core.mappedVehicles()) {
                    const carla::geom::Transform* tf = backend.lastAppliedPose(kv.second);
                    if (tf) poseLog << simTime << "," << kv.first << "," << tf->location.x
                                    << "," << tf->location.y << "," << tf->rotation.yaw << "\n";
                }
            }

            // ---- PRE-tick: spectator follow (#254) --------------------------
            // The camera is placed BEFORE the tick that renders the frame, or it
            // trails by one step. Mirrored vehicles only -- a physics ego has no
            // pre-tick pose, so it keeps the post-tick snap below.
            if (spectatorFollow && !(carlaOwnsEgo && centeredViewId == egoId)
                && interestedIds.count(centeredViewId)) {
                const auto& mappedPre = core.mappedVehicles();
                auto cit = mappedPre.find(centeredViewId);
                if (cit != mappedPre.end()) {
                    if (const carla::geom::Transform* tf = backend.lastAppliedPose(cit->second)) {
                        carla::geom::Location loc = tf->location; loc.z += spectatorHeight;
                        const float yaw = spectatorAlignYaw ? (tf->rotation.yaw - 90.f) : -90.f;

                        // Into the SAME batch as the vehicles: a standalone
                        // SetTransform RPC can land in a different tick from the
                        // poses it is meant to be centred on.

                        backend.queueTransform(spectator->GetId(),
                            carla::geom::Transform(loc, carla::geom::Rotation(-90.f, yaw, 0.f)));
                    }
                }
            }

            // One atomic apply per tick, acknowledged before we tick.

            _t0 = _clk();
            backend.flushBatch();   // one acknowledged apply: vehicles + camera
            phFlush += _el(_t0);
            _t0 = _clk();

            world.Tick(10s);               // advance Carla one sub-step (10s: TM sync work rides on the tick)
            phTick += _el(_t0);

            // #325 (finding A): the feed boundary alone. Pairing it with the recv
            // silently drops the advisory on the ticks the recv does not land.
            const bool onFeed = fixs::onFeedBoundary(simTime, 1e-6);

            // SUMO<->CARLA elevation audit, once per exchange. Here rather than inside
            // setVehiclePose because it asks whether the two MAPS agree, which no
            // interpolated sub-step can change - see CarlaBackend::auditZAlignment.
            if (onFeed) { _t0 = _clk(); backend.auditZAlignment(); phAudit += _el(_t0); }

            _t0 = _clk();
            // ---- POST-tick L0+: the Carla-driven ego -> FIXS (TL injects into SUMO)
            if (carlaOwnsEgo) {
                virenv::EgoState es;
                if (backend.readEgoState(egoId, es)) {
            if (onFeed && core.ENABLE_REALSIM) {
                        VehFullData_t d;
                        d.id = egoId; d.type = config.EgoSetup.Type;
                        // L2: report the COMMANDED advisory as speedDesired (measured
                        // speed stays in `speed`) so the DataLogger captures both and
                        // the ego's tracking of the external target is verifiable.
                        d.speed = (float)es.speed;
                        d.speedDesired = (float)lastAdvisory;
                        d.positionX = (float)es.x; d.positionY = (float)es.y; d.positionZ = (float)es.z;
                        d.heading = (float)es.heading; d.grade = (float)es.grade;
                        // What the ego was COMMANDED this tick, whoever produced it:
                        // applyEgoActuation records too, so this is not "what the
                        // in-bridge driver decided". Free on the wire -- a field is
                        // only serialised if the config lists it.
                        {
                            const auto& cmd = backend.lastEgoCommand();
                            d.acceleratorPedalDesired = (float)cmd.throttle;
                            d.brakePedalDesired       = (float)cmd.brake;
                            d.steerAngleDesired       = (float)(cmd.steer * kMaxSteerRad);
                        }
                        // Report the ego to FIXS only when something is going to
                        // INJECT it. With EnableExternalControl false there is no
                        // carlaOwnsId path in TrafficLayer, so this record falls
                        // through to the ordinary application-layer branch and is
                        // applied as a plain setSpeed on the traffic simulator's
                        // ego -- from the highest-numbered client, so it also beats
                        // the controller that is supposed to be driving it. That
                        // turns "own an ego in Carla, but do not feed it back"
                        // into "silently drive the traffic simulator's ego from
                        // Carla", which is the opposite of what the flag says.
                        // Still logged either way: the ego's state is worth
                        // recording whether or not anyone is consuming it.
                        if (virEnvOwnsEgo)
                            core.Msg_c.VehDataSend_um[core.Sock_c.serverSock[sock0]].push_back(d);
                        if (dataLog.isOpen() && logWanted(d.id)) dataLog.logVehicle(simTime, d);
                        // Handover trace: the first 5 s after the ego becomes ours, one
                        // line per feed. "The ego just sits there" is always one of
                        // these three: no advisory, not tracking it, or a frozen pose.
                        if (onFeed && egoTraceLeft > 0) {
                            --egoTraceLeft;
                            const auto& cmd = backend.lastEgoCommand();
                            std::cout << "[ego] t=" << simTime
                                      << " advisory=" << lastAdvisory << " m/s"
                                      << " measured=" << es.speed << " m/s"
                                      << " thr=" << cmd.throttle << " brk=" << cmd.brake
                                      << " steer=" << cmd.steer
                                      // A mirrored TWIN of the ego would be teleported
                                      // through the physics ego every tick and knock it
                                      // about; the core is supposed to skip the ego's
                                      // SUMO echo entirely, so this must stay 0.
                                      << " mirroredTwin=" << core.mappedVehicles().count(egoId)
                                      << " pos=(" << es.x << ", " << es.y << ", " << es.z << ")\n";
                        }
                        // (the SUMO-view row is logged once per feed below, for BOTH
                        // modes -- see the "ego_sumo" block after the readback loop)
                    }
                    if (spectatorFollow && egoId == centeredViewId && backend.egoActor()) {
                        carla::geom::Transform eTf = backend.egoActor()->GetTransform();
                        carla::geom::Location loc = eTf.location; loc.z += spectatorHeight;
                        const float yaw = spectatorAlignYaw ? (eTf.rotation.yaw - 90.f) : -90.f;
                        spectator->SetTransform(carla::geom::Transform(loc, carla::geom::Rotation(-90.f, yaw, 0.f)));
                    }
                }
            }

            // ---- POST-tick: interested-id readback (feed) ----------------------
            // The spectator used to be snapped here too; it now runs pre-tick, see
            // the PRE-tick block above (#254).
            const auto& mapped = core.mappedVehicles();
            for (const std::string& iid : interestedIds) {
                if (carlaOwnsEgo && iid == egoId) continue;   // ego handled above (never mapped)
                auto mit = mapped.find(iid);
                if (mit == mapped.end()) continue;
                carla::SharedPtr<carla::client::Vehicle> actor = backend.actorOf(mit->second);
                if (!actor) continue;
                carla::geom::Transform cTf = actor->GetTransform();

                // OBSERVING is not COUPLING: the record is built and LOGGED every
                // feed; only the push back into FIXS is gated on ego ownership.
                // In a control run this loop is the only thing that sees the ego,
                // and logging it is how the mirror gets verified (#305).
                if (onFeed) {
                    carla::geom::Vector3D ext = actor->GetBoundingBox().extent;
                    carla::geom::Vector3D vel = actor->GetVelocity();
                    carla::geom::Transform sTf = BridgeHelper::map_transfrom_Carla_to_Sumo(cTf, ext);
                    VehFullData_t d;
                    d.id = iid; d.type = "ego";
                    d.speed = (float)std::sqrt(vel.x * vel.x + vel.y * vel.y);
                    d.speedDesired = (float)std::sqrt(vel.x * vel.x + vel.y * vel.y);
                    d.positionX = sTf.location.x; d.positionY = sTf.location.y; d.positionZ = sTf.location.z;
                    d.heading = sTf.rotation.yaw; d.grade = (float)(sTf.rotation.pitch * M_PI / 180.0);
                    if (virEnvOwnsEgo)
                        core.Msg_c.VehDataSend_um[core.Sock_c.serverSock[sock0]].push_back(d);
                    if (dataLog.isOpen() && logWanted(d.id)) dataLog.logVehicle(simTime, d);
                }
            }

            // ---- the SUMO-VIEW ego, once per feed, in BOTH modes ----------------
            // What SUMO reports back this feed, on the same clock as the Carla row
            // above, logged as "ego_sumo" so the two are directly comparable. In a
            // synchronous co-sim they should agree; a gap is the mirror drifting.
            if (onFeed && dataLog.isOpen()) {
                auto itSumo = core.Msg_c.VehDataRecv_um.find(egoId);
                if (itSumo != core.Msg_c.VehDataRecv_um.end()) {
                    VehFullData_t dsumo = itSumo->second;
                    dsumo.id = "ego_sumo";
                    if (logWanted(dsumo.id)) dataLog.logVehicle(simTime, dsumo);
                }
            }

            // ---- driver owns the send: once per FIXS feed (pairs with the recv) ----
            if (onFeed && core.ENABLE_REALSIM) {
                if (core.Sock_c.sendData(core.Sock_c.serverSock[sock0], sock0, (float)simTime, 1, core.Msg_c) < 0) {
                    if (WSAGetLastError() != WSAEINTR && WSAGetLastError() != WSAEFAULT)
                        std::cerr << "send to traffic layer failed\n";
                    break;
                }
            }
            if (onFeed) {
                core.Msg_c.clearRecvStorage();
                core.Msg_c.clearSendStorage();
            }
            if (onFeed) feedCount++;
            phBack += _el(_t0);
            simTime = (++stepCount) * carlaStep;   // step counter avoids fp drift

            // Realtime pacing (viz): sleep so each sub-tick lands at its wall-clock
            // sim-time -> the sub-ticks spread evenly instead of bursting, so a
            // follow-cam renders smooth. Never over-throttles (if we fell behind,
            // sleep is skipped and the reference resyncs). OFF for XIL.
            _t0 = _clk();
            if (realtimePacing) {
                using namespace std::chrono;
                auto target = wallStart + duration_cast<steady_clock::duration>(duration<double>(simTime));
                auto now = steady_clock::now();
                if (now < target) std::this_thread::sleep_until(target);
                else if (now - target > milliseconds(250))
                    wallStart = now - duration_cast<steady_clock::duration>(duration<double>(simTime));
            }
            phPace += _el(_t0);
        }

        // How fast the bridge actually ran -- the same line the Python bridge
        // prints, so the two are directly comparable. Timing a whole stack (SUMO,
        // TrafficLayer, the controller, a warm-up) and attributing the difference
        // to the bridge is how that question gets answered wrong.
        {
            const double el = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - loopStart).count();
            if (el > 0 && stepCount)
                std::printf("Bridge loop: %ld exchanges, %ld ticks in %.1f s "
                            "(%.1f exchanges/s, %.2f ms/tick)\n",
                            feedCount, stepCount, el, feedCount / el,
                            1000.0 * el / stepCount);
            // Same phases, same order, same units as the Python bridge prints, so a
            // difference lands on a part of the workflow rather than on a language.
            const double phs[7] = { phRecv, phTick, phFlush, phOrch, phAudit, phBack, phPace };
            const char*  nms[7] = { "fixs recv", "world.tick", "flush batch",
                                    "orchestrate", "z audit", "readback+send",
                                    "pacing sleep" };
            double acct = 0;
            for (int q = 0; q < 7; q++) {
                acct += phs[q];
                std::printf("             %-14s %7.2f ms/tick  %4.1f%%%%\n",
                            nms[q], 1000.0 * phs[q] / stepCount, 100.0 * phs[q] / el);
            }
            std::printf("             %-14s %7.2f ms/tick  %4.1f%%%%  (loop overhead)\n",
                        "unaccounted", 1000.0 * (el - acct) / stepCount,
                        100.0 * (el - acct) / el);
        }
        if (dataLog.isOpen()) { std::cout << "DataLogger closed: " << dataLog.path() << "\n"; dataLog.close(); }
        if (carlaOwnsEgo) backend.destroyEgo();
        settings = world.GetSettings();
        if (settings.synchronous_mode) {
            settings.synchronous_mode = false;
            world.ApplySettings(settings, 1s);
        }
    }
    catch (const carla::client::TimeoutException& e) { std::cerr << "\n" << e.what() << "\n"; return 1; }
    catch (const std::exception& e) { std::cerr << "\nException: " << e.what() << "\n"; return 2; }
    return 0;
}
