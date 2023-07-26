#include "MsgHelper.h"

using namespace std;

MsgHelper::MsgHelper() {
}

#ifndef RS_DSPACE
void MsgHelper::getConfig(ConfigHelper Config_c) {
	VehicleMessageField_v = Config_c.SimulationSetup.VehicleMessageField;

	for (int i = 0; i < VehicleMessageField_v.size(); i++) {
		VehicleMessageField_set.insert(VehicleMessageField_v[i]);
	}
}
#endif

void MsgHelper::clearRecvStorage() {
	VehDataRecv_um.clear();
	TlsDataRecv_um.clear();
	DetDataRecv_um.clear();

}

void MsgHelper::clearSendStorage() {
	VehDataSend_um.clear();
	DetDataSend_um.clear();
	TlsDataSend_um.clear();
}

void MsgHelper::printVehData(VehFullData_t VehData) {
	printf("PRINTING VehData: \n");
	if (VehicleMessageField_set.find("id") != VehicleMessageField_set.end()) {
		printf("\t id: %s\n", VehData.id.c_str());
	}
	if (VehicleMessageField_set.find("type") != VehicleMessageField_set.end()) {
		printf("\t type: %s\n", VehData.type.c_str());
	}
	if (VehicleMessageField_set.find("vehicleClass") != VehicleMessageField_set.end()) {
		printf("\t vehicleClass: %s\n", VehData.vehicleClass.c_str());
	}
	if (VehicleMessageField_set.find("speed") != VehicleMessageField_set.end()) {
		printf("\t speed: %f\n", VehData.speed);
	}
	if (VehicleMessageField_set.find("acceleration") != VehicleMessageField_set.end()) {
		printf("\t acceleration: %f\n", VehData.acceleration);
	}
	if (VehicleMessageField_set.find("positionX") != VehicleMessageField_set.end()) {
		printf("\t positionX: %f\n", VehData.positionX);
	}
	if (VehicleMessageField_set.find("positionY") != VehicleMessageField_set.end()) {
		printf("\t positionY: %f\n", VehData.positionY);
	}
	if (VehicleMessageField_set.find("positionZ") != VehicleMessageField_set.end()) {
		printf("\t positionZ: %f\n", VehData.positionZ);
	}
	if (VehicleMessageField_set.find("heading") != VehicleMessageField_set.end()) {
		printf("\t heading: %f\n", VehData.heading);
	}
	if (VehicleMessageField_set.find("color") != VehicleMessageField_set.end()) {
		printf("\t color: %d\n", VehData.color);
	}
	if (VehicleMessageField_set.find("linkId") != VehicleMessageField_set.end()) {
		printf("\t linkId: %s\n", VehData.linkId.c_str());
	}
	if (VehicleMessageField_set.find("laneId") != VehicleMessageField_set.end()) {
		printf("\t laneId: %d\n", VehData.laneId);
	}
	if (VehicleMessageField_set.find("distanceTravel") != VehicleMessageField_set.end()) {
		printf("\t distanceTravel: %f\n", VehData.distanceTravel);
	}
	if (VehicleMessageField_set.find("speedDesired") != VehicleMessageField_set.end()) {
		printf("\t speedDesired: %f\n", VehData.speedDesired);
	}
	if (VehicleMessageField_set.find("accelerationDesired") != VehicleMessageField_set.end()) {
		printf("\t accelerationDesired: %f\n", VehData.accelerationDesired);
	}
	if (VehicleMessageField_set.find("hasPrecedingVehicle") != VehicleMessageField_set.end()) {
		printf("\t hasPrecedingVehicle: %d\n", VehData.hasPrecedingVehicle);
	}
	if (VehicleMessageField_set.find("precedingVehicleId") != VehicleMessageField_set.end()) {
		printf("\t precedingVehicleId: %s\n", VehData.precedingVehicleId.c_str());
	}
	if (VehicleMessageField_set.find("precedingVehicleDistance") != VehicleMessageField_set.end()) {
		printf("\t precedingVehicleDistance: %f\n", VehData.precedingVehicleDistance);
	}
	if (VehicleMessageField_set.find("precedingVehicleSpeed") != VehicleMessageField_set.end()) {
		printf("\t precedingVehicleSpeed: %f\n", VehData.precedingVehicleSpeed);
	}
	if (VehicleMessageField_set.find("signalLightId") != VehicleMessageField_set.end()) {
		printf("\t signalLightId: %s\n", VehData.signalLightId.c_str());
	}
	if (VehicleMessageField_set.find("signalLightHeadId") != VehicleMessageField_set.end()) {
		printf("\t signalLightHeadId: %d\n", VehData.signalLightHeadId);
	}
	if (VehicleMessageField_set.find("signalLightDistance") != VehicleMessageField_set.end()) {
		printf("\t signalLightDistance: %f\n", VehData.signalLightDistance);
	}
	if (VehicleMessageField_set.find("signalLightColor") != VehicleMessageField_set.end()) {
		printf("\t signalLightColor: %d\n", VehData.signalLightColor);
	}
	if (VehicleMessageField_set.find("speedLimit") != VehicleMessageField_set.end()) {
		printf("\t speedLimit: %f\n", VehData.speedLimit);
	}
	if (VehicleMessageField_set.find("speedLimitNext") != VehicleMessageField_set.end()) {
		printf("\t speedLimitNext: %f\n", VehData.speedLimitNext);
	}
	if (VehicleMessageField_set.find("speedLimitChangeDistance") != VehicleMessageField_set.end()) {
		printf("\t speedLimitChangeDistance: %f\n", VehData.speedLimitChangeDistance);
	}
	if (VehicleMessageField_set.find("linkIdNext") != VehicleMessageField_set.end()) {
		printf("\t linkIdNext: %s\n", VehData.linkIdNext.c_str());
	}
	if (VehicleMessageField_set.find("grade") != VehicleMessageField_set.end()) {
		printf("\t grade: %f\n", VehData.grade);
	}
	if (VehicleMessageField_set.find("activeLaneChange") != VehicleMessageField_set.end()) {
		printf("\t activeLaneChange: %d\n", VehData.activeLaneChange);
	}


	// printVehData: add new vehicle message field here
}


void MsgHelper::printVehDataToFile(const std::string fileName, VehFullData_t VehData) {

	FILE* f = fopen(fileName.c_str(), "a");

	fprintf(f, "PRINTING VehData: \n");

	if (VehicleMessageField_set.find("id") != VehicleMessageField_set.end()) {
		fprintf(f, "\t id: %s\n", VehData.id.c_str());
	}
	if (VehicleMessageField_set.find("type") != VehicleMessageField_set.end()) {
		fprintf(f, "\t type: %s\n", VehData.type.c_str());
	}
	if (VehicleMessageField_set.find("vehicleClass") != VehicleMessageField_set.end()) {
		fprintf(f, "\t vehicleClass: %s\n", VehData.vehicleClass.c_str());
	}
	if (VehicleMessageField_set.find("speed") != VehicleMessageField_set.end()) {
		fprintf(f, "\t speed: %f\n", VehData.speed);
	}
	if (VehicleMessageField_set.find("acceleration") != VehicleMessageField_set.end()) {
		fprintf(f, "\t acceleration: %f\n", VehData.acceleration);
	}
	if (VehicleMessageField_set.find("positionX") != VehicleMessageField_set.end()) {
		fprintf(f, "\t positionX: %f\n", VehData.positionX);
	}
	if (VehicleMessageField_set.find("positionY") != VehicleMessageField_set.end()) {
		fprintf(f, "\t positionY: %f\n", VehData.positionY);
	}
	if (VehicleMessageField_set.find("positionZ") != VehicleMessageField_set.end()) {
		fprintf(f, "\t positionZ: %f\n", VehData.positionZ);
	}
	if (VehicleMessageField_set.find("heading") != VehicleMessageField_set.end()) {
		fprintf(f, "\t heading: %f\n", VehData.heading);
	}
	if (VehicleMessageField_set.find("color") != VehicleMessageField_set.end()) {
		fprintf(f, "\t color: %d\n", VehData.color);
	}
	if (VehicleMessageField_set.find("linkId") != VehicleMessageField_set.end()) {
		fprintf(f, "\t linkId: %s\n", VehData.linkId.c_str());
	}
	if (VehicleMessageField_set.find("laneId") != VehicleMessageField_set.end()) {
		fprintf(f, "\t laneId: %d\n", VehData.laneId);
	}
	if (VehicleMessageField_set.find("distanceTravel") != VehicleMessageField_set.end()) {
		fprintf(f, "\t distanceTravel: %f\n", VehData.distanceTravel);
	}
	if (VehicleMessageField_set.find("speedDesired") != VehicleMessageField_set.end()) {
		fprintf(f, "\t speedDesired: %f\n", VehData.speedDesired);
	}
	if (VehicleMessageField_set.find("accelerationDesired") != VehicleMessageField_set.end()) {
		fprintf(f, "\t accelerationDesired: %f\n", VehData.accelerationDesired);
	}
	if (VehicleMessageField_set.find("hasPrecedingVehicle") != VehicleMessageField_set.end()) {
		fprintf(f, "\t hasPrecedingVehicle: %d\n", VehData.hasPrecedingVehicle);
	}
	if (VehicleMessageField_set.find("precedingVehicleId") != VehicleMessageField_set.end()) {
		fprintf(f, "\t precedingVehicleId: %s\n", VehData.precedingVehicleId.c_str());
	}
	if (VehicleMessageField_set.find("precedingVehicleDistance") != VehicleMessageField_set.end()) {
		fprintf(f, "\t precedingVehicleDistance: %f\n", VehData.precedingVehicleDistance);
	}
	if (VehicleMessageField_set.find("precedingVehicleSpeed") != VehicleMessageField_set.end()) {
		fprintf(f, "\t precedingVehicleSpeed: %f\n", VehData.precedingVehicleSpeed);
	}
	if (VehicleMessageField_set.find("signalLightId") != VehicleMessageField_set.end()) {
		fprintf(f, "\t signalLightId: %s\n", VehData.signalLightId.c_str());
	}
	if (VehicleMessageField_set.find("signalLightHeadId") != VehicleMessageField_set.end()) {
		fprintf(f, "\t signalLightHeadId: %d\n", VehData.signalLightHeadId);
	}
	if (VehicleMessageField_set.find("signalLightDistance") != VehicleMessageField_set.end()) {
		fprintf(f, "\t signalLightDistance: %f\n", VehData.signalLightDistance);
	}
	if (VehicleMessageField_set.find("signalLightColor") != VehicleMessageField_set.end()) {
		fprintf(f, "\t signalLightColor: %d\n", VehData.signalLightColor);
	}
	if (VehicleMessageField_set.find("speedLimit") != VehicleMessageField_set.end()) {
		fprintf(f, "\t speedLimit: %f\n", VehData.speedLimit);
	}
	if (VehicleMessageField_set.find("speedLimitNext") != VehicleMessageField_set.end()) {
		fprintf(f, "\t speedLimitNext: %f\n", VehData.speedLimitNext);
	}
	if (VehicleMessageField_set.find("speedLimitChangeDistance") != VehicleMessageField_set.end()) {
		fprintf(f, "\t speedLimitChangeDistance: %f\n", VehData.speedLimitChangeDistance);
	}
	if (VehicleMessageField_set.find("linkIdNext") != VehicleMessageField_set.end()) {
		fprintf(f, "\t linkIdNext: %s\n", VehData.linkIdNext.c_str());
	}
	if (VehicleMessageField_set.find("grade") != VehicleMessageField_set.end()) {
		fprintf(f, "\t grade: %f\n", VehData.grade);
	}
	if (VehicleMessageField_set.find("activeLaneChange") != VehicleMessageField_set.end()) {
		fprintf(f, "\t activeLaneChange: %d\n", VehData.activeLaneChange);
	}

	fclose(f);

}

void MsgHelper::packString(string strData, char* buffer, int* iByte) {
	uint8_t tempUint8;

	tempUint8 = (uint8_t) strData.size();
	memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);
	memcpy(buffer + *iByte, (char*)strData.c_str(), tempUint8);
	*iByte = *iByte + tempUint8;
}

void MsgHelper::packHeader(uint8_t simState, float simTime, uint32_t totalMsgSize, char* buffer, int* iByte) {
	memcpy(buffer + *iByte, (char*)&simState, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);
	memcpy(buffer + *iByte, (char*)&simTime, sizeof(float));
	*iByte = *iByte + sizeof(float);
	memcpy(buffer + *iByte, (char*)&totalMsgSize, sizeof(uint32_t));
	*iByte = *iByte + sizeof(uint32_t);
}

void MsgHelper::depackHeader(char* buffer, uint8_t* simState, float* simTime, uint32_t* totalMsgSize) {
	int iByte = 0;

	uint32_t tempUint32;
	uint8_t tempUint8;
	float tempFloat;

	memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
	iByte += sizeof(uint8_t);
	*simState = tempUint8;

	memcpy(&tempFloat, buffer + iByte, sizeof(float));
	iByte += sizeof(float);
	*simTime = tempFloat;

	memcpy(&tempUint32, buffer + iByte, sizeof(uint32_t));
	iByte += sizeof(uint32_t);
	*totalMsgSize = tempUint32;
}

void MsgHelper::depackMsgType(char* buffer, uint16_t* msgSize, uint8_t* msgType) {
	uint8_t tempUint8;
	uint16_t tempUint16;

	int iByte = 0;
	memcpy(&tempUint16, buffer + iByte, sizeof(uint16_t));
	iByte += sizeof(uint16_t);
	*msgSize = tempUint16;

	memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
	iByte += sizeof(uint8_t);
	*msgType = tempUint8;
}

void MsgHelper::stringVehDataToBuffer(string vehDataString, std::string VehDataName, char* buffer, int* iByte) {
	if (VehicleMessageField_set.find(VehDataName) != VehicleMessageField_set.end())
	{
		uint8_t tempUint8;

		tempUint8 = (uint8_t)vehDataString.size();
		memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
		*iByte = *iByte + sizeof(uint8_t);
		memcpy(buffer + *iByte, (char*)vehDataString.c_str(), tempUint8);
		*iByte = *iByte + tempUint8;
	}
};


template <typename T> void MsgHelper::numericVehDataToBuffer(T vehDataField, std::string VehDataName, char* buffer, int* iByte) {
	if (VehicleMessageField_set.find(VehDataName) != VehicleMessageField_set.end())
	{
		memcpy(buffer + *iByte, (char*)&vehDataField, sizeof(T));
		*iByte = *iByte + sizeof(T);
	}
}

void MsgHelper::bufferToStringVehData(char* buffer, int* iByte, std::string VehDataName, string& tempString) {
	uint8_t tempUint8;

	if (VehicleMessageField_set.find(VehDataName) != VehicleMessageField_set.end()) {
		memcpy(&tempUint8, buffer + *iByte, sizeof(uint8_t));
		*iByte += sizeof(uint8_t);
		tempString.assign(buffer + *iByte, buffer + *iByte + tempUint8);
		*iByte += tempUint8;
	}
}

template <typename T> void MsgHelper::bufferToNumericVehData(char* buffer, int* iByte, std::string VehDataName, T& tempVal) {
	if (VehicleMessageField_set.find(VehDataName) != VehicleMessageField_set.end()) {
		memcpy(&tempVal, buffer + *iByte, sizeof(tempVal));
		*iByte += sizeof(tempVal);
	}
}

void MsgHelper::packVehData(VehFullData_t VehData, char* buffer, int* iByte) {

	uint8_t tempUint8;
	uint16_t tempUint16;

	//bool tempBool;

	int initByte = *iByte;

	*iByte = *iByte + sizeof(uint16_t);
	tempUint8 = 1;
	memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);

	// NOTE: order of these data field DOES matter!
	stringVehDataToBuffer(VehData.id, "id", buffer, iByte);
	stringVehDataToBuffer(VehData.type, "type", buffer, iByte);
	stringVehDataToBuffer(VehData.vehicleClass, "vehicleClass", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.speed, "speed", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.acceleration, "acceleration", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.positionX, "positionX", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.positionY, "positionY", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.positionZ, "positionZ", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.heading, "heading", buffer, iByte);
	numericVehDataToBuffer<uint32_t>(VehData.color, "color", buffer, iByte);
	stringVehDataToBuffer(VehData.linkId, "linkId", buffer, iByte);
	numericVehDataToBuffer<int32_t>(VehData.laneId, "laneId", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.distanceTravel, "distanceTravel", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.speedDesired, "speedDesired", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.accelerationDesired, "accelerationDesired", buffer, iByte);
	numericVehDataToBuffer<int8_t>(VehData.hasPrecedingVehicle, "hasPrecedingVehicle", buffer, iByte);
	stringVehDataToBuffer(VehData.precedingVehicleId, "precedingVehicleId", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.precedingVehicleDistance, "precedingVehicleDistance", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.precedingVehicleSpeed, "precedingVehicleSpeed", buffer, iByte);
	stringVehDataToBuffer(VehData.signalLightId, "signalLightId", buffer, iByte);
	numericVehDataToBuffer<int32_t>(VehData.signalLightHeadId, "signalLightHeadId", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.signalLightDistance, "signalLightDistance", buffer, iByte);
	numericVehDataToBuffer<int8_t>(VehData.signalLightColor, "signalLightColor", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.speedLimit, "speedLimit", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.speedLimitNext, "speedLimitNext", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.speedLimitChangeDistance, "speedLimitChangeDistance", buffer, iByte);
	stringVehDataToBuffer(VehData.linkIdNext, "linkIdNext", buffer, iByte);
	numericVehDataToBuffer<float>(VehData.grade, "grade", buffer, iByte);
	numericVehDataToBuffer<int8_t>(VehData.activeLaneChange, "activeLaneChange", buffer, iByte);

	// packVehData: add new vehicle message field here
	
	// go back to the first byte location and parser message size 
	tempUint16 = (*iByte - initByte);
	memcpy(buffer + initByte, (char*)&tempUint16, sizeof(tempUint16));
}

void MsgHelper::depackVehData(char* buffer, VehFullData_t& VehData) {

	// here only depacking the core vehicle data part
	//		i.e. the initial message size and identifier are excluded 

	uint32_t tempUint32 = 0;
	int32_t tempInt32 = -1;
	string tempString = "";
	float tempFloat = -1.0;
	int8_t tempInt8 = -1;

	int iByte = 0;

	bufferToStringVehData(buffer, &iByte, "id", tempString); VehData.id = tempString;
	bufferToStringVehData(buffer, &iByte, "type", tempString); VehData.type = tempString;
	bufferToStringVehData(buffer, &iByte, "vehicleClass", tempString); VehData.vehicleClass = tempString;
	bufferToNumericVehData<float>(buffer, &iByte, "speed", tempFloat); VehData.speed = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "acceleration", tempFloat); VehData.acceleration = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "positionX", tempFloat); VehData.positionX = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "positionY", tempFloat); VehData.positionY = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "positionZ", tempFloat); VehData.positionZ = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "heading", tempFloat); VehData.heading = tempFloat;
	bufferToNumericVehData<uint32_t>(buffer, &iByte, "color", tempUint32); VehData.color = tempUint32;
	bufferToStringVehData(buffer, &iByte, "linkId", tempString); VehData.linkId = tempString;
	bufferToNumericVehData<int32_t>(buffer, &iByte, "laneId", tempInt32); VehData.laneId = tempInt32;
	bufferToNumericVehData<float>(buffer, &iByte, "distanceTravel", tempFloat); VehData.distanceTravel = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "speedDesired", tempFloat); VehData.speedDesired = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "accelerationDesired", tempFloat); VehData.accelerationDesired = tempFloat;
	bufferToNumericVehData<int8_t>(buffer, &iByte, "hasPrecedingVehicle", tempInt8); VehData.hasPrecedingVehicle = tempInt8;
	bufferToStringVehData(buffer, &iByte, "precedingVehicleId", tempString); VehData.precedingVehicleId = tempString;
	bufferToNumericVehData<float>(buffer, &iByte, "precedingVehicleDistance", tempFloat); VehData.precedingVehicleDistance = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "precedingVehicleSpeed", tempFloat); VehData.precedingVehicleSpeed = tempFloat;
	bufferToStringVehData(buffer, &iByte, "signalLightId", tempString); VehData.signalLightId = tempString;
	bufferToNumericVehData<int32_t>(buffer, &iByte, "signalLightHeadId", tempInt32); VehData.signalLightHeadId = tempInt32;
	bufferToNumericVehData<float>(buffer, &iByte, "signalLightDistance", tempFloat); VehData.signalLightDistance = tempFloat;
	bufferToNumericVehData<int8_t>(buffer, &iByte, "signalLightColor", tempInt8); VehData.signalLightColor = tempInt8;
	bufferToNumericVehData<float>(buffer, &iByte, "speedLimit", tempFloat); VehData.speedLimit = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "speedLimitNext", tempFloat); VehData.speedLimitNext = tempFloat;
	bufferToNumericVehData<float>(buffer, &iByte, "speedLimitChangeDistance", tempFloat); VehData.speedLimitChangeDistance = tempFloat;
	bufferToStringVehData(buffer, &iByte, "linkIdNext", tempString); VehData.linkIdNext = tempString;
	bufferToNumericVehData<float>(buffer, &iByte, "grade", tempFloat); VehData.grade = tempFloat;
	bufferToNumericVehData<int8_t>(buffer, &iByte, "activeLaneChange", tempInt8); VehData.activeLaneChange = tempInt8;
	// depackVehData: add new vehicle message field here

}

void MsgHelper::packTrafficLightData(TrafficLightData_t TrafficLightData, char* buffer, int* iByte) {
	/*
	MESSAGE STRUCTURE:

	2 bytes, uint16_t, Data length

	1 byte, uint8_t, data identifier

	1 byte, uint8_t, length of the string of signal light name
	X bytes, string, string of signal light name (max 255 bytes)

	2 bytes, uint16_t, unique number of signal light

	1 byte, uint8_t, length of the string of signal state. order of each phase need to be referred to an additional file
	X bytes, string, string of signal state

	*/


	uint8_t tempUint8;
	uint16_t tempUint16;

	int initByte = *iByte;

	// SKIP MESSAGE LENGTH UNTIL FILL THE REST OF BUFFER
	*iByte = *iByte + sizeof(uint16_t);

	// 1 byte, uint8_t, data identifier
	tempUint8 = 2;
	memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);

	// 1 byte, uint8_t, length of the string of signal light name
	// X bytes, string, string of signal light name(max 255 bytes)
	tempUint8 = TrafficLightData.name.size();
	memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);
	memcpy(buffer + *iByte, (char*)TrafficLightData.name.c_str(), tempUint8);
	*iByte = *iByte + tempUint8;

	// 2 bytes, uint16_t, unique number of signal light  
	tempUint16 = TrafficLightData.id;
	memcpy(buffer + *iByte, (char*)&tempUint16, sizeof(uint16_t));

	// 1 byte, uint8_t, length of the string of signal state.order of each phase need to be referred to an additional file
	// X bytes, string, string of signal state
	tempUint8 = TrafficLightData.state.size();
	memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);
	memcpy(buffer + *iByte, (char*)TrafficLightData.state.c_str(), tempUint8);
	*iByte = *iByte + tempUint8;

	// go back to the first byte location and parser message size 
	tempUint16 = (*iByte - initByte);
	memcpy(buffer + initByte, (char*)&tempUint16, sizeof(tempUint16));
}

void MsgHelper::depackTrafficLightData(char* buffer, TrafficLightData_t* TrafficLightData) {

	// here only depacking the core vehicle data part
	//		i.e. the initial message size and identifier are excluded 

	/*
	MESSAGE STRUCTURE:

	1 byte, uint8_t, length of the string of signal light name
	X bytes, string, string of signal light name (max 255 bytes)

	2 bytes, uint16_t, unique number of signal light

	1 byte, uint8_t, length of the string of signal state. order of each phase need to be referred to an additional file
	X bytes, string, string of signal state

	*/

	uint16_t tempUint16;
	//uint32_t tempUint32;
	//int tempInt;
	string tempString;
	uint8_t tempUint8;
	//float tempFloat;

	int iByte = 0;

	memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
	iByte += sizeof(uint8_t);
	tempString.assign(buffer + iByte, buffer + iByte + tempUint8);
	TrafficLightData->name = tempString;
	iByte += tempUint8;

	memcpy(&tempUint16, buffer + iByte, sizeof(uint16_t));
	TrafficLightData->id = tempUint16;
	iByte += sizeof(uint16_t);

	memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
	iByte += sizeof(uint8_t);
	tempString.assign(buffer + iByte, buffer + iByte + tempUint8);
	TrafficLightData->state = tempString;
	iByte += tempUint8;
}

void MsgHelper::packDetectorData(TlsDetector_t DetectorData, char* buffer, int* iByte) {
	/*
	MESSAGE STRUCTURE:

	2 bytes, uint16_t, Data length

	1 byte, uint8_t, data identifier

	1 byte, uint8_t, length of the string of signal light name
	X bytes, string, string of signal light name (max 255 bytes)

	2 bytes, uint16_t, unique number of signal light

	** NOW BEGIN STACK OF DETECTORS
	1 byte, uint8_t, length of the string of detector name.
	X bytes, string, string of detector name
	1 byte, uint8_t, unique number of detector
	1 byte, uint8_t, detector state

	1 byte, uint8_t, length of the string of detector name.
	X bytes, string, string of detector name
	1 byte, uint8_t, unique number of detector
	1 byte, uint8_t, detector state

	....

	*/

	uint8_t tempUint8;
	uint16_t tempUint16;

	int initByte = *iByte;

	// SKIP MESSAGE LENGTH UNTIL FILL THE REST OF BUFFER
	*iByte = *iByte + sizeof(uint16_t);

	// !! IDENTIFIER
	tempUint8 = 3;
	memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);

	// 1 byte, uint8_t, length of the string of signal light name
	// X bytes, string, string of signal light name(max 255 bytes)
	// 2 bytes, uint16_t, unique number of signal light
	tempUint8 = get<1>(DetectorData).size();
	memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
	*iByte = *iByte + sizeof(uint8_t);
	memcpy(buffer + *iByte, (char*)get<1>(DetectorData).c_str(), tempUint8);
	*iByte = *iByte + tempUint8;

	tempUint16 = get<0>(DetectorData);
	memcpy(buffer + *iByte, (char*)&tempUint16, sizeof(uint16_t));
	*iByte = *iByte + sizeof(uint16_t);

	size_t nDet = get<2>(DetectorData).size();
	for (unsigned int i = 0; i < nDet; i++) {
		// 1 byte, uint8_t, length of the string of detector name.
		// X bytes, string, string of detector name
		// 1 byte, uint8_t, unique number of detector
		// 1 byte, uint8_t, detector state
		DetectorData_t Det = get<2>(DetectorData)[i];

		tempUint8 = Det.name.size();
		memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
		*iByte = *iByte + sizeof(uint8_t);
		memcpy(buffer + *iByte, (char*)Det.name.c_str(), tempUint8);
		*iByte = *iByte + tempUint8;

		tempUint8 = Det.id;
		memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
		*iByte = *iByte + sizeof(uint8_t);

		tempUint8 = Det.state;
		memcpy(buffer + *iByte, (char*)&tempUint8, sizeof(uint8_t));
		*iByte = *iByte + sizeof(uint8_t);
	}

	// go back to the first byte location and parser message size 
	tempUint16 = (*iByte - initByte);
	memcpy(buffer + initByte, (char*)&tempUint16, sizeof(tempUint16));
}

void MsgHelper::depackDetectorData(char* buffer, int msgSize, TlsDetector_t* DetectorData) {

	// here only depacking the core vehicle data part
	//		i.e. the initial message size and identifier are excluded 

	/*
	MESSAGE STRUCTURE:

	[[[***2 bytes, uint16_t, Data length

	1 byte, uint8_t, data identifier
	]]]

	1 byte, uint8_t, length of the string of signal light name
	X bytes, string, string of signal light name (max 255 bytes)

	2 bytes, uint16_t, unique number of signal light

	** NOW BEGIN STACK OF DETECTORS
	1 byte, uint8_t, length of the string of detector name.
	X bytes, string, string of detector name
	1 byte, uint8_t, unique number of detector
	1 byte, uint8_t, detector state

	1 byte, uint8_t, length of the string of detector name.
	X bytes, string, string of detector name
	1 byte, uint8_t, unique number of detector
	1 byte, uint8_t, detector state

	....

	*/
	uint16_t tempUint16;
	//uint32_t tempUint32;
	//int tempInt;
	string tempString;
	uint8_t tempUint8;
	//float tempFloat;

	int iByte = 0;

	string signalName;
	int signalId;
	vector <DetectorData_t> DetAll_v;

	memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
	iByte += sizeof(uint8_t);
	tempString.assign(buffer + iByte, buffer + iByte + tempUint8);
	signalName = tempString;
	iByte += tempUint8;

	memcpy(&tempUint16, buffer + iByte, sizeof(uint16_t));
	signalId = tempUint16;
	iByte += sizeof(uint16_t);

	// already processed the signal information contained in the detector message. signal name length, signal name, signal id
	int msgProcessed = sizeof(uint8_t) + signalName.length() + sizeof(uint16_t);
	while (msgProcessed < msgSize) {

		DetectorData_t Det;

		memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
		iByte += sizeof(uint8_t);
		tempString.assign(buffer + iByte, buffer + iByte + tempUint8);
		Det.name = tempString;
		iByte += tempUint8;

		memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
		Det.id = tempUint8;
		iByte += sizeof(uint8_t);

		memcpy(&tempUint8, buffer + iByte, sizeof(uint8_t));
		Det.state = tempUint8;
		iByte += sizeof(uint8_t);

		DetAll_v.push_back(Det);

		msgProcessed += 3 + Det.name.length();
	}

	*DetectorData = make_tuple(signalId, signalName, DetAll_v);
}
