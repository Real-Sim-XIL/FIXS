#pragma once


#include <unordered_map>
#include <unordered_set>
#include <tuple>

#include <math.h>

//#include "TraCIAPI.h"
//#define NOMINMAX

#include <libsumo/libtraci.h>

#include "SocketHelper.h"


class TrafficHelper
{

public:
    // constructor
	TrafficHelper();

	void connectionSetup(int nClient);
	void connectionSetup(std::string trafficIp, int trafficPort, int nClientInput);

	void enableVehSub();
	void disableVehSub();
	void enableDetSub();
	void disableDetSub();
	void selectVISSIM(std::vector <int> vissimSockInput, std::vector <std::string> vissimSockNames);
	void selectSUMO();

	void close();

	int sendToTrafficSimulator(double simTime, MsgHelper Msg_c);
	void runOneStepSimulation();
	int runSimulation(double endTime);
	int recvFromTrafficSimulator(double* simTime, MsgHelper& Msg_c);

	int addEgoVehicle(double simTime);

	int checkIfEgoExist(double* simTime);

	int getSimulationTime(double* simTime);

	int sendToSUMO(double simTime, MsgHelper Msg_c);
	int recvFromSUMO(double* simTime, MsgHelper& Msg_c);

	int sendToVISSIM(double simTime, MsgHelper Msg_c);
	int recvFromVISSIM(double* simTime, MsgHelper& Msg_c);

	void parseSendMsg(MsgHelper MsgIn_c, MsgHelper& MsgOut_c);

	//======================
	// Traffic Simulation connection setup
	//======================

	
	//======================
	// This is for VISSIM
	//======================
	std::vector <int> vissimSock;
	//std::vector <int> clientSock;
	std::unordered_map <int, std::string> vissimSockName_um;
	//int vissimSignalStep = 10; // how many timestep get/send once signal detector data 
	//double vissimBaseDt = 0.1; // 0.1 seconds is the base simulation step
	std::string SIGNAL_SOCK_PATTERN = "Signal"; // this we do not need to change

	//======================
	// This is for SUMO
	//======================
	//class Client : public TraCIAPI {
	//public:
	//	Client() {};
	//	~Client() {};
	//};

	//Client traci;

	
	/********************************************
	* GET SUBSCRIPTIONvissimSignalStep
	*********************************************/
	// list of SUMO vehicle data id
	std::vector<int> VehDataSubscribeList;


	std::unordered_set <std::string> edgeSubscribeId_v;

	// vehicle id -> radius
	std::unordered_map <std::string, double > vehicleSubscribeId_v;

	std::pair <bool, double> subscribeAllVehicle = { std::make_pair(false, 0) };

	// tuple x, y, z, radius
	// name of the point -> x, y, z, r
	std::unordered_map <std::string, std::tuple<double, double, double, double> > pointSubscribeId_v;
	//std::vector <std::string> pointNamePoi_v;

	std::unordered_map <std::string, double> vehicleTypeSubscribedId_v;

	bool edgeHasSubscribed = false;
	bool pointHasSubscribed = false;
	bool allVehicleHasSubscribed = false;
	std::vector <bool> vehicleHasSubscribed_v;



	std::vector<std::string> VehIdInSimulator;

	std::vector <std::string> detSelectId_v;

	/********************************************
	* Simulation setups and flags
	*********************************************/


	// will be updated based on inputs to connectionSetup()
	int nClient; // each client of the traffic layer would need a copy of the message

	/********************************************
	* !!! THESE NEEDS TO BE REPLACE BY READING A CONFIG FILE
	*********************************************/
	ConfigHelper* Config_c;

	void getConfig();
	std::vector <std::string> VehicleMessageField_v;
	std::unordered_set <std::string> VehicleMessageField_set;

	bool ENABLE_VEH_SUB = false;
	bool ENABLE_DET_SUB = false;
	bool ENABLE_SIG_SUB = false;

	bool ENABLE_VERBOSE = false;

	bool ENABLE_EXT_DYN = false;

	bool ENABLE_VEH_SIMULATOR = false;

	double tSimuEnd = 90000;


	std::string SUMO_OR_VISSIM = "SUMO"; // will be updated using selectVISSIM() or selectSUMO()

	// NEED TO WORK ON THESE
	//std::string DET_SEL_PATTERN = ""; //"SB75_SB";
	//std::string tlsSelect = ""; //"2881";


	std::string MasterLogName = "";

	//======================
	// This is for SUMO
	//======================
	// speed limit storage
	// look up table: edge, lane, vClass -> speed limit. 
	// this is for every edge in the network
	// A hash function used to hash a tuple
	struct hash_tuple {

		template <class T1, class T2>

		size_t operator()(
			const std::pair<T1, T2>& x)
			const
		{
			std::hash<std::string> h_s{};

			return h_s(std::get<0>(x))
				^ h_s(std::get<1>(x));
		}
	};

	std::unordered_map <std::pair <std::string, std::string>, double, hash_tuple> LaneVehClass2SpeedLimit_um;
	std::unordered_map <std::pair <std::string, std::string>, double, hash_tuple> EdgeVehClass2SpeedLimit_um;

	// vehicle id->edge list
	std::unordered_map<std::string, std::vector<std::string>> VehicleId2EdgeList_um;
	// vehcile id->current edge list index, do not need this as SUMO will give this
	// std::unordered_map<std::string, std::vector<std::string>> VehicleId2EdgeListIdx_um;

	std::unordered_set <std::string> AllEdgeList;

	// counter to only add a vehicle if it has been in the subscribed list for last X seconds
	std::unordered_map <std::string, double> VehicleId2LastSubTime_um;
	std::unordered_map <std::string, int> VehicleId2SubCount_um;

	int shouldSendVehicle(std::string vehicleId, double simTime);

private:
	void parserSumoSubscription(libsumo::TraCIResults VehDataSubscribeTraciResults, std::string vehId, VehFullData_t& CurVehData);

	const double M_PI = 3.14159265358979323846;

	struct SignalSubscriptionFlags_t {
		bool idHasSubscribed = false;
	};
	struct DetectorSubscriptionFlags_t {
		bool patternHasSubscribed = false;
	};

	SignalSubscriptionFlags_t SignalSubscriptionFlags;
	DetectorSubscriptionFlags_t DetectorSubscriptionFlags;

	const double SUB_CONT_TIME_THLD = 1; // has to be in for 10 seconds

	const double SIM_STEP = 0.1;

	const int N_MAX_VEH = 30;
};

