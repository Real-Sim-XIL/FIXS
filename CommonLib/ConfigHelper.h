#pragma once

#include "yaml-cpp/yaml.h"
//#ifndef WINDOWS_INCLUDED
//#define WINDOWS_INCLUDED
//#define _WINSOCKAPI_
//#include <windows.h>
//#endif
//#include <tchar.h>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>

//#include <fileapi.h>

//template <typename T> std::unordered_map<std::string, std::vector<T> > subAttMap;

// [ego][id]
// multiple ego/point/link -----> {attribute: list of value}, one for each client/port/connection

typedef typename std::unordered_map<std::string, std::vector<std::string> > SubAttMap_t;

// type, attribute, ip, port
// string, SubAttMap_t, vector <string>, vector <int>
typedef typename std::vector <std::tuple < std::string, SubAttMap_t, std::vector <std::string>, std::vector <int>> > Subscription_t;


struct SimulationSetup_t {
	bool EnableRealSim;

	bool EnableVerboseLog;

	double SimulationEndTime;

	// NEED to fix later
	// flag to set SUMO speed through setPreviousSpeed
	bool EnableExternalDynamics;

	std::vector<std::string> VehicleMessageField;

	std::string SelectedTrafficSimulator;

	std::string TrafficSimulatorIP;

	int TrafficSimulatorPort;

	int SimulationMode;

	double SimulationModeParameter;

	std::string TrafficLayerIP;

	int TrafficLayerPort;
};

struct ApplicationSetup_t {
	bool EnableApplicationLayer;

	int NumberOfApplications;

	std::vector <int> ApplicationPort;


	Subscription_t VehicleSubscription;

	Subscription_t DetectorSubscription;

	Subscription_t SignalSubscription;

};

struct XilSetup_t {
	bool EnableXil;

	bool AsServer;

	Subscription_t VehicleSubscription;

	Subscription_t SignalSubscription;

	Subscription_t DetectorSubscription;

};


struct CarMakerSetup_t {
	bool EnableCosimulation;

	bool EnableEgoSimulink;

	std::string CarMakerIP;

	int CarMakerPort;

	double TrafficRefreshRate;

	std::string EgoId;

	std::string EgoType;

	bool SynchronizeTrafficSignal;

	int TrafficSignalPort;

};


struct SumoSetup_t {

	int SpeedMode;

};

typedef struct SubscriptionVehicleList_t {
	std::unordered_set <std::string> edgeSubscribeId_v;

	// vehicle id -> radius
	std::unordered_map <std::string, double > vehicleSubscribeId_v;

	std::pair <bool, double> subscribeAllVehicle = { std::make_pair(false, 0) };

	// tuple x, y, z, radius
	// name of the point -> x, y, z, r
	std::unordered_map <std::string, std::tuple<double, double, double, double> > pointSubscribeId_v;
	//std::vector <std::string> pointNamePoi_v;

	std::unordered_map <std::string, double> vehicleTypeSubscribedId_v;
};

typedef struct SubscriptionSignalList_t {
	std::unordered_set <std::string> signalId_v;

	bool subAllSignalFlag; 
};

typedef struct SubscriptionDetectorList_t {
	std::unordered_set <std::string> pattern_v;;
};

typedef struct SubscriptionAllList_t {
	SubscriptionVehicleList_t VehicleList;
	SubscriptionSignalList_t SignalList = { {}, false };
	SubscriptionDetectorList_t DetectorList;

};

class ConfigHelper
{
public:
	ConfigHelper();

	int getConfig(std::string configName);

	bool parserFlag(YAML::Node node, std::string name);
	std::string parserString(YAML::Node node, std::string name);
	double parserDouble(YAML::Node node, std::string name);
	int parserInteger(YAML::Node node, std::string name);
	void parserIntegerVector(YAML::Node node, std::string name, std::vector<int>& outIntegerVector);
	void parserStringVector(YAML::Node node, std::string name, std::vector<std::string>& outStringVector);

	void parserSubscription(YAML::Node node, std::string name, Subscription_t& subscription);


	void getVehSubscriptionList(Subscription_t VehSub, std::unordered_set <std::string>& edgeSubscribeId_v, std::unordered_map <std::string, double >& vehicleSubscribeId_v, std::pair <bool, double>& subscribeAllVehicle, std::unordered_map <std::string, std::tuple<double, double, double, double> >& pointSubscribeId_v, std::unordered_map <std::string, double>& vehicleTypeSubscribedId_v);
	
	void getSigSubscriptionList(Subscription_t SigSub);
	void getDetSubscriptionList(Subscription_t DetSub);

	// subscription abstraction, these variables are raw configuration specs
	// users can use these raw specs to create their own subscription containers for different usages
	SimulationSetup_t SimulationSetup;
	ApplicationSetup_t ApplicationSetup;
	XilSetup_t XilSetup;
	CarMakerSetup_t CarMakerSetup;
	SumoSetup_t SumoSetup;




	// default containers to facilitate loop over each subscribed element, e.g., each vehicle id

	// provide containers
	struct FlagSetup{

	}FlagSetup;


	SubscriptionVehicleList_t SubscriptionVehicleList;

	SubscriptionSignalList_t SubscriptionSignalList;

	SubscriptionDetectorList_t SubscriptionDetectorList;


	// creat maps to hold socket port -> subscription list so that can use it to distribute messages

	std::unordered_map <int, SubscriptionAllList_t> SocketPort2SubscriptionList_um;

	void resetConfig();

private:
	void popErrorMessageBox(std::string message, std::string title);

	void extractSubscriptionAttributes(YAML::Node attnode, std::string type, std::string att, SubAttMap_t& attMap);

};

