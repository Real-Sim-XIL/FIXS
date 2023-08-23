#include "ConfigHelper.h"

using namespace std;


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
		printf("\nWill enable real sim as default!\n");
	}
	if (node["EnableVerboseLog"]) {
		SimulationSetup.EnableVerboseLog = parserFlag(node, "EnableVerboseLog");
	}
	else {
		SimulationSetup.EnableVerboseLog = false;
		printf("\nWill disable verbose log as default!\n");
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
		printf("\nTraffic Simulator not specified! Will use SUMO as default!\n");
	}
	if (node["TrafficSimulatorIP"]) {
		SimulationSetup.TrafficSimulatorIP = parserString(node, "TrafficSimulatorIP");
	}
	else {
		SimulationSetup.TrafficSimulatorIP = "127.0.0.1";
		printf("\nTraffic Simulator IP not specified! Will use localhost 127.0.0.1 as default!\n");
	}
	if (node["TrafficSimulatorPort"]) {
		SimulationSetup.TrafficSimulatorPort = parserInteger(node, "TrafficSimulatorPort");
	}
	else {
		SimulationSetup.TrafficSimulatorPort = 1337;
		printf("\nTraffic Simulator Port not specified! Will use 1337 as default!\n");
	}
	if (node["SimulationMode"]) {
		SimulationSetup.SimulationMode = parserInteger(node, "SimulationMode");
	}
	else {
		SimulationSetup.SimulationMode = 0;
		printf("\nSimulation mode not specified! Will use Mode 0 as default!\n");
}
	if (node["SimulationModeParameter"]) {
		SimulationSetup.SimulationModeParameter = parserDouble(node, "SimulationModeParameter");
	}
	else {
		SimulationSetup.SimulationModeParameter = 0;
		printf("\nSimulation mode parameter not specified! Will use Parameter=0 as default!\n");
	}
	if (node["VehicleMessageField"]) {
		parserStringVector(node, "VehicleMessageField", SimulationSetup.VehicleMessageField);
	}
	else {
		SimulationSetup.VehicleMessageField = { "id", "type", "speed", "positionX", "positionY", "positionZ", "heading", "color", "linkId", "laneId", "distanceTravel", "acceleration", "speedDesired", "acceleartionDesired", "hasPrecedingVehicle", "precedingVehicleId", "precedingVehicleDistance", "precedingVehicleSpeed", "signalLightDistance", "signalLightColor", "speedLimit", "speedLimitNext", "speedLimitChangeDistance", "linkIdNext", "grade" , "activeLaneChange", "signalLightId", "signalLightHeadId"};
		printf("\nWill use all available vehicle message field!\n");
	}



	// -------------------
	//  Store Vehicle Message Field to an unordered_set
	// -------------------
	std::unordered_set <std::string> VehicleMessageField_set;
	printf("Vehicle message selected:");
	for (size_t i = 0; i < SimulationSetup.VehicleMessageField.size(); i++) {
		VehicleMessageField_set.insert(SimulationSetup.VehicleMessageField[i]);
		printf("\t%s", SimulationSetup.VehicleMessageField[i].c_str());
	}

	// -------------------
	//  SANITY CHECK: Vehicle Message Field
	// -------------------
	// currently, mandate messages are id, speed, one of speedDesired or accelerationDesired
	printf("\n\n");
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
		printf("\nXil not specified as server or client! Will set Xil as client!\n");
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
	if (node["EgoId"]) {
		CarMakerSetup.EgoId = parserString(node, "EgoId");
	}
	else {
		// try to parser ego vehicle info, if and only if there is just one vehicle subscribed
		if (SubscriptionVehicleList.vehicleSubscribeId_v.size() == 1) {
			CarMakerSetup.EgoId = SubscriptionVehicleList.vehicleSubscribeId_v.begin()->first;
		}
		else {
			CarMakerSetup.EgoId = "egoCm";
		}
		//printf("\nCarMaker IP not specified! Will use localhost 127.0.0.1 as default!\n");
	}
	if (node["EgoType"]) {
		CarMakerSetup.EgoType = parserString(node, "EgoType");
	}
	else {
		CarMakerSetup.EgoType = "";
		//printf("\nCarMaker IP not specified! Will use localhost 127.0.0.1 as default!\n");
	}

	if (node["SynchronizeTrafficSignal"]) {
		CarMakerSetup.SynchronizeTrafficSignal = parserFlag(node, "SynchronizeTrafficSignal");
	}
	else {
		CarMakerSetup.SynchronizeTrafficSignal = false;
	}
	SubscriptionSignalList.subAllSignalFlag = CarMakerSetup.SynchronizeTrafficSignal;

	if (CarMakerSetup.SynchronizeTrafficSignal) {
		//SocketPort2SubscriptionList_um[CarMakerSetup.CarMakerPort].SignalList.subAllSignalFlag = true;
		SocketPort2SubscriptionList_um[2444].SignalList.subAllSignalFlag = true;
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
				if (att.compare("id") == 0 || att.compare("name") == 0) {
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

	////if (CarMakerSetup.EnableCosimulation && CarMakerSetup.SynchronizeTrafficSignal){
	//if (CarMakerSetup.SynchronizeTrafficSignal) {
	//	SubscriptionSignalList.subAllSignalFlag = true;
	//}
	//else {
	//	SubscriptionSignalList.subAllSignalFlag = false;
	//}
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
