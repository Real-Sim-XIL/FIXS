#include "TrafficHelper.h"
#include "FixsProtocol.h"   // fixs::kFeedPeriodS - one FIXS exchange == one traffic step

#include <stdexcept>


//const unsigned short selfServerPort[NSERVER] = { 420 };

using namespace std;

// The traffic simulator must step exactly once per FIXS exchange, so its step
// length IS the exchange period (see FixsProtocol.h). Formatted once here rather
// than written as a bare "0.1" at each auto-launch site.
static const std::string kTrafficStepArg = std::to_string(fixs::kFeedPeriodS);

// #177: lane id "edge_index" -> "edge" (strip the trailing _<laneIndex>).
static std::string edgeOfLane(const std::string& laneId) {
	size_t p = laneId.rfind('_');
	return (p == std::string::npos) ? laneId : laneId.substr(0, p);
}

// #177: SUMO TLS state char -> FIXS signalLightColor code. Identical mapping to the
// original inline getNextTLS handling, factored out so the cached path reuses it.
static int tlsStateToColor(char tlsState) {
	switch (tlsState) {
	case 'r': return 1;
	case 'y': return 2;
	case 'g': case 'G': return 3;
	case 'u': return 4;
	case 'o': return 5;
	case 'O': return 6;
	case 's': return 7;
	default:  return 0;
	}
}

#ifdef ENABLE_LIBSUMO
// #70: libsumocpp.dll is delay-loaded (TrafficLayer.vcxproj <DelayLoadDLLs>), so a
// missing runtime dependency does not surface at startup - it surfaces on the first
// libsumo call, which is Simulation::start(). That failure arrives as a Win32 loader
// exception (0xC06D007E), NOT a C++ exception, so the try/catch around start() cannot
// catch it and TrafficLayer dies with no message in the console or TrafficLayer.err.
// Loading the DLL explicitly first turns that into a normal, catchable error.
static void libsumoPreflight() {
#ifdef WIN32
#ifdef _DEBUG
	const char* dllName = "libsumocppD.dll";
#else
	const char* dllName = "libsumocpp.dll";
#endif
	if (LoadLibraryA(dllName) == NULL) {
		printf("ERROR: cannot load %s (GetLastError=%lu).\n", dllName, GetLastError());
		printf("       A runtime dependency is missing from CommonLib/libsumo/bin.\n");
		printf("       Re-fetch the native deps: scripts\\dispatch\\fetch_native_deps.ps1\n");
		throw std::runtime_error(std::string("failed to load ") + dllName +
			" - missing runtime dependency in CommonLib/libsumo/bin");
	}
#endif
}
#endif

//CentralCtrl CentralCtrl_g;

// The convention is that, Server always provide service to the Client. 
// i.e. Manager will be the Server to provide traffic data service to all possible clients, e.g. Controller, Communication, Vehicle. 
// The Controller can be another service provider to Vehicle to command desired speed
// The Communication will be another service provider to Controller to provide traffic information. 
// 
// [              ] - [Controller] - [      ]
// [Another Server] - [Controller] - [Client]
// [              ] - [Controller] - [      ]
//
// First, connect to other server
// Then, wait all clients connect
// Next, signal other server to start service
// Last, start own service => return to the caller of Socket initConnection
TrafficHelper::TrafficHelper(){
}

void TrafficHelper::connectionSetup(string trafficIp, int trafficPort, int nClientInput, int order) {

	nClient = nClientInput;

	edgeHasSubscribed = false;
	//nEdgeSubscribe = 0;

	//system("sumo -c \"C:\Users\y0n\Dropbox (ORNL)\2_projects\1_2_sumo\1_4_speedHarmTest\speedHarmTest.sumocfg\" --remote-port 1337 ");

	// ===========================================================================
	// Auto-launch SUMO if configured
	// ===========================================================================
	if (Config_c->SumoSetup.EnableAutoLaunch) {
		printf("Auto-launch SUMO enabled\n");
		printf("  Config file: %s\n", Config_c->SumoSetup.SumoConfigFile.c_str());
		printf("  Num clients: %d\n", Config_c->SumoSetup.NumClients);

#ifdef ENABLE_LIBSUMO
		// -----------------------------------------------------------------------
		// OPTION A: Direct launch via libsumo (headless only, no GUI)
		// OPT-IN, not the default: enabled by #define ENABLE_LIBSUMO in
		// TrafficHelper.h, which is commented out. Stock builds take OPTION B.
		// libsumo on Windows cannot drive sumo-gui - SUMO silently falls back to
		// headless ("Libsumo on Windows does not work with GUI"), so use OPTION B
		// whenever a visible network is wanted.
		// -----------------------------------------------------------------------
		// #70: libsumo runs SUMO *inside this process*. There is no TraCI server and
		// no second client, so the multi-client options below are not just useless
		// here, they are actively harmful:
		//   --num-clients : meaningless without a TraCI server.
		//   --remote-port : DO NOT ADD. It makes SUMO open a real TraCI server and
		//                   block inside start() until a separate client connects,
		//                   which never happens in-process -> TrafficLayer hangs.
		// Ordering (setOrder) is likewise a multi-client concept and is rejected by
		// libsumo at runtime - see the guard below.
		//
		// Those two settings are user-facing YAML keys (SumoSetup.NumClients,
		// SumoSetup.ExecutionOrder). Dropping them silently would let a config ask
		// for something the build cannot do and still appear to run, so report it.
		if (Config_c->SumoSetup.NumClients != 1) {
			printf("ERROR: SumoSetup.NumClients = %d, but this build embeds SUMO via libsumo.\n",
				Config_c->SumoSetup.NumClients);
			printf("       libsumo runs SUMO in-process with no TraCI server, so there is exactly\n");
			printf("       one client and additional clients could never connect.\n");
			printf("       Use a libtraci build (comment out ENABLE_LIBSUMO) for multi-client runs,\n");
			printf("       or set NumClients: 1.\n");
			throw std::runtime_error("SumoSetup.NumClients > 1 is not supported by a libsumo build "
				"(no TraCI server; use libtraci or set NumClients: 1)");
		}
		if (order != 1) {
			// Not fatal: with a single in-process client the ordering is simply moot.
			printf("WARNING: SumoSetup.ExecutionOrder = %d is ignored in libsumo mode.\n", order);
			printf("         Client ordering is a multi-client TraCI concept and libsumo rejects\n");
			printf("         setOrder(). The in-process simulation is the only client.\n");
		}
		libsumoPreflight();
		try {
			std::vector<std::string> cmd = {"sumo", "-c", Config_c->SumoSetup.SumoConfigFile, "--start",
				"--step-length", kTrafficStepArg};

			printf("Launching SUMO via libsumo::Simulation::start()...\n");
			libsumo::Simulation::start(cmd);
			// NOTE: no setOrder() here. libsumo throws "Multi client support
			// (including connection switching) is not implemented in libsumo."
			// A non-default ExecutionOrder was already reported above.
			printf("SUMO launched successfully via libsumo (in-process, headless)\n");
		}
		catch (const std::exception& e) {
			printf("ERROR: Failed to start SUMO via libsumo: %s\n", e.what());
			throw;
		}
#else
		// -----------------------------------------------------------------------
		// OPTION B: External process launch with sumo-gui (supports GUI)
		// -----------------------------------------------------------------------
		try {
			// Build command line for sumo-gui with multi-client support
			std::string sumoCmd = "sumo-gui -c \"" + Config_c->SumoSetup.SumoConfigFile +
				"\" --remote-port " + std::to_string(trafficPort) +
				" --num-clients " + std::to_string(Config_c->SumoSetup.NumClients) +
				" --step-length " + kTrafficStepArg + " --start";

			printf("Launching SUMO-GUI as external process...\n");
			printf("  Command: %s\n", sumoCmd.c_str());

			// Launch in background (platform-specific)
#ifdef WIN32
			std::string launchCmd = "start /B " + sumoCmd;  // Windows: use start /B
#else
			std::string launchCmd = sumoCmd + " &";         // Linux: append &
#endif
			int result = system(launchCmd.c_str());
			if (result != 0) {
				printf("ERROR: Failed to launch SUMO-GUI (exit code: %d)\n", result);
				throw std::runtime_error("Failed to launch SUMO-GUI process");
			}

			printf("SUMO-GUI process started successfully\n");

			// Wait a bit for SUMO to initialize
			printf("Waiting for SUMO to initialize...\n");
#ifdef WIN32
			Sleep(3000); // Wait 3 seconds (Windows)
#else
			sleep(3); // Wait 3 seconds (Linux)
#endif

			// Now connect via TraCI as usual
			printf("Connecting to SUMO via TraCI...\n");
			SUMO_TRACI_NAMESPACE::Simulation::init(trafficPort, libsumo::DEFAULT_NUM_RETRIES, trafficIp);
			SUMO_TRACI_NAMESPACE::Simulation::setOrder(order);
			printf("Connected to SUMO successfully\n");
		}
		catch (const std::exception& e) {
			printf("ERROR: Failed to auto-launch SUMO: %s\n", e.what());
			throw;
		}
#endif
	}
	else {
		// Original behavior: connect to existing SUMO instance
#ifdef ENABLE_LIBSUMO
		// #70: libsumo embeds the simulation in this process; it cannot attach to a
		// SUMO that is already running elsewhere. Both calls below are libtraci-only
		// and throw at runtime under libsumo, so fail loudly with an actionable
		// message instead of dying inside the library.
		printf("ERROR: EnableAutoLaunch is false, but this build embeds SUMO via libsumo.\n");
		printf("       libsumo cannot attach to an externally running SUMO instance.\n");
		printf("       Set EnableAutoLaunch: true, or build without ENABLE_LIBSUMO to use libtraci.\n");
		throw std::runtime_error("libsumo build cannot connect to an external SUMO instance "
			"(set EnableAutoLaunch: true, or build with libtraci)");
#else
		try {
			SUMO_TRACI_NAMESPACE::Simulation::init(trafficPort, libsumo::DEFAULT_NUM_RETRIES, trafficIp);
			SUMO_TRACI_NAMESPACE::Simulation::setOrder(order);
		}
		catch (const std::exception& e) {
			printf("ERROR: Failed to connect to SUMO at %s:%d - %s\n", trafficIp.c_str(), trafficPort, e.what());
			throw;
		}
#endif
	}

	/********************************************
	* GET VEH SUBSCRIPTION
	*********************************************/
	//VehDataSubscribeList.push_back(libsumo::TRACI_ID_LIST);
	VehDataSubscribeList.push_back(libsumo::VAR_TYPE);
	VehDataSubscribeList.push_back(libsumo::VAR_SPEED);
	VehDataSubscribeList.push_back(libsumo::VAR_POSITION3D);
	VehDataSubscribeList.push_back(libsumo::VAR_ANGLE); // north 0 deg, clockwise
	VehDataSubscribeList.push_back(libsumo::VAR_COLOR);
	VehDataSubscribeList.push_back(libsumo::VAR_ROAD_ID);
	VehDataSubscribeList.push_back(libsumo::VAR_LANE_INDEX);
	VehDataSubscribeList.push_back(libsumo::VAR_DISTANCE);
	VehDataSubscribeList.push_back(libsumo::VAR_LANEPOSITION);

	VehDataSubscribeList.push_back(libsumo::VAR_LANE_ID);
	VehDataSubscribeList.push_back(libsumo::VAR_VEHICLECLASS);
	VehDataSubscribeList.push_back(libsumo::VAR_ROUTE_INDEX);
	// #177 Phase 2: route id keys the per-route TLS map and detects reroutes
	// (a reroute changes the route id), so the map is never stale. Subscribed
	// (not per-veh getRouteID) to keep it free per step.
	VehDataSubscribeList.push_back(libsumo::VAR_ROUTE_ID);

	// testing new data
	VehDataSubscribeList.push_back(libsumo::VAR_ACCELERATION);
	//// no speedDesired accelerationDesired
	//// will return id and distance
	//VehDataSubscribeList.push_back(libsumo::VAR_LEADER);
	//// then need to query speed of that vehicle
	//// or use followSpeed??
	//VehDataSubscribeList.push_back(libsumo::VAR_FOLLOW_SPEED);
	// signal light
	// will return tlsID (signal light id), tlsIndex (signal head id), distance, state
	//VehDataSubscribeList.push_back(libsumo::VAR_NEXT_TLS);
	// speed limit
	VehDataSubscribeList.push_back(libsumo::VAR_ALLOWED_SPEED);
	VehDataSubscribeList.push_back(libsumo::VAR_SPEED_FACTOR);
	// get next speed limit
	// handle it at beginning when this vehicle enters
	// #86: subscribed but never read -- parserSumoSubscription has no
	// VehDataSubscribeTraciResults[VAR_VIA] anywhere, so SUMO serialises a string
	// list per vehicle per step that is then discarded. Commented rather than
	// deleted: it is one of the few upcoming-route variables, and whoever wants
	// it back should see that it used to be here.
	//VehDataSubscribeList.push_back(libsumo::VAR_VIA);
	// next link
	// retrieved as part of the speed limit information
	
	//// lane change
	//// need to check if this even works
	//VehDataSubscribeList.push_back(libsumo::CMD_CHANGELANE);

	// get slope of the road
	VehDataSubscribeList.push_back(libsumo::VAR_SLOPE);

	// get signals
	//Name	Bit
	//VEH_SIGNAL_BLINKER_RIGHT	0
	//VEH_SIGNAL_BLINKER_LEFT	1
	//VEH_SIGNAL_BLINKER_EMERGENCY	2
	//VEH_SIGNAL_BRAKELIGHT	3
	//VEH_SIGNAL_FRONTLIGHT	4
	//VEH_SIGNAL_FOGLIGHT	5
	//VEH_SIGNAL_HIGHBEAM	6
	//VEH_SIGNAL_BACKDRIVE	7
	//VEH_SIGNAL_WIPER	8
	//VEH_SIGNAL_DOOR_OPEN_LEFT	9
	//VEH_SIGNAL_DOOR_OPEN_RIGHT	10
	//VEH_SIGNAL_EMERGENCY_BLUE	11
	//VEH_SIGNAL_EMERGENCY_RED	12
	//VEH_SIGNAL_EMERGENCY_YELLOW	13
	VehDataSubscribeList.push_back(libsumo::VAR_SIGNALS);

	// Return the length, width, height of the vehicle
	VehDataSubscribeList.push_back(libsumo::VAR_LENGTH);
	VehDataSubscribeList.push_back(libsumo::VAR_WIDTH);
	VehDataSubscribeList.push_back(libsumo::VAR_HEIGHT);
	// -------------------
	// These variables are subscribed for testing purposes
	// -------------------
	VehDataSubscribeList.push_back(libsumo::VAR_SPEED_WITHOUT_TRACI);



	//VehDataSubscribeList.push_back(libsumo::VAR_NEXT_TLS); // Returns upcoming traffic lights, along with distanceand state [(tlsID, tlsIndex, distance, state), ...]

	//VehDataSubscribeList.push_back(libsumo::VAR_LEADER);/*Returns the id of the leading vehicleand its distance, if the string is empty, no leader was found within the given range.Only vehicles ahead on the currently list of best lanes are considered(see above).This means, the leader is only valid until the next lane - change maneuver.The returned distance is measured from the ego vehicle front bumper + minGap to the back bumper of the leader vehicle.*/

	//VehDataSubscribeList.push_back(libsumo::VAR_ALLOWED_SPEED); //Returns the maximum allowed speed on the current lane regarding speed factor in m/s for this vehicle.
	//
	//VehDataSubscribeList.push_back(libsumo::VAR_SPEED_FACTOR); // Returns the road speed multiplier for this vehicle[double]

	//VehDataSubscribeList.push_back(libsumo::VAR_SLOPE); // Retrieves the slope at the current vehicle position in degrees





	/********************************************
	* GET DETECTOR SUBSCRIPTION
	*********************************************/
	// obtain detector ids of the selected intersection and subscribe to results
	if (ENABLE_DET_SUB) {
		//string detSelectPattern = DET_SEL_PATTERN; // select which intersection we want to do signal control in the loop SCIP
		////===================================================
		//// Retreive DETECTOR configuration for the scenario BEFORE simulation starts
		////===================================================
		//vector <string> detAreaAllId_v = traci.lanearea.getIDList();
		//vector <string> detInductAllId_v = traci.inductionloop.getIDList();

		//vector <int> detSubscribeList;

		//detSubscribeList.push_back(libsumo::LAST_STEP_VEHICLE_NUMBER);
		//for (int iD = 0; iD < detAreaAllId_v.size(); iD++) {
		//	if (detAreaAllId_v[iD].find(detSelectPattern) != std::string::npos) {
		//		detSelectId_v.push_back(detAreaAllId_v[iD]);
		//		traci.lanearea.subscribe(detAreaAllId_v[iD], detSubscribeList, 0, tSimuEnd);
		//	}
		//}
	}



	/********************************************
	* GET Speed Limit of every edge, lane
	*********************************************/
	// Get list of all lanes and edges in the network
	vector <string> laneList = SUMO_TRACI_NAMESPACE::Lane::getIDList();

	// Build vehicle class list from all vehicle types defined in SUMO
	vector <string> vehClassList;

	vector <string> vehTypeList = SUMO_TRACI_NAMESPACE::VehicleType::getIDList();
	for (int i = 0; i < vehTypeList.size(); i++) {
		string vehType = vehTypeList[i];
		string vehClass = SUMO_TRACI_NAMESPACE::VehicleType::getVehicleClass(vehType);

		vehClassList.push_back(vehClass);
	}

	// Only retrieve speed limits if speedLimit or speedLimitNext is defined in VehicleMessageField
	// This avoids expensive queries when speed limit data is not needed
	if (VehicleMessageField_set.find("speedLimit") != VehicleMessageField_set.end() ||
		VehicleMessageField_set.find("speedLimitNext") != VehicleMessageField_set.end()) {
		for (int i = 0; i < laneList.size(); i++) {
			string laneId = laneList[i];
			string edgeId = SUMO_TRACI_NAMESPACE::Lane::getEdgeID(laneId);

			vector <string> allowClassList = SUMO_TRACI_NAMESPACE::Lane::getAllowed(laneId);
			vector <string> disallowClassList = SUMO_TRACI_NAMESPACE::Lane::getDisallowed(laneId);

			// If no specific vehicle class restrictions, apply to all vehicle classes
			if (allowClassList.size() == 0 && disallowClassList.size() == 0) {
				for (int iC = 0; iC < vehClassList.size(); iC++) {
					string vClass = vehClassList[iC];
					LaneVehClass2SpeedLimit_um[make_pair(laneId, vClass)] = SUMO_TRACI_NAMESPACE::Lane::getMaxSpeed(laneId);

					EdgeVehClass2SpeedLimit_um[make_pair(edgeId, vClass)] = SUMO_TRACI_NAMESPACE::Lane::getMaxSpeed(laneId);
				}
			}
			// Otherwise, only apply to allowed vehicle classes
			else {
				for (int iC = 0; iC < allowClassList.size(); iC++) {
					string vClass = allowClassList[iC];
					LaneVehClass2SpeedLimit_um[make_pair(laneId, vClass)] = SUMO_TRACI_NAMESPACE::Lane::getMaxSpeed(laneId);

					EdgeVehClass2SpeedLimit_um[make_pair(edgeId, vClass)] = SUMO_TRACI_NAMESPACE::Lane::getMaxSpeed(laneId);
				}
			}
		}
	}

	vector <string> edgeList = SUMO_TRACI_NAMESPACE::Edge::getIDList();
	for (int i = 0; i < edgeList.size(); i++) {
		AllEdgeList.insert(edgeList[i]);
	}

}

void TrafficHelper::connectionSetup(int nClientInput) {

	nClient = nClientInput;

	edgeHasSubscribed = false;
	//nEdgeSubscribe = 0;

}

void TrafficHelper::enableVehSub() {
	ENABLE_VEH_SUB = true;
}

void TrafficHelper::disableVehSub() {
	ENABLE_VEH_SUB = false;
}

void TrafficHelper::enableDetSub() {
	ENABLE_DET_SUB = true;
}

void TrafficHelper::disableDetSub() {
	ENABLE_DET_SUB = false;
}

void TrafficHelper::selectVISSIM(vector <int> vissimSockInput, vector <string> vissimSockNames) {
	SUMO_OR_VISSIM = "VISSIM";

	for (int i = 0; i < vissimSockInput.size(); i++) {
		vissimSock.push_back(vissimSockInput[i]);
		vissimSockName_um[vissimSockInput[i]] = vissimSockNames[i];
	}
	//for (int i = 0; i < clientSockInput.size(); i++) {
	//	clientSock.push_back(clientSockInput[i]);
	//}
}

void TrafficHelper::selectSUMO() {
	SUMO_OR_VISSIM = "SUMO";
}

void TrafficHelper::close() {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		SUMO_TRACI_NAMESPACE::Simulation::close();
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {

	}
	else {

	}
}


void TrafficHelper::getConfig() {

	if (Config_c->SimulationSetup.EnableVerboseLog) {
		ENABLE_VERBOSE = true;
	}
	else {
		ENABLE_VERBOSE = false;
	}

	if (Config_c->ApplicationSetup.VehicleSubscription.size() > 0) {
		ENABLE_VEH_SUB = true;
	}
	else {
		if (Config_c->XilSetup.EnableXil) {
			ENABLE_VEH_SUB = true;
		}
		else {
			ENABLE_VEH_SUB = false;
		}
	}
	if (Config_c->ApplicationSetup.DetectorSubscription.size() > 0) {
		ENABLE_DET_SUB = true;
	}
	else {
		if (Config_c->XilSetup.DetectorSubscription.size() > 0) {
			ENABLE_DET_SUB = true;
		}
		else{
			ENABLE_DET_SUB = false;
		}
	}
	if (Config_c->ApplicationSetup.SignalSubscription.size() > 0) {
		ENABLE_SIG_SUB = true;
	}
	else {
		if (Config_c->XilSetup.SignalSubscription.size() > 0) {
			ENABLE_SIG_SUB = true;
		}
		else {
			ENABLE_SIG_SUB = false;
		}
	}

	//if (Config_c->CarMakerSetup.EnableCosimulation && Config_c->CarMakerSetup.SynchronizeTrafficSignal) {
	//	ENABLE_SIG_SUB = true;
	//}

	if (Config_c->SimulationSetup.EnableExternalDynamics) {
		ENABLE_EXT_DYN = true;
	}
	else {
		ENABLE_EXT_DYN = false;
	}
	tSimuEnd = Config_c->SimulationSetup.SimulationEndTime;

	// get vehicle message that needs to be sent out
	VehicleMessageField_v = Config_c->SimulationSetup.VehicleMessageField;

	for (int i = 0; i < VehicleMessageField_v.size(); i++) {
		VehicleMessageField_set.insert(VehicleMessageField_v[i]);
	}

	// #177: only run the expensive per-vehicle TraCI getters in
	// parserSumoSubscription when their output is actually forwarded. getNextTLS
	// (O(upcoming-route length)) feeds the signalLight* fields; getLeader/getSpeed
	// feed the precedingVehicle* fields. MsgHelper gates packing on the same set.
	NEED_NEXT_TLS =
		VehicleMessageField_set.count("signalLightId") ||
		VehicleMessageField_set.count("signalLightHeadId") ||
		VehicleMessageField_set.count("signalLightDistance") ||
		VehicleMessageField_set.count("signalLightColor");
	NEED_PRECEDING_VEH =
		VehicleMessageField_set.count("precedingVehicleId") ||
		VehicleMessageField_set.count("precedingVehicleDistance") ||
		VehicleMessageField_set.count("precedingVehicleSpeed") ||
		VehicleMessageField_set.count("hasPrecedingVehicle");

	// get subscription information
	// variable to store subscription that need to check
	// if application layer is disabled, xil is enabled then use subscription of xil, this means traffic layer directly connects to xil
	if (!Config_c->ApplicationSetup.EnableApplicationLayer && Config_c->XilSetup.EnableXil) {
		Config_c->getVehSubscriptionList(Config_c->XilSetup.VehicleSubscription, edgeSubscribeId_v, vehicleSubscribeId_v, subscribeAllVehicle, pointSubscribeId_v, vehicleTypeSubscribedId_v);
	}
	// otherwise find out the subscription of application layer
	else {
		Config_c->getVehSubscriptionList(Config_c->ApplicationSetup.VehicleSubscription, edgeSubscribeId_v, vehicleSubscribeId_v, subscribeAllVehicle, pointSubscribeId_v, vehicleTypeSubscribedId_v);
	}

	// #86: the warm-up watches BOTH layers' by-id subscriptions, not the either/or
	// list above -- ConfigHelper builds that union so the trigger sees the XIL ego
	// even when the application layer is the one TrafficLayer talks to.
	warmupEgoId_v = Config_c->WarmUpEgoIds;

	// #176: the `all` subscription means every vehicle in the network, unbounded.
	// If a radius was also configured alongside `all`, it is ignored — warn so the
	// config author is not surprised.
	if (get<0>(subscribeAllVehicle) && get<1>(subscribeAllVehicle) != 0) {
		printf("WARNING (#176): 'all' vehicle subscription is enabled; configured radius %.1f is ignored (all vehicles in the network are sent).\n", get<1>(subscribeAllVehicle));
	}

	vehicleHasSubscribed_v.clear();
	vehicleHasSubscribed_v.resize(vehicleSubscribeId_v.size());
	fill(vehicleHasSubscribed_v.begin(), vehicleHasSubscribed_v.end(), false);
}


int TrafficHelper::sendToTrafficSimulator(double simTime, MsgHelper Msg_c) {
	
	int sendStatus = 0;

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		sendStatus = this->sendToSUMO(simTime, Msg_c);
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {
		sendStatus = this->sendToVISSIM(simTime, Msg_c);
	}
	else {

	}

	return sendStatus;
}


// Which layer owns this vehicle's motion, if the traffic simulator does not?
//
// The two answers used to be two conditions written at two call sites, one keyed
// on CarMakerSetup.EgoId and one on CarlaSetup.InterestedIds, each with its own
// copy of the mirror that follows. They describe the same thing -- SUMO holding a
// vehicle it does not drive -- so they are one function now, and sendToSUMO has
// one body for whatever it returns.
TrafficHelper::ExternalEgoOwner TrafficHelper::externalEgoOwnerOf(const std::string& vehId) const {

	// VirEnv FIRST, and not for tidiness: when the CarMakerSetup section is absent,
	// CarMakerSetup.EgoId is inferred from the lone vehicle subscription and can
	// come out equal to the virtual environment's ego id. Testing CarMaker first
	// would then shadow the VirEnv injection and moveToXY a vehicle nobody added.
	// VirEnv ownership is the more specific claim -- it also requires external
	// control to be on and the id to be listed -- so it is the one that wins.
	if (ENABLE_CARLA && ENABLE_CARLA_EXTERNAL_CONTROL &&
		find(Config_c->CarlaSetup.InterestedIds.begin(), Config_c->CarlaSetup.InterestedIds.end(), vehId)
			!= Config_c->CarlaSetup.InterestedIds.end()) {
		return ExternalEgoOwner::VirEnv;
	}

	if (ENABLE_VEH_SIMULATOR && !Config_c->CarMakerSetup.EgoId.empty() &&
		vehId == Config_c->CarMakerSetup.EgoId) {
		return ExternalEgoOwner::VehSimulator;
	}

	return ExternalEgoOwner::None;
}


int TrafficHelper::addEgoVehicle(double simTime) {

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		if (ENABLE_VEH_SIMULATOR) {
			// !!!!check if what received is ego vehicle
			// use default type if not specified!!
			string idStr = Config_c->CarMakerSetup.EgoId;   // filled by EgoSetup.Id

			// Get all vehicle IDs from SUMO
			vector<string> vehicleIds = SUMO_TRACI_NAMESPACE::Vehicle::getIDList();

			// Check if ego vehicle already exists
			bool vehicleExist = (find(vehicleIds.begin(), vehicleIds.end(), idStr) != vehicleIds.end());

			// if ego not exist yet, add it
			if (!vehicleExist) {
				string typeStr = Config_c->CarMakerSetup.EgoType;   // filled by EgoSetup.SumoType

				// if is empty
				if (typeStr.size() == 0) {
					SUMO_TRACI_NAMESPACE::Vehicle::add(idStr, "");
				}
				else {
					SUMO_TRACI_NAMESPACE::Vehicle::add(idStr, "", typeStr);
				}
				SUMO_TRACI_NAMESPACE::Vehicle::setColor(idStr, libsumo::TraCIColor(255, 0, 0));
			}
		}

		return 1;
	}
	else {
		return 0;
	}

}

int TrafficHelper::addEgoVehicleFromXY(double simTime, std::string vehicleId, std::string vehicleType, double positionX, double positionY) {

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		// NOTE: no ENABLE_VEH_SIMULATOR gate here. This is called from the Carla
		// external-control inject path, which runs WITHOUT a CarMaker/XIL coupling
		// (ENABLE_VEH_SIMULATOR false) -- the old gate silently skipped the add and
		// returned success, so the Carla ego could never enter SUMO.
		{
			// Map the x y positon to an edge for spawing the ego vehicle
			libsumo::TraCIRoadPosition edgePosition = SUMO_TRACI_NAMESPACE::Simulation::convertRoad(positionX, positionY, false);
			// Create a dummy route for the ego vehicle
			std::string dummyedgeID = edgePosition.edgeID;
			double lanePos = edgePosition.pos; // position along the edge
			int laneIndex = edgePosition.laneIndex;
			std::string dummyRouteId = "route_" + vehicleId;
			std::vector<std::string> dummyRoute;
			dummyRoute.push_back(dummyedgeID);
			SUMO_TRACI_NAMESPACE::Route::add(dummyRouteId, dummyRoute);

			// Depart AT the mapped position (not "base"=pos 0): the edge start is
			// where background flows enter, so "base" insertion can stay blocked
			// indefinitely -- the ego would never depart.
			SUMO_TRACI_NAMESPACE::Vehicle::add(vehicleId, dummyRouteId, vehicleType, "now",
				std::to_string(laneIndex), std::to_string(lanePos), "0");
			// setColor on a not-yet-departed vehicle can throw "not known" -- the add
			// above already succeeded, so never let the color abort the injection.
			try { SUMO_TRACI_NAMESPACE::Vehicle::setColor(vehicleId, libsumo::TraCIColor(255, 0, 0)); }
			catch (const std::exception&) { /* recolor happens once it departs */ }
		}

		return 1;
	}
	else {
		return 0;
	}

}


// Is any warm-up ego in the network yet? (#86)
//
// "Any", not "all": if egos A and B enter at 100 s and 150 s and the warm-up
// waited for both, then from 100-150 s A is on the road being driven by SUMO's
// car-following model while its XIL is still held behind a closed boundary --
// its entry would be simulated by the wrong thing. Stopping at the first arrival
// has no such failure: B simply is not in the network yet, which is the ordinary
// no-warm-up situation.
//
// Replaces checkIfEgoExist, which returned from inside the first loop iteration
// on BOTH branches (so only one id was ever checked) and fell off the end with
// no return value at all when the subscription list was empty.
bool TrafficHelper::isWarmUpEgoInNetwork(double* simTime) {

	if (SUMO_OR_VISSIM.compare("SUMO") != 0) {
		// Unreachable: ConfigHelper rejects a warm-up on VISSIM, because
		// TrafficLayer does not own the VISSIM clock. Kept explicit so this can
		// never silently answer "no" forever the way it used to.
		return false;
	}

	*simTime = SUMO_TRACI_NAMESPACE::Simulation::getTime();
	vector <string> VehIdInSimulator = SUMO_TRACI_NAMESPACE::Vehicle::getIDList();

	for (auto& idStr : warmupEgoId_v) {
		if (find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) != VehIdInSimulator.end()) {
			printf("Warm-up complete: ego '%s' entered the network at t=%.2f\n",
				idStr.c_str(), *simTime);
			return true;
		}
	}

	return false;
}

int TrafficHelper::getSimulationTime(double* simTime) {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		*simTime = SUMO_TRACI_NAMESPACE::Simulation::getTime();
		return 1;
	}
	else {
		return 0;
	}
}

// Advance the traffic simulator to an absolute time in ONE call -- the WarmUpTime
// path (#86). SUMO runs to endTime internally, so there is no per-step TraCI round
// trip and no opportunity for anything else to be observed on the way; that is why
// WarmUpTime and WarmUpUntilEgoEntry are mutually exclusive.
//
// Verified to work with a second TraCI client attached (the topology where an
// application-layer controller holds order 2): both clients land exactly on
// endTime. SUMO only advances as fast as the OTHER client steps, so a controller
// that keeps doing per-step socket I/O during its own warm-up bounds this.
int TrafficHelper::runSimulation(double endTime) {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		SUMO_TRACI_NAMESPACE::Simulation::step(endTime);
		return 1;
	}
	// VISSIM drives its own clock through the DriverModel DLL; there is nothing to
	// batch-step here. ConfigHelper rejects a warm-up on VISSIM so this cannot be
	// reached -- it used to return 0 while the caller marked the warm-up done,
	// which is how mode 4 became a silent no-op on VISSIM.
	return -1;
}


int TrafficHelper::sendToSUMO(double simTime, MsgHelper Msg_c) {

	int sendStatus = 0;

	if (ENABLE_VERBOSE) {
		printf("sending to SUMO\n ");

		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "sending to SUMO\n ");
		fclose(f);

	}

	//int nVeh = VehIdInSimulator.size();

	//if (nVeh > 0) {
	//	for (int iV = 0; iV < nVeh; iV++) {
	//		string idStr = VehIdInSimulator[iV];
	//		//unsigned int id = stoi(idStr.substr(12));

	//		//if (idStr.compare("flow_0.10") == 0) {
	//		//	traci.vehicle.setColor(idStr, libsumo::TraCIColor(255, 0, 0, 255));
	//		//}
	//		//if (idStr.compare("flow_1.9") == 0) {
	//		//	traci.vehicle.setColor(idStr, libsumo::TraCIColor(0, 166, 255, 255));
	//		//}

	//		//if (std::find(Msg_c.VehIdRecv_v.begin(), Msg_c.VehIdRecv_v.end(), idStr) != Msg_c.VehIdRecv_v.end()) {
	//		if (Msg_c.VehDataRecv_um.find(idStr) != Msg_c.VehDataRecv_um.end()) {
	//			traci.vehicle.setSpeed(idStr, Msg_c.VehDataRecv_um[idStr].speed);
	//			traci.vehicle.setSpeedMode(idStr, 0); // most checks off


	//			if (ENABLE_VERBOSE) {
	//				//cout << "setSpeed ego # " << idStr << " speed " << VehCmdRecv_um[idStr].speed << endl;
	//				printf("setSpeed ego %s speed %.4f\n", idStr.c_str(), Msg_c.VehDataRecv_um[idStr].speed);
	//				//cout << "setSpeed ego # " << idStr << " speed " << VehCmdRecv_um[idStr].speed << '\n';
	//			}
	//		}
	//		else {
	//			//traci.vehicle.setSpeedMode(idStr, 31);
	//		}
	//	}

	//	if (Msg_c.TlsIdRecv_v.size() > 0) {
	//		if (Msg_c.TlsDataRecv_um.find(tlsSelect) != Msg_c.TlsDataRecv_um.end()) {
	//			string setTlsState = Msg_c.TlsDataRecv_um[tlsSelect].state;
	//			traci.trafficlights.setRedYellowGreenState(tlsSelect, setTlsState);

	//			if (ENABLE_VERBOSE) {
	//				//cout << "set " << " time " << simTime << " traffic light " << tlsSelect << " state " << setTlsState << endl;
	//				printf("set time %.1f traffic light %s state %s \n", simTime, tlsSelect.c_str(), setTlsState.c_str());
	//			}
	//		}
	//	}
	//}

	try {
		vector <string> VehIdInSimulator = SUMO_TRACI_NAMESPACE::Vehicle::getIDList();
		//uint32_t color = 4278190335;
		//uint8_t r = (color >> 24) & 0xFF;
		//uint8_t g = (color >> 16) & 0xFF;
		//uint8_t b = (color >> 8) & 0xFF;
		//uint8_t a = (color) & 0xFF;
		//traci.vehicle.setColor("flow_0.0", libsumo::TraCIColor(r, g, b, a));

		for (int iV = 0; iV < VehIdInSimulator.size(); iV++) {
			//traci.vehicle.setSpeedMode(VehIdInSimulator[iV], 31); // default speed mode
		}
		for (int iV = 0; iV < Msg_c.VehDataSend_um[0].size(); iV++) {
			string idStr = Msg_c.VehDataSend_um[0][iV].id;
			
			double speed;
			if (VehicleMessageField_set.find("speedDesired") != VehicleMessageField_set.end()) {
				speed = Msg_c.VehDataSend_um[0][iV].speedDesired;
			}
			else {
				//double accel = std::any_cast<float>(Msg_c.VehDataSend_um[0][iV]["accelerationDesired"]);
				//speed = 
				printf("ERROR: SUMO does not support control by accelerationDesired yet\n");
				printf("\tPlease select speedDesired in VehicleMessageField instead\n");
				exit(-1);
			}


			if (ENABLE_VERBOSE) {
				if (!ENABLE_VEH_SIMULATOR && find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) != VehIdInSimulator.end()) {
					double speedOld = SUMO_TRACI_NAMESPACE::Vehicle::getSpeed(idStr);
					printf("Set SUMO id %s from speed %.4f to speed %.4f\n", idStr.c_str(), speedOld, speed);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "Set SUMO id %s from speed %.4f to speed %.4f\n", idStr.c_str(), speedOld, speed);
					fclose(f);
				}
			}

			// #305 ONE mirror for an externally-driven ego.
			//
			// CarMaker/XIL and the virtual environment are the same situation from
			// here: a vehicle SUMO holds but does not drive, whose position is
			// written in every tick. What differs is only which config named the id,
			// so ownership is resolved once (externalEgoOwnerOf) and this body runs
			// for either answer. The two places it still branches on the owner are
			// marked, and neither is a difference in mechanism.
			const ExternalEgoOwner egoOwner = externalEgoOwnerOf(idStr);

			if (egoOwner != ExternalEgoOwner::None) {

				const double positionX = (double)Msg_c.VehDataSend_um[0][iV].positionX;
				const double positionY = (double)Msg_c.VehDataSend_um[0][iV].positionY;
				const double heading   = (double)Msg_c.VehDataSend_um[0][iV].heading;
				const std::string vehicleType = Msg_c.VehDataSend_um[0][iV].type;
				const bool vehicleExist =
					find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) != VehIdInSimulator.end();

				try {
					// --- 1. inject once, if the scenario did not declare the ego ---
					//
					// OWNER-SPECIFIC, and this one is about timing, not mechanism.
					// CarMaker's ego is added at the very first step by
					// addEgoVehicle(), on a route SUMO picks; a vehicle that has been
					// added but has not departed yet is ABSENT from getIDList, so
					// running the add here too would try to create it a second time
					// and throw. The VirEnv ego has no such first-step hook, and its
					// spawn position is only known from the feed, so it is injected
					// here from (x,y).
					//
					// Either way: add ONCE (re-adding every step resets the pending
					// vehicle so it never departs), then moveToXY EVERY step --
					// moveToXY also works on a not-yet-departed vehicle and INSERTS it
					// at the given position, which is what gets the ego past an edge
					// start that background flow is occupying.
					//
					// And a vehicle the scenario ALREADY declared is adopted, not
					// recreated: the traffic simulator inserts it on its own route at
					// its own depart time, and the virtual environment takes its
					// dynamics over from there.
					if (egoOwner == ExternalEgoOwner::VirEnv &&
						!vehicleExist && externalEgoInjected_.find(idStr) == externalEgoInjected_.end()) {
						addEgoVehicleFromXY(simTime, idStr, vehicleType, positionX, positionY);
						externalEgoInjected_.insert(idStr);
					}

					// --- 2. off-map guard (#174), warn-only ------------------------
					// getPosition is where SUMO placed the ego on the PREVIOUS
					// moveToXY. Far from what we fed it then means SUMO could not keep
					// it on the drivable network -- it snapped, or the mapping failed,
					// i.e. the ego left the road. Isolated try so a getPosition hiccup
					// cannot skip the move below. Shared with CarMaker now; it is a
					// diagnostic, and the failure it reports is not Carla-specific.
					auto itLast = externalEgoLastFedXY_.find(idStr);
					if (itLast != externalEgoLastFedXY_.end()) {
						try {
							auto sp = SUMO_TRACI_NAMESPACE::Vehicle::getPosition(idStr);
							double ex = sp.x - itLast->second.first, ey = sp.y - itLast->second.second;
							fixs::RS_XIL_GUARD("ego_off_sumo_network", std::sqrt(ex * ex + ey * ey), 5.0);
						}
						catch (...) {}
					}

					// --- 3. write the externally-computed motion in ----------------
					if (ENABLE_EXT_DYN) {
						// SimulationSetup.EnableExternalDynamics: the plant integrates
						// the ego and SUMO is told the speed at (k), which is reflected
						// at (k) immediately rather than at (k+1).
						SUMO_TRACI_NAMESPACE::Vehicle::setPreviousSpeed(idStr, speed);
					}
					else {
						// keepRoute is EgoSetup.KeepRoute for BOTH owners now (default
						// 6, the value the CarMaker call site used to hardcode). It is
						// really a choice of FAILURE MODE for a vehicle SUMO no longer
						// drives: bit 1 (2/6) places it at the exact position and lets
						// it leave the drivable network -- which is what allows
						// off-road driving but also lets it quietly lose its lane, and
						// with it the next-signal lookup an eco/CAV controller depends
						// on. Bit 0 (1/5) pins it to its own route and makes SUMO raise
						// when it cannot, i.e. fail loudly instead.
						SUMO_TRACI_NAMESPACE::Vehicle::moveToXY(idStr, "", -1, positionX, positionY, heading,
							Config_c->EgoSetup.KeepRoute);
						externalEgoLastFedXY_[idStr] = std::make_pair(positionX, positionY);

						// OWNER-SPECIFIC. setSpeed makes SUMO's getSpeed report the ego's
						// ACTUAL speed -- the `.speed` field, never `speedDesired`, which
						// under L2 is the command and would give SUMO a shadow speed. It
						// matters because background car-following reads it. Not applied
						// to the CarMaker ego only because nobody has run CarMaker since
						// this merged, and its speed has always come out of the moveToXY
						// displacement instead; the mechanism applies there equally.
						if (egoOwner == ExternalEgoOwner::VirEnv) {
							SUMO_TRACI_NAMESPACE::Vehicle::setSpeed(idStr, (double)Msg_c.VehDataSend_um[0][iV].speed);
						}
					}

					// --- 4. indicators, when the application publishes them ---------
					if (VehicleMessageField_set.find("lightIndicators") != VehicleMessageField_set.end()) {
						SUMO_TRACI_NAMESPACE::Vehicle::setSignals(idStr, (int)Msg_c.VehDataSend_um[0][iV].lightIndicators);
					}
				}
				catch (const std::exception& e) {
					printf("Externally-driven ego '%s' mirror failed: %s\n", idStr.c_str(), e.what());
				}

			}
			else {
				if (1 && find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) != VehIdInSimulator.end()) {
					if (ENABLE_EXT_DYN) {
						SUMO_TRACI_NAMESPACE::Vehicle::setPreviousSpeed(idStr, speed); // setting speed at (k) will be reflected at (k) "immediately", i.e., be considered in the next integration
					}
					else {
						SUMO_TRACI_NAMESPACE::Vehicle::setSpeed(idStr, speed); // speed set at (k) essentially will be reflected at (k+1), not considered in the integration

						/*
						bit0: Regard safe speed
						bit1 : Regard maximum acceleration
						bit2 : Regard maximum deceleration
						bit3 : Regard right of way at intersections(only applies to approaching foe vehicles outside the intersection)
						bit4 : Brake hard to avoid passing a red light
						bit5 : Disregard right of way within intersections(only applies to foe vehicles that have entered the intersection).
						*/

						SUMO_TRACI_NAMESPACE::Vehicle::setSpeedMode(idStr, Config_c->SumoSetup.SpeedMode); // 000000 most checks off
						//SUMO_TRACI_NAMESPACE::Vehicle::setSpeedMode(idStr, 0); // 000000 most checks off
						//SUMO_TRACI_NAMESPACE::Vehicle::setSpeedMode(idStr, 24); // 011000
						//SUMO_TRACI_NAMESPACE::Vehicle::setSpeedMode(idStr, 8); // 001000

						//SUMO_TRACI_NAMESPACE::Vehicle::setSpeedFactor(idStr, 1);
					}

					// change vehicle color if needed
					if (VehicleMessageField_set.find("color") != VehicleMessageField_set.end()) {
						uint32_t color = Msg_c.VehDataSend_um[0][iV].color;
						uint8_t r = (color >> 24) & 0xFF;
						uint8_t g = (color >> 16) & 0xFF;
						uint8_t b = (color >> 8) & 0xFF;
						uint8_t a = (color) & 0xFF;
						SUMO_TRACI_NAMESPACE::Vehicle::setColor(idStr, libsumo::TraCIColor(r, g, b, a));
					}

				}
			}


		}


		if (ENABLE_SIG_SUB) {
			for (int iS = 0; iS < Msg_c.TlsDataSend_um[0].size(); iS++) {
				string idStr = Msg_c.TlsDataSend_um[0][iS].name;

				SUMO_TRACI_NAMESPACE::TrafficLight::setRedYellowGreenState(idStr, Msg_c.TlsDataSend_um[0][iS].state);
			}
		}


		if (ENABLE_VERBOSE) {
			printf("send SUMO complete\n ");

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "send SUMO complete\n ");
			fclose(f);
		}

	}
	catch (const std::exception& e) {
		std::cout << e.what();
		return -1;
	}
	catch (...) {
		printf("UNKNOWN ERROR: send to SUMO fails\n");
		return -1;
	}

		//if (Msg_c.TlsIdRecv_v.size() > 0) {
		//	if (Msg_c.TlsDataRecv_um.find(tlsSelect) != Msg_c.TlsDataRecv_um.end()) {
		//		string setTlsState = Msg_c.TlsDataRecv_um[tlsSelect].state;
		//		traci.trafficlights.setRedYellowGreenState(tlsSelect, setTlsState);

		//		if (ENABLE_VERBOSE) {
		//			//cout << "set " << " time " << simTime << " traffic light " << tlsSelect << " state " << setTlsState << endl;
		//			printf("set time %.1f traffic light %s state %s \n", simTime, tlsSelect.c_str(), setTlsState.c_str());
		//		}
		//	}
		//}


	return sendStatus;
}

int TrafficHelper::sendToVISSIM(double simTime, MsgHelper Msg_c) {

	int sendStatus = 0;

	SocketHelper Sock_temp;
	
	try {
		for (int iClient = 0; iClient < vissimSock.size(); iClient++) {
			if (ENABLE_VERBOSE) {
				printf("send to VISSIM %s\n", vissimSockName_um[vissimSock[iClient]].c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "send to VISSIM %s\n", vissimSockName_um[vissimSock[iClient]].c_str());
				fclose(f);

			}

			//if (vissimSock.size() > 1 && vissimSockName_um[vissimSock[i]].find(SIGNAL_SOCK_PATTERN) != string::npos) {
			//	// if is signal controller
			//	if (abs(round(simTime / (vissimSignalStep * vissimBaseDt)) - simTime / (vissimSignalStep * vissimBaseDt)) < 1e-5) {
			//		Sock_temp.sendData(vissimSock[i], i, simTime, 1, Msg_c);
			//	}
			//}
			//else {
			//	Sock_temp.sendData(vissimSock[i], i, simTime, 1, Msg_c);
			//}
			sendStatus = Sock_temp.sendData(vissimSock[iClient], iClient, simTime, 1, Msg_c);
		}

		if (ENABLE_VERBOSE) {
			printf("send VISSIM complete\n");

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "send VISSIM complete\n");
			fclose(f);

		}

	}
	catch (const std::exception& e) {
		std::cout << e.what();
		return -1;
	}
	catch (...) {
		printf("UNKNOWN ERROR: send to VISSIM fails\n");
		return -1;
	}

	return sendStatus;

}

void TrafficHelper::parseSendMsg(MsgHelper MsgIn_c, MsgHelper& MsgOut_c) {

	// MsgClient_c/MsgIn recv => MsgServer_c/MsgOut send

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		// SUMO use a dummy socket index 0
		MsgOut_c.VehDataSend_um[0] = {}; 
		MsgOut_c.TlsDataSend_um[0] = {}; 
		MsgOut_c.DetDataSend_um[0] = {}; 
		for (auto it : MsgIn_c.VehDataRecv_um) {
			MsgOut_c.VehDataSend_um[0].push_back(it.second);
		}
		for (auto it : MsgIn_c.TlsDataRecv_um) {
			MsgOut_c.TlsDataSend_um[0].push_back(it.second);
		}
		for (auto it : MsgIn_c.DetDataRecv_um) {
			MsgOut_c.DetDataSend_um[0].push_back(it.second);
		}
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {
		for (int i = 0; i < vissimSock.size(); i++) {
			// if current socket is signal, then only send signal
			if (vissimSockName_um[vissimSock[i]].find(SIGNAL_SOCK_PATTERN) != string::npos) {
				MsgOut_c.VehDataSend_um[vissimSock[i]] = {};
				MsgOut_c.TlsDataSend_um[vissimSock[i]] = {};
				MsgOut_c.DetDataSend_um[vissimSock[i]] = {};
				for (auto it : MsgIn_c.TlsDataRecv_um) {
					MsgOut_c.TlsDataSend_um[vissimSock[i]].push_back(it.second);
				}
				for (auto it : MsgIn_c.DetDataRecv_um) {
					MsgOut_c.DetDataSend_um[vissimSock[i]].push_back(it.second);
				}
			}
			else {
				MsgOut_c.VehDataSend_um[vissimSock[i]] = {};
				MsgOut_c.TlsDataSend_um[vissimSock[i]] = {};
				MsgOut_c.DetDataSend_um[vissimSock[i]] = {};
				for (auto it : MsgIn_c.VehDataRecv_um) {
					MsgOut_c.VehDataSend_um[vissimSock[i]].push_back(it.second);
				}
			}
		}
	}
	
}

void TrafficHelper::runOneStepSimulation() {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		SUMO_TRACI_NAMESPACE::Simulation::step();
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {

	}
	else {

	}
}

int TrafficHelper::recvFromTrafficSimulator(double* simTime, MsgHelper& Msg_c) {
	int recvStatus = 0;

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		recvStatus = this->recvFromSUMO(simTime, Msg_c);
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {
		recvStatus = this->recvFromVISSIM(simTime, Msg_c);
	}
	else {

	}

	return recvStatus;
}

int TrafficHelper::recvFromSUMO(double* simTime, MsgHelper& Msg_c) {
	int recvStatus = 0;

	Msg_c.clearSendStorage();

	VehIdInSimulator.clear();

	*simTime = SUMO_TRACI_NAMESPACE::Simulation::getTime();
	VehIdInSimulator = SUMO_TRACI_NAMESPACE::Vehicle::getIDList();
	
	int nVeh = VehIdInSimulator.size(); // number of vehicles

	if (ENABLE_VERBOSE) {
		printf("SUMO time step %f\n", *simTime);

		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "SUMO time step %f\n", *simTime);
		fclose(f);

	}

	//=================
	// SUMO will have vehicles except for the first few time steps
	//=================
	int nVehSend = 0;

	if (ENABLE_VEH_SUB && nVeh > 0) {

		// only subscribe once
		if (!edgeHasSubscribed) {
			// (self, objectID, domain, dist, varIDs=None, begin=-1073741824.0, end=-1073741824.0, parameters=None)


			// -------------------
			//  subscribe edge
			// -------------------
			for (auto& iter: edgeSubscribeId_v) {
				double radius = 0;
				string id = iter;

				SUMO_TRACI_NAMESPACE::Edge::subscribeContext(id, libsumo::CMD_GET_VEHICLE_VARIABLE, 100, VehDataSubscribeList, 0, tSimuEnd);
			}

			edgeHasSubscribed = true;

		}

		if (!pointHasSubscribed) {
			// (self, objectID, domain, dist, varIDs=None, begin=-1073741824.0, end=-1073741824.0, parameters=None)

			// -------------------
			//  subscribe point
			// -------------------
			//pointNamePoi_v.clear();
			int i = 0; 
			for (auto& iter: pointSubscribeId_v) {
				double x = get<0>(iter.second);
				double y = get<1>(iter.second);
				double z = get<2>(iter.second);
				double r = get<3>(iter.second);

				/*Both polygonsand points of interest may be located within a "layer".Shapes with lowest layer values are below those with a higher layer number.The network itself is drawn as layer 0. An additional file may contain definitions for both points of interestand polygons.*/
				//imgFile	string	A bitmap to use for rendering this polygon
				//angle	float	angle of rendered image in degree
				//lineWidth	double	Drawing width of unfilled polygons in m, default 1

				//string poiName = "RealSimPOI_"+to_string(i);
				string poiName = iter.first;
				/*int 	r,
					int 	g,
					int 	b,
					int 	a = 255*/
				libsumo::TraCIColor color(255,0,255); // use magenta as color
				string type = "RealSim";
				int layer = 999; // put it as a very high layer
				string imgFile = ""; // no image file
				double width = 0; // width	float	width of rendered image in meters
				double height = 0; // height	float	height of rendered image in meters
				double angle = 0; // angle	float	angle of rendered image in degree
				SUMO_TRACI_NAMESPACE::POI::add(poiName, x, y, color, type, layer, imgFile, width, height, angle);
				SUMO_TRACI_NAMESPACE::POI::subscribeContext(poiName, libsumo::CMD_GET_VEHICLE_VARIABLE, r, VehDataSubscribeList, 0, tSimuEnd);

				//pointNamePoi_v.push_back(poiName);
				i++;
			}

			pointHasSubscribed = true;

		}

		if (!allVehicleHasSubscribed) {
			// -------------------
			//  subscribe vehicle
			// -------------------
			
			// get list of all vehicles entered network
			vector <string> vehDepartedId_v = SUMO_TRACI_NAMESPACE::Simulation::getDepartedIDList();
			allVehicleHasSubscribed = true;
			// only able to get vehicle subscription for vehicles already in the network
			int i = 0;
			for (auto & iter: vehicleSubscribeId_v) {
				// if any one of vehicle has not been subscribed yet
				if (!vehicleHasSubscribed_v[i]) {
					allVehicleHasSubscribed = false;
					// id of the vehicle to be subscribed
					string id = iter.first;
					// if the vehicle to subscribe just entered the network
					if (find(vehDepartedId_v.begin(), vehDepartedId_v.end(), id)!=vehDepartedId_v.end()) {
						double radius = iter.second;

						SUMO_TRACI_NAMESPACE::Vehicle::subscribeContext(id, libsumo::CMD_GET_VEHICLE_VARIABLE, radius, VehDataSubscribeList, 0, tSimuEnd);

						vehicleHasSubscribed_v[i] = true;
					}

					i++;
				}

			}

		}
		else {
			int aa = 1;

		}

		// -------------------------------------------------------------------
		//  #176: subscribe ALL vehicles when the YAML `all` flag is set.
		//  `all` means every vehicle in the network, unbounded (the configured radius is
		//  ignored, see init warning). We issue a plain per-vehicle subscription for each
		//  vehicle as it ENTERS, using the same getDepartedIDList() pattern as the per-ego
		//  radius path above. SUMO removes a vehicle's subscription automatically when it
		//  leaves the network, so no client-side tracking set and no explicit unsubscribe
		//  are needed (unsubscribing an already-departed id would error). Results are read
		//  below via Vehicle::getAllSubscriptionResults().
		// -------------------------------------------------------------------
		if (get<0>(subscribeAllVehicle)) {
			vector <string> vehDepartedAll_v = SUMO_TRACI_NAMESPACE::Simulation::getDepartedIDList();
			for (const string& vid : vehDepartedAll_v) {
				SUMO_TRACI_NAMESPACE::Vehicle::subscribe(vid, VehDataSubscribeList, 0, tSimuEnd);
				// #86: subscribe the leader too, so it arrives in the same batch as
				// everything else. Without this, parserSumoSubscription pays a
				// getLeader round-trip PER VEHICLE PER STEP - measured at 161/step on
				// the MLK corridor, 34.7 us each. Only when a precedingVehicle* field
				// is actually forwarded; the subscription is issued once, on entry.
				if (NEED_PRECEDING_VEH) {
					SUMO_TRACI_NAMESPACE::Vehicle::subscribeLeader(
						vid, Config_c->SumoSetup.PrecedingVehicleLookahead, 0, tSimuEnd);
				}
			}
		}

		// this might make it slightly faster to not get repeated vehicles
		unordered_set <string> processedVehId_us;

		// temp buffer to store all vehicle received
		std::unordered_map <std::string, VehFullData_t> VehDataRecv_um_tmp;

		// #177 Phase 2: build the static controlled-link topology once (TLS list is
		// available after the net is loaded). tlsID -> incomingLane -> [(idx, outEdge)].
		if (NEED_NEXT_TLS && !tlsTopologyBuilt) {
			vector<string> tlsIds = SUMO_TRACI_NAMESPACE::TrafficLight::getIDList();
			for (auto& tls : tlsIds) {
				auto links = SUMO_TRACI_NAMESPACE::TrafficLight::getControlledLinks(tls);
				auto& laneMap = TlsTopology_um[tls];
				for (int idx = 0; idx < (int)links.size(); idx++) {
					for (auto& lk : links[idx]) {
						laneMap[lk.fromLane].push_back(std::make_pair(idx, edgeOfLane(lk.toLane)));
					}
				}
			}
			tlsTopologyBuilt = true;
		}

		// #177 Phase 2: fresh per-step TLS-state snapshot; filled on demand in
		// parserSumoSubscription for the (sparse) set of TLS that are some vehicle's
		// next signal this step, then reused across vehicles sharing that signal.
		CurTlsState_um.clear();

		// ===========================================================================
		// 			GET SUBSCRIBED VEHICLE
		// ===========================================================================
		libsumo::ContextSubscriptionResults VehicleSubscribeRaw;
		VehicleSubscribeRaw = SUMO_TRACI_NAMESPACE::Vehicle::getAllContextSubscriptionResults();

		//{
		//int i = 0;
		//for (auto& it : vehicleSubscribeId_v) {
		//	if (vehicleHasSubscribed_v[i]) {
		for (auto& it : VehicleSubscribeRaw) {
			//string id = it.first;
			libsumo::SubscriptionResults VehDataSubscribeResults = it.second;

			for (auto& iter : VehDataSubscribeResults) {

				string tempvehId = iter.first;

				VehFullData_t CurVehData;

				if (processedVehId_us.find(tempvehId) == processedVehId_us.end()) {
					processedVehId_us.insert(tempvehId);
				}
				else {
					continue;
				}

				this->parserSumoSubscription(iter.second, tempvehId, CurVehData);
				//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId]

				//=================
				// save to Msg_c recv buffer
				//=================
				//Msg_c.VehDataRecvAll_v.push_back(CurVehData);
				VehDataRecv_um_tmp[tempvehId] = CurVehData;

				if (ENABLE_VERBOSE) {
					float speed = CurVehData.speed;
					printf("recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);
					fclose(f);

				}

			}
		}

		// -------------------------------------------------------------------
		//  #176: read ALL-vehicle subscription results (plain per-vehicle
		//  subscriptions issued above when the `all` flag is set). Same parser as
		//  the context results; dedup against processedVehId_us so a vehicle already
		//  returned by a context/edge/point subscription is not counted twice.
		// -------------------------------------------------------------------
		if (get<0>(subscribeAllVehicle)) {
			libsumo::SubscriptionResults VehAllSubscribeRaw = SUMO_TRACI_NAMESPACE::Vehicle::getAllSubscriptionResults();
			for (auto& iter : VehAllSubscribeRaw) {
				string tempvehId = iter.first;
				if (processedVehId_us.find(tempvehId) != processedVehId_us.end()) {
					continue;
				}
				processedVehId_us.insert(tempvehId);

				VehFullData_t CurVehData;
				this->parserSumoSubscription(iter.second, tempvehId, CurVehData);
				VehDataRecv_um_tmp[tempvehId] = CurVehData;

				if (ENABLE_VERBOSE) {
					float speed = CurVehData.speed;
					printf("recv SUMO (all) veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "recv SUMO (all) veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);
					fclose(f);
				}
			}
		}

	/*		i++;

		}
		}*/

		// ===========================================================================
		// 			GET SUBSCRIBED point
		// ===========================================================================
		libsumo::ContextSubscriptionResults PointSubscribeRaw;
		PointSubscribeRaw = SUMO_TRACI_NAMESPACE::POI::getAllContextSubscriptionResults();

		for (auto& it : PointSubscribeRaw) {
			string poiName = it.first;
			libsumo::SubscriptionResults VehDataSubscribeResults = it.second;

			for (auto& iter : VehDataSubscribeResults) {

				string tempvehId = iter.first;

				VehFullData_t CurVehData;

				if (processedVehId_us.find(tempvehId) == processedVehId_us.end()) {
					processedVehId_us.insert(tempvehId);
				}
				else {
					continue;
				}

				this->parserSumoSubscription(iter.second, tempvehId, CurVehData);
				//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId]

				//=================
				// save to Msg_c recv buffer
				//=================
				//Msg_c.VehDataRecvAll_v.push_back(CurVehData);
				VehDataRecv_um_tmp[tempvehId] = CurVehData;

				if (ENABLE_VERBOSE) {
					float speed = CurVehData.speed;
					printf("recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);
					fclose(f);
				}
			}
		}


		// ===========================================================================
		// 			GET SUBSCRIBED EDGE
		// ===========================================================================
		libsumo::ContextSubscriptionResults EdgeSubscribeRaw;
		EdgeSubscribeRaw = SUMO_TRACI_NAMESPACE::Edge::getAllContextSubscriptionResults();


		if (edgeHasSubscribed) {
			nVehSend = 0;

			int temp = 1;

			for (auto & it: EdgeSubscribeRaw) {
				libsumo::SubscriptionResults VehDataSubscribeResults = it.second;

				auto iter = VehDataSubscribeResults.begin();
				nVehSend = min((int)VehDataSubscribeResults.size(), 200);

				vector <string> tempVehIdList;
				//tempVehIdList.push_back(egoIdVec[iC]);
				for (int iV = 0; iV < VehDataSubscribeResults.size(); iV++) {
					//if (iter->first != egoIdVec[iC]) {
					tempVehIdList.push_back(iter->first);
					//}
					iter++;
				}

				for (int iV = 0; iV < nVehSend; iV++) {
					VehFullData_t CurVehData;

					string tempvehId = tempVehIdList[iV];

					if (processedVehId_us.find(tempvehId) == processedVehId_us.end()) {
						processedVehId_us.insert(tempvehId);
					}
					else {
						continue;
					}

					this->parserSumoSubscription(VehDataSubscribeResults[tempvehId], tempvehId, CurVehData);
					//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId]



					//Msg_c.packVehData(CurVehData, tempVehDataBuffer[iC], &tempVehDataByte[iC]);
					//tempVehDataSend_v.push_back(CurVehData);

					//tempVehIdSend_v.push_back(tempvehId);

					//=================
					// save to Msg_c recv buffer          
					//=================
					//Msg_c.VehDataRecvAll_v.push_back(CurVehData);
					VehDataRecv_um_tmp[tempvehId] = CurVehData;

					if (ENABLE_VERBOSE) {
						float speed = CurVehData.speed;
						printf("recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);

						FILE* f = fopen(MasterLogName.c_str(), "a");
						fprintf(f, "recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);
						fclose(f);

					}

					//}
					/*VehData_t iVehFullData;
					Sock_c.depackVehData(tempVehDataBuffer[iC]+3, &iVehFullData);*/
				}
			}
			//nVehSend = tempVehIdSend_v.size();
		}


		// #86 pass 2: the leader's SPEED. Deliberately not done in
		// parserSumoSubscription: the leader may be parsed after the vehicle that
		// needs it, so a lookup there would depend on hash iteration order - the
		// same vehicle could get a real speed on one run and a fallback on the
		// next. Here every vehicle is parsed, so the map is complete.
		//
		// The leader is itself a subscribed vehicle in the ordinary case, so its
		// speed is already in hand and costs nothing. Only a leader outside the
		// subscribed set (context-mode configs, or one beyond the subscribed
		// region) needs asking, and that is counted so a config paying the old
		// cost is visible in the log instead of only under a profiler.
		if (NEED_PRECEDING_VEH) {
			for (auto& it : VehDataRecv_um_tmp) {
				VehFullData_t& veh = it.second;
				if (veh.hasPrecedingVehicle == 0) {
					continue;
				}
				auto leadIt = VehDataRecv_um_tmp.find(veh.precedingVehicleId);
				if (leadIt != VehDataRecv_um_tmp.end()) {
					veh.precedingVehicleSpeed = leadIt->second.speed;
				}
				else {
					// Leader is not itself subscribed -- usually one that
					// entered this step, so it has no results yet. Ask SUMO,
					// exactly as every vehicle did before #86.
#ifdef RS_DEBUG
					nLeaderSpeedQueried++;
#endif
					veh.precedingVehicleSpeed = SUMO_TRACI_NAMESPACE::Vehicle::getSpeed(
						veh.precedingVehicleId);
				}
			}
#ifdef RS_DEBUG
			// Reported as a periodic total, never per step. A step-by-step print
			// is both noise (6481 lines in one run) and a syscall in the hot loop
			// -- it would show up as the very cost this change removes.
			nStepsSinceLeaderReport++;
			if (nStepsSinceLeaderReport >= 3000) {
				if (nLeaderIdQueried > 0 || nLeaderSpeedQueried > 0) {
					printf("leader lookups: %ld id and %ld speed fallbacks over the "
						"last %ld steps (a leader that is not itself subscribed -- "
						"usually one that entered this step -- still needs asking)\n",
						nLeaderIdQueried, nLeaderSpeedQueried, nStepsSinceLeaderReport);
				}
				nLeaderIdQueried = 0;
				nLeaderSpeedQueried = 0;
				nStepsSinceLeaderReport = 0;
			}
#endif
		}

		// !!!temporary fix
		// if doing vehicle simulator, e.g., CarMaker, only send limited number of vehicles
		if (ENABLE_VEH_SIMULATOR) {
			libsumo::TraCIPosition posEgo = SUMO_TRACI_NAMESPACE::Vehicle::getPosition(Config_c->CarMakerSetup.EgoId);

			// sort distance, pair distance to ego, vehId
			vector <pair <double, string>> dist2ego_v;
			for (auto& it : VehDataRecv_um_tmp) {
				dist2ego_v.push_back(make_pair(pow(it.second.positionX - posEgo.x, 2) + pow(it.second.positionY - posEgo.y, 2), it.first));
			}
			sort(dist2ego_v.begin(), dist2ego_v.end());

			for (auto&it: dist2ego_v) {
				string tempvehId = it.second;

				if (this->shouldSendVehicle(tempvehId, *simTime)) {
					Msg_c.VehDataRecv_um[tempvehId] = VehDataRecv_um_tmp[tempvehId];
				}

				if (Msg_c.VehDataRecv_um.size() >= N_MAX_VEH) {
					break;
				}
			}
				
		}
		else {
			for (auto& it : VehDataRecv_um_tmp) {
				string tempvehId = it.first;

				// if not doing driving simulator, shouldSendVehicle will currently always return 1
				if (this->shouldSendVehicle(tempvehId, *simTime)) {
					Msg_c.VehDataRecv_um[tempvehId] = VehDataRecv_um_tmp[tempvehId];
				}
			}
		}


		//=================
		// remove vehicle from list
		//=================
		vector <string> vehArrivedIdList = SUMO_TRACI_NAMESPACE::Simulation::getArrivedIDList();
		for (int i = 0; i < vehArrivedIdList.size(); i++) {
			VehicleId2EdgeList_um.erase(vehArrivedIdList[i]);
			VehicleId2Tls_um.erase(vehArrivedIdList[i]);   // #177 Phase 2 TLS cache
		}

	}

	//char tempDetDataBuffer[NCLIENT_MACRO][8096];
	//int tempDetDataByte[NCLIENT_MACRO] = { 0 };

	if (ENABLE_DET_SUB) {
		if (!DetectorSubscriptionFlags.patternHasSubscribed) {
			//===================================================
			// Retreive DETECTOR configuration for the scenario BEFORE simulation starts
			//===================================================
			vector <string> detAreaAllId_v = SUMO_TRACI_NAMESPACE::LaneArea::getIDList();
			vector <string> detInductAllId_v = SUMO_TRACI_NAMESPACE::InductionLoop::getIDList();

			// obtain detector ids of the selected intersection and subscribe to results

			vector <int> detSubscribeList = {libsumo::LAST_STEP_VEHICLE_NUMBER};

			for (auto it: Config_c->SubscriptionDetectorList.pattern_v){
				for (int iD = 0; iD < detAreaAllId_v.size(); iD++) {
					if (detAreaAllId_v[iD].find(it) != std::string::npos) {
						SUMO_TRACI_NAMESPACE::LaneArea::subscribe(detAreaAllId_v[iD], detSubscribeList, 0, tSimuEnd);
					}
				}
			}

			DetectorSubscriptionFlags.patternHasSubscribed = true;

		}

		libsumo::SubscriptionResults DetSubscribeRaw;
		DetSubscribeRaw = SUMO_TRACI_NAMESPACE::LaneArea::getAllSubscriptionResults();


		vector <DetectorData_t> tempDetData_v;
		
		std::shared_ptr<libsumo::TraCIInt> tempIntPtr;

		for (auto& it : DetSubscribeRaw) {
			DetectorData_t curDet;

			curDet.id = 0;
			curDet.name = it.first;
			tempIntPtr = static_pointer_cast<libsumo::TraCIInt> (it.second[libsumo::LAST_STEP_VEHICLE_NUMBER]);
			if (tempIntPtr->value > 0) {
				curDet.state = 1;
			}
			else {
				curDet.state = 0;
			};

			tempDetData_v.push_back(curDet);
		}

		Msg_c.DetDataRecv_um["NA"] = make_tuple(0, "NA", tempDetData_v);

	}

	if (ENABLE_SIG_SUB) {
		if (!SignalSubscriptionFlags.idHasSubscribed) {
			//===================================================
			// Retreive DETECTOR configuration for the scenario BEFORE simulation starts
			//===================================================
			vector <string> sigAllId_v = SUMO_TRACI_NAMESPACE::TrafficLight::getIDList();

			// obtain detector ids of the selected intersection and subscribe to results

			vector <int> sigSubscribeList = { libsumo::TL_RED_YELLOW_GREEN_STATE };

			printf("[signal-sub] subAllSignalFlag=%d  namedIds=%zu  tlsInNetwork=%zu\n",
				(int)Config_c->SubscriptionSignalList.subAllSignalFlag,
				Config_c->SubscriptionSignalList.signalId_v.size(),
				sigAllId_v.size());

			if (!Config_c->SubscriptionSignalList.subAllSignalFlag) {
				for (auto it : Config_c->SubscriptionSignalList.signalId_v) {
					SUMO_TRACI_NAMESPACE::TrafficLight::subscribe(it.c_str(), sigSubscribeList, 0, tSimuEnd);
				}
			}
			else {
				for (auto it : sigAllId_v) {
					SUMO_TRACI_NAMESPACE::TrafficLight::subscribe(it.c_str(), sigSubscribeList, 0, tSimuEnd);
				}
			}

			SignalSubscriptionFlags.idHasSubscribed = true;

		}

		// if already subscribed, then get signal data out
		libsumo::SubscriptionResults SigSubscribeRaw;
		SigSubscribeRaw = SUMO_TRACI_NAMESPACE::TrafficLight::getAllSubscriptionResults();

		vector <TrafficLightData_t> tempSigData_v;

		std::shared_ptr<libsumo::TraCIString> tempStringPtr;

		int idx = 0;
		for (auto& it : SigSubscribeRaw) {
			TrafficLightData_t curSig;

			curSig.id = idx++;
			curSig.name = it.first;
			tempStringPtr = static_pointer_cast<libsumo::TraCIString> (it.second[libsumo::TL_RED_YELLOW_GREEN_STATE]);
			curSig.state = tempStringPtr->value;

			Msg_c.TlsDataRecv_um[curSig.name] = curSig;

		}



	}

	return recvStatus;
}


// this is only for visualization
int TrafficHelper::shouldSendVehicle(std::string vehicleId, double simTime) {
	int shouldSendFlag = 0;

	if (!ENABLE_VEH_SIMULATOR) {
		shouldSendFlag = 1;
	}
	else if (ENABLE_VEH_SIMULATOR){
		if (vehicleId.compare(Config_c->CarMakerSetup.EgoId) == 0) {
			shouldSendFlag = 1;
		}
		else {
			// if does not have yet, add it
			if (VehicleId2SubCount_um.find(vehicleId) == VehicleId2SubCount_um.end()) {
				VehicleId2SubCount_um[vehicleId] = 0;
			}
			else {
				// if last sub and this one is continous, then add count by 1, otherwise reset count
				if (abs(VehicleId2LastSubTime_um[vehicleId] - simTime) <= SIM_STEP + 1e-5) {
					VehicleId2SubCount_um[vehicleId] = VehicleId2SubCount_um[vehicleId] + 1;
				}
				else {
					VehicleId2SubCount_um[vehicleId] = 0;
				}
			}
			VehicleId2LastSubTime_um[vehicleId] = simTime;

			// check last sub time
			if (VehicleId2SubCount_um[vehicleId] > SUB_CONT_TIME_THLD / SIM_STEP) {
				shouldSendFlag = 1;
			}
		}
	}

	return shouldSendFlag;
}


void TrafficHelper::parserSumoSubscription(libsumo::TraCIResults VehDataSubscribeTraciResults, std::string vehId, VehFullData_t& CurVehData) {

	// if does not have this vehicle yet
	if (VehicleId2EdgeList_um.find(vehId) == VehicleId2EdgeList_um.end()) {
		vector <string> edgeList = SUMO_TRACI_NAMESPACE::Vehicle::getRoute(vehId);
		VehicleId2EdgeList_um[vehId] = edgeList;
		//vector <libsumo::TraCIConnection> nextLinkList = traci.vehicle.getNextLinks(vehId);
		//int aa = 1;
	}

	//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId];
	std::shared_ptr<libsumo::TraCIString> tempStringPtr;
	std::shared_ptr<libsumo::TraCIInt> tempIntPtr;
	std::shared_ptr<libsumo::TraCIDouble> tempDoublePtr;
	std::shared_ptr<libsumo::TraCIPosition> tempPositionPtr;
	std::shared_ptr<libsumo::TraCIColor> tempColorPtr;
	std::shared_ptr<libsumo::TraCIDouble> tempDoublePtr2;

	CurVehData.id = vehId;

	//for (int iD = 0; iD < VehDataSubscribeList.size(); iD++) {
	tempStringPtr = static_pointer_cast<libsumo::TraCIString> (VehDataSubscribeTraciResults[libsumo::VAR_TYPE]);
	CurVehData.type = tempStringPtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SPEED]);
	CurVehData.speed = tempDoublePtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SPEED_WITHOUT_TRACI]);
	float speedWithoutTraci = tempDoublePtr->value;


	tempPositionPtr = static_pointer_cast<libsumo::TraCIPosition> (VehDataSubscribeTraciResults[libsumo::VAR_POSITION3D]);
	CurVehData.positionX = tempPositionPtr->x;
	CurVehData.positionY = tempPositionPtr->y;
	CurVehData.positionZ = tempPositionPtr->z;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_ANGLE]);
	CurVehData.heading = tempDoublePtr->value;

	tempColorPtr = static_pointer_cast<libsumo::TraCIColor> (VehDataSubscribeTraciResults[libsumo::VAR_COLOR]);
	uint8_t r = tempColorPtr->r;
	uint8_t g = tempColorPtr->g;
	uint8_t b = tempColorPtr->b;
	uint8_t a = tempColorPtr->a;
	CurVehData.color = (r << 24) + (g << 16) + (b << 8) + a;

	tempStringPtr = static_pointer_cast<libsumo::TraCIString> (VehDataSubscribeTraciResults[libsumo::VAR_ROAD_ID]);
	CurVehData.linkId = tempStringPtr->value;

	tempIntPtr = static_pointer_cast<libsumo::TraCIInt> (VehDataSubscribeTraciResults[libsumo::VAR_LANE_INDEX]);
	CurVehData.laneId = (int32_t) tempIntPtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_DISTANCE]);
	//tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_LANEPOSITION]);
	CurVehData.distanceTravel = (float) tempDoublePtr->value;


	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_ACCELERATION]);
	CurVehData.acceleration = tempDoublePtr->value;

	CurVehData.speedDesired = CurVehData.speed;
	CurVehData.accelerationDesired = 0.0;

	//=================
	// get preceding vehicle
	//=================
	// #177: getLeader(...,1000) + getSpeed are per-vehicle TraCI round-trips; only
	// pay them when a precedingVehicle* field is actually forwarded (NEED_PRECEDING_VEH).
	CurVehData.precedingVehicleId = "";
	CurVehData.precedingVehicleDistance = -1.0;
	CurVehData.hasPrecedingVehicle = 0;
	CurVehData.precedingVehicleSpeed = -1.0;
	if (NEED_PRECEDING_VEH) {
		// The leader rides in this vehicle's own subscription (subscribeLeader,
		// issued on entry), so the common path costs no round-trip at all.
		//
		// Unpacking needs care. libtraci has no VAR_LEADER case in its compound
		// reader, so it falls through to a generic rule: a two-element compound of
		// (string, double) is wrapped in a TraCIRoadPosition, edgeID = the string,
		// pos = the double. For a leader that is exactly (id, gap) - a misleading
		// type name carrying the right data. Verified against getLeader over 464371
		// samples on SUMO 1.22: 0 id mismatches, max gap error 0.0. Identical on
		// 1.23 and 1.27.1.
		//
		// That mapping is an implementation detail, not a documented contract
		// (eclipse-sumo/sumo#15962 is about giving these values proper handling), so
		// a cast that does not land falls back to asking rather than silently
		// reporting "no leader" - which would quietly degrade car-following.
		// NOT "does this vehicle have a leader" -- "did we get the subscribed
		// value". A vehicle with no leader HAS the value: SUMO sends ("", -1),
		// which lands in the branch below and costs no round-trip. Measured:
		// VAR_LEADER present for 464371 of 464371 vehicles, id fallbacks 0/step
		// while ~30 of 161 vehicles per step have no leader.
		bool gotSubscribedLeader = false;
		auto leaderIt = VehDataSubscribeTraciResults.find(libsumo::VAR_LEADER);
		if (leaderIt != VehDataSubscribeTraciResults.end() && leaderIt->second) {
			if (auto rp = std::dynamic_pointer_cast<libsumo::TraCIRoadPosition>(leaderIt->second)) {
				CurVehData.precedingVehicleId = rp->edgeID;
				CurVehData.precedingVehicleDistance = rp->pos;
				gotSubscribedLeader = true;
			}
		}
		if (!gotSubscribedLeader) {
			// The value was ABSENT or an unexpected type -- not "no leader".
			// Ask SUMO directly: same call as before #86, same answer, just a
			// round-trip instead of free.
			// Absent means this vehicle came from a CONTEXT subscription (the
			// container is subscribed, not the vehicle, so it carries no
			// VAR_LEADER); unexpected type means a future SUMO returns the pair
			// differently.
#ifdef RS_DEBUG
			nLeaderIdQueried++;
#endif
			pair<string, double> leaderIdNGap = SUMO_TRACI_NAMESPACE::Vehicle::getLeader(
				vehId, Config_c->SumoSetup.PrecedingVehicleLookahead);
			CurVehData.precedingVehicleId = get<0>(leaderIdNGap);
			CurVehData.precedingVehicleDistance = get<1>(leaderIdNGap);
		}
		if (CurVehData.precedingVehicleId.compare("") != 0) {
			CurVehData.hasPrecedingVehicle = 1;
			// precedingVehicleSpeed is filled by fillPrecedingVehicleSpeed() once
			// every vehicle has been parsed: the leader may be parsed AFTER this
			// one, so resolving it here would depend on hash iteration order.
		}
	}

	//=================
	// get signal information
	//=================
	// #177: getNextTLS is O(remaining route length) and was called per vehicle per
	// step. Its result depends only on the route, so seed it ONCE per vehicle (and on
	// reroute) and reconstruct the nearest signal every step by O(1) arithmetic. Only
	// when a signalLight* field is actually forwarded (NEED_NEXT_TLS).
	CurVehData.signalLightId = "";
	CurVehData.signalLightHeadId = -1;
	CurVehData.signalLightDistance = -1.0;
	CurVehData.signalLightColor = -1;
	if (NEED_NEXT_TLS) {
		// route id (subscribed, free) keys the cache and flags reroutes.
		std::shared_ptr<libsumo::TraCIString> routeIdPtr =
			static_pointer_cast<libsumo::TraCIString>(VehDataSubscribeTraciResults[libsumo::VAR_ROUTE_ID]);
		string routeId = routeIdPtr->value;
		// raw DOUBLE odometer (CurVehData.distanceTravel is cast to float, which would
		// round cumDist-odo off getNextTLS's double dist by ~1 float ULP).
		double odo = static_pointer_cast<libsumo::TraCIDouble>(
			VehDataSubscribeTraciResults[libsumo::VAR_DISTANCE])->value;

		// Is this vehicle one whose motion an external environment owns? Its route
		// is then the one thing SUMO still decides for it, and a reroute nobody
		// asked for is how a co-sim silently stops being signal-aware -- see the
		// two guards below.
		// Same test the mirror uses, so the two can never disagree about which
		// vehicle is externally driven -- and so the CarMaker ego, which is also
		// moveToXY-placed, gets the per-tick reseed it has always needed.
		const bool externallyDriven = (externalEgoOwnerOf(vehId) != ExternalEgoOwner::None);

		// seed once per vehicle (or when the route changed): one getNextTLS walk
		// captures every TLS ahead of this vehicle for the rest of its route.
		auto cacheIt = VehicleId2Tls_um.find(vehId);
		// #86 diagnostic: did THIS tick re-seed the cache? Recorded by the opt-in
		// route log below, because "the route changed" and "the next signal
		// vanished" are the two halves of the question and only their coincidence
		// distinguishes a replaced route from a passed last signal.
		// An externally-driven ego reseeds EVERY tick. The cache is a #177 cost
		// optimisation: seed the signals-ahead list once, then locate the nearest by
		// arithmetic (cumDist - odo) instead of a TraCI call per vehicle per step.
		// That arithmetic assumes the odometer advances monotonically with distance
		// travelled along the route -- true for a driven vehicle, false for one
		// placed by moveToXY.
		//
		// Measured on mlk_eco_driving, closed loop: the ego's odometer FREEZES for
		// several ticks at a junction and then REGRESSES (-17.2 m at t=29175.6),
		// while the Carla ego is on the road 0.02 m off the lane centreline moving
		// at 4.5 m/s. cumDist is seeded once and never revisited, so one regression
		// biases every later distance by that amount for good -- median error
		// +21.7 m, worst -176 m -- and the list then names a signal the ego has
		// already passed. That is the whole t.id != nt[0].id divergence: it starts
		// on the exact tick the odometer freezes (1706 of 1710 mismatches follow it)
		// and it is NOT caused by lane changes -- L0's ego changes lane five times,
		// the head correctly follows (1->2), and it never diverges once in 5089
		// ticks.
		//
		// Reseeding per tick makes cumDist = t.dist + odo evaluated NOW, so
		// cumDist - odo collapses to t.dist -- SUMO's own live answer -- and the id,
		// head and distance all come from one consistent reading. Costs one
		// getNextTLS per tick for ONE vehicle; the #177 saving across the other ~180
		// is untouched.
		const bool tlsCacheReseeded =
			(cacheIt == VehicleId2Tls_um.end() || cacheIt->second.routeId != routeId
			 || externallyDriven);
		// #86 diagnostic scratch, filled by the head reconstruction below and read by
		// the opt-in route log. 0 = no head resolved, 1 = fast path (topology table),
		// 2 = slow path (raw getNextTLS).
		int         egoHeadPath    = 0;
		int         egoRawTlsCount = -1;
		std::string egoRawTlsId;
		int         egoRawTlsIndex = -1;
		double      egoRawTlsDist  = -1.0;
		if (tlsCacheReseeded) {
			// The route changed, this is the first sight of the vehicle, or the
			// vehicle is externally driven and reseeds every tick (see above).
			// Refresh the cached edge list: it is read below to reconstruct the
			// signal head, and it is captured when a vehicle is FIRST seen -- so
			// after a reroute it describes a route the vehicle is no longer on.
			if (cacheIt != VehicleId2Tls_um.end()) {
				vector<string> newEdges = SUMO_TRACI_NAMESPACE::Vehicle::getRoute(vehId);

				// GUARD: a route REPLACED BY THE MIRROR -- which is not the same
				// event as a route change. Changing a vehicle's route is a normal
				// thing for an application to do: route guidance is a CAV function,
				// and a rerouting device is the scenario's own business. Raising on
				// "the routeId is different" would mean the first controller that
				// reroutes its ego teaches everyone to ignore this.
				//
				// What is not normal is SUMO replacing the route because moveToXY
				// could not keep the fed pose on it, and that has a signature: per
				// SUMO's semantics the replacement "consists of that edge only"
				// (see SumoSetup.EgoKeepRoute). A single-edge route is what no
				// deliberate reroute produces, and it also predicts the damage --
				// one edge holds at most one signal, so a controller planning on
				// this vehicle is about to lose its stop bar.
				//
				// Ordinary looping cannot reach here at all: <route repeat="n"> is
				// expanded when the scenario loads, so a vehicle laps a 70-edge
				// corridor 21 times under ONE routeId holding 1470 edges (measured
				// on the MLK arterial scenario).
				if (externallyDriven && newEdges.size() <= 1) {
					fixs::RS_XIL_GUARD("ego_sumo_route_replaced", 1.0, 0.0);
				}
				VehicleId2EdgeList_um[vehId] = std::move(newEdges);
			}
			VehTlsCache entry;
			entry.routeId = routeId;
			vector<libsumo::TraCINextTLSData> seed = SUMO_TRACI_NAMESPACE::Vehicle::getNextTLS(vehId);
			entry.list.reserve(seed.size());
			for (auto& t : seed) {
				entry.list.push_back(TlsOnRoute{ t.id, t.tlIndex, t.dist + odo });
			}
			VehicleId2Tls_um[vehId] = std::move(entry);
			cacheIt = VehicleId2Tls_um.find(vehId);
		}

		// nearest TLS still ahead: distance = cumDist - odo, first strictly positive
		// (matches getNextTLS, which only returns upcoming signals and drops one the
		// step the vehicle passes it). State fetched once per step per distinct TLS
		// and reused across vehicles sharing it.
		for (auto& t : cacheIt->second.list) {
			double dist = t.cumDist - odo;
			if (dist > 0.0) {
				CurVehData.signalLightId = t.id;
				CurVehData.signalLightDistance = dist;

				// tlIndex (signal head) is LANE-dependent, not route-static, so the
				// seed value goes stale after a lane change. Fast path: reconstruct it
				// byte-exact from the static controlled-link topology keyed on the
				// subscribed CURRENT lane + the next route edge (the connection this
				// vehicle takes through the signal).
				int headIdx = -1;
				auto topoIt = TlsTopology_um.find(t.id);
				if (topoIt != TlsTopology_um.end()) {
					string curLaneId = static_pointer_cast<libsumo::TraCIString>(
						VehDataSubscribeTraciResults[libsumo::VAR_LANE_ID])->value;
					int ridx = static_pointer_cast<libsumo::TraCIInt>(
						VehDataSubscribeTraciResults[libsumo::VAR_ROUTE_INDEX])->value;
					vector<string>& routeEdges = VehicleId2EdgeList_um[vehId];
					string outEdge = (ridx + 1 < (int)routeEdges.size()) ? routeEdges[ridx + 1] : "";
					auto laneIt = topoIt->second.find(curLaneId);
					if (laneIt != topoIt->second.end()) {
						for (auto& pr : laneIt->second) {
							if (pr.second == outEdge) { headIdx = pr.first; break; }
						}
					}
				}
				if (headIdx < 0) {
					// Fallback: the vehicle is mid-junction on an internal lane, or the
					// signal is several edges ahead so its incoming lane is a best-lane
					// projection rather than the current lane. Ask getNextTLS.
					//
					// MATCH ON THE SIGNAL ID. This used to take nt[0].tlIndex outright,
					// which is only correct while getNextTLS's first entry happens to be
					// the same signal the cached list resolved to -- and those two views
					// diverge whenever the odometer bookkeeping this cache is indexed by
					// drifts from SUMO's live position, which is exactly what an
					// externally-driven (moveToXY) ego does. It then stapled one light's
					// head onto a different light.
					//
					// Measured on mlk_eco_driving L2 before this fix: the fallback runs
					// on 84.8% of ticks (not "rare"), getNextTLS's first entry was a
					// DIFFERENT signal than the cached one on 2249 of 5511 of them, and
					// the head was reported as e.g. 202605864 head 9 when the 9 belonged
					// to 202587081 81 m away. Head indices that the application then
					// rejected: 1669 of those 2249 (74.2%), against 0 of 3262 where the
					// two agreed. A SUMO-driven ego (L0) never diverges, so it never saw
					// this -- 0 rejections in 6502 ticks.
					//
					// No match means SUMO does not consider t.id upcoming any more; the
					// seeded index is a better answer than another signal's head.
					vector<libsumo::TraCINextTLSData> nt =
						SUMO_TRACI_NAMESPACE::Vehicle::getNextTLS(vehId);
					headIdx = t.index;
					for (const libsumo::TraCINextTLSData& cand : nt) {
						if (cand.id == t.id) { headIdx = cand.tlIndex; break; }
					}
					// #86 diagnostic: which path answered, and what SUMO ITSELF said.
					// The topology table is keyed on controlled-link fromLane, i.e.
					// APPROACH lanes, so a vehicle on an internal junction lane can
					// never match it and always lands here -- in L0 and L2 alike. So
					// when the two runs disagree about the head mid-junction, the
					// disagreement is in getNextTLS's own answer, not in our table.
					// Record it raw so that is checkable rather than inferred.
					egoHeadPath = 2;
					egoRawTlsCount = (int)nt.size();
					if (!nt.empty()) {
						egoRawTlsId = nt[0].id;
						egoRawTlsIndex = nt[0].tlIndex;
						egoRawTlsDist = nt[0].dist;
					}
				}
				else {
					egoHeadPath = 1;
				}
				CurVehData.signalLightHeadId = headIdx;

				// state for this TLS, fetched once per step per distinct TLS and reused
				// across vehicles sharing it; indexed by the reconstructed head.
				auto sIt = CurTlsState_um.find(t.id);
				if (sIt == CurTlsState_um.end()) {
					sIt = CurTlsState_um.emplace(
						t.id, SUMO_TRACI_NAMESPACE::TrafficLight::getRedYellowGreenState(t.id)).first;
				}
				const string& state = sIt->second;
				char tlsState = (headIdx >= 0 && headIdx < (int)state.size()) ? state[headIdx] : 0;
				CurVehData.signalLightColor = tlsStateToColor(tlsState);
				break;
			}
		}

		// ---- #86 opt-in: what SUMO says this vehicle's ROUTE and NEXT SIGNAL are ----
		// Set RS_EGO_ROUTE_LOG=<path> to record one row per exchange for the id in
		// RS_EGO_ROUTE_ID (default "ego"). Off by default and costs nothing then.
		//
		// This exists because "the ego lost its next signal" has two candidate
		// causes that look identical downstream -- the route was replaced (so there
		// is nothing ahead to find) versus the cached list is fine but the odometer
		// arithmetic no longer resolves -- and no existing output distinguishes
		// them. routeId + edge count + route index + odometer + cache size, side by
		// side with the resulting signalLightId, separates them on the first run:
		// a replaced route shows nRouteEdges collapse and reseeded=1 on the same
		// tick the signal goes empty; an odometer problem shows the cache still
		// populated with the signal empty and reseeded=0.
		if (const char* erl = std::getenv("RS_EGO_ROUTE_LOG")) {
			const char* wantEnv = std::getenv("RS_EGO_ROUTE_ID");
			const std::string wantId = wantEnv ? wantEnv : "ego";
			if (vehId == wantId) {
				static FILE* f = nullptr;
				if (!f) {
					f = fopen(erl, "w");
					if (f) fprintf(f, "simTime,vehId,routeId,nRouteEdges,routeIndex,laneId,"
					                  "odo,reseeded,nCachedTls,signalLightId,signalLightHeadId,"
					                  "signalLightDistance,headPath,outEdge,curEdgeAtIdx,"
					                  "rawTlsCount,rawTlsId,rawTlsIndex,rawTlsDist\n");
				}
				if (f) {
					const std::vector<std::string>& re = VehicleId2EdgeList_um[vehId];
					int ridxLog = -1;
					std::string laneLog;
					auto riIt = VehDataSubscribeTraciResults.find(libsumo::VAR_ROUTE_INDEX);
					if (riIt != VehDataSubscribeTraciResults.end())
						ridxLog = static_pointer_cast<libsumo::TraCIInt>(riIt->second)->value;
					auto liIt = VehDataSubscribeTraciResults.find(libsumo::VAR_LANE_ID);
					if (liIt != VehDataSubscribeTraciResults.end())
						laneLog = static_pointer_cast<libsumo::TraCIString>(liIt->second)->value;
					// the two inputs the fast path keys on, so a miss is attributable:
					// outEdge is what it looked FOR, curEdgeAtIdx is whether the index
					// even points at the edge the vehicle is on.
					const std::string outEdgeLog =
						(ridxLog >= 0 && ridxLog + 1 < (int)re.size()) ? re[ridxLog + 1] : "";
					const std::string curEdgeAtIdxLog =
						(ridxLog >= 0 && ridxLog < (int)re.size()) ? re[ridxLog] : "";
					fprintf(f, "%.3f,%s,%s,%zu,%d,%s,%.3f,%d,%zu,%s,%d,%.3f,%d,%s,%s,%d,%s,%d,%.3f\n",
					        SUMO_TRACI_NAMESPACE::Simulation::getTime(), vehId.c_str(),
					        routeId.c_str(), re.size(), ridxLog, laneLog.c_str(), odo,
					        tlsCacheReseeded ? 1 : 0,
					        (cacheIt != VehicleId2Tls_um.end()) ? cacheIt->second.list.size() : (size_t)0,
					        CurVehData.signalLightId.c_str(), CurVehData.signalLightHeadId,
					        CurVehData.signalLightDistance,
					        egoHeadPath, outEdgeLog.c_str(), curEdgeAtIdxLog.c_str(),
					        egoRawTlsCount, egoRawTlsId.c_str(), egoRawTlsIndex, egoRawTlsDist);
					fflush(f);
				}
			}
		}

		// NOTE: "the ego had a next signal and now has none" is deliberately NOT
		// guarded here. signalLightId only goes empty when no cached entry is still
		// ahead of the odometer, which means either the cached list is empty -- the
		// route was replaced by one without signals, which changed routeId and the
		// guard above already fired -- or the vehicle has passed the last signal on
		// its route, which is ordinary. Guarding it would warn at the end of every
		// run and teach everyone to ignore the one warning that matters.
	}

	//=================
	// get speed limit
	//=================
	// retrieve current speed limit
	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_ALLOWED_SPEED]);
	tempDoublePtr2 = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SPEED_FACTOR]);
	CurVehData.speedLimit = tempDoublePtr->value / tempDoublePtr2->value;
	// VAR_ALLOWED_SPEED is "max speed on the current lane AND speed factor", i.e.
	// what this particular vehicle will cruise at. Reported as-is so consumers do
	// not have to reconstruct it (a speedLimit * speedFactor product would also
	// miss the vType maxSpeed cap that SUMO already applies here).
	CurVehData.speedFreeFlow = tempDoublePtr->value;

	// retrieve next speed limit
	vector <string> edgeRouteList = VehicleId2EdgeList_um[vehId];

	tempIntPtr = static_pointer_cast<libsumo::TraCIInt> (VehDataSubscribeTraciResults[libsumo::VAR_ROUTE_INDEX]);
	int edgeListIdx = tempIntPtr->value;
	CurVehData.linkIdNext = "";
	if (edgeListIdx + 1 < edgeRouteList.size()) {
		CurVehData.linkIdNext = edgeRouteList[edgeListIdx + 1];
	}

	tempStringPtr = static_pointer_cast<libsumo::TraCIString> (VehDataSubscribeTraciResults[libsumo::VAR_VEHICLECLASS]);
	CurVehData.vehicleClass = tempStringPtr->value;

	CurVehData.speedLimitNext = -1;
	// if there is speed limit for next link and vehclass
	if (EdgeVehClass2SpeedLimit_um.find(make_pair(CurVehData.linkIdNext, CurVehData.vehicleClass)) != EdgeVehClass2SpeedLimit_um.end()) {
		CurVehData.speedLimitNext = EdgeVehClass2SpeedLimit_um[make_pair(CurVehData.linkIdNext, CurVehData.vehicleClass)];
	}

	// for following information, need to get them one by one rather than through subscription
	CurVehData.speedLimitChangeDistance = -1;
	//if (AllEdgeList.find(CurVehData.linkIdNext) != AllEdgeList.end()) {
	//	CurVehData.speedLimitChangeDistance = max(traci.vehicle.getDrivingDistance(vehId, CurVehData.linkIdNext, 0), -1.0);
	//}

	//=================
	// grade
	//=================
	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SLOPE]);
	CurVehData.grade = tempDoublePtr->value * M_PI/180;

	//=================
	// get lane change
	//=================
	//vector <libsumo::TraCIBestLanesData> bestLanesData = traci.vehicle.getBestLanes(vehId);
	CurVehData.activeLaneChange = 0;


	//=================
	// get length, width, height
	//=================
	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_LENGTH]);
	CurVehData.length = tempDoublePtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_WIDTH]);
	CurVehData.width = tempDoublePtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_HEIGHT]);
	CurVehData.height = tempDoublePtr->value;
	//=================
	// get vehicle indicators
	//=================
	//Name	Bit
	//VEH_SIGNAL_BLINKER_RIGHT	0
	//VEH_SIGNAL_BLINKER_LEFT	1
	//VEH_SIGNAL_BLINKER_EMERGENCY	2
	//VEH_SIGNAL_BRAKELIGHT	3
	//VEH_SIGNAL_FRONTLIGHT	4
	//VEH_SIGNAL_FOGLIGHT	5
	//VEH_SIGNAL_HIGHBEAM	6
	//VEH_SIGNAL_BACKDRIVE	7
	//VEH_SIGNAL_WIPER	8
	//VEH_SIGNAL_DOOR_OPEN_LEFT	9
	//VEH_SIGNAL_DOOR_OPEN_RIGHT	10
	//VEH_SIGNAL_EMERGENCY_BLUE	11
	//VEH_SIGNAL_EMERGENCY_RED	12
	//VEH_SIGNAL_EMERGENCY_YELLOW	13
	tempIntPtr = static_pointer_cast<libsumo::TraCIInt> (VehDataSubscribeTraciResults[libsumo::VAR_SIGNALS]);
	// (n & (1 << k)) >> k
	CurVehData.lightIndicators = tempIntPtr->value;
}



int TrafficHelper::recvFromVISSIM(double* simTime, MsgHelper& Msg_c) {
	int recvStatus = 0;

	SocketHelper Sock_temp;
	
	int simStateRecv;
	float simTimeRecv;

	if (ENABLE_VERBOSE) {
		printf("receive VISSIM ......\n");
	
		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "recv VISSIM\n");
		fclose(f);
	}

	for (int i = 0; i < vissimSock.size(); i++) {
		if (ENABLE_VERBOSE) {
			printf("receive VISSIM %s \n", vissimSockName_um[vissimSock[i]].c_str());
		
			//FILE* f = fopen(MasterLogName.c_str(), "a");
			//fprintf(f, "receive VISSIM %s \n", vissimSockName_um[vissimSock[i]].c_str());
			//fclose(f);
		}

		//if (vissimSock.size() > 1 && vissimSockName_um[vissimSock[i]].find(SIGNAL_SOCK_PATTERN) != string::npos) {
		//	// if is signal controller
		//	// if simTime has reached steps for SC
		//	if (abs(round((*simTime- vissimBaseDt) / (vissimSignalStep * vissimBaseDt)) - (*simTime - vissimBaseDt) / (vissimSignalStep * vissimBaseDt)) < 1e-5) {
		//		// if not simTime = 0
		//		if (*simTime > (vissimSignalStep * vissimBaseDt) - 1e-5) {
		//			Sock_temp.recvData(vissimSock[i], &simStateRecv, &simTimeRecv, Msg_c);
		//			*simTime = simTimeRecv;
		//		}
		//	}
		//}
		//else {
		//	Sock_temp.recvData(vissimSock[i], &simStateRecv, &simTimeRecv, Msg_c);
		//	*simTime = simTimeRecv;
		//}
		recvStatus = Sock_temp.recvData(vissimSock[i], &simStateRecv, &simTimeRecv, Msg_c);
		*simTime = simTimeRecv;

		if (recvStatus < 0) {
			return -1;
		}

		if (ENABLE_VERBOSE) {
			printf("\treceive simTime %f\n", *simTime);

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "\treceive simTime %f\n", *simTime);
			fclose(f);

			for (auto& it : Msg_c.VehDataRecv_um) {
				printf("\treceive vehicle id %s\n", it.first.c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\trecv veh id %s\n", it.first.c_str());
				fclose(f);
			}
			for (auto& it : Msg_c.DetDataRecv_um) {
				printf("\treceive detector at intersection id %s\n", it.first.c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\treceive detector at intersection id %s\n", it.first.c_str());
				fclose(f);
			}
			for (auto& it : Msg_c.TlsDataRecv_um) {
				printf("\treceive signal intersection id %s\n", it.first.c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\treceive signal intersection id %s\n", it.first.c_str());
				fclose(f);
			}
		}
	}

	if (ENABLE_VERBOSE && recvStatus >= 0) {
		printf("receive VISSIM complete \n");
	
		//FILE* f = fopen(MasterLogName.c_str(), "a");
		//fprintf(f, "receive VISSIM complete \n");
		//fclose(f);
	}

	int aa = 1;

	return recvStatus;
}