#pragma once


#include <unordered_map>
#include <unordered_set>
#include <set>
#include <tuple>

#include <math.h>

//#define ENABLE_LIBSUMO

#ifdef ENABLE_LIBSUMO
#include <libsumo/libsumo.h>
#define SUMO_TRACI_NAMESPACE libsumo
#else
#include <libsumo/libtraci.h>
#define SUMO_TRACI_NAMESPACE libtraci
#endif
#include "SocketHelper.h"
#include "XilGuard.h"


class TrafficHelper
{

public:
    // constructor
	TrafficHelper();

	void connectionSetup(int nClient);
	void connectionSetup(std::string trafficIp, int trafficPort, int nClientInput, int order);

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
	// Ids the warm-up watches for (#86) -- the union of both layers' by-id vehicle
	// subscriptions, copied from ConfigHelper::WarmUpEgoIds in getConfig().
	std::unordered_set <std::string> warmupEgoId_v;
	int recvFromTrafficSimulator(double* simTime, MsgHelper& Msg_c);

	int addEgoVehicle(double simTime);
	int addEgoVehicleFromXY(double simTime, std::string vehicleId, std::string vehicleType, double positionX, double positionY);

	// #305 Which layer owns this vehicle's motion, if the traffic simulator does
	// not? CarMaker/XIL and the virtual environment (Carla) are the SAME situation
	// from SUMO's side -- a vehicle it holds but does not drive, whose position is
	// written in from outside every tick. They were two branches only because two
	// config sections named the id. One test now answers it for both, and one body
	// in sendToSUMO mirrors whatever it returns.
	//
	// The order inside matters: when the CarMakerSetup section is absent,
	// CarMakerSetup.EgoId is inferred from the lone subscription and can equal the
	// Carla ego id. VirEnv ownership is the more specific condition (it also needs
	// EnableExternalControl and the id in InterestedIds), so it wins.
	enum class ExternalEgoOwner { None, VehSimulator, VirEnv };
	ExternalEgoOwner externalEgoOwnerOf(const std::string& vehId) const;
	bool isWarmUpEgoInNetwork(double* simTime);

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
	// Using libtraci - no custom client class needed

	
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

	bool ENABLE_CARLA = false;
	bool ENABLE_CARLA_EXTERNAL_CONTROL = false;
	// Externally-driven ego ids already added to the traffic simulator (add ONCE,
	// then wait for insertion; see the mirror block in sendToSUMO).
	std::set<std::string> externalEgoInjected_;
	// #174: last (x,y) fed to moveToXY per externally-driven ego -- lets the
	// off-map guard compare SUMO's placement (getPosition, n-1) to what we asked
	// for last tick.
	std::unordered_map<std::string, std::pair<double, double>> externalEgoLastFedXY_;

	// #177: skip per-vehicle TraCI getters whose output is never sent. Derived
	// once from VehicleMessageField_set in connectionSetup -- getNextTLS is only
	// needed for the signalLight* fields, getLeader/getSpeed for precedingVehicle*.
	bool NEED_NEXT_TLS = false;
	bool NEED_PRECEDING_VEH = false;

#ifdef RS_DEBUG
	// #86: how many vehicles per reporting window still needed a DIRECT TraCI
	// query for their leader instead of reading it out of the subscription.
	// A direct query is Vehicle::getLeader / getSpeed -- the same call every
	// vehicle made before #86, so the answer is identical; only the transport
	// differs. Counts, hence the n prefix and a 0 start.
	//
	// Reported every 3000 steps: a config that quietly queries every vehicle is
	// paying the pre-#86 cost, which is invisible otherwise -- the output is
	// still correct, only slower. Build with -p:RS_DEBUG=1 to see it.
	long nLeaderIdQueried = 0;
	long nLeaderSpeedQueried = 0;
	long nStepsSinceLeaderReport = 0;
#endif

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

	// #177 Phase 2: cache the upcoming-TLS list so getNextTLS (O(remaining route
	// length)) runs ONCE PER VEHICLE instead of every step. A vehicle's getNextTLS
	// captures every TLS from its current position to route end; it only ever PASSES
	// those, never gains new ones, so one seed is valid for its whole route. cumDist =
	// getNextTLS dist + odometer at seed (route-start-relative when departPos=0), so
	// per step distance = cumDist - odometer (O(1), byte-exact -- verified vs
	// getNextTLS). Re-seeded when the routeId changes (a reroute changes it), so a
	// runtime route change can never give a stale answer. (True ONCE-PER-ROUTE needs
	// the full-route walk -> deferred Phase 3 offline map; seeding a shared route map
	// from a mid-route vehicle would miss TLS behind it.)
	struct TlsOnRoute { std::string id; int index; double cumDist; };
	struct VehTlsCache { std::string routeId; std::vector<TlsOnRoute> list; };
	std::unordered_map<std::string, VehTlsCache> VehicleId2Tls_um;
	// per-step snapshot of TLS R/Y/G state, filled on demand for the (sparse) set of
	// TLS that are actually some subscribed vehicle's next signal, then distributed.
	std::unordered_map<std::string, std::string> CurTlsState_um;

	// #177 Phase 2: static controlled-link topology, built once from
	// TrafficLight::getControlledLinks. tlsID -> incomingLaneId -> [(linkIndex,
	// outgoingEdge)]. getNextTLS's tlIndex (signal head) is lane-dependent, not route-
	// static, so we reconstruct it byte-exact each step from the subscribed CURRENT
	// lane + next route edge (verified vs getNextTLS, 11079/11079) instead of caching
	// a lane-stale value. Static topology -> precomputable offline (Phase 3).
	std::unordered_map<std::string,
		std::unordered_map<std::string, std::vector<std::pair<int, std::string>>>> TlsTopology_um;
	bool tlsTopologyBuilt = false;
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

	const int N_MAX_VEH = 100;
};





