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
const vector <int> serverPort = { 2333 };


typedef struct Timer_t {
	LARGE_INTEGER StartingTime;
	LARGE_INTEGER EndingTime;
	LARGE_INTEGER ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
};


//vector <string> egoIdVec = { "vehicle_0" };
//const unordered_map<string, double> egoSpeedInitMap = {

vector <string> egoIdVec = { "3" };
//const unordered_map<string, double> egoSpeedInitMap = {
//								{egoIdVec[0], 13.0},
//								{egoIdVec[1], 14.0},
//};
const unordered_map<string, double> egoSpeedInitMap = {
									{egoIdVec[0], 13.4}
};


bool ENABLE_TIMER = 0;

string debugLogName = "../../CommonLib/debug.txt";

int main(int argc, char* argv[]) {
	printf("this is client %s\n", debugLogName.c_str());

	Timer_t TimerDelay = { 0 };
	QueryPerformanceFrequency(&TimerDelay.Frequency);

	//SocketHelper Sock_c(NSERVER, NCLIENT, ENABLE_SERVER, ENABLE_CLIENT, selfServerPortUserInput, ENABLE_WAIT_CLIENT_TRIGGER);
	//Sock_c.initConnection(serverAddr, serverPort);

	SocketHelper Sock_c;

	Sock_c.socketSetup(serverAddr, serverPort);
	Sock_c.disableServerTrigger();
	Sock_c.initConnection();

	ConfigHelper Config_c;

	if (Config_c.getConfig("..\\..\\..\\RealSimReleaseToyota\\config.yaml") < 0) {
		return -1;
	}


	MsgHelper Msg_c;
	Msg_c.getConfig(Config_c);


	ifstream f(debugLogName.c_str());
	if (f.good()) {
		f.close();
		remove(debugLogName.c_str());
	}

	while (1) {
		//unordered_map <string, VehFullData_t> VehCmdRecv_um;
		//unordered_map <string, TrafficLightData_t> TlsCmdRecv_um;
		//unordered_map <string, DetectorData_t> DetCmdRecv_um;

		//vector <string> recvVehId_v;
		//vector <string> recvTlsId_v;
		//vector <string> recvDetId_v;
		int simStateRecv;
		float simTimeRecv;

		Msg_c.clearRecvStorage();

		int iS = 0;
		Sock_c.recvData(Sock_c.serverSock[iS], &simStateRecv, &simTimeRecv, Msg_c);

		if (ENABLE_TIMER) {
			QueryPerformanceCounter(&TimerDelay.EndingTime);
			TimerDelay.ElapsedMicroseconds.QuadPart = TimerDelay.EndingTime.QuadPart - TimerDelay.StartingTime.QuadPart;
			TimerDelay.ElapsedMicroseconds.QuadPart *= 1000000;
			TimerDelay.ElapsedMicroseconds.QuadPart /= TimerDelay.Frequency.QuadPart;

			//cout << "send to receive time: " << TimerDelay.ElapsedMicroseconds.QuadPart / 1000000.0 << " seconds " << endl;
			printf("send to receive time: %f seconds\n", TimerDelay.ElapsedMicroseconds.QuadPart / 1000000.0);


			//fstream debugLog;
			//debugLog.open(debugLogName.c_str(), fstream::in | fstream::out | fstream::app);
			//debugLog << TimerDelay.ElapsedMicroseconds.QuadPart / 1000000.0 << endl;
			//debugLog.close();
		}

		double egoSpeed;
		VehData_t VehDataSend;
		// there's ego vehicle in the data received
		if (Msg_c.VehDataRecv_um.find(egoIdVec[0]) != Msg_c.VehDataRecv_um.end()) {
			egoSpeed = std::any_cast<float>(Msg_c.VehDataRecv_um[egoIdVec[0]]["speed"]);
			float egoSpeedDes = std::any_cast<float>(Msg_c.VehDataRecv_um[egoIdVec[0]]["speedDesired"]);
			VehDataSend = Msg_c.VehDataRecv_um[egoIdVec[0]];

			//float speedDes = std::any_cast<float>(Msg_c.VehDataRecv_um[egoIdVec[0]]["speed"]);
			////float speedmod = (float)(13 + 2 * sin(simTimeRecv));
			//float speedmod = speedDes + 0 * (float)(((float)rand() / (RAND_MAX)) - 0.5) / 10;
			//VehDataSend["speedDesired"] = (float) egoSpeed;

			//fstream debugLog;
			//debugLog.open(debugLogName.c_str(), fstream::in | fstream::out | fstream::app);
			//debugLog << speedmod << endl;

			printf("speed %f, speedDes %f\n", egoSpeed, egoSpeedDes);

		}
		else {
			egoSpeed = egoSpeedInitMap.at(egoIdVec[0]);
			if (Msg_c.VehicleMessageField_set.find("id") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["id"] = (string)"None";
			}
			if (Msg_c.VehicleMessageField_set.find("type") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["type"] = (string)"None";
			}
			if (Msg_c.VehicleMessageField_set.find("speed") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["speed"] = (float)egoSpeed;
			}
			if (Msg_c.VehicleMessageField_set.find("acceleration") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["acceleration"] = (float)0;
			}
			if (Msg_c.VehicleMessageField_set.find("positionX") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["positionX"] = (float)0;
			}
			if (Msg_c.VehicleMessageField_set.find("positionY") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["positionY"] = (float)0;
			}
			if (Msg_c.VehicleMessageField_set.find("positionZ") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["positionZ"] = (float)0;
			}
			if (Msg_c.VehicleMessageField_set.find("heading") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["heading"] = (float)0;
			}
			if (Msg_c.VehicleMessageField_set.find("color") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["color"] = (uint32_t)0;
			}
			if (Msg_c.VehicleMessageField_set.find("linkId") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["linkId"] = (string)"None";
			}
			if (Msg_c.VehicleMessageField_set.find("laneId") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["laneId"] = (int32_t)0;
			}
			if (Msg_c.VehicleMessageField_set.find("distanceTravel") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["distanceTravel"] = (float)0;
			}
			if (Msg_c.VehicleMessageField_set.find("speedDesired") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["speedDesired"] = (float)egoSpeed;
			}
			if (Msg_c.VehicleMessageField_set.find("accelerationDesired") != Msg_c.VehicleMessageField_set.end()) {
				VehDataSend["accelerationDesired"] = (float)0;
			}
		}
		//VehDataSend.speed = 10;

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

		char sendServerBuffer[8096];
		// !! WE don't know the message size yet, pack a dummy header
		Msg_c.packHeader(simStateSend, simTimeRecv, MSG_HEADER_SIZE, sendServerBuffer, &iByte);
		Sock_c.sendServerByte[iS] = { MSG_HEADER_SIZE };

		if (Msg_c.VehDataRecv_um.find(egoIdVec[0]) != Msg_c.VehDataRecv_um.end()) {
			Msg_c.packVehData(VehDataSend, sendServerBuffer, &Sock_c.sendServerByte[iS]);
		}

		// repack the header with the correct size
		iByte = 0;
		Msg_c.packHeader(simStateSend, simTimeRecv, Sock_c.sendServerByte[iS], sendServerBuffer, &iByte);

		if (ENABLE_TIMER) {
			QueryPerformanceCounter(&TimerDelay.StartingTime);
		}

		if (send(Sock_c.serverSock[iS], sendServerBuffer, Sock_c.sendServerByte[iS], 0) != Sock_c.sendServerByte[iS]) {
			fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
			exit(1);
		}
	}


}