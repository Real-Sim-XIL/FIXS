#pragma once

#include <ctime>
#include <chrono>
#include <queue>

#include "SocketHelper.h"
#include <CarMaker.h>

class RealSimCmHelper
{
public:
    RealSimCmHelper();

    SocketHelper Sock_c;
    MsgHelper Msg_c;

    std::string CmErrorFile = "RealSimCarMaker.err";
    std::string configPath = "config.yaml";

    bool ENABLE_REALSIM = true;

    bool veryFirstStep = true;

    bool ENABLE_SEPARATE_EGO_TRAFFIC = false;

    // map vehicle id from traffic simulator to carmaker
    std::unordered_map <std::string, int> TrafficSimulatorId2CarMakerId;

    // queue to store currently available id of Cm traffic object
    // separate different vehicle classes
    std::queue <int> CmAvailableCarId_queue;
    std::queue <int> CmAvailableTruckId_queue;
    std::queue <int> CmAvailableBusId_queue;

    // store id of vehicles that should be removed from CarMaker
    std::unordered_set <std::string> TrafficSimulatorId2Remove;

    typedef struct {

        double positionX;
        double positionY;
        double positionZ;
        //double heading; // heading in degree, north is 0 degree, then increasing clockwise. i.e., east is 90 degree.
        double pitch; // radian, essentially negative of grade angle of RealSim convention
        double yaw; // radian, rotation along z axis, east is 0, north is pi/2, south is -pi/2. counterclockwise is positive till west, then clockwise is negative till west

    }VehDataAuxiliary_t;

    // these are for interpolation, so need (t, data) type 
    std::unordered_map <std::string, std::pair<double, VehDataAuxiliary_t>> TrafficStatePrevious_um;
    std::unordered_map <std::string, std::pair<double, VehDataAuxiliary_t>> TrafficStateNext_um;




    std::vector <std::string> serverAddr = { "127.0.0.1" };
    std::vector <int> serverPort = { 7331 };

    //std::string egoId = "egoCm";
    //std::string egoId = "egoCm";
    ConfigHelper Config_c;


    std::string RealSimCarNamePattern = "RS_C";
    std::string RealSimTruckNamePattern = "RS_T";

    void shutdown();

    int initialization(std::string& errorMsg);

    int runStep(double simTime, std::string& errorMsg);


    enum InitializationError_enum {
        ERROR_INIT_READ_CONFIG = -1,
        ERROR_INIT_MSG_FIELD = -2,
        ERROR_INIT_SOCKET = -3,
        ERROR_INIT_TRAFFIC = -4
    };

    enum RunStepError_enum {
        ERROR_STEP_RECV_REALSIM = -1,
        ERROR_STEP_MAP_ID = -2,
        ERROR_STEP_REMOVE_ID = -3,
        ERROR_STEP_UPDATE_STATE = -4,
        ERROR_STEP_SEND_EGO = -5,
        ERROR_STEP_REFRESH_TRAFFIC = -6

    };

private:

};

