#include "CarlaBackend.h"
#include "../../CommonLib/XilGuard.h"

#include <carla/client/ActorList.h>
#include <carla/client/ActorBlueprint.h>
#include <carla/client/TrafficLight.h>
#include <carla/geom/Location.h>
#include <carla/geom/Rotation.h>
#include <carla/geom/Vector3D.h>
#include <carla/trafficmanager/TrafficManager.h>
#include <carla/rpc/VehicleControl.h>

#include <iostream>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Tolerance for the SUMO<->CARLA per-vehicle z-alignment guard (#193 placeholder).
// Above this, a teleported car is off the CARLA road surface enough to warn. With
// the densified elevation net the residual is ~mesh discretization (a few cm), so
// 0.5 m warns only on a genuine map/elevation mismatch. Policy/threshold config: #193.
static constexpr double kZMismatchTolM = 0.5;

// How many exchanges the z audit takes to cover every mapped vehicle. Mirrors
// kZAuditStride in Carla/VirEnv/CarlaBackend.py so both bridges sample it the same.
static constexpr int kZAuditStride = 5;

namespace virenv {

// #174 coord fix: the SUMO/FIXS wire carries the FRONT-of-vehicle position; the
// Carla actor transform is the actor PIVOT, which (empirically) sits at the
// bounding-box CENTER horizontally (bbox.location.x ~= 0). So to land the model's
// FRONT on the SUMO front we must step back the actor HALF-length == bbox.extent.x.
// map_transfrom_Sumo_to_Carla(..,extent) does exactly that with extent.x, so the
// CORRECT extent is the actor's real GetBoundingBox().extent -- which the reverse
// readback (Carla->Sumo) already uses, so this also makes forward/reverse symmetric.
//
// The OLD code passed a hard-coded (4.8,2.0,1.8) -- a placeholder for the
// commented-out (length/2,width/2,height/2). 4.8 as a half-length implies a 9.6 m
// vehicle, so every actor was anchored ~2.4 m too far back. kDefaultExtent below is
// only a sane passenger-car fallback for the TRANSIENT spawn transform (the actor's
// bbox isn't queryable until it exists; setVehiclePose overwrites it the same tick).
static const carla::geom::Vector3D kDefaultExtent(2.3f, 1.0f, 0.75f);
static const float kSpawnOffsetZ = 0.1f;

void CarlaBackend::log(const char* msg)      { std::cout << msg << std::endl; }
void CarlaBackend::logError(const char* msg) { std::cerr << msg << std::endl; }

carla::geom::Transform CarlaBackend::sumoTransformOf(const Pose& p) {
    // verbatim from mainVirCarla: Location(x,y,z), Rotation(grade_deg, heading, 0)
    carla::geom::Location loc((float)p.x, (float)p.y, (float)p.z);
    carla::geom::Rotation rot((float)(p.gradeRad * 180.0 / M_PI), (float)p.headingDeg, 0.0f);
    return carla::geom::Transform(loc, rot);
}

void CarlaBackend::loadSignalTable(const char* path) {
    if (path == nullptr || path[0] == '\0') return;
    trafficLightMap_ = BridgeHelper::readTrafficLightTable(path);
}

void CarlaBackend::initTrafficPool() {
    if (!bpLib_ && world_) bpLib_ = world_->GetBlueprintLibrary();
}

VehHandle CarlaBackend::spawnVehicle(const std::string& vType, const std::string& vClass,
                                     const Pose& spawnPose) {
    if (!world_) return kNoHandle;
    if (!bpLib_) bpLib_ = world_->GetBlueprintLibrary();

    // spawn transform is transient: setVehiclePose corrects it with the real bbox
    // the same tick (before world.Tick), so a passenger-car default is sufficient.
    carla::geom::Transform carlaTf = BridgeHelper::map_transfrom_Sumo_to_Carla(sumoTransformOf(spawnPose), kDefaultExtent);
    carlaTf.location.z += kSpawnOffsetZ;

    std::string bpId = useVType_ ? vType : BridgeHelper::map_Sumo_vClass_to_Carla_blueprintId(vClass);
    const carla::client::ActorBlueprint* bp = bpLib_->Find(bpId);
    if (bp == nullptr) {
        logError(("Blueprint not found: " + bpId).c_str());
        return kNoHandle;
    }
    carla::client::ActorBlueprint bpLocal = *bp;   // copy so we can modify

    carla::SharedPtr<carla::client::Actor> actor = world_->TrySpawnActor(bpLocal, carlaTf);
    if (actor == nullptr) {
        if (verbose_) std::cout << "[Warning] Failed to spawn actor (vClass=" << vClass << ")\n";
        return kNoHandle;
    }
    carla::SharedPtr<carla::client::Vehicle> veh =
        boost::static_pointer_cast<carla::client::Vehicle>(actor);
    veh->SetSimulatePhysics(false);

    VehHandle h = (VehHandle)veh->GetId();
    actors_[h] = veh;
    if (verbose_) std::cout << "Spawned Carla actor " << h << " (" << bpId << ")\n";
    return h;
}

void CarlaBackend::despawnVehicle(VehHandle h) {
    auto it = actors_.find(h);
    if (it != actors_.end()) {
        if (it->second) it->second->Destroy();
        actors_.erase(it);
    }
    // Drop the applied-pose record too. It was left behind before, so it grew for the
    // whole run (a thousand-plus arrived vehicles over a few minutes) and the pose of
    // a destroyed actor stayed queryable through lastAppliedPose() - which the
    // per-exchange z audit and the A/B log both read.
    lastApplied_.erase(h);
}

void CarlaBackend::setVehiclePose(VehHandle h, const Pose& p) {
    // Use the actor's REAL half-size so the model FRONT lands on the SUMO front
    // (anchor == bbox.extent.x == half-length). Symmetric with the Carla->Sumo
    // readback, which already uses GetBoundingBox().extent.
    carla::geom::Vector3D ext = kDefaultExtent;
    auto it = actors_.find(h);
    if (it != actors_.end() && it->second) {
        carla::geom::Vector3D e = it->second->GetBoundingBox().extent;
        if (e.x > 0.1f) ext = e;   // guard a not-yet-populated bbox on the spawn frame
    }
    carla::geom::Transform carlaTf = BridgeHelper::map_transfrom_Sumo_to_Carla(sumoTransformOf(p), ext);
    lastApplied_[h] = carlaTf;   // A/B instrumentation + the per-exchange z audit

    // batched, applied in flushBatch() before the world Tick -- same as mainVirCarla
    batch_.push_back(carla::rpc::Command::ApplyTransform((carla::rpc::ActorId)h, carlaTf));
}


//----------------------------------------------------------------------------
//  SUMO<->CARLA z-alignment audit (#193 placeholder).
//
//  A teleported (physics-off) car sits at SUMO's z. If SUMO's road elevation
//  diverges from the CARLA road surface under it -- a coarsely-sampled net against
//  the xodr, or an inconsistent map pair -- the car floats or sinks. So compare the
//  applied pose against the CARLA road and report past tolerance. #174 only warns;
//  the abort / snap-to-road / dyno-invalid policy is #193.
//
//  Called ONCE PER FIXS EXCHANGE by the driver, not from setVehiclePose. This asks
//  whether the two MAPS agree on elevation, and two maps do not start agreeing
//  halfway through a 0.1 s interval - so re-asking it on every interpolated sub-step
//  bought nothing and cost a whole-map waypoint search per vehicle per tick (at
//  CarlaTimeStep 0.01 with a few hundred vehicles, thousands of searches per
//  exchange). The XIL guard's own rate limiting spares only the LOG: the search
//  happens before the guard is ever called.
//----------------------------------------------------------------------------
void CarlaBackend::auditZAlignment() {
    if (!world_) return;
    if (!map_) map_ = world_->GetMap();
    if (!map_) return;
    // Sampled over a rotating slice, not exhaustive. This asks whether the two MAPS
    // agree on elevation -- a STATIC property of the map pair, not of the traffic --
    // so checking every vehicle every exchange re-answers one question hundreds of
    // times per second, each answer costing a whole-map waypoint search. Measured on
    // the Python peer, where the same loop was 9.6 ms of a 42.7 ms tick: 23% of the
    // bridge's own work for a #193 placeholder that only warns.
    //
    // Stride 5 covers every mapped vehicle within 0.5 s at the 0.1 s feed, shorter
    // than the shortest violation observed on the MLK corridor (~5 consecutive
    // exchanges), so nothing that was reported before is missed.
    zAuditPhase_ = (zAuditPhase_ + 1) % kZAuditStride;
    std::size_t n = 0;
    for (const std::pair<const VehHandle, carla::geom::Transform>& kv : lastApplied_) {
        if ((n++ % kZAuditStride) != (std::size_t)zAuditPhase_) continue;
        carla::SharedPtr<carla::client::Waypoint> wp = map_->GetWaypoint(kv.second.location);
        if (wp)
            fixs::RS_XIL_GUARD("sumo_carla_z_mismatch",
                               kv.second.location.z - wp->GetTransform().location.z,
                               kZMismatchTolM);
    }
}

void CarlaBackend::flushBatch() {
    // ApplyBatchSync, not ApplyBatch: ApplyBatch returns before the server has
    // applied the commands, and the very next thing the loop does is Tick(), so
    // the bridge raced its own message and rendered vehicles a step behind.
    if (client_) client_->ApplyBatchSync(batch_, false);
    batch_.clear();
}

void CarlaBackend::queueTransform(carla::rpc::ActorId id, const carla::geom::Transform& tf) {
    batch_.push_back(carla::rpc::Command::ApplyTransform(id, tf));
}

void CarlaBackend::syncTrafficLight(const std::string& junctionId, const std::string& stateStr) {
    auto jit = trafficLightMap_.find(junctionId);
    if (jit == trafficLightMap_.end()) return;
    for (size_t linkId = 0; linkId < stateStr.size(); ++linkId) {
        SumoTrafficLightState ss = BridgeHelper::get_Sumo_traffic_light_state_from_char(stateStr[linkId]);
        carla::rpc::TrafficLightState cs = BridgeHelper::map_Sumo_traffic_light_state_to_Carla(ss);
        auto lit = jit->second.find((int)linkId);
        if (lit != jit->second.end() && lit->second.carlaTrafficLightActorPtr)
            lit->second.carlaTrafficLightActorPtr->SetState(cs);
    }
}

void CarlaBackend::freezeAndMatchTrafficLights() {
    if (!world_) return;
    carla::SharedPtr<carla::client::ActorList> tlActors =
        world_->GetActors()->Filter("traffic.traffic_light");
    for (const carla::SharedPtr<carla::client::Actor>& actor : *tlActors) {
        carla::SharedPtr<carla::client::TrafficLight> tlPtr =
            boost::static_pointer_cast<carla::client::TrafficLight>(actor);
        tlPtr->Freeze(true);
        std::string aid = std::to_string(actor->GetId());
        carla::geom::Location sloc = BridgeHelper::map_location_Carla_to_Sumo(actor->GetLocation());
        std::pair<std::string, int> idp =
            BridgeHelper::find_closest_trafficLight_id(trafficLightMap_, sloc.x, sloc.y);
        if (idp.first.empty()) continue;
        TrafficLight& tl = trafficLightMap_[idp.first][idp.second];
        tl.carlaTrafficLightActorId = aid;
        tl.carlaTrafficLightActorPtr = tlPtr;
    }
}

carla::SharedPtr<carla::client::Vehicle> CarlaBackend::actorOf(VehHandle h) {
    auto it = actors_.find(h);
    return (it != actors_.end()) ? it->second : nullptr;
}

//----------------------------------------------------------------------------
//  L0+ ego ownership (EgoMode >= 1): Carla drives the ego.
//----------------------------------------------------------------------------
VehHandle CarlaBackend::spawnEgo(const std::string& blueprintId, const Pose& spawnPose, int tmPort) {
    if (!world_) return kNoHandle;
    if (!bpLib_) bpLib_ = world_->GetBlueprintLibrary();

    const carla::client::ActorBlueprint* bp = bpLib_->Find(blueprintId);
    if (bp == nullptr) {
        logError(("Ego blueprint not found: " + blueprintId).c_str());
        return kNoHandle;
    }
    carla::client::ActorBlueprint bpLocal = *bp;

    carla::geom::Transform tf = BridgeHelper::map_transfrom_Sumo_to_Carla(sumoTransformOf(spawnPose), kDefaultExtent);
    tf.location.z += 0.3f;   // drop-in margin: physics ON -> the car settles on its tires

    carla::SharedPtr<carla::client::Actor> actor = world_->TrySpawnActor(bpLocal, tf);
    if (actor == nullptr) {
        logError("Ego spawn failed (spawn point blocked?)");
        return kNoHandle;
    }
    egoActor_ = boost::static_pointer_cast<carla::client::Vehicle>(actor);
    egoActor_->SetSimulatePhysics(true);                 // full PhysX: tire contact, dynamics
    (void)tmPort;  // driver (TM autopilot or the EgoDriver module) is wired separately, post-settle
    std::cout << "L0 ego spawned: " << blueprintId << " actor " << egoActor_->GetId()
              << " (physics ON)\n";
    return (VehHandle)egoActor_->GetId();
}

void CarlaBackend::enableEgoTM(int tmPort, double targetSpeedMps) {
    // NATIVE L0: Carla's server-side Traffic Manager drives the ego. Works once
    // the map's OpenDRIVE junction topology is routable (the simple_loop
    // junction-id / road-id collision that broke this is fixed in the generator).
    // Spawn + physics must already be done; TM must be synchronous in a sync
    // world -- world.Tick() then drives TM's SynchronousTick on the in-process
    // instance. Sequence mirrors the verified standalone probe.
    if (!egoActor_ || !client_) return;
    auto tm = client_->GetInstanceTM((uint16_t)tmPort);
    tm.SetSynchronousMode(true);
    std::vector<carla::SharedPtr<carla::client::Actor>> one{ egoActor_ };
    tm.RegisterVehicles(one);
    egoActor_->SetAutopilot(true, (uint16_t)tmPort);
    tm.SetDesiredSpeed(egoActor_, (float)(targetSpeedMps * 3.6));   // TM speed is km/h
    tmPort_ = tmPort;          // remembered so applyEgoControl (L2) can re-fetch the TM instance
    egoUsesTM_ = true;         // native TM path -> applyEgoControl uses SetDesiredSpeed
    std::cout << "L0 ego: NATIVE Traffic Manager autopilot (TM port " << tmPort
              << ", target " << targetSpeedMps << " m/s)\n";
}

void CarlaBackend::setEgoRoute(const std::vector<std::pair<double, double>>& fixsPts,
                               int /*repeat*/, int /*tmPort*/) {
    // FALLBACK L0: hand the closed route to the SDK-free EgoDriver module. The
    // only Carla-specific step here is the frame conversion (FIXS -> Carla is a
    // Y flip); the module then owns densification + the pursuit control law.
    if (fixsPts.empty()) return;
    std::vector<std::pair<double, double>> carlaPts;
    carlaPts.reserve(fixsPts.size());
    for (const std::pair<double, double>& p : fixsPts)
        carlaPts.emplace_back(p.first, -p.second);       // FIXS -> Carla planar frame
    egoDriver_.setRoute(carlaPts, /*closed=*/true);
    std::cout << "L0 ego route: " << fixsPts.size() << " waypoints -> "
              << egoDriver_.routeSize() << " path points (EgoDriver fallback module)\n";
}

void CarlaBackend::driveEgoFallback(double targetSpeed) {
    // Per-tick fallback driver: read the ego pose in the Carla frame, ask the
    // module for a neutral DriveCommand, apply it through full PhysX dynamics.
    if (!egoActor_ || !egoDriver_.hasRoute()) return;
    carla::geom::Transform tf = egoActor_->GetTransform();
    carla::geom::Vector3D  vel = egoActor_->GetVelocity();
    const double v = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    const double yawRad = tf.rotation.yaw * M_PI / 180.0;

    // L2: an external advisory (egoDesiredOverride_ >= 0) supersedes the static
    // cruise target passed in; otherwise use the configured EgoTargetSpeed.
    const double tgt = (egoDesiredOverride_ >= 0.0) ? egoDesiredOverride_ : targetSpeed;
    DriveCommand dc = egoDriver_.computeControl(tf.location.x, tf.location.y, yawRad, v, tgt);

    carla::rpc::VehicleControl c;
    c.throttle = (float)dc.throttle;
    c.brake    = (float)dc.brake;
    c.steer    = (float)dc.steer;
    egoActor_->ApplyControl(c);

    // Remember what was applied, and the target it was chasing. Without this the
    // only observable is "the ego did not move" -- which is the same symptom for
    // a driver commanding nothing, a driver commanding the wrong thing, and a
    // vehicle that cannot act on what it was told. Reported on the ego's own
    // record so it lands in whatever log is already collecting it.
    lastEgoCmd_ = dc;
    lastEgoTarget_ = tgt;
}

void CarlaBackend::applyEgoActuation(double throttle, double brake, double steerNorm) {
    // #174 unified EgoDriver apply-path: the actuation comes from an external FIXS
    // client (EgoDriver client / L4 controller) via the ego's wire record; Carla just
    // realizes it on the physics ego. Same VehicleControl seam as driveEgoFallback.
    if (!egoActor_) return;
    carla::rpc::VehicleControl c;
    c.throttle = (float)std::max(0.0, std::min(1.0, throttle));
    c.brake    = (float)std::max(0.0, std::min(1.0, brake));
    c.steer    = (float)std::max(-1.0, std::min(1.0, steerNorm));
    egoActor_->ApplyControl(c);
    // Record what was APPLIED. The datalog asks the backend what the ego was
    // commanded this tick; who produced the command is not its business.
    lastEgoCmd_.throttle = c.throttle;
    lastEgoCmd_.brake    = c.brake;
    lastEgoCmd_.steer    = c.steer;
}

void CarlaBackend::applyEgoControl(const std::string& /*egoId*/, double desiredSpeed) {
    // L2 actuation seam: route an EXTERNAL desired-speed advisory to whichever L0
    // driver owns the ego. Native TM -> SetDesiredSpeed (km/h) on the ego's TM
    // instance; EgoDriver fallback -> stash the target for the next driveEgoFallback
    // tick. No ego -> nothing to advise.
    if (!egoActor_) return;
    egoDesiredOverride_ = desiredSpeed;
    if (egoUsesTM_ && client_) {
        auto tm = client_->GetInstanceTM((uint16_t)tmPort_);
        tm.SetDesiredSpeed(egoActor_, (float)(desiredSpeed * 3.6));   // TM speed is km/h
    }
}

void CarlaBackend::destroyEgo() {
    if (egoActor_) {
        egoActor_->Destroy();
        egoActor_ = nullptr;
    }
}

bool CarlaBackend::readEgoState(const std::string& /*egoId*/, EgoState& out) {
    if (!egoActor_) return false;   // EgoMode 0: no owned ego (driver readback path)
    carla::geom::Transform cTf = egoActor_->GetTransform();
    carla::geom::Vector3D  ext = egoActor_->GetBoundingBox().extent;
    carla::geom::Vector3D  vel = egoActor_->GetVelocity();
    carla::geom::Transform sTf = BridgeHelper::map_transfrom_Carla_to_Sumo(cTf, ext);
    out.x = sTf.location.x; out.y = sTf.location.y; out.z = sTf.location.z;
    out.heading = sTf.rotation.yaw;
    out.grade   = sTf.rotation.pitch * M_PI / 180.0;
    out.speed   = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    return true;
}

std::string CarlaBackend::debugHeader() const {
    return ",carla_x,carla_y,carla_z,carla_yaw,carla_pitch";
}

std::string CarlaBackend::debugFields(VehHandle h) const {
    auto it = lastApplied_.find(h);
    if (it == lastApplied_.end()) return ",,,,,";
    const carla::geom::Transform& t = it->second;
    char buf[160];
    std::snprintf(buf, sizeof(buf), ",%.4f,%.4f,%.4f,%.4f,%.4f",
                  t.location.x, t.location.y, t.location.z, t.rotation.yaw, t.rotation.pitch);
    return std::string(buf);
}

} // namespace virenv
