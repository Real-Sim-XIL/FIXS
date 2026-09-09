#include "ConfigHelper.h"
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

using namespace std;

// Helper functions for path operations (C++14 compatible replacement for std::filesystem)
namespace {
	std::string getParentPath(const std::string& path) {
		size_t pos = path.find_last_of("\\/");
		return (pos != std::string::npos) ? path.substr(0, pos) : "";
	}

	std::string getCurrentDirectory() {
		char buffer[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, buffer);
		return std::string(buffer);
	}

	bool isRelativePath(const std::string& path) {
		if (path.empty()) return true;
		// Check for drive letter (e.g., "C:")
		if (path.length() > 1 && path[1] == ':') return false;
		// Check for UNC path or root path
		if (path[0] == '\\' || path[0] == '/') return false;
		return true;
	}

	std::string combinePaths(const std::string& base, const std::string& relative) {
		char combined[MAX_PATH];
		PathCombineA(combined, base.c_str(), relative.c_str());
		char normalized[MAX_PATH];
		PathCanonicalizeA(normalized, combined);
		return std::string(normalized);
	}
}


// Value-Defintions of the different String values
enum TypeNames_enum {
	ego,
	link,
	point,
	vehicleType,
	intersection,
	detector
};

// Map to associate the strings with the enum values
static std::map<std::string, TypeNames_enum> s_mapTypeValues;


ConfigHelper::ConfigHelper() {
	s_mapTypeValues["ego"] = ego;
	s_mapTypeValues["link"] = link;
	s_mapTypeValues["point"] = point;
	s_mapTypeValues["vehicleType"] = vehicleType;
	s_mapTypeValues["intersection"] = intersection;
	s_mapTypeValues["detector"] = detector;


	// initialize structs;
}

int ConfigHelper::getConfig(string configName) {
#ifdef UNICODE

	//DWORD  retval = 0;
	//BOOL   success;
	//TCHAR  buffer[4096] = TEXT("");
	//TCHAR  buf[4096] = TEXT("");
	//TCHAR** lppPart = { NULL };

	//// Retrieve the full path name for a file. 
	//// The file does not need to exist.
	//wchar_t messageBuff[1000];
	//size_t msgOutLen;
	//mbstowcs_s(&msgOutLen, messageBuff, configName.length() + 1, configName.c_str(), configName.length());

	//retval = GetFullPathName(messageBuff,
	//	4096,
	//	buffer,
	//	lppPart);

	//wstring test(&buffer[0]);
	//string temp(test.begin(), test.end());

	//ifstream infile(configName);
	//if (!infile.good()) {
	//	//string message = "Could not find configuration file\n" + temp + "\nRealSim Exits...";
	//	string message = "Could not find configuration file\n\nRealSim Exits...";
	//	string title = "Configuration File Error";
	//	popErrorMessageBox(message, title);

	//	return -1;
	//};
#else
	// DWORD  retval = 0;
	// BOOL   success = 0;
	// TCHAR  buffer[4096] = TEXT("");
	// TCHAR  buf[4096] = TEXT("");
	// TCHAR** lppPart = { NULL };

	// // Retrieve the full path name for a file. 
	// // The file does not need to exist.
	// retval = GetFullPathName(configName.c_str(),
		// 4096,
		// buffer,
		// lppPart);

	// string temp(&buffer[0]);
	// //string temp(test.begin(), test.end());

	// ifstream infile(configName);
	// if (!infile.good()) {
		// //string message = "Could not find configuration file\n" + temp + "\nRealSim Exits...";
		// string message = "Could not find configuration file\n" + temp + "\nRealSim Exits...";
		// string title = "Configuration File Error";
		// popErrorMessageBox(message, title);

		// return -1;
	// }
#endif

	// Store the config file's directory for resolving relative paths
	std::string configDir = getParentPath(configName);
	if (configDir.empty()) {
		configDir = getCurrentDirectory();
	}

	YAML::Node node;

	YAML::Node config = YAML::LoadFile(configName);

	// ===========================================================================
	// 			READ Simulation Setup section
	// ===========================================================================
	node = config["SimulationSetup"];
	if (node["EnableRealSim"]) {
		SimulationSetup.EnableRealSim = parserFlag(node, "EnableRealSim");
	}
	else {
		SimulationSetup.EnableRealSim = true;
		if (!SuppressDefaultMessages) printf("\nWill enable real sim as default!\n");
	}
	if (node["EnableVerboseLog"]) {
		SimulationSetup.EnableVerboseLog = parserFlag(node, "EnableVerboseLog");
	}
	else {
		SimulationSetup.EnableVerboseLog = false;
		if (!SuppressDefaultMessages) printf("\nWill disable verbose log as default!\n");
	}
	// SubEgoOnly (FIXS #158 Stage B+). Default true preserves legacy FIXS
	// DriverModel behavior; set to false in the par-file to enable the
	// per-tick send/recv path that the DSProxy + DriverModel relay needs.
	if (node["SubEgoOnly"]) {
		SimulationSetup.SubEgoOnly = parserFlag(node, "SubEgoOnly");
	}
	else {
		SimulationSetup.SubEgoOnly = true;
	}
	if (node["SimulationEndTime"]) {
		SimulationSetup.SimulationEndTime = parserDouble(node, "SimulationEndTime");
	}
	else {
		SimulationSetup.SimulationEndTime = 90000;
	}
	// NEED to fix later
	// flag to set SUMO speed through setPreviousSpeed
	if (node["EnableExternalDynamics"]) {
		SimulationSetup.EnableExternalDynamics = parserFlag(node, "EnableExternalDynamics");
	}
	else {
		SimulationSetup.EnableExternalDynamics = false;
	}
	if (node["SelectedTrafficSimulator"]) {
		SimulationSetup.SelectedTrafficSimulator = parserString(node, "SelectedTrafficSimulator");
	}
	else {
		SimulationSetup.SelectedTrafficSimulator = "SUMO";
		if (!SuppressDefaultMessages) printf("\nTraffic Simulator not specified! Will use SUMO as default!\n");
	}
	if (node["TrafficSimulatorIP"]) {
		SimulationSetup.TrafficSimulatorIP = parserString(node, "TrafficSimulatorIP");
	}
	else {
		SimulationSetup.TrafficSimulatorIP = "127.0.0.1";
		if (!SuppressDefaultMessages) printf("\nTraffic Simulator IP not specified! Will use localhost 127.0.0.1 as default!\n");
	}
	if (node["TrafficSimulatorPort"]) {
		SimulationSetup.TrafficSimulatorPort = parserInteger(node, "TrafficSimulatorPort");
	}
	else {
		SimulationSetup.TrafficSimulatorPort = 1337;
		if (!SuppressDefaultMessages) printf("\nTraffic Simulator Port not specified! Will use 1337 as default!\n");
	}
	// Warm-up (#86). Replaced SimulationMode/SimulationModeParameter, which no
	// config sets any more -- see doc/ConfigSetup.md for the mapping.
	if (node["WarmUpUntilEgoEntry"]) {
		SimulationSetup.WarmUpUntilEgoEntry = parserFlag(node, "WarmUpUntilEgoEntry");
	}
	else {
		SimulationSetup.WarmUpUntilEgoEntry = false;
	}
	if (node["WarmUpTime"]) {
		SimulationSetup.WarmUpTime = parserDouble(node, "WarmUpTime");
	}
	else {
		SimulationSetup.WarmUpTime = 0;
	}
	SimulationSetup.WarmUpServePorts.clear();
	if (node["WarmUpServePorts"]) {
		parserIntegerVector(node, "WarmUpServePorts", SimulationSetup.WarmUpServePorts);
	}
	if (node["VehicleMessageField"]) {
		parserStringVector(node, "VehicleMessageField", SimulationSetup.VehicleMessageField);
	}
	else {
		SimulationSetup.VehicleMessageField = { "id", "type", "speed", "positionX", "positionY", "positionZ", "heading", "color", "linkId", "laneId", "distanceTravel", "acceleration", "speedDesired", "acceleartionDesired", "hasPrecedingVehicle", "precedingVehicleId", "precedingVehicleDistance", "precedingVehicleSpeed", "signalLightDistance", "signalLightColor", "speedLimit", "speedLimitNext", "speedLimitChangeDistance", "linkIdNext", "grade" , "activeLaneChange", "signalLightId", "signalLightHeadId", "lightIndicators"};
		if (!SuppressDefaultMessages) printf("\nWill use all available vehicle message field!\n");
	}



	// -------------------
	//  Store Vehicle Message Field to an unordered_set
	// -------------------
	std::unordered_set <std::string> VehicleMessageField_set;
	if (!SuppressDefaultMessages) {
		printf("Vehicle message selected:");
		for (size_t i = 0; i < SimulationSetup.VehicleMessageField.size(); i++) {
			VehicleMessageField_set.insert(SimulationSetup.VehicleMessageField[i]);
			printf("\t%s", SimulationSetup.VehicleMessageField[i].c_str());
		}
		printf("\n\n");
	}
	else {
		for (size_t i = 0; i < SimulationSetup.VehicleMessageField.size(); i++) {
			VehicleMessageField_set.insert(SimulationSetup.VehicleMessageField[i]);
		}
	}

	// -------------------
	//  SANITY CHECK: Vehicle Message Field
	// -------------------
	// currently, mandate messages are id, speed, one of speedDesired or accelerationDesired
	if (VehicleMessageField_set.find("id") == VehicleMessageField_set.end()) {
		printf("ERROR: Must select 'id' as part of VehicleMessageField\n");
		exit(-1);
	}
	if (VehicleMessageField_set.find("speed") == VehicleMessageField_set.end()) {
		printf("ERROR: Must select 'speed' as part of VehicleMessageField\n");
		exit(-1);
	}
	if (VehicleMessageField_set.find("speedDesired") == VehicleMessageField_set.end() && VehicleMessageField_set.find("accelerationDesired") == VehicleMessageField_set.end()) {
		printf("ERROR: Must select one of 'speedDesired' or 'accelerationDesired' as part of VehicleMessageField\n");
		exit(-1);
	}

	// ===========================================================================
	// 			READ Application Setup section
	// ===========================================================================
	node = config["ApplicationSetup"];
	ApplicationSetup.EnableApplicationLayer = parserFlag(node, "EnableApplicationLayer");
	parserIntegerVector(node, "ApplicationPort", ApplicationSetup.ApplicationPort);

	parserSubscription(node, "VehicleSubscription", ApplicationSetup.VehicleSubscription);

	parserSubscription(node, "DetectorSubscription", ApplicationSetup.DetectorSubscription);

	parserSubscription(node, "SignalSubscription", ApplicationSetup.SignalSubscription);


	// ===========================================================================
	// 			READ XIL Setup section
	// ===========================================================================
	node = config["XilSetup"];

	XilSetup.EnableXil = parserFlag(node, "EnableXil");

	if (node["AsServer"]) {
		XilSetup.AsServer = parserFlag(node, "AsServer");
	}else{
		XilSetup.AsServer = false;
		if (!SuppressDefaultMessages) printf("\nXil not specified as server or client! Will set Xil as client!\n");
	}

	parserSubscription(node, "VehicleSubscription", XilSetup.VehicleSubscription);

	parserSubscription(node, "SignalSubscription", XilSetup.SignalSubscription);

	parserSubscription(node, "DetectorSubscription", XilSetup.DetectorSubscription);


	// ===========================================================================
	// 			Process subscription
	// ===========================================================================
	// get subscription information
	// variable to store subscription that need to check
	// if application layer is disabled, xil is enabled then use subscription of xil, this means traffic layer directly connects to xil
	if (!ApplicationSetup.EnableApplicationLayer && XilSetup.EnableXil) {
		getVehSubscriptionList(XilSetup.VehicleSubscription, SubscriptionVehicleList.edgeSubscribeId_v, SubscriptionVehicleList.vehicleSubscribeId_v, SubscriptionVehicleList.subscribeAllVehicle, SubscriptionVehicleList.pointSubscribeId_v, SubscriptionVehicleList.vehicleTypeSubscribedId_v);
		getSigSubscriptionList(XilSetup.SignalSubscription);
		getDetSubscriptionList(XilSetup.DetectorSubscription);
	}
	// otherwise find out the subscription of application layer
	else {
		getVehSubscriptionList(ApplicationSetup.VehicleSubscription, SubscriptionVehicleList.edgeSubscribeId_v, SubscriptionVehicleList.vehicleSubscribeId_v, SubscriptionVehicleList.subscribeAllVehicle, SubscriptionVehicleList.pointSubscribeId_v, SubscriptionVehicleList.vehicleTypeSubscribedId_v);
		getSigSubscriptionList(ApplicationSetup.SignalSubscription);
		getDetSubscriptionList(ApplicationSetup.DetectorSubscription);
	}

	// ===========================================================================
	// 			Warm-up ego list + validation (#86)
	// ===========================================================================
	// The union of the by-id vehicle subscriptions of BOTH layers -- see the
	// WarmUpEgoIds comment in ConfigHelper.h for why this is not the either/or
	// list built just above. Throwaway containers: only the ids are wanted.
	WarmUpEgoIds.clear();
	{
		Subscription_t bothLayers = ApplicationSetup.VehicleSubscription;
		bothLayers.insert(bothLayers.end(), XilSetup.VehicleSubscription.begin(),
			XilSetup.VehicleSubscription.end());

		unordered_set <string> edgeIgnored;
		unordered_map <string, double> vehIds;
		pair <bool, double> allIgnored = make_pair(false, 0.0);
		unordered_map <string, tuple<double, double, double, double> > pointIgnored;
		unordered_map <string, double> typeIgnored;
		getVehSubscriptionList(bothLayers, edgeIgnored, vehIds, allIgnored, pointIgnored, typeIgnored);

		for (auto& it : vehIds) {
			WarmUpEgoIds.insert(it.first);
		}
	}

	// Exclusive by construction: WarmUpTime is a single batch step, so it cannot
	// also watch for an ego arriving part-way through. Ego entry wins because it
	// is the one that can be wrong to overrun -- an ego on the road while the
	// boundary is still closed is being driven by the traffic model, not its XIL.
	if (SimulationSetup.WarmUpUntilEgoEntry && SimulationSetup.WarmUpTime > 0) {
		printf("\nWARNING (#86): both WarmUpUntilEgoEntry and WarmUpTime are set; "
			"using ego entry and ignoring WarmUpTime: %g\n", SimulationSetup.WarmUpTime);
		SimulationSetup.WarmUpTime = 0;
	}

	if (SimulationSetup.WarmUpUntilEgoEntry && WarmUpEgoIds.empty()) {
		printf("\nERROR (#86): 'WarmUpUntilEgoEntry: true' needs at least one vehicle "
			"subscription by id.\n");
		printf("\tNo by-id subscription was found in ApplicationSetup or XilSetup "
			"(an 'all'/radius/edge subscription has no ego to wait for),\n");
		printf("\tso the warm-up could never end. Subscribe the ego by id, or use "
			"WarmUpTime instead.\n\n");
		exit(-1);
	}

	// WarmUpServePorts only means anything with a warm-up, and every port in it
	// has to be a client this scenario actually declares -- a typo would
	// otherwise read as "serve nobody", i.e. exactly the failure the key exists
	// to prevent, and it would look like the key simply did not work.
	if (!SimulationSetup.WarmUpServePorts.empty()) {
		if (!SimulationSetup.WarmUpUntilEgoEntry && SimulationSetup.WarmUpTime <= 0) {
			printf("\nWARNING (#86): WarmUpServePorts is set but no warm-up is configured; "
				"it has no effect.\n\n");
		}
		// BOTH layers: XilSetup declares its own subscriptions (parsed separately,
		// see getVehSubscriptionList above), so scanning ApplicationSetup alone
		// would reject a valid XIL-only scenario -- and reject it fatally.
		std::unordered_set<int> declared;
		auto collectPorts = [&declared](const auto& subs) {
			for (auto& sub : subs)
				for (int p : std::get<3>(sub)) declared.insert(p);
		};
		collectPorts(ApplicationSetup.VehicleSubscription);
		collectPorts(ApplicationSetup.SignalSubscription);
		collectPorts(ApplicationSetup.DetectorSubscription);
		collectPorts(XilSetup.VehicleSubscription);
		collectPorts(XilSetup.SignalSubscription);
		collectPorts(XilSetup.DetectorSubscription);
		for (int p : SimulationSetup.WarmUpServePorts) {
			if (declared.find(p) == declared.end()) {
				printf("\nERROR (#86): WarmUpServePorts lists port %d, which no subscription "
					"in this configuration uses.\n", p);
				printf("\tIt would be served by nobody, which is indistinguishable from the "
					"key having no effect.\n\n");
				exit(-1);
			}
		}
	}

	if (SimulationSetup.WarmUpTime >= SimulationSetup.SimulationEndTime) {
		printf("\nERROR (#86): WarmUpTime (%g) is at or past SimulationEndTime (%g); "
			"the run would end before anything is exchanged.\n\n",
			SimulationSetup.WarmUpTime, SimulationSetup.SimulationEndTime);
		exit(-1);
	}

	// Warm-up is driven by TrafficLayer stepping the simulator, which it only does
	// for SUMO -- for VISSIM the DriverModel DLL owns the clock. Say so instead of
	// pretending: this is exactly where the old mode 4 silently did nothing and the
	// old mode 1 hung forever.
	if (SimulationSetup.SelectedTrafficSimulator.compare("VISSIM") == 0 &&
		(SimulationSetup.WarmUpUntilEgoEntry || SimulationSetup.WarmUpTime > 0)) {
		printf("\nERROR (#86): warm-up (WarmUpUntilEgoEntry / WarmUpTime) is SUMO-only "
			"today; VISSIM parity is tracked separately.\n");
		printf("\tRemove the key to run VISSIM without a warm-up.\n\n");
		exit(-1);
	}

	// figure out TrafficLayer IP
	// Both branches assume the chosen subscription has at least one entry,
	// which is true for every shipped scenario today. Guarding the empty
	// case here lets a Stage A DSProxy probe config — which deliberately
	// has no subscriptions yet — load without an access violation. The IP/
	// port stay at default-constructed values; consumers further down the
	// main path require at least one subscription anyway.
	if (XilSetup.EnableXil && !XilSetup.VehicleSubscription.empty()) {
		SimulationSetup.TrafficLayerIP = get<2>(XilSetup.VehicleSubscription[0])[0];
		SimulationSetup.TrafficLayerPort = get<3>(XilSetup.VehicleSubscription[0])[0];
	}
	else if (!XilSetup.EnableXil && !ApplicationSetup.VehicleSubscription.empty()) {
		SimulationSetup.TrafficLayerIP = get<2>(ApplicationSetup.VehicleSubscription[0])[0];
		SimulationSetup.TrafficLayerPort = get<3>(ApplicationSetup.VehicleSubscription[0])[0];
	}


	// ===========================================================================
	// 			READ CarMaker Setup section
	// ===========================================================================
	node = config["CarMakerSetup"];
	
	if (node["EnableCosimulation"]) {
		CarMakerSetup.EnableCosimulation = parserFlag(node, "EnableCosimulation");
	}
	else {
		CarMakerSetup.EnableCosimulation = false;
		//printf("\nXil not specified as server or client! Will set Xil as client!\n");
	}
	if (node["EnableEgoSimulink"]) {
		CarMakerSetup.EnableEgoSimulink = parserFlag(node, "EnableEgoSimulink");
	}
	else {
		CarMakerSetup.EnableEgoSimulink = false;
		//printf("\nXil not specified as server or client! Will set Xil as client!\n");
	}
	if (node["CarMakerIP"]) {
		CarMakerSetup.CarMakerIP = parserString(node, "CarMakerIP");
	}
	else {
		if (SubscriptionVehicleList.vehicleSubscribeId_v.size() == 1) {
			if (!ApplicationSetup.EnableApplicationLayer && XilSetup.EnableXil) {
				CarMakerSetup.CarMakerIP = get<2>(XilSetup.VehicleSubscription[0])[0];
			}
			else {
				CarMakerSetup.CarMakerIP = get<2>(ApplicationSetup.VehicleSubscription[0])[0];
			}
		}
		else {
			CarMakerSetup.CarMakerIP = "127.0.0.1";
			//printf("\nCarMaker IP not specified! Will use localhost 127.0.0.1 as default!\n");
		}
	}
	if (node["CarMakerPort"]) {
		CarMakerSetup.CarMakerPort = parserInteger(node, "CarMakerPort");
	}
	else {
		if (SubscriptionVehicleList.vehicleSubscribeId_v.size() == 1) {
			if (!ApplicationSetup.EnableApplicationLayer && XilSetup.EnableXil) {
				CarMakerSetup.CarMakerPort = get<3>(XilSetup.VehicleSubscription[0])[0];
			}
			else {
				CarMakerSetup.CarMakerPort = get<3>(ApplicationSetup.VehicleSubscription[0])[0];
			}
		}
		else {
			CarMakerSetup.CarMakerPort = 7331;
			//printf("\nCarMaker Port not specified! Will use 7331 as default!\n");
		}
	}
	if (node["TrafficRefreshRate"]) {
		CarMakerSetup.TrafficRefreshRate = parserDouble(node, "TrafficRefreshRate");
	}
	else {
		CarMakerSetup.TrafficRefreshRate = 0.001;
		//printf("\nCarMaker Port not specified! Will use 7331 as default!\n");
	}	
	// EgoId / EgoType moved to the EgoSetup section at the end of this file (#305).

	if (node["SynchronizeTrafficSignal"]) {
		CarMakerSetup.SynchronizeTrafficSignal = parserFlag(node, "SynchronizeTrafficSignal");
	}
	else {
		CarMakerSetup.SynchronizeTrafficSignal = false;
	}
	// OR rather than assign: getSigSubscriptionList() runs earlier in getConfig() and
	// may already have set this from the YAML `SignalSubscription` `all` attribute.
	// A plain assignment here silently discarded that, which is why the YAML path
	// could never work regardless of how it was parsed.
	SubscriptionSignalList.subAllSignalFlag =
		SubscriptionSignalList.subAllSignalFlag || CarMakerSetup.SynchronizeTrafficSignal;

	if (node["TrafficSignalPort"]) {
		CarMakerSetup.TrafficSignalPort = parserInteger(node, "TrafficSignalPort");
	}
	else {
		CarMakerSetup.TrafficSignalPort = 2444;
		if (!ApplicationSetup.EnableApplicationLayer && XilSetup.EnableXil) {
			if (XilSetup.SignalSubscription.size() == 1) {
				CarMakerSetup.TrafficSignalPort = get<3>(XilSetup.SignalSubscription[0])[0];
			}
		}
		else {
			if (ApplicationSetup.SignalSubscription.size() == 1) {
				CarMakerSetup.TrafficSignalPort = get<3>(ApplicationSetup.SignalSubscription[0])[0];
			}
		}
	}
	if (CarMakerSetup.SynchronizeTrafficSignal) {
		//SocketPort2SubscriptionList_um[CarMakerSetup.CarMakerPort].SignalList.subAllSignalFlag = true;

		SocketPort2SubscriptionList_um[CarMakerSetup.TrafficSignalPort].SignalList.subAllSignalFlag = true;
	}

	// ===========================================================================
	// 			READ Sumo Setup Section
	// ===========================================================================
	node = config["SumoSetup"];

	if (node["SpeedMode"]) {
		SumoSetup.SpeedMode = parserInteger(node, "SpeedMode");
	}
	else {
		SumoSetup.SpeedMode = 0;
		//printf("\nXil not specified as server or client! Will set Xil as client!\n");
	}
	if (node["ExecutionOrder"]) {
		SumoSetup.ExecutionOrder = parserInteger(node, "ExecutionOrder");
	}
	else {
		SumoSetup.ExecutionOrder = 1;
		if (!SuppressDefaultMessages) printf("\nSumo Execution Order not specified! Will use 1 as default!\n");
	}
	// How far ahead to look for a preceding vehicle (metres). Default 1000 keeps
	// the previously hard-coded behaviour; lower it to narrow the car-following
	// horizon an application sees.
	if (node["PrecedingVehicleLookahead"]) {
		SumoSetup.PrecedingVehicleLookahead = parserDouble(node, "PrecedingVehicleLookahead");
	}
	else {
		SumoSetup.PrecedingVehicleLookahead = 1000.0;
	}
	// keepRoute for the externally-driven ego's moveToXY. 6 is what that call
	// site hardcoded, so an absent key changes nothing. See SumoSetup_t.
	if (node["EgoKeepRoute"]) {
		SumoSetup.EgoKeepRoute = parserInteger(node, "EgoKeepRoute");
	}
	else {
		SumoSetup.EgoKeepRoute = 6;
	}
	if (node["EnableAutoLaunch"]) {
		SumoSetup.EnableAutoLaunch = parserFlag(node, "EnableAutoLaunch");
	}
	else {
		SumoSetup.EnableAutoLaunch = false;
	}
	if (node["SumoConfigFile"]) {
		SumoSetup.SumoConfigFile = parserString(node, "SumoConfigFile");

		// Convert relative path to absolute path (relative to config file directory)
		if (!SumoSetup.SumoConfigFile.empty() && isRelativePath(SumoSetup.SumoConfigFile)) {
			SumoSetup.SumoConfigFile = combinePaths(configDir, SumoSetup.SumoConfigFile);
		}
	}
	else {
		SumoSetup.SumoConfigFile = "";
	}
	if (node["NumClients"]) {
		SumoSetup.NumClients = parserInteger(node, "NumClients");
	}
	else {
		SumoSetup.NumClients = 1;
	}
	if (node["RuntimeLibraryPath"]) {
		SumoSetup.RuntimeLibraryPath = parserString(node, "RuntimeLibraryPath");
	}
	else {
		SumoSetup.RuntimeLibraryPath = "";
	}


	// ===========================================================================
	// 			READ Carla Setup section
	// ===========================================================================
	node = config["CarlaSetup"];
	if (node["EnableVerboseLog"]) {
		CarlaSetup.EnableVerboseLog = parserFlag(node, "EnableVerboseLog");
	}
	else {
		CarlaSetup.EnableVerboseLog = false;
		if (!SuppressDefaultMessages) printf("\nWill disable verbose log as default!\n");
	}
	if (node["EnableCosimulation"]) {
		CarlaSetup.EnableCosimulation = parserFlag(node, "EnableCosimulation");
	}
	else {
		CarlaSetup.EnableCosimulation = false;
	}

	// Co-sim bridge selector (consumed by run_cosim.py, not this engine); parsed
	// here for schema parity with ConfigHelper.py. Default true (= Python bridge).
	CarlaSetup.EnablePythonBackend = node["EnablePythonBackend"]
		? parserFlag(node, "EnablePythonBackend") : true;

	if (node["UseVehicleTypeAsBlueprint"]) {
		CarlaSetup.UseVehicleTypeAsBlueprint = parserFlag(node, "UseVehicleTypeAsBlueprint");
	}
	else {
		CarlaSetup.UseVehicleTypeAsBlueprint = false;
	}

	if (node["CenteredViewId"]) {
		CarlaSetup.CenteredViewId = parserString(node, "CenteredViewId");
	}
	else {
		CarlaSetup.CenteredViewId = "ego";
		if (!SuppressDefaultMessages) printf("\nCentered View Id not specified! Will use ego as default!\n");
	}

	// Spectator BEV follow (rigid top-down snap). Default ON, 50 m up, north-up.
	CarlaSetup.EnableSpectatorFollow = node["EnableSpectatorFollow"] ? parserFlag(node, "EnableSpectatorFollow") : true;
	CarlaSetup.SpectatorHeight       = node["SpectatorHeight"]       ? parserDouble(node, "SpectatorHeight") : 50.0;
	CarlaSetup.SpectatorAlignYaw     = node["SpectatorAlignYaw"]     ? parserFlag(node, "SpectatorAlignYaw") : false;

	// Real-time frame pacing (spread sub-ticks evenly). Default OFF (XIL-safe).
	CarlaSetup.RealtimePacing = node["RealtimePacing"] ? parserFlag(node, "RealtimePacing") : false;

	// EgoMode / EgoL0Driver / EnableExternalControl: read in the EgoSetup section
	// at the end of this file, as the v0.9.0 spelling of Dynamics and
	// ActuationSource (#305).
	// EgoId / EgoSumoType / EgoDynamics / EgoActuationSource / EgoController moved
	// to the EgoSetup section at the end of this file (#305).
	CarlaSetup.EgoBlueprint = node["EgoBlueprint"] ? parserString(node, "EgoBlueprint") : "vehicle.tesla.model3";
	if (node["EgoSpawnPose"]) {
		for (std::size_t i = 0; i < node["EgoSpawnPose"].size(); i++)
			CarlaSetup.EgoSpawnPose.push_back(node["EgoSpawnPose"][i].as<double>());
	}
	CarlaSetup.TrafficManagerPort = node["TrafficManagerPort"] ? parserInteger(node, "TrafficManagerPort") : 8000;
	if (node["EgoRoutePoints"]) {
		for (std::size_t i = 0; i < node["EgoRoutePoints"].size(); i++) {
			CarlaSetup.EgoRoutePoints.push_back(std::make_pair(
				node["EgoRoutePoints"][i][0].as<double>(), node["EgoRoutePoints"][i][1].as<double>()));
		}
	}
	CarlaSetup.EgoRouteRepeat = node["EgoRouteRepeat"] ? parserInteger(node, "EgoRouteRepeat") : 50;
	CarlaSetup.EgoTargetSpeed = node["EgoTargetSpeed"] ? parserDouble(node, "EgoTargetSpeed") : 8.33;

	if (node["CarlaServerIP"]) {
		CarlaSetup.CarlaServerIP = parserString(node, "CarlaServerIP");
	}
	else {
		CarlaSetup.CarlaServerIP = "127.0.0.1";
	}

	if (node["CarlaServerPort"]) {
		CarlaSetup.CarlaServerPort = parserInteger(node, "CarlaServerPort");
	}
	else {
		CarlaSetup.CarlaServerPort = 2000;
	}

	if (node["CarlaClientIP"]) {
		CarlaSetup.CarlaClientIP = parserString(node, "CarlaClientIP");
	}
	else {
		if (SubscriptionVehicleList.vehicleSubscribeId_v.size() == 1) {
			if (!ApplicationSetup.EnableApplicationLayer && XilSetup.EnableXil) {
				CarlaSetup.CarlaClientIP = get<2>(XilSetup.VehicleSubscription[0])[0];
			}
			else {
				CarlaSetup.CarlaClientIP = get<2>(ApplicationSetup.VehicleSubscription[0])[0];
			}
		}
		else {
			CarlaSetup.CarlaClientIP = "127.0.0.1";
		}
	}

	if (node["CarlaClientPort"]) {
		CarlaSetup.CarlaClientPort = parserInteger(node, "CarlaClientPort");
	}
	else {
		if (SubscriptionVehicleList.vehicleSubscribeId_v.size() == 1) {
			if (!ApplicationSetup.EnableApplicationLayer && XilSetup.EnableXil) {
				CarlaSetup.CarlaClientPort = get<3>(XilSetup.VehicleSubscription[0])[0];
			}
			else {
				CarlaSetup.CarlaClientPort = get<3>(ApplicationSetup.VehicleSubscription[0])[0];
			}
		}
		else {
			CarlaSetup.CarlaClientPort = 2001;
		}
	}
	if (node["CarlaMap"]) {
		CarlaSetup.CarlaMapName = parserString(node, "CarlaMapName");
	}
	else {
		CarlaSetup.CarlaMapName = "Town01";
		if (!SuppressDefaultMessages) printf("\nCarla Map not specified! Will use Town01 as default!\n");
	}
	if (node["TrafficRefreshRate"]) {
		CarlaSetup.TrafficRefreshRate = parserDouble(node, "TrafficRefreshRate");
	}
	else {
		// 0 == every Carla tick. This is the pose RE-APPLY cadence, not the feed
		// period; absent, mainVirCarla resolves it to CarlaTimeStep (#261).
		CarlaSetup.TrafficRefreshRate = 0.0;
	}

	// Carla render sub-step (interpolate the feed for smoother motion). 0 -> 1:1.
	CarlaSetup.CarlaTimeStep = node["CarlaTimeStep"] ? parserDouble(node, "CarlaTimeStep") : 0.0;

	if (node["InterestedIds"]) {
		parserStringVector(node, "InterestedIds", CarlaSetup.InterestedIds);
	}
	else {
		CarlaSetup.InterestedIds = {"ego"};
		if (!SuppressDefaultMessages) printf("\nDefault to track actor with id ego\n");
	}

	// ===========================================================================
	// 			READ VissimSetup section (issue #158, Stage A)
	// ===========================================================================
	// Default all members up front. If the block is absent, this is the final
	// state. If present, the per-key overrides below replace any of them.
	VissimSetup.EnableDSProxy = false;
	VissimSetup.NetworkFile = "";
	VissimSetup.VissimVersion = 2022;
	VissimSetup.DllPath = "";
	VissimSetup.SimulatorFrequency = 10;
	VissimSetup.VisibilityRadius = -1.0;
	VissimSetup.MaxSimulatorVeh = 10;
	VissimSetup.MaxSimulatorPed = 0;
	VissimSetup.MaxSimulatorDet = 0;
	VissimSetup.MaxTotalVeh = 50000;
	VissimSetup.MaxVissimPed = 0;
	VissimSetup.MaxVissimSigGrp = 1000;
	VissimSetup.EnableDriverModelRelay = false;

	node = config["VissimSetup"];
	if (node) {
		if (node["EnableDSProxy"])           VissimSetup.EnableDSProxy           = parserFlag(node, "EnableDSProxy");
		if (node["NetworkFile"])             VissimSetup.NetworkFile             = parserString(node, "NetworkFile");
		if (node["VissimVersion"])           VissimSetup.VissimVersion           = parserInteger(node, "VissimVersion");
		if (node["DllPath"])                 VissimSetup.DllPath                 = parserString(node, "DllPath");
		if (node["SimulatorFrequency"])      VissimSetup.SimulatorFrequency      = parserInteger(node, "SimulatorFrequency");
		if (node["VisibilityRadius"])        VissimSetup.VisibilityRadius        = parserDouble(node, "VisibilityRadius");
		if (node["MaxSimulatorVeh"])         VissimSetup.MaxSimulatorVeh         = parserInteger(node, "MaxSimulatorVeh");
		if (node["MaxSimulatorPed"])         VissimSetup.MaxSimulatorPed         = parserInteger(node, "MaxSimulatorPed");
		if (node["MaxSimulatorDet"])         VissimSetup.MaxSimulatorDet         = parserInteger(node, "MaxSimulatorDet");
		if (node["MaxTotalVeh"])             VissimSetup.MaxTotalVeh             = parserInteger(node, "MaxTotalVeh");
		if (node["MaxVissimPed"])            VissimSetup.MaxVissimPed            = parserInteger(node, "MaxVissimPed");
		if (node["MaxVissimSigGrp"])         VissimSetup.MaxVissimSigGrp         = parserInteger(node, "MaxVissimSigGrp");
		if (node["EnableDriverModelRelay"])  VissimSetup.EnableDriverModelRelay  = parserFlag(node, "EnableDriverModelRelay");
	}

	// ===========================================================================
	// 			READ DataLog Setup section (generic FIXS infrastructure logging)
	// ===========================================================================
	node = config["DataLogSetup"];
	if (node) {
		if (node["EnableDataLog"]) DataLogSetup.EnableDataLog = parserFlag(node, "EnableDataLog");
		if (node["DataLogPath"])   DataLogSetup.DataLogPath   = parserString(node, "DataLogPath");
		if (node["DataLogWho"])    parserStringVector(node, "DataLogWho", DataLogSetup.DataLogWho);
		if (node["DataLogFields"]) parserStringVector(node, "DataLogFields", DataLogSetup.DataLogFields);
	}

	// ===========================================================================
	// 			READ Ego Setup section (#305)
	// ===========================================================================
	// LAST on purpose: it resolves the per-backend keys it replaced, which are
	// still read here as fallbacks. See #305.
	{
		YAML::Node egoNode   = config["EgoSetup"];
		YAML::Node cmNode    = config["CarMakerSetup"];
		YAML::Node carlaNode = config["CarlaSetup"];

		// dependency-free ASCII fold: "carlaTM"/"CarlaTM"/"carlatm" read alike
		auto lowerAscii = [](const std::string& in) {
			std::string out = in;
			for (size_t i = 0; i < out.size(); i++)
				if (out[i] >= 'A' && out[i] <= 'Z') out[i] = (char)(out[i] - 'A' + 'a');
			return out;
		};
		auto hasMsgField = [&](const char* f) {
			for (size_t i = 0; i < SimulationSetup.VehicleMessageField.size(); i++)
				if (SimulationSetup.VehicleMessageField[i] == f) return true;
			return false;
		};
		auto legacy = [&](YAML::Node n, const char* key) -> std::string {
			return (n && n[key]) ? parserString(n, key) : std::string();
		};
		auto resolve = [&](const char* newKey, const char* cmKey, const char* carlaKey,
		                   std::string& out) -> bool {
			const std::string fromCm    = legacy(cmNode, cmKey);
			const std::string fromCarla = legacy(carlaNode, carlaKey);
			if (egoNode && egoNode[newKey]) {
				out = parserString(egoNode, newKey);
				const char* olds[2] = { cmKey, carlaKey };
				const std::string vals[2] = { fromCm, fromCarla };
				const char* sects[2] = { "CarMakerSetup", "CarlaSetup" };
				for (int i = 0; i < 2; i++)
					if (!vals[i].empty() && vals[i] != out)
						printf("WARNING: EgoSetup.%s ('%s') overrides %s.%s ('%s'). "
						       "Delete the second one.\n", newKey, out.c_str(),
						       sects[i], olds[i], vals[i].c_str());
				return true;
			}
			if (!fromCarla.empty()) { out = fromCarla; return true; }
			if (!fromCm.empty())    { out = fromCm;    return true; }
			return false;
		};

		// ---- Id -------------------------------------------------------------
		if (!resolve("Id", "EgoId", "EgoId", EgoSetup.Id)) {
			// Nothing named it: infer from the lone vehicle subscription, which
			// beats the old per-backend defaults (#305).
			if (SubscriptionVehicleList.vehicleSubscribeId_v.size() == 1) {
				EgoSetup.Id = SubscriptionVehicleList.vehicleSubscribeId_v.begin()->first;
			}
			else if (CarlaSetup.EnableCosimulation) {
				EgoSetup.Id = "ego";
			}
			else if (CarMakerSetup.EnableCosimulation) {
				// Only a vehicle-simulator run needs an ego id; an 'all'
				// subscription has nothing to infer from (#176, #214).
				printf("ERROR: EgoSetup.Id is not defined and cannot be inferred "
				       "(expected an explicit 'Id' under EgoSetup, or exactly one ego "
				       "VehicleSubscription, found %zu). Define the ego id in config.yaml.\n",
				       SubscriptionVehicleList.vehicleSubscribeId_v.size());
				exit(-1);
			}
			else {
				EgoSetup.Id = "";   // no ego-owning backend in this run
			}
		}

		// ---- SumoType --------------------------------------------------------
		if (!resolve("Type", "EgoType", "EgoSumoType", EgoSetup.Type))
			EgoSetup.Type = CarlaSetup.EnableCosimulation ? "car" : "";

		// ---- KeepRoute -------------------------------------------------------
		// ---- Controller ------------------------------------------------------
		resolve("Controller", "EgoController", "EgoController", EgoSetup.Controller);

		// ---- Dynamics --------------------------------------------------------
		// v0.9.0 said this with EgoMode AND EnableExternalControl, two keys that
		// could disagree -- which is the bug EgoSetup replaced.
		if (egoNode && egoNode["Dynamics"]) {
			EgoSetup.Dynamics = lowerAscii(parserString(egoNode, "Dynamics"));
		}
		else if (carlaNode && (carlaNode["EnableExternalControl"] || carlaNode["EgoMode"])) {
			const bool ext = carlaNode["EnableExternalControl"]
			                 && parserFlag(carlaNode, "EnableExternalControl");
			const int mode = carlaNode["EgoMode"] ? parserInteger(carlaNode, "EgoMode") : 0;
			EgoSetup.Dynamics = (ext && mode >= 1) ? "virenv" : "traffic";
		}
		{
			const std::string dyn = EgoSetup.Dynamics;
			if (dyn.empty() || dyn == "traffic" || dyn == "virenv") {
				// nothing to derive: L0 and L2 are ONE configuration, and the
				// level is topology (is an advisory client wired in upstream),
				// not a config value.
			}
			else if (dyn == "xil") {
				printf("ERROR: EgoSetup.Dynamics: 'xil' names the case where an external\n"
				       "       plant owns the ego. That case exists and works, but it is\n"
				       "       switched on by CarMakerSetup.EnableCosimulation today, not\n"
				       "       by this key. Leave Dynamics unset for a CarMaker/XIL run.\n");
				exit(-1);
			}
			else {
				printf("ERROR: EgoSetup.Dynamics must be one of traffic|virenv|xil, got '%s'\n",
				       EgoSetup.Dynamics.c_str());
				exit(-1);
			}
		}

		// ---- ActuationSource -------------------------------------------------
		// v0.9.0's EgoL0Driver named the driver AND where it runs.
		if (egoNode && egoNode["ActuationSource"]) {
			EgoSetup.ActuationSource = lowerAscii(parserString(egoNode, "ActuationSource"));
		}
		else {
			const std::string l0 = lowerAscii(legacy(carlaNode, "EgoL0Driver"));
			if      (l0 == "tm")                            EgoSetup.ActuationSource = "simulator";
			else if (l0 == "pursuit" || l0 == "fallback"
			         || l0 == "egodriver")                  EgoSetup.ActuationSource = "fixs";
			else if (l0 == "actuation" || l0 == "embedded") EgoSetup.ActuationSource = "user";
		}
		if (!EgoSetup.ActuationSource.empty()) {
			const std::string src = EgoSetup.ActuationSource;
			if (src != "simulator" && src != "fixs" && src != "user") {
				printf("ERROR: EgoSetup.ActuationSource must be one of simulator|fixs|user, "
				       "got '%s'\n", EgoSetup.ActuationSource.c_str());
				exit(-1);
			}
			// "user" is ONE value; the Controller key decides where it runs.
			const bool userOnTheWire = (src == "user" && EgoSetup.Controller.empty());

			// A controller can only produce a field that is on the wire; without
			// this check the omission is silent (FIXS#305).
			if (userOnTheWire) {
				const char* need[] = { "acceleratorPedalDesired", "brakePedalDesired", "steerAngleDesired" };
				for (int i = 0; i < 3; i++) {
					if (!hasMsgField(need[i])) {
						printf("ERROR: EgoSetup.ActuationSource: user control over the feed needs\n"
						       "       '%s' in VehicleMessageField; without it the controller's\n"
						       "       command is stripped before it reaches the ego.\n", need[i]);
						exit(-1);
					}
				}
			}
		}
	}

	return 0;
}


void ConfigHelper::resetConfig() {
	   
	SimulationSetup.VehicleMessageField.clear();
	SimulationSetup.SelectedTrafficSimulator.clear();

	ApplicationSetup.ApplicationPort.clear();
	ApplicationSetup.VehicleSubscription.clear();
	ApplicationSetup.DetectorSubscription.clear();
	ApplicationSetup.SignalSubscription.clear();

	XilSetup.VehicleSubscription.clear();
	XilSetup.SignalSubscription.clear();
	XilSetup.DetectorSubscription.clear();

}

bool ConfigHelper::parserFlag(YAML::Node node, string name) {
	bool flag = false;
	const std::string Flag_str = node[name].as<std::string>();
	if (Flag_str.compare("false") == 0) {
		flag = false;
	}
	else if (Flag_str.compare("true") == 0) {
		flag = true;
	}
	else {
		string message = "Error configuration of variable:\n " + name + " ";
		string title = "Configuration File Error";
		//popErrorMessageBox(message, title);
	}

	return flag;

}

string ConfigHelper::parserString(YAML::Node node, string name) {
	const std::string strVal = node[name].as<std::string>();

	return strVal;
}


double ConfigHelper::parserDouble(YAML::Node node, string name) {
	double value = node[name].as<double>();

	return value;

}

int ConfigHelper::parserInteger(YAML::Node node, string name) {
	int value = node[name].as<int>();

	return value;

}

void ConfigHelper::parserIntegerVector(YAML::Node node, string name, vector<int>& outIntegerVector) {
	YAML::Node subnode = node[name];

	for (size_t i = 0; i < subnode.size(); i++) {
		outIntegerVector.push_back(subnode[i].as<int>());
	}

}

void ConfigHelper::parserStringVector(YAML::Node node, string name, vector<string>& outStringVector) {
	YAML::Node subnode = node[name];

	for (size_t i = 0; i < subnode.size(); i++) {
		outStringVector.push_back(subnode[i].as<std::string>());

	}

}




void ConfigHelper::parserSubscription(YAML::Node rootnode, std::string name, Subscription_t& subscription) {

	// node can contain several subscriptions
	YAML::Node node = rootnode[name];

	// loop over each subscription
	for (size_t i = 0; i < node.size(); i++) {
		YAML::Node subnode = node[i];

		// each subscription should contain a type and an attribute field
		std::string type;
		YAML::Node attnode;
		YAML::Node ipnode;
		YAML::Node portnode;
		vector<string> ip_v;
		vector<int> port_v;
		for (YAML::const_iterator it = subnode.begin(); it != subnode.end(); ++it) {
			const std::string field = it->first.as<std::string>();

			if (field.compare("type") == 0) {
				type = it->second.as<std::string>();
			}
			if (field.compare("attribute") == 0) {
				attnode = it->second;
			}
			if (field.compare("port") == 0) {
				portnode = it->second;

				parserIntegerVector(subnode, "port", port_v);
			}
			if (field.compare("ip") == 0) {
				ipnode = it->second;

				parserStringVector(subnode, "ip", ip_v);
			}
		}

		// loop over each attribute
		SubAttMap_t attMap;
		for (YAML::const_iterator it2 = attnode.begin(); it2 != attnode.end(); ++it2) {
			const std::string att = it2->first.as<std::string>();
			//YAML::Node valnode = attnode[att];

			switch (s_mapTypeValues[type]) {
			case ego:
				if (att.compare("id") == 0 || att.compare("radius") == 0 || att.compare("all") == 0) {
					extractSubscriptionAttributes(attnode, type, att, attMap);
				}
				else {

				}

				break;

			case link:
				if (att.compare("id") == 0) {
					extractSubscriptionAttributes(attnode, type, att, attMap);
				}
				else {
					string message = "Error configuration of variable:\n " + name + " " + type + " " + att + " ";
					string title = "Configuration File Error";
					//popErrorMessageBox(message, title);
				}

				break;

			case point:
				if (att.compare("radius") == 0 || att.compare("x") == 0 || att.compare("y") == 0 || att.compare("z") == 0) {
					extractSubscriptionAttributes(attnode, type, att, attMap);
				}
				else {
					string message = "Error configuration of variable:\n " + name + " " + type + " " + att + " ";
					string title = "Configuration File Error";
					//popErrorMessageBox(message, title);
				}

				break;

			case vehicleType:
				if (att.compare("id") == 0 || att.compare("radius") == 0) {
					extractSubscriptionAttributes(attnode, type, att, attMap);
				}
				else {

				}

				break;

			case intersection:
				// `all` accepted alongside id/name, matching the `ego` case above.
				// Without it the attribute was dropped here before reaching attMap,
				// so getSigSubscriptionList() could never see it -- and because the
				// error box below is commented out, the drop was silent.
				if (att.compare("id") == 0 || att.compare("name") == 0 || att.compare("all") == 0) {
					extractSubscriptionAttributes(attnode, type, att, attMap);
				}
				else {
					string message = "Error configuration of variable:\n " + name + " " + type + " " + att + " ";
					string title = "Configuration File Error";
					//popErrorMessageBox(message, title);
				}

				break;

			case detector:
				if (att.compare("id") == 0 || att.compare("name") == 0 || att.compare("pattern") == 0) {
					extractSubscriptionAttributes(attnode, type, att, attMap);
				}
				else {
					string message = "Error configuration of variable:\n " + name + " " + type + " " + att + " ";
					string title = "Configuration File Error";
					//popErrorMessageBox(message, title);
				}

				break;

			default:
				string message = "Error configuration of variable:\n " + name + " " + type + " ";
				string title = "Configuration File Error";
				//popErrorMessageBox(message, title);

				break;
			}
		}

		subscription.push_back(make_tuple(type, attMap, ip_v, port_v));

		//int aa = 1;

	}

}


void ConfigHelper::extractSubscriptionAttributes(YAML::Node attnode, string type, string att, SubAttMap_t& attMap) {

	vector <string> stringVector;
	this->parserStringVector(attnode, att, stringVector);
	//// if we already have the current type, then means add additional attributes/key to the attMap
	//if (subscription.find(type) != subscription.end()) {
	//	attMap = subscription[type];
	//	attMap[att] = stringVector;
	//	subscription[type] = attMap;
	//}
	//// else, create the current type
	//else {
	attMap[att] = stringVector;
	//subscription[type] = attMap;
//}
}

void ConfigHelper::getVehSubscriptionList(Subscription_t VehSub, std::unordered_set <std::string>& edgeSubscribeId_v, std::unordered_map <std::string, double >& vehicleSubscribeId_v, std::pair <bool, double>& subscribeAllVehicle, std::unordered_map <std::string, std::tuple<double, double, double, double> >& pointSubscribeId_v, std::unordered_map <std::string, double>& vehicleTypeSubscribedId_v) {

	// type, attribute, ip, port
	// string, SubAttMap_t, vector <string>, vector <int>
	for (size_t iSub = 0; iSub < VehSub.size(); iSub++) {
		string type;
		type = get<0>(VehSub[iSub]);

		vector <int> port_v;
		port_v = get<3>(VehSub[iSub]);
		
		if (type.compare("ego") == 0) {
			SubAttMap_t att = get<1>(VehSub[iSub]);
			if (att.find("id") != att.end() && att.find("radius") != att.end()) {
				vector <string> idlist = att["id"];
				vector <string> radiuslist = att["radius"];
				if (idlist.size() != radiuslist.size()) {
					// ERROR HANDLING

				}
				else {
					for (size_t i = 0; i < idlist.size(); i++) {
						double radius = strtod(radiuslist[i].c_str(), NULL);
						if (vehicleSubscribeId_v.find(idlist[i]) == vehicleSubscribeId_v.end()) {
							vehicleSubscribeId_v[idlist[i]] = radius;
						}
						else {
							// ERROR HANDLING
							// this means the vehicle already subscribed, then just use the larger radius of the two
							vehicleSubscribeId_v[idlist[i]] = max(radius, vehicleSubscribeId_v[idlist[i]]);

						}
					}


					// get port map
					for (auto it : port_v) {
						// if does not has socket port, then need to initialize
						if (SocketPort2SubscriptionList_um.find(it) == SocketPort2SubscriptionList_um.end()) {
							SubscriptionAllList_t subAllList;
							SocketPort2SubscriptionList_um[it] = subAllList;
						}
						for (size_t i = 0; i < idlist.size(); i++) {
							double radius = strtod(radiuslist[i].c_str(), NULL);
							if (SocketPort2SubscriptionList_um[it].VehicleList.vehicleSubscribeId_v.find(idlist[i]) == SocketPort2SubscriptionList_um[it].VehicleList.vehicleSubscribeId_v.end()) {
								SocketPort2SubscriptionList_um[it].VehicleList.vehicleSubscribeId_v[idlist[i]] = radius;
							}
							else {
								// ERROR HANDLING
								// this means the vehicle already subscribed, then just use the larger radius of the two
								SocketPort2SubscriptionList_um[it].VehicleList.vehicleSubscribeId_v[idlist[i]] = max(radius, SocketPort2SubscriptionList_um[it].VehicleList.vehicleSubscribeId_v[idlist[i]]);

							}
						}
					}
				}

			}
			else if (att.find("all") != att.end()) {
				vector <string> flaglist = att["all"];
				vector <string> radiuslist;
				if (att.find("radius") != att.end()) {
					radiuslist = att["radius"];
				}
				else {
					vector <string> temp(flaglist.size(), "0");
					radiuslist = temp;
				}
				if (flaglist.size() != radiuslist.size() && flaglist.size() > 1) {
					// ERROR HANDLING

				}
				else {
					bool subAllFlag = false;
					if (flaglist[0].compare("true") == 0) {
						subAllFlag = true;
					}
					subscribeAllVehicle = make_pair(subAllFlag, strtod(radiuslist[0].c_str(), NULL));


					// get port map
					for (auto it : port_v) {
						// if does not has socket port, then need to initialize
						if (SocketPort2SubscriptionList_um.find(it) == SocketPort2SubscriptionList_um.end()) {
							SubscriptionAllList_t subAllList;
							SocketPort2SubscriptionList_um[it] = subAllList;
						}
						SocketPort2SubscriptionList_um[it].VehicleList.subscribeAllVehicle = make_pair(subAllFlag, strtod(radiuslist[0].c_str(), NULL));
					}
				}

			}
		}
		else if (type.compare("link") == 0) {
			SubAttMap_t att = get<1>(VehSub[iSub]);
			if (att.find("id") != att.end()) {
				vector <string> idlist = att["id"];
				for (size_t iId = 0; iId < idlist.size(); iId++) {
					edgeSubscribeId_v.insert(idlist[iId]);
				}

				// get port map
				for (auto it : port_v) {
					// if does not has socket port, then need to initialize
					if (SocketPort2SubscriptionList_um.find(it) == SocketPort2SubscriptionList_um.end()) {
						SubscriptionAllList_t subAllList;
						SocketPort2SubscriptionList_um[it] = subAllList;
					}
					for (size_t iId = 0; iId < idlist.size(); iId++) {
						SocketPort2SubscriptionList_um[it].VehicleList.edgeSubscribeId_v.insert(idlist[iId]);
					}
				}
			}
			else {
				// ERROR HANDLING

			}
		}
		// tuple x, y, z, radius
		else if (type.compare("point") == 0) {
			SubAttMap_t att = get<1>(VehSub[iSub]);
			if (att.find("x") != att.end() && att.find("y") != att.end() && att.find("z") != att.end() && att.find("radius") != att.end()) {
				vector <string> xlist = att["x"];
				vector <string> ylist = att["y"];
				vector <string> zlist = att["z"];
				vector <string> rlist = att["radius"];

				if (0) {
					// ERROR HANDLING
					// check vector sizes
				}
				else {
					for (size_t i = 0; i < xlist.size(); i++) {
						stringstream ss;
						//int len = (int) pointSubscribeId_v.size();
						ss << pointSubscribeId_v.size();
						//string poiName = "RealSimPOI_" + to_string(pointSubscribeId_v.size());
						string poiName = "RealSimPOI_" + ss.str();

						pointSubscribeId_v[poiName] = make_tuple(strtod(xlist[i].c_str(), NULL), strtod(ylist[i].c_str(), NULL), strtod(zlist[i].c_str(), NULL), strtod(rlist[i].c_str(), NULL));
					}
					// get port map
					for (auto it : port_v) {
						// if does not has socket port, then need to initialize
						if (SocketPort2SubscriptionList_um.find(it) == SocketPort2SubscriptionList_um.end()) {
							SubscriptionAllList_t subAllList;
							SocketPort2SubscriptionList_um[it] = subAllList;
						}
						for (size_t i = 0; i < xlist.size(); i++) {
							stringstream ss;
							//int len = (int) pointSubscribeId_v.size();
							ss << pointSubscribeId_v.size();
							//string poiName = "RealSimPOI_" + to_string(pointSubscribeId_v.size());
							string poiName = "RealSimPOI_" + ss.str();

							SocketPort2SubscriptionList_um[it].VehicleList.pointSubscribeId_v[poiName] = make_tuple(strtod(xlist[i].c_str(), NULL), strtod(ylist[i].c_str(), NULL), strtod(zlist[i].c_str(), NULL), strtod(rlist[i].c_str(), NULL));
						}
					}
				}
			}
			else {
				// ERROR HANDLING
			}
		}
		else if (type.compare("vehicleType") == 0) {
			SubAttMap_t att = get<1>(VehSub[iSub]);
			if (att.find("id") != att.end() && att.find("radius") != att.end()) {
				vector <string> idlist = att["id"];
				vector <string> radiuslist = att["radius"];
				if (idlist.size() != radiuslist.size()) {
					// ERROR HANDLING

				}
				else {
					for (size_t i = 0; i < idlist.size(); i++) {
						double radius = strtod(radiuslist[i].c_str(), NULL);
						if (vehicleTypeSubscribedId_v.find(idlist[i]) == vehicleTypeSubscribedId_v.end()) {
							vehicleTypeSubscribedId_v[idlist[i]] = radius;
						}
						else {
							// ERROR HANDLING

						}
					}
				}
			}
		}
		else {
			// ERROR HANDLING

		}
	}
}

void ConfigHelper::getSigSubscriptionList(Subscription_t SigSub) {

	// type, attribute, ip, port
	// string, SubAttMap_t, vector <string>, vector <int>
	for (size_t iSub = 0; iSub < SigSub.size(); iSub++) {
		string type;
		type = get<0>(SigSub[iSub]);
		vector <string> idlist;
		if (type.compare("intersection") == 0) {
			SubAttMap_t att = get<1>(SigSub[iSub]);
			if (att.find("name") != att.end()) {
				idlist = att["name"];
				for (size_t i = 0; i < idlist.size(); i++) {
					if (SubscriptionSignalList.signalId_v.find(idlist[i]) == SubscriptionSignalList.signalId_v.end()) {
						SubscriptionSignalList.signalId_v.insert(idlist[i]);
					}
				}
			}

			// `all` subscription for signals, mirroring the vehicle `all` handling
			// added for #176. `attribute: {all: ['true']}` subscribes every traffic
			// light in the network instead of an explicit `name` list. Consumed in
			// TrafficHelper::recvFromTrafficSimulator, which already branches on
			// subAllSignalFlag to subscribe TrafficLight::getIDList().
			bool subAllFlag = false;
			if (att.find("all") != att.end() && !att["all"].empty()) {
				subAllFlag = (att["all"][0].compare("true") == 0);
			}
			if (subAllFlag) {
				SubscriptionSignalList.subAllSignalFlag = true;
				if (!idlist.empty()) {
					printf("\nWARNING: 'all' signal subscription is enabled; the configured 'name' list is ignored (all traffic lights in the network are sent).\n");
				}
			}

			// get port map
			vector <int> port_v;
			port_v = get<3>(SigSub[iSub]);

			for (auto it : port_v) {
				// if already has this socket port, then no need to initialize
				if (SocketPort2SubscriptionList_um.find(it) != SocketPort2SubscriptionList_um.end()) {

				}
				else {
					SubscriptionAllList_t subAllList;
					SocketPort2SubscriptionList_um[it] = subAllList;
				}

				if (subAllFlag) {
					SocketPort2SubscriptionList_um[it].SignalList.subAllSignalFlag = true;
				}

				for (size_t i = 0; i < idlist.size(); i++) {
					if (SocketPort2SubscriptionList_um[it].SignalList.signalId_v.find(idlist[i]) == SocketPort2SubscriptionList_um[it].SignalList.signalId_v.end()) {
						SocketPort2SubscriptionList_um[it].SignalList.signalId_v.insert(idlist[i]);
					}
				}
			}

		}
		else {
			// ERROR HANDLING

		}

		
	}

	// (The CarMaker SynchronizeTrafficSignal path that used to be sketched here now
	// lives in getConfig(), where it ORs into subAllSignalFlag rather than
	// overwriting whatever this function derived from the YAML.)
}

void ConfigHelper::getDetSubscriptionList(Subscription_t DetSub) {

	// type, attribute, ip, port
	// string, SubAttMap_t, vector <string>, vector <int>
	for (size_t iSub = 0; iSub < DetSub.size(); iSub++) {
		string type;
		type = get<0>(DetSub[iSub]);
		vector <string> idlist;
		if (type.compare("detector") == 0) {
			SubAttMap_t att = get<1>(DetSub[iSub]);
			if (att.find("pattern") != att.end()) {
				idlist = att["pattern"];

				for (size_t i = 0; i < idlist.size(); i++) {
					if (SubscriptionDetectorList.pattern_v.find(idlist[i]) == SubscriptionDetectorList.pattern_v.end()) {
						SubscriptionDetectorList.pattern_v.insert(idlist[i]);
					}
				}
			}

			// get port map
			vector <int> port_v;
			port_v = get<3>(DetSub[iSub]);

			for (auto it : port_v) {
				// if already has this socket port, then no need to initialize
				if (SocketPort2SubscriptionList_um.find(it) != SocketPort2SubscriptionList_um.end()) {

				}
				else {
					SubscriptionAllList_t subAllList;
					SocketPort2SubscriptionList_um[it] = subAllList;
				}

				for (size_t i = 0; i < idlist.size(); i++) {
					if (SocketPort2SubscriptionList_um[it].DetectorList.pattern_v.find(idlist[i]) == SocketPort2SubscriptionList_um[it].DetectorList.pattern_v.end()) {
						SocketPort2SubscriptionList_um[it].DetectorList.pattern_v.insert(idlist[i]);
					}
				}
			}
		}
		else {
			// ERROR HANDLING

		}
	}
}

void ConfigHelper::popErrorMessageBox(string message, string title) {
#ifdef UNICODE
	//wchar_t messageBuff[1000];
	//size_t msgOutLen;
	//mbstowcs_s(&msgOutLen, messageBuff, message.length() + 1, message.c_str(), message.length());

	//wchar_t titleBuff[1000];
	//size_t titleOutLen;
	//mbstowcs_s(&titleOutLen, titleBuff, title.length() + 1, title.c_str(), title.length());


	//int msgboxID = MessageBox(
	//	NULL,
	//	messageBuff,
	//	titleBuff,
	//	MB_ICONERROR | MB_OK
	//);


	//switch (msgboxID)
	//{
	//case IDOK:
	//	// TODO: add code
	//	break;
	//}

#else
	//char messageBuff[1000];
	//size_t msgOutLen;
	//mbstowcs_s(&msgOutLen, messageBuff, message.length() + 1, message.c_str(), message.length());

	//char titleBuff[1000];
	//size_t titleOutLen;
	//mbstowcs_s(&titleOutLen, titleBuff, title.length() + 1, title.c_str(), title.length());


	// int msgboxID = MessageBox(
		// NULL,
		// message.c_str(),
		// title.c_str(),
		// MB_ICONERROR | MB_OK
	// );


	// switch (msgboxID)
	// {
	// case IDOK:
		// // TODO: add code
		// break;
	// }
#endif

}
