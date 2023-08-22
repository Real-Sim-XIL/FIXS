#ifndef DEVICE_V2X_H
#define DEVICE_V2X_H

#include "VehDataMsgDefs.h"
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <cmath>
#include <cstdio>
#include <fstream>

/*This will be the class to represent each V2X communication device.*/

using namespace std;

class DeviceV2X
{
public:
	DeviceV2X();
	DeviceV2X(string typeInput, int idInput);

	/*simulate the communication, considering 1) communication packet drop 2) communication delay*/
	//bool isCommSuccess(RDB_COORD_t pos1, RDB_COORD_t pos2);
	void simulateComm(COORD_t pos1, COORD_t pos2, bool* IS_COMM_SUCCESS, double* commLatency);

	// based on current simTime, retrieve the vehicle data 
	void getCurrentBsm(double simTime, VehData_t& currentBsm);
	
	// store vehicle data
	void pushBsmBuf(double simTime, VehData_t VehData);
	///*A V2X device will be able to broadcast information from all following "devices"*/

	// buffer: <time vehicle data should be sent out, vehicle data>
	multimap <double, VehData_t> bsmBuf; 


	string alias;
	string type; // RSU or OBU
	int id;

	COORD_t pos; // position of the comm device

private:



};

#endif