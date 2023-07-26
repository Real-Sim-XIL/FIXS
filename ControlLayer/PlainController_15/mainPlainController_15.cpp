#include <iostream>
#include <fstream>
#include <unordered_map>

#include <event2/bufferevent.h>
#include <event2/util.h>
#include <event2/event.h>

#include "SocketHelper.h"

/*
TO DO

	- [done] need to provide a way to easily test the connection to dSPACE

	- [done] when recv multiple server, should has a byte to define what type of message it is, then parser accordingly
	- everytime after getting from all servers, should combine data to get unique informaiton about traffic and signal at this moment
	- after combined data, should direct message to each client, which may request different ego vehicle commands
	- then wait for getting actual from all clients, then combine the data
	- !! clients should also send message according to this format !! current format is basically the defined structure for traffic data
	- the client data need to be combined with other vehicle commands calculate from the controller. then send back to server
	- currently everything is blocking, for communication, the recv() call should be more like a event? potentially do a event based call
	- [done] way to enable no client, basically just controller with SUMO

	- what if no ego vehicle yet
*/


//#include "CentralCtrl.h"

//#define MAXPENDING 5    /* Maximum outstanding connection requests */
//#define RECVCLIENTBUFSIZE 2048 
//#define SENDCLIENTBUFSIZE 8096

#define MAXSENDVEH 100

//#define NCLIENT 1
//#define NSERVER 1

#define CZ_MAIN_LINK_ID "1"
#define CZ_RAMP_LINK_ID "2"

using namespace std;

bool ENABLE_CAV_APP = 1;
bool ENABLE_TCP = 1;


///********************************************
//* DEFINE USER CASE
//*********************************************/
const vector <string> serverAddr = { "127.0.0.1" };
const vector <int> serverPort = { 2347 };

const vector <unsigned short> selfServerPortUserInput = { 434 }; // client connect to this port
const vector <string> egoIdVec = { "flow_1.16" };
//const vector <unsigned short> selfServerPortUserInput = { 421 }; // client connect to this port
//const vector <string> egoIdVec = { "flow_1.9" };
//const vector <unsigned short> selfServerPortUserInput = { 420, 421 }; // client connect to this port
//const vector <string> egoIdVec = { "flow_0.10", 
//								"flow_1.9" };
//const vector <unsigned short> selfServerPortUserInput = { 420, 421 }; // client connect to this port
//const vector <string> egoIdVec = { "vehicle_0", 
//								"vehicle_1" };

//const vector <unsigned short> selfServerPortUserInput = { 420, 421, 422, 423, 424, 425, 426, 427, 428, 429 }; // client connect to this port
//const vector <string> egoIdVec = { "flow_0.10",
//								"flow_1.9",
//								"flow_1.8",
//								"flow_1.7",
//								"flow_1.10",
//								"flow_1.11",
//								"flow_0.9",
//								"flow_0.8",
//								"flow_0.11",
//								"flow_0.12" };

//const vector <unsigned short> selfServerPortUserInput = { 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439 }; // client connect to this port
//const vector <string> egoIdVec = { "flow_0.10",
//								"flow_1.09",
//								"flow_1.08",
//								"flow_1.07",
//								"flow_1.10",
//								"flow_1.11",
//								"flow_0.09",
//								"flow_0.08",
//								"flow_0.11",
//								"flow_0.12",
//								"flow_1.12",
//								"flow_1.13",
//								"flow_1.14",
//								"flow_1.15",
//								"flow_1.16",
//								"flow_0.13",
//								"flow_0.14",
//								"flow_0.15",
//								"flow_0.16",
//								"flow_0.17",
//								};

bool ENABLE_VERBOSE = 0;

double t_des = 0.1;

int main(int argc, char* argv[]) {
	//std::ios::sync_with_stdio(false);

	LARGE_INTEGER DebugStartingTime, DebugEndingTime, DebugElapsedMicroseconds;
	LARGE_INTEGER DebugFrequency;

	QueryPerformanceFrequency(&DebugFrequency);


	SocketHelper Sock_c(serverAddr, serverPort, selfServerPortUserInput);
	Sock_c.disableWaitClientTrigger();
	Sock_c.disableServerTrigger();
	//Sock_c.disableClient();
	//Sock_c.disableServer();
	Sock_c.initConnection();

	int recvServerMsgSize;
	int sendServerMsgSize;

	int recvClientMsgSize;
	int sendClientMsgSize;

	int simState = 0;
	float simTime = 0;

	unordered_map <string, VehFullData_t> VehDataLastStep_v;
	unordered_map <string, VehFullData_t> VehDataNextStep_v;
	vector <string> recvVehId_v;

	while (1) {

		/*==============================
		* 1) recv traffic from Server
		==============================*/
		VehDataLastStep_v.clear();
		recvVehId_v.clear();

		if (Sock_c.ENABLE_SERVER) {
			for (unsigned int iS = 0; iS < Sock_c.NSERVER; iS++) {
				unordered_map <string, TrafficLightData_t> TlsLastStep_um;
				unordered_map <string, DetectorData_t> DetLastStep_um;
				vector <string> recvTlsId_v;
				vector <string> recvDetId_v;

				char recvBuffer[200];

				int recvSize;

				Sock_c.recvData(Sock_c.serverSock[iS], &simState, &simTime, VehDataLastStep_v, recvVehId_v, TlsLastStep_um, recvTlsId_v, DetLastStep_um, recvDetId_v);

				if (ENABLE_VERBOSE) {
					printf("recv SERVER %d vehicles \n", recvVehId_v.size());
				}
			}
		}
		else {
			VehFullData_t iVehFullData;
			iVehFullData.id = "None";
			iVehFullData.type = "None";
			iVehFullData.speed = 13;
			iVehFullData.positionX = 5.1;
			iVehFullData.positionY = 1.2;
			iVehFullData.positionZ = 0;
			iVehFullData.heading = 0;
			iVehFullData.color = 0;
			iVehFullData.linkId = "None";
			iVehFullData.laneId = 0;
			iVehFullData.distanceTravel = 0;

			VehDataLastStep_v[iVehFullData.id] = iVehFullData;
		}

		/*==============================
		* 2) obtain commands for target vehicles
		==============================*/
		//int nTargetVeh = recvVehId_v.size();
		//std::vector <std::string> TargetVehId_v;
		//for (int iV = 0; iV < nTargetVeh; iV++) {
		//	TargetVehId_v.push_back(recvVehId_v[iV]);
		//}

		int nTargetVeh = egoIdVec.size();
		vector <string> TargetVehId_v;
		for (int iV = 0; iV < nTargetVeh; iV++) {
			TargetVehId_v.push_back(egoIdVec[iV]);
		}

		//+++++++++
		// 3) Reparser vehicle message and then send out traffic states and ego commands to client
		//+++++++++
		// !! NOTE: order of vehicle data is different for each client. the ego vehicle of each client is at the first location
		// !!	SEND ALL TRAFFIC INFORMATION 
		if (Sock_c.ENABLE_CLIENT) {

			for (unsigned int iC = 0; iC < Sock_c.NCLIENT; iC++) {

				if (!Sock_c.ENABLE_SERVER) {
					simState = 1;
					simTime = simTime + 0.1;
				}

				int iByte = 0;
				float simTimeSend = simTime;
				char sendClientBuffer[SENDCLIENTBUFSIZE];

				// !! WE don't know the message size yet, pack a dummy header
				Sock_c.packHeader(simState, simTimeSend, MSG_HEADER_SIZE, sendClientBuffer, &iByte);
				Sock_c.sendClientByte[iC] = { MSG_HEADER_SIZE };

				if (VehDataLastStep_v.find(egoIdVec[iC]) != VehDataLastStep_v.end()) {
					Sock_c.packVehFullData(VehDataLastStep_v[egoIdVec[iC]], sendClientBuffer, &Sock_c.sendClientByte[iC]);
				}
				//for (int iV = 0; iV < recvVehId_v.size(); iV++) {
				//	if (recvVehId_v[iV] != egoIdVec[iC]) {
				//		Sock_c.packVehFullData(VehDataLastStep_v[recvVehId_v[iV]], sendClientBuffer, &Sock_c.sendClientByte[iC]);
				//	}
				//}

				// repack the header with the correct size
				iByte = 0;
				Sock_c.packHeader(simState, simTime, Sock_c.sendClientByte[iC], sendClientBuffer, &iByte);

				if (send(Sock_c.clientSock[iC], sendClientBuffer, Sock_c.sendClientByte[iC], 0) != Sock_c.sendClientByte[iC]) {
					fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
					exit(1);
				}

				if (ENABLE_VERBOSE) {
					printf("send CLIENT %d bytes\n", Sock_c.sendClientByte[iC]);
				}

			}
		}

		/*==============================
		* 4) wait and recv for next ego states to come in
		==============================*/
		//+++++++++
		// Group target vehicle speeds send to Server
		//+++++++++

		VehDataNextStep_v.clear();

		for (int iV = 0; iV < TargetVehId_v.size(); iV++) {
			if (VehDataLastStep_v.find(egoIdVec[iV]) != VehDataLastStep_v.end()) {
				VehDataNextStep_v[TargetVehId_v[iV]] = VehDataLastStep_v[TargetVehId_v[iV]];
			}
		}

		if (Sock_c.ENABLE_CLIENT) {
			for (unsigned int iC = 0; iC < Sock_c.NCLIENT; iC++) {
				unordered_map <string, VehFullData_t> VehNext_um;
				unordered_map <string, TrafficLightData_t> TlsNext_um;
				unordered_map <string, DetectorData_t> DetNext_um;
				vector <string> nextVehId_v;
				vector <string> nextTlsId_v;
				vector <string> nextDetId_v;

				char recvBuffer[200];

				int recvSize;

				Sock_c.recvData(Sock_c.clientSock[iC], &simState, &simTime, VehNext_um, nextVehId_v, TlsNext_um, nextTlsId_v, DetNext_um, nextDetId_v);

				//+++++++++
				// replace VehData from server for those IDs in the client msg.
				//+++++++++	
				if (VehDataLastStep_v.find(egoIdVec[iC]) != VehDataLastStep_v.end()) {
					if (VehNext_um.find(egoIdVec[iC]) != VehNext_um.end()) {
						VehDataNextStep_v[egoIdVec[iC]].speed = VehNext_um[egoIdVec[iC]].speed;
					}
				}
			}
		}


		/*==============================
		* 5) forward next ego states and traffic commands to Server
		==============================*/
		if (Sock_c.ENABLE_SERVER) {
			// pack header
			for (unsigned int iS = 0; iS < Sock_c.NSERVER; iS++) {
				uint8_t simStateSend = simState;

				int iByte = 0;

				char sendServerBuffer[SENDSERVERBUFSIZE];
				// !! WE don't know the message size yet, pack a dummy header
				Sock_c.packHeader(simStateSend, simTime, MSG_HEADER_SIZE, sendServerBuffer, &iByte);
				Sock_c.sendServerByte[iS] = { MSG_HEADER_SIZE };

				for (int iV = 0; iV < nTargetVeh; iV++) {
					if (VehDataLastStep_v.find(egoIdVec[iV]) != VehDataLastStep_v.end()) {
						Sock_c.packVehFullData(VehDataNextStep_v[TargetVehId_v[iV]], sendServerBuffer, &Sock_c.sendServerByte[iS]);
					}
				}
				// repack the header with the correct size
				iByte = 0;
				Sock_c.packHeader(simState, simTime, Sock_c.sendServerByte[iS], sendServerBuffer, &iByte);

				if (send(Sock_c.serverSock[iS], sendServerBuffer, Sock_c.sendServerByte[iS], 0) != Sock_c.sendServerByte[iS]) {
					fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
					exit(1);
				}

				if (ENABLE_VERBOSE) {
					printf("send SERVER %d bytes\n", Sock_c.sendServerByte[iS]);
				}

			}
		}

	}
}