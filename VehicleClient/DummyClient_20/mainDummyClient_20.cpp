#include <iostream>
#include <fstream>
#include <unordered_map>
#include "SocketHelper.h"


using namespace std;

///********************************************
//* DEFINE USER CASE
//*********************************************/
//const int NSERVER = 1;
//const int NCLIENT = 0;
//const bool ENABLE_SERVER = 1;
//const bool ENABLE_CLIENT = 0;
//const vector <unsigned short> selfServerPortUserInput = { 420 };
//const bool ENABLE_WAIT_CLIENT_TRIGGER = 0;

const vector <string> serverAddr = { "127.0.0.1" };
const vector <int> serverPort = { 439 };


typedef struct Timer_t {
	LARGE_INTEGER StartingTime;
	LARGE_INTEGER EndingTime;
	LARGE_INTEGER ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
};


vector <string> egoIdVec = { "flow_0.17" };
//const unordered_map<string, double> egoSpeedInitMap = {
//								{egoIdVec[0], 13.0},
//								{egoIdVec[1], 14.0},
//};
const unordered_map<string, double> egoSpeedInitMap = {
									{egoIdVec[0], 13.0}
};


bool ENABLE_TIMER = 1;

string debugLogName = "../../MatlabCodes/timing_20.txt";


int main(int argc, char* argv[]) {
	printf("this is client %s\n", debugLogName.c_str());

	Timer_t TimerDelay = {0};
	QueryPerformanceFrequency(&TimerDelay.Frequency);

	//SocketHelper Sock_c(NSERVER, NCLIENT, ENABLE_SERVER, ENABLE_CLIENT, selfServerPortUserInput, ENABLE_WAIT_CLIENT_TRIGGER);
	//Sock_c.initConnection(serverAddr, serverPort);

	SocketHelper Sock_c(serverAddr, serverPort);
	Sock_c.disableServerTrigger();
	Sock_c.initConnection();


	ifstream f(debugLogName.c_str());
	if (f.good()) {
		f.close();
		remove(debugLogName.c_str());
	}

	while (1) {
		unordered_map <string, VehFullData_t> VehCmdRecv_um;
		unordered_map <string, TrafficLightData_t> TlsCmdRecv_um;
		unordered_map <string, DetectorData_t> DetCmdRecv_um;

		vector <string> recvVehId_v;
		vector <string> recvTlsId_v;
		vector <string> recvDetId_v;
		int simStateRecv;
		float simTimeRecv;

		int iS = 0;
		Sock_c.recvData(Sock_c.serverSock[iS], &simStateRecv, &simTimeRecv, VehCmdRecv_um, recvVehId_v, TlsCmdRecv_um, recvTlsId_v, DetCmdRecv_um, recvDetId_v);
		
		if (ENABLE_TIMER) {
			QueryPerformanceCounter(&TimerDelay.EndingTime);
			TimerDelay.ElapsedMicroseconds.QuadPart = TimerDelay.EndingTime.QuadPart - TimerDelay.StartingTime.QuadPart;
			TimerDelay.ElapsedMicroseconds.QuadPart *= 1000000;
			TimerDelay.ElapsedMicroseconds.QuadPart /= TimerDelay.Frequency.QuadPart;

			//cout << "send to receive time: " << TimerDelay.ElapsedMicroseconds.QuadPart / 1000000.0 << " seconds " << endl;
			printf("send to receive time: %f seconds\n", TimerDelay.ElapsedMicroseconds.QuadPart / 1000000.0);


			fstream debugLog;
			debugLog.open(debugLogName.c_str(), fstream::in | fstream::out | fstream::app);
			debugLog << TimerDelay.ElapsedMicroseconds.QuadPart / 1000000.0 << endl;
			debugLog.close();
		}

		double egoSpeed;
		VehFullData_t VehDataSend;
		// there's ego vehicle in the data received
		if (VehCmdRecv_um.find(egoIdVec[0]) != VehCmdRecv_um.end()) {
			egoSpeed = VehCmdRecv_um[egoIdVec[0]].speed;
			VehDataSend = VehCmdRecv_um[egoIdVec[0]];
		}
		else {
			egoSpeed = egoSpeedInitMap.at(egoIdVec[0]);
			VehDataSend.id = "None";
			VehDataSend.type = "None";
			VehDataSend.speed = egoSpeed;
			VehDataSend.positionX = 0;
			VehDataSend.positionY = 0;
			VehDataSend.positionZ = 0;
			VehDataSend.heading = 0;
			VehDataSend.color = 0;
			VehDataSend.linkId = "None";
			VehDataSend.laneId = 0;
			VehDataSend.distanceTravel = 0;
		}

		//uint16_t speedSend = round(egoSpeed * 1000.0); // 0-65.535 m/s;
		//char sendBuffer[256];
		//memcpy(sendBuffer, (char*)&speedSend, sizeof(uint16_t));
		//

		//if (ENABLE_TIMER) {
		//	QueryPerformanceCounter(&TimerDelay.StartingTime);
		//}

		//if (send(Sock_c.serverSock[iS], sendBuffer, sizeof(uint16_t), 0) != sizeof(uint16_t)) {
		//	fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
		//	exit(1);
		//}

		uint8_t simStateSend = simStateRecv;

		int iByte = 0;

		char sendServerBuffer[SENDSERVERBUFSIZE];
		// !! WE don't know the message size yet, pack a dummy header
		Sock_c.packHeader(simStateSend, simTimeRecv, MSG_HEADER_SIZE, sendServerBuffer, &iByte);
		Sock_c.sendServerByte[iS] = { MSG_HEADER_SIZE };

		Sock_c.packVehFullData(VehDataSend, sendServerBuffer, &Sock_c.sendServerByte[iS]);

		// repack the header with the correct size
		iByte = 0;
		Sock_c.packHeader(simStateSend, simTimeRecv, Sock_c.sendServerByte[iS], sendServerBuffer, &iByte);

		if (ENABLE_TIMER) {
			QueryPerformanceCounter(&TimerDelay.StartingTime);
		}

		if (send(Sock_c.serverSock[iS], sendServerBuffer, Sock_c.sendServerByte[iS], 0) != Sock_c.sendServerByte[iS]) {
			fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
			exit(1);
		}
	}
	

}