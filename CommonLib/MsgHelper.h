/*
This MsgHelper Class is dedicated to handling the messages. 
	-help with decode and encode message to buffer bytes
	-holds the necessary storage for each message types

*/

#pragma once

#include <unordered_map>
#include <unordered_set>
#include "VehDataMsgDefs.h"

#ifndef RS_DSPACE
#include "ConfigHelper.h"
#else
#include <stdio.h>
#endif

#include <tuple>

#include <string.h> // to have memcpy

class MsgHelper
{

public:
	MsgHelper();

	void packVehData(VehFullData_t VehFullData, char* buffer, int* iByte);
	void depackVehData(char* buffer, VehFullData_t& VehFullData);
	void packHeader(uint8_t simState, float simTime, uint32_t totalMsgSize, char* buffer, int* iByte);
	void depackHeader(char* buffer, uint8_t* simState, float* simTime, uint32_t* totalMsgSize);
	void depackMsgType(char* buffer, uint16_t* msgSize, uint8_t* msgType);
	
	void packString(std::string strData, char* buffer, int* iByte);
	void packTrafficLightData(TrafficLightData_t TrafficLightData, char* buffer, int* iByte);
	void depackTrafficLightData(char* buffer, TrafficLightData_t* TrafficLightData);
	void packDetectorData(TlsDetector_t DetectorData, char* buffer, int* iByte);
	void depackDetectorData(char* buffer, int msgSize, TlsDetector_t* DetectorData);

	void clearRecvStorage();
	void clearSendStorage();
	void clearStorage();

	std::unordered_map <std::string, VehFullData_t> VehDataRecv_um;
	std::unordered_map <std::string, TrafficLightData_t> TlsDataRecv_um;
	std::unordered_map <std::string, TlsDetector_t> DetDataRecv_um;


	// when receive, no need to distinguish where we get the data, so always combine the data
	// when send out, essentially separate all received information into different subsets according to subscription of each client/server


	// the size of these containers should be the same as the number of clients. essentially we could send different messages to different clients, these storages will be able to handle that. e.g. VehDataSend_v[0] contains a vector of vehicle data to be sent to the first client
	std::unordered_map <int, std::vector <VehFullData_t> > VehDataSend_um;
	std::unordered_map <int, std::vector <TlsDetector_t> > DetDataSend_um;
	std::unordered_map <int, std::vector <TrafficLightData_t> > TlsDataSend_um;


	int msgHeaderSize = 9;
	int msgEachHeaderSize = 3;

	// ===========================================================================
	// get configuration file
	// ===========================================================================
#ifndef RS_DSPACE
	void getConfig(ConfigHelper Config_c);
#endif
	std::vector <std::string> VehicleMessageField_v;
	std::unordered_set <std::string> VehicleMessageField_set;

	void printVehData(VehFullData_t VehData);
	void printVehDataToFile(const std::string fileName, VehFullData_t VehData);

private:
	void stringVehDataToBuffer(std::string vehDataString, std::string VehDataName, char* buffer, int* iByte);
	void bufferToStringVehData(char* buffer, int* iByte, std::string VehDataName, std::string& tempString);

	template <typename T> void numericVehDataToBuffer(T vehDataField, std::string VehDataName, char* buffer, int* iByte);
	template <typename T> void bufferToNumericVehData(char* buffer, int* iByte, std::string VehDataName, T& tempVal);

};