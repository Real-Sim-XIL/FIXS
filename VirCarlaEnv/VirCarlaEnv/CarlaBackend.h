#pragma once
//============================================================================
//  CarlaBackend  (#174 -- the Carla half of the verb interface)
//----------------------------------------------------------------------------
//  Implements IVirEnvBackend against the Carla C++ client API. The only file on
//  the Carla path that needs the Carla SDK -- VirEnvCore stays SDK-free. Every
//  verb is a lift of the per-vehicle Carla code that used to live inline in
//  mainVirCarla's loop (TrySpawnActor/Destroy, BridgeHelper transforms, batched
//  ApplyTransform, the junction TLS dispatch). The thin mainVirCarla driver owns
//  the world tick + batch flush + interested-vehicle readback + spectator.
//============================================================================

#include "../../CommonLib/IVirEnvBackend.h"
#include "../../CommonLib/EgoDriver.h"
#include "BridgeHelper.h"

#include <carla/client/World.h>
#include <carla/client/Client.h>
#include <carla/client/BlueprintLibrary.h>
#include <carla/client/Vehicle.h>
#include <carla/client/Map.h>
#include <carla/client/Waypoint.h>
#include <carla/rpc/Command.h>
#include <carla/Memory.h>

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

namespace virenv {

class CarlaBackend : public IVirEnvBackend {
public:
    CarlaBackend(carla::client::World* world, carla::client::Client* client,
                 bool useVehicleTypeAsBlueprint, bool verbose)
        : world_(world), client_(client),
          useVType_(useVehicleTypeAsBlueprint), verbose_(verbose) {}

    void log(const char* msg) override;
    void logError(const char* msg) override;

    void loadSignalTable(const char* path) override;
    void initTrafficPool() override;                                       // lazy spawn -> caches blueprint lib

    VehHandle spawnVehicle(const std::string& vType, const std::string& vClass,
                           const Pose& spawnPose) override;
    void      despawnVehicle(VehHandle h) override;

    void setVehiclePose(VehHandle h, const Pose& p) override;              // -> batched ApplyTransform
    void setVehicleLights(VehHandle, bool, bool, bool) override {}         // mainVirCarla sets no lights
    void parkSpares() override {}                                          // no spare pool in Carla

    void syncTrafficLight(const std::string& junctionId, const std::string& stateStr) override;

    // ---- SUMO<->CARLA z-alignment audit (#193 placeholder) -----------------
    // Compares the last pose applied to each mapped vehicle against the CARLA road
    // surface under it, and reports past tolerance through the XIL guard. Called by
    // the driver ONCE PER FIXS EXCHANGE, not per pose: it tests whether the two maps
    // agree on elevation, which cannot change between interpolated sub-steps of the
    // same SUMO sample. It used to live inside setVehiclePose, so a sub-stepping run
    // paid a whole-map waypoint search per vehicle per tick (10x at CarlaTimeStep
    // 0.01) to re-answer a question with the same answer.
    void auditZAlignment();

    // mode A (EgoMode >= 1): read the Carla-driven ego actor back in FIXS terms.
    // Returns false when no ego actor is owned (EgoMode 0 -- driver readback only).
    bool readEgoState(const std::string&, EgoState& out) override;

    // unified RS_DEBUG: append the APPLIED Carla transform to the core's csv row
    std::string debugHeader() const override;
    std::string debugFields(VehHandle h) const override;

    // ---- L0+ ego ownership (EgoMode >= 1) ----------------------------------
    // Spawn the ego with PHYSICS ON. spawnPose is the raw FIXS pose. The driver
    // (native TM or the EgoDriver module) is wired separately below. Returns
    // kNoHandle on failure.
    VehHandle spawnEgo(const std::string& blueprintId, const Pose& spawnPose, int tmPort);

    // NATIVE L0: hand the ego to Carla's Traffic Manager (server-side autopilot).
    // Requires a routable map (see the simple_loop junction-id fix). targetSpeed
    // in m/s (TM's SetDesiredSpeed is km/h -- converted here).
    void enableEgoTM(int tmPort, double targetSpeedMps);

    // FALLBACK L0: feed the map-agnostic EgoDriver module a closed route (the
    // CM-Route analog). fixsPts are raw FIXS-frame [x,y]; the Carla Y-flip is
    // applied here so the module stays frame-neutral.
    void setEgoRoute(const std::vector<std::pair<double, double>>& fixsPts,
                     int repeat, int tmPort);
    void stepEgoDriver(double targetSpeed);   // per-tick: EgoDriver -> ApplyControl

    // What stepEgoDriver last applied, and the speed it was chasing. "The ego
    // did not move" is the same symptom whether the driver commanded nothing,
    // commanded the wrong thing, or commanded correctly into a vehicle that could
    // not act on it -- these separate the three.
    const DriveCommand& lastEgoCommand() const { return lastEgoCmd_; }
    double              lastEgoTarget()  const { return lastEgoTarget_; }

    // #174 unified EgoDriver: apply an ACTUATION command supplied by an external FIXS
    // client (EgoDriver client for L0/L2, a real controller for L4). throttle/brake in
    // [0,1], steerNorm in [-1,1]. Carla owns no in-process driver in this mode.
    void applyEgoActuation(double throttle, double brake, double steerNorm);

    // L2 (EgoMode >= 2): advise the active L0 driver of an EXTERNAL desired speed
    // (m/s), overriding the static EgoTargetSpeed until changed. Native TM path ->
    // tm.SetDesiredSpeed; EgoDriver fallback -> the per-tick target used by
    // stepEgoDriver. No-op if no ego is owned. This is the actuation seam; the
    // advisory SOURCE (wire field / external client) is the caller's concern.
    void applyEgoControl(const std::string& egoId, double desiredSpeed) override;

    void destroyEgo();
    carla::SharedPtr<carla::client::Vehicle> egoActor() { return egoActor_; }

    // ---- driver hooks (not part of IVirEnvBackend) ------------------------
    void flushBatch();                                                     // ApplyBatchSync(the transform commands)
    // Put ANY actor's transform into this tick's batch -- used for the spectator,
    // so the camera lands in the same atomic apply as the vehicles it follows
    // rather than in a separate RPC that can miss the tick.
    void queueTransform(carla::rpc::ActorId id, const carla::geom::Transform& tf);
    void freezeAndMatchTrafficLights();                                    // map traffic.traffic_light actors -> junctions
    carla::SharedPtr<carla::client::Vehicle> actorOf(VehHandle h);         // for interested readback / spectator

    // expose for the driver's TLS / interested handling
    std::unordered_map<std::string, std::unordered_map<int, TrafficLight>>& trafficLightMap() { return trafficLightMap_; }

    // #174 A/B instrumentation: the Carla transform last APPLIED to a handle this
    // tick (post BridgeHelper transform). The driver logs it keyed by SUMO id so
    // old-vs-new can be diffed exactly, with no Carla-readback / blueprint / sort
    // confounds. Returns nullptr if the handle has no applied pose yet.
    const carla::geom::Transform* lastAppliedPose(VehHandle h) const {
        auto it = lastApplied_.find(h);
        return (it != lastApplied_.end()) ? &it->second : nullptr;
    }

private:
    carla::client::World*  world_  = nullptr;
    carla::client::Client* client_ = nullptr;
    bool useVType_  = false;
    bool verbose_   = false;

    carla::SharedPtr<carla::client::BlueprintLibrary> bpLib_;
    carla::SharedPtr<carla::client::Map> map_;                             // cached for the z-alignment guard
    carla::SharedPtr<carla::client::Vehicle> egoActor_;                    // EgoMode >= 1: the Carla-driven ego
    EgoDriver egoDriver_;                                                  // fallback L0 driver (SDK-free module)
    int    tmPort_ = 0;                                                    // TM instance port (set by enableEgoTM)
    bool   egoUsesTM_ = false;                                            // true: native TM drives the ego; false: EgoDriver
    double egoDesiredOverride_ = -1.0;                                    // L2 advisory target (m/s); <0 = none (use static)
    DriveCommand lastEgoCmd_;                                             // last actuation the fallback driver applied
    double       lastEgoTarget_ = -1.0;                                   // ... and the target speed it was chasing
    std::vector<carla::rpc::Command> batch_;                               // ApplyTransform commands this tick
    std::unordered_map<VehHandle, carla::SharedPtr<carla::client::Vehicle>> actors_;
    std::unordered_map<VehHandle, carla::geom::Transform> lastApplied_;   // A/B instrumentation
    std::unordered_map<std::string, std::unordered_map<int, TrafficLight>> trafficLightMap_;
    int zAuditPhase_ = -1;                                                 // rotating slice for the z audit

    static carla::geom::Transform sumoTransformOf(const Pose& p);
};

} // namespace virenv
