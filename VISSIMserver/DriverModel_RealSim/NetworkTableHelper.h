#pragma once

#include "VehDataMsgDefs.h"

#include<unordered_map>
#include <sstream>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <numeric>

#include "yaml-cpp/yaml.h"


typedef struct SignalHead_t {
	int signalHeadIndex;
	std::string signalHeadName;
	int signalGroupIndex;
	std::string signalGroupName;
	int signalControllerIndex;
	std::string signalControllerName;
};

/*Sort each vector of SpeedLimit_t by vector position, increasing order*/
typedef struct SpeedLimit_t {
	double position;
	int speedLimitType;
	double speedLimitValue; // m/s
	double timeFrom;
	double timeTo;
};

// <link, lane, vehicleType>
typedef std::tuple <std::string, int, std::string> linkLaneVehtype;

// from link, from position, to link, to position
typedef std::tuple <std::string, double, std::string, double> connectorTopo_t;


class NetworkTableHelper
{

public:
	NetworkTableHelper();

	int readSignalTable(std::string SignalTableFullName);

	std::unordered_map <int, SignalHead_t > SignalHeadInfo_um; // signal head index => signal head configurations

	std::unordered_map <int, TrafficLightData_t> SignalData; // signal controller index => trafficlightdata


	int readLinkTable(std::string LinkTableFullName);


	// connector list
	// id ===> 
	std::unordered_map <std::string, connectorTopo_t> ConnectorInfo_um;

	// link infor
	// id ===> length, numLanes
	std::unordered_map <std::string, std::pair<double, int> > LinkInfo_um;

	int readSpeedLimitTable(std::string SpeedLimitFullName);

	int getSpeedLimit(double simTime, VehicleDataAuxiliary_t VehicleDataAuxiliary, VehFullData_t& VehicleData);

	// A hash function used to hash a tuple
	struct hash_tuple {

		template <class T1, class T2, class T3>

		size_t operator()(
			const std::tuple<T1, T2, T3>& x)
			const
		{
			std::hash<std::string> h_s{};
			std::hash<int> h_i{};

			return h_s(get<0>(x))
				^ h_i(get<1>(x))
				^ h_s(get<2>(x));

			//return (get<0>(x))
			//	^ (get<1>(x))
			//	^ (get<2>(x));
		}
	};

	// A hash function used to hash a tuple
	struct hash_tuple4 {

		template <class T1, class T2, class T3, class T4>

		size_t operator()(
			const std::tuple<T1, T2, T3, T4>& x)
			const
		{
			std::hash<std::string> h_s{};
			std::hash<int> h_i{};

			return h_s(get<0>(x))
				^ h_i(get<1>(x))
				^ h_s(get<2>(x))
				^ h_s(get<2>(x));

		}
	};
	// unordered_map <link, lane, vehicleType> ===> vecotr of all SpeedLimit intervals
	std::unordered_map <linkLaneVehtype, std::vector <SpeedLimit_t>, hash_tuple> SpeedLimitInfo_um;

	// for each vehicle, map it to an index corresponding to current speed limit vector
	// string vehicle id ===> <current interval index, vector of all SpeedLimit intervals> 
	std::unordered_map <std::string, int> VehicleId2SpeedLimitIntervals;

	// for each vehicle, store its current/last speed limit 
	std::unordered_map <std::string, double> VehicleId2SpeedLimitCurrent;


	// if provided a route file, then can read these files
	// A hash function used to hash a tuple
	struct hash_pair {

		template <class T1, class T2>

		size_t operator()(
			const std::pair<T1, T2>& x)
			const
		{
			std::hash<std::string> h_s{};

			return h_s(get<0>(x))
				^ h_s(get<1>(x));
		}
	};
	// vehicle type ==> linklane list
	// vehicle id, vehicle type ==> linklane list
	//		then as starting Vissim dll, convert these to a map of
	//		link, lane, vehicle type => next speed limit, distance to next speed limit counting from next link
	//		link, lane, vehicle type, id => next speed limit, distance to next speed limit counting from next link
	std::unordered_map <std::string, std::vector<std::pair<std::string, int>>> VehicleType2linklaneRoute;
	std::unordered_map <std::pair<std::string, std::string>, std::vector<std::pair<std::string, int>>, hash_pair> VehIdVehType2linklaneRoute;

	// essentially these two are maps to the next speed limit as well as the distance to next speed limit counting from next link. so to find the correct distance to next speed limit, need to add this distance with remaining distance on current link.  
	std::unordered_map <linkLaneVehtype, std::pair<double, double>, hash_tuple> RouteVehTypeSpeedLimit_um;
	std::unordered_map <std::tuple<std::string,int,std::string,std::string>, std::pair<double, double>, hash_tuple4> RouteVehIdSpeedLimit_um;


	void parserStringVector(YAML::Node node, std::string name, std::vector<std::string>& outStringVector);
	int readRoutes(std::string routeFileName);
	void parserRouteSpeedLimit(std::string type, std::vector<std::pair<std::string, int>> linklane_v, std::vector <double>& nextSpeedLimit_v, std::vector <double>& nextSpeedLimitDistance_v);


private:

};

