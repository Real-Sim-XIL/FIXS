#pragma once
/*Define vehicle data structures using only Plain Old Data (POD), i.e. string, double, int, etc., no complex struct, class what so ever*/
//#include <any>
#include <unordered_map>
#include <vector>

// MESSAGE IDENTIFIER 1
// Full vehicle data structure that will be shared between SUMO and other simulators
// !!! This does not necessary mean all data directly communicated between simulators
typedef struct  {
	std::string id; 
	std::string type; 
	std::string vehicleClass;
	float speed; 
	float acceleration;
	float positionX;
	float positionY; 
	float positionZ; 
	float heading; 
	uint32_t color; 
	std::string linkId;
	int32_t laneId;
	float distanceTravel;
	float speedDesired;
	float accelerationDesired;

	int8_t hasPrecedingVehicle;
	std::string precedingVehicleId;
	float precedingVehicleDistance; // distance to preceding vehicle
	float precedingVehicleSpeed; // absolute speed of preceding vehicle
	std::string signalLightId;
	int32_t signalLightHeadId;
	float signalLightDistance; // distance to next signal light
	int8_t signalLightColor; // color of next signal light
	float speedLimit;
	float speedLimitNext;
	float speedLimitChangeDistance; 

	std::string linkIdNext;
	float grade; 


	int8_t activeLaneChange; // 1 to the left, -1 to the right, 0 stay on the lane, 


	// variables not retrievable yet
	float length;
	float width;
	float weight;


	//double positionOnLink;


}VehFullData_t;


typedef struct  {
	
	double positionOnLink;

	//int isNextSpeedLimitFound;
	//double nextSpeedLimitDistanceCalc;
	//double nextSpeedLimit;

	std::vector <std::string> nextLink_v;

	double positionXrear;
	double positionYrear;

	double accelerationDesired;


}VehicleDataAuxiliary_t;




// MESSAGE IDENTIFIER 2
typedef struct  {
	// unique number of signal light 
	// in VISSIM, this is the index for each signal group
	uint16_t id;

	// this is name of the signal light
	// in SUMO, it is naturally using string to distinguish each TLS
	// in VISSIM, this name can be empty if not set in VISSIM
	std::string name; 

	// this is state, consistent to SUMO definition, 
	// so state of each signal group (VISSIM terminology) or each phase will be represented by one single string, 
	// starting from left to right, that is e.g. phase 1 (signal group 1)-> phase 8 (signal group 8)
	std::string state; 

}TrafficLightData_t;

// MESSAGE IDENTIFIER 3
typedef struct  {
	//std::string intersectionId; // intersection this detector associated with
	uint8_t id; // port number of detector/detector id 
	std::string name; //
	uint8_t state; // state of detector 0 off, 1 active

}DetectorData_t;

// intersection id, intersection name, vector of detectors
typedef std::tuple <int, std::string, std::vector < DetectorData_t>> TlsDetector_t;





typedef struct {
	double x;
	double y;
	double z;
}COORD_t;
