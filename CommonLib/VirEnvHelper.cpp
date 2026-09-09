#include "VirEnvHelper.h"

#include <CarMaker.h>

using namespace std;

VirEnvHelper::VirEnvHelper() {}

int VirEnvHelper::CM_Log(const char* MsgChar)    { Log(MsgChar); return 0; }
int VirEnvHelper::CM_LogErrF(const char* MsgChar) { LogErrF(EC_General, MsgChar); return 0; }

int VirEnvHelper::initialization(const char** errorMsg, const char* configPath,
                                 const char* signalTablePath) {
    veryFirstStep = 0;
    core_.setBackend(&backend_);
    core_.interpolateTraffic = true;   // CarMaker sub-steps -> interpolate

#ifndef RS_DSPACE
    if (Config_c.getConfig(configPath) < 0) {
        *errorMsg = "RealSim: Read Configuration Yaml File Failed";
        return virenv::VirEnvCore::ERROR_INIT_READ_CONFIG;
    }
    core_.Msg_c.getConfig(Config_c);

    core_.ENABLE_REALSIM              = Config_c.CarMakerSetup.EnableCosimulation;
    core_.ENABLE_SEPARATE_EGO_TRAFFIC = Config_c.CarMakerSetup.EnableEgoSimulink;
    core_.SYNCHRONIZE_TRAFFIC_SIGNAL  = Config_c.CarMakerSetup.SynchronizeTrafficSignal;
    core_.egoId_              = Config_c.EgoSetup.Id;
    core_.egoType_            = Config_c.CarMakerSetup.EgoType;
    core_.trafficLayerIP_     = Config_c.SimulationSetup.TrafficLayerIP;
    core_.vehDataPort_        = Config_c.CarMakerSetup.CarMakerPort;
    core_.trafficSignalPort_  = Config_c.CarMakerSetup.TrafficSignalPort;
    core_.trafficRefreshRate_ = Config_c.CarMakerSetup.TrafficRefreshRate;
#else
    // dSPACE: Config_s was filled field-by-field by VirEnv_readConfig.
    core_.Msg_c.VehicleMessageField_v = Config_s.VehicleMessageField_v;
    for (const auto& fld : Config_s.VehicleMessageField_v)
        core_.Msg_c.VehicleMessageField_set.insert(fld);

    core_.ENABLE_REALSIM              = Config_s.EnableCosimulation;
    core_.ENABLE_SEPARATE_EGO_TRAFFIC = Config_s.EnableEgoSimulink;
    core_.SYNCHRONIZE_TRAFFIC_SIGNAL  = Config_s.SynchronizeTrafficSignal;
    core_.egoId_              = Config_s.EgoId;
    core_.egoType_            = Config_s.EgoType;
    core_.trafficLayerIP_     = Config_s.TrafficLayerIP;
    core_.vehDataPort_        = Config_s.CarMakerPort;
    core_.trafficSignalPort_  = Config_s.TrafficSignalPort;
    core_.trafficRefreshRate_ = Config_s.TrafficRefreshRate;
#endif

    return core_.initialization(errorMsg, configPath, signalTablePath);
}

int VirEnvHelper::runStep(double simTime, const char** errorMsg) {
    return core_.runStep(simTime, errorMsg);
}

void VirEnvHelper::shutdown() {
    core_.shutdown();
    veryFirstStep = 1;
}
