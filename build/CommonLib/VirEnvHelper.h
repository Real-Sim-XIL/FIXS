#pragma once

#include <ctime>
#include <chrono>
#include <queue>

#ifdef RS_DSPACE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#endif

#include "SocketHelper.h"
#include <CarMaker.h>

//namespace RealSimVirEnv
//{

    class VirEnvHelper
    {
    public:
        VirEnvHelper();

        SocketHelper Sock_c;
        MsgHelper Msg_c;

        std::string CmErrorFile = "RealSimCarMaker.err";
        std::string configPath = "config.yaml";

        bool ENABLE_REALSIM = true;

        int veryFirstStep = 1;

        // if this flag is true, then ego send to simulink, traffic send to this c code
        bool ENABLE_SEPARATE_EGO_TRAFFIC = false;

        bool SYNCHRONIZE_TRAFFIC_SIGNAL = true;


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



        std::vector <std::string> serverAddr = {};
        std::vector <int> serverPort = {};

#ifndef RS_DSPACE
        ConfigHelper Config_c;
#else
        typedef struct{
            std::vector <std::string> VehicleMessageField_v;
            bool EnableCosimulation;
            bool EnableEgoSimulink;
            std::string EgoId;
            std::string EgoType;
            std::string TrafficLayerIP;
            int CarMakerPort;
            double TrafficRefreshRate;
            int TrafficSignalPort;
            bool SynchronizeTrafficSignal;
            std::string SignalTableFilename;
        }Config_t;

        Config_t Config_s;

#endif

        std::string RealSimCarNamePattern = "RS_C";
        std::string RealSimTruckNamePattern = "RS_T";

		//
        void shutdown();

//#ifndef RS_DSPACE
        int initialization(const char** errorMsgChar, const char* configPathInput, const char* signalTablePathInput);
//#else
//        int initialization(const char** errorMsgChar, const char* signalTablePathInput);
//#endif

        int runStep(double simTime, const char** errorMsgChar);
		//

		int CM_Log(const char* MsgChar);
		int CM_LogErrF(const char* MsgChar);

        enum InitializationError_enum {
            ERROR_INIT_READ_CONFIG = -1,
            ERROR_INIT_MSG_FIELD = -2,
            ERROR_INIT_SOCKET = -3,
            ERROR_INIT_TRAFFIC = -4,
        };

        enum RunStepError_enum {
            ERROR_STEP_RECV_REALSIM = -1,
            ERROR_STEP_MAP_ID = -2,
            ERROR_STEP_REMOVE_ID = -3,
            ERROR_STEP_UPDATE_STATE = -4,
            ERROR_STEP_SEND_EGO = -5,
            ERROR_STEP_REFRESH_TRAFFIC = -6,
            ERROR_STEP_SYNC_TRAFFIC_SIGNAL = -7

        };

    private:
        // signal controller map
        // sumo controller id -> head id, trflight index
        std::unordered_map <std::string, std::vector <std::pair<int, int>> > SignalController2HeadIdTrfLightIndex;

        tTLState tlsChar2CmState(char charState);

        int readSignalTable(const char* signalTablePathInput);

        typedef struct  {
            std::string signalControllerName;
            int signalGroupId;
            int signalHeadId;
            int cmTrafficLightIndex;
            std::string cmControllerId;
        } SignalTable_t;

    };


    //VirEnvHelper VirEnv_c;

//}