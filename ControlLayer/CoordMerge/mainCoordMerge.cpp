#include <iostream>
#include <fstream>
#include <unordered_map>

#include <event2/bufferevent.h>
#include <event2/util.h>
#include <event2/event.h>

#include "SocketHelper.h"
#include "ConfigHelper.h"
#include "CentralCtrl.h"

#include <ctime>
#include <chrono>

using namespace std;

#define SENDSERVERBUFSIZE 8192

ConfigHelper Config_c;

const string MasterLogName = "CoordMerge.log";

static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << endl
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-f,--file PATH\\FILENAME\tSpecify the path and filename of configuration yaml file"
		<< "\t-c,--control PATH\\FILENAME\tSpecify the path and filename of control zone setup txt file"
		<< std::endl;
}


int main(int argc, char* argv[]) {

	// reset logs
	if (1) {
		ifstream f(MasterLogName.c_str());
		if (f.good()) {
			f.close();
			remove(MasterLogName.c_str());
		}
	}

	// get start time
	auto simStartTimestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
	char simStartTimestampChar[100];
	ctime_s(simStartTimestampChar, sizeof simStartTimestampChar, &simStartTimestamp);

	// read config.yaml
	string configPath = "..\\..\\config.yaml";
	string czFilename = "ControlZoneDefault.txt";

	for (int i = 1; i < argc; i++) {
		string arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			show_usage(argv[0]);
			return 0;
		}
		else if (arg == "-f" || arg == "--file") {
			if (i + 1 < argc) {
				configPath = argv[++i];
			}
			else {
				std::cerr << "--file option requires one argument." << std::endl;
				return -1;
			}
		}
		else if (arg == "-c" || arg == "--control") {
			if (i + 1 < argc) {
				czFilename = argv[++i];
			}
			else {
				std::cerr << "--control option requires one argument." << std::endl;
				return -1;
			}
		}
		else {
			printf("Check options\n");
			show_usage(argv[0]);
			return 0;
		}
	}

	if (Config_c.getConfig(configPath) < 0) {
		return -1;
	}

	bool ENABLE_VERBOSE = false;
	bool ENABLE_XIL = false;
	bool ENABLE_VISSIM = false;
	bool XIL_AS_SERVER = false;

	VehFullData_t IpgVehData;
	IpgVehData.id = (string)"nan";
	IpgVehData.type = (string)"nan";
	IpgVehData.speed = (float)0.0;
	IpgVehData.positionX = (float)0.0;
	IpgVehData.positionY = (float)0.0;
	IpgVehData.positionZ = (float)0.0;
	IpgVehData.distanceTravel = (float)0.0;

	vector <string> serverAddr = {};
	vector <int> serverPort = {};
	vector <int> selfServerPortUserInput = {}; // client connect to this port
	vector <string> egoIdVec = {};

	vector <string> serverAddrAll = {};
	vector <int> serverPortAll = {};

	vector <string> selfServerAddrAll = {};
	vector <int> selfServerPortAll = {};
	unordered_map <int, int> selfServerPort2Sock_um;


	if (Config_c.SimulationSetup.EnableVerboseLog) {
		ENABLE_VERBOSE = true;
	}
	else {
		ENABLE_VERBOSE = false;
	}

	for (int i = 0; i < Config_c.ApplicationSetup.VehicleSubscription.size(); i++) {
		vector <int> port_v;
		vector <string> ip_v;
		ip_v = get<2>(Config_c.ApplicationSetup.VehicleSubscription[i]);
		port_v = get<3>(Config_c.ApplicationSetup.VehicleSubscription[i]);
		for (int iP = 0; iP < ip_v.size(); iP++) {
			serverAddrAll.push_back(ip_v[iP]);
		}
		for (int iP = 0; iP < port_v.size(); iP++) {
			serverPortAll.push_back(port_v[iP]);
		}
	}
	for (int i = 0; i < Config_c.ApplicationSetup.DetectorSubscription.size(); i++) {
		vector <int> port_v;
		vector <string> ip_v;
		ip_v = get<2>(Config_c.ApplicationSetup.DetectorSubscription[i]);
		port_v = get<3>(Config_c.ApplicationSetup.DetectorSubscription[i]);
		for (int iP = 0; iP < ip_v.size(); iP++) {
			serverAddrAll.push_back(ip_v[iP]);
		}
		for (int iP = 0; iP < port_v.size(); iP++) {
			serverPortAll.push_back(port_v[iP]);
		}
	}

	if (serverPortAll.size() != serverAddrAll.size()) {
		printf("Error, ApplicationLayer number of ip should equal to number of port!\n");
		return -1;
	}

	serverPort = serverPortAll;
	auto it = unique(serverPort.begin(), serverPort.end());
	serverPort.resize(distance(serverPort.begin(), it));

	serverAddr = { serverAddrAll[0] };
	//serverAddr.resize(distance(serverPort.begin(), it));

	if (Config_c.XilSetup.EnableXil) {
		ENABLE_XIL = true;
	}
	else {
		ENABLE_XIL = false;
	}

	if (Config_c.XilSetup.AsServer) {
		XIL_AS_SERVER = true;
	}
	else {
		XIL_AS_SERVER = false;
	}

	if (Config_c.SimulationSetup.SelectedTrafficSimulator.compare("VISSIM") == 0) {
		ENABLE_VISSIM = true;
		printf("Selected Traffic Simulator VISSIM\n");
	}
	else {
		ENABLE_VISSIM = false;
		printf("Selected Traffic Simulator SUMO\n");
	}

	for (int i = 0; i < Config_c.XilSetup.VehicleSubscription.size(); i++) {
		vector <int> port_v;
		vector <string> ip_v;
		ip_v = get<2>(Config_c.XilSetup.VehicleSubscription[i]);
		port_v = get<3>(Config_c.XilSetup.VehicleSubscription[i]);
		for (int iP = 0; iP < ip_v.size(); iP++) {
			selfServerAddrAll.push_back(ip_v[iP]);
		}
		for (int iP = 0; iP < port_v.size(); iP++) {
			selfServerPortAll.push_back(port_v[iP]);
		}

		string type;
		type = get<0>(Config_c.XilSetup.VehicleSubscription[i]);
		if (type.compare("ego") == 0) {
			SubAttMap_t att = get<1>(Config_c.XilSetup.VehicleSubscription[i]);
			if (att.find("id") != att.end()) {
				vector <string> idlist = att["id"];
				for (int iId = 0; iId < idlist.size(); iId++) {
					egoIdVec.push_back(idlist[iId]);
				}

			}
		}

	}

	for (int i = 0; i < egoIdVec.size(); i++) {
		printf("set ego vehicle %s\n", egoIdVec[i].c_str());
	}

	for (int i = 0; i < Config_c.XilSetup.SignalSubscription.size(); i++) {
		vector <int> port_v;
		vector <string> ip_v;
		ip_v = get<2>(Config_c.XilSetup.SignalSubscription[i]);
		port_v = get<3>(Config_c.XilSetup.SignalSubscription[i]);
		for (int iP = 0; iP < ip_v.size(); iP++) {
			selfServerAddrAll.push_back(ip_v[iP]);
		}
		for (int iP = 0; iP < port_v.size(); iP++) {
			selfServerPortAll.push_back(port_v[iP]);
		}
	}
	for (int i = 0; i < Config_c.XilSetup.DetectorSubscription.size(); i++) {
		vector <int> port_v;
		vector <string> ip_v;
		ip_v = get<2>(Config_c.XilSetup.DetectorSubscription[i]);
		port_v = get<3>(Config_c.XilSetup.DetectorSubscription[i]);
		for (int iP = 0; iP < ip_v.size(); iP++) {
			selfServerAddrAll.push_back(ip_v[iP]);
		}
		for (int iP = 0; iP < port_v.size(); iP++) {
			selfServerPortAll.push_back(port_v[iP]);
		}
	}
	// number of ports equal to size of Sock_c.clientSock
	// map each port to an element of Sock_c.clientSock
	selfServerPortUserInput = selfServerPortAll;
	auto it2 = unique(selfServerPortUserInput.begin(), selfServerPortUserInput.end());
	selfServerPortUserInput.resize(distance(selfServerPortUserInput.begin(), it2));
	for (int i = 0; i < selfServerPortUserInput.size(); i++) {
		int port = selfServerPortUserInput[i];
		// if not exist
		if (selfServerPort2Sock_um.find(port) == selfServerPort2Sock_um.end()) {
			selfServerPort2Sock_um[port] = selfServerPort2Sock_um.size();
		}
	}

	//std::ios::sync_with_stdio(false);

	LARGE_INTEGER DebugStartingTime, DebugEndingTime, DebugElapsedMicroseconds;
	LARGE_INTEGER DebugFrequency;

	QueryPerformanceFrequency(&DebugFrequency);

	if (XIL_AS_SERVER) {
		// sanity check
		if (selfServerPortAll.size() > 1) {
			printf("Error, currently only support one Xil server! \n");
			return -1;
		}

		serverAddr.push_back(selfServerAddrAll[0]);
		serverPort.push_back(selfServerPortAll[0]);
	}

	SocketHelper Sock_c;
	if (XIL_AS_SERVER) {
		Sock_c.socketSetup(serverAddr, serverPort);
		Sock_c.N_ACT_CLIENT = 0;

	}
	else {
		Sock_c.socketSetup(serverAddr, serverPort, selfServerPortUserInput);
		Sock_c.N_ACT_CLIENT = selfServerPortUserInput.size();  
	}
	Sock_c.disableWaitClientTrigger(); 
	if (ENABLE_VISSIM) {
		Sock_c.disableServerTrigger();
	}
	else {
		Sock_c.disableServerTrigger();
	}
	if (!ENABLE_XIL) {
		Sock_c.disableClient();
	}
	//Sock_c.disableServer();
	Sock_c.initConnection();

	if (XIL_AS_SERVER) {
		//!!!!
		// bypassing....
		//!!!!
		Sock_c.ENABLE_CLIENT = 1;
		Sock_c.NCLIENT = 1;
		Sock_c.NSERVER = 1;

		Sock_c.clientSock.push_back(Sock_c.serverSock[1]);

		Sock_c.ClientConnected.push_back(1);

		Sock_c.selfServerSock.push_back(0);

		Sock_c.sendClientByte.push_back(0);
		Sock_c.recvClientMsgSize.push_back(0);
		Sock_c.ClientRequested.push_back(1);

		//!!!!
		// bypassing....
		//!!!!

	}
	int recvServerMsgSize;
	int sendServerMsgSize;

	int recvClientMsgSize;
	int sendClientMsgSize;

	int simState = 0;
	float simTime = 0;

	unordered_map <string, VehFullData_t> VehDataLastStep_v;
	unordered_map <string, VehFullData_t> VehDataNextStep_v;
	vector <string> recvVehId_v;

	///********************************************
	//* Centralized Controller
	//*********************************************/
	CentralCtrl CentralCtrl_g(czFilename);
	bool ENABLE_IPG = false;
	if (CentralCtrl_g.ENABLE_IPG > 0) {
		ENABLE_IPG = true;
	}

	CentralCtrl_g.setVerbose(ENABLE_VERBOSE);
	CentralCtrl_g.MasterLogName = MasterLogName;

	for (int i = 0; i < egoIdVec.size(); i++) {
		CentralCtrl_g.egoCzState[egoIdVec[i]] = -1;
	}

	/********************************************
	* Message Setups
	*********************************************/
	MsgHelper MsgServer_c;
	MsgHelper MsgClient_c;

	MsgServer_c.getConfig(Config_c);
	MsgClient_c.getConfig(Config_c);




	if (ENABLE_VERBOSE) {
		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "\n============================================");
		fprintf(f, "CoordMerge Starts at %s\n", simStartTimestampChar);
		fclose(f);
	}




	while (1) {
		//Receive v(t)
		//	Get Control v(t + dt), a(t + dt)
		//	Send cmd ego v(t + dt)
		//	Recv actual ego a(t + dt)
		//	Send actual ego a(t + dt), cmd a(t + dt)

		/*==============================
		* 1) recv traffic from Server
		==============================*/
		recvVehId_v.clear();

		if (ENABLE_VERBOSE || 1) {
			printf("\n===========New Cycle==============\n");
		
			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "\n===========New Cycle==============\n");
			fclose(f);
		}

		if (Sock_c.ENABLE_SERVER) {
			if (ENABLE_VERBOSE) {
				printf("receive from server\n");

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "receive from server\n");
				fclose(f);
			}

			for (unsigned int iS = 0; iS < Sock_c.NSERVER; iS++) {

				if (ENABLE_VERBOSE) {
					printf("receive from ip %s, port %d \n", serverAddr[iS].c_str(), serverPort[iS]);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "receive from ip %s, port %d \n", serverAddr[iS].c_str(), serverPort[iS]);
					fclose(f);
				}

				MsgServer_c.clearRecvStorage();

				if (Sock_c.recvData(Sock_c.serverSock[iS], &simState, &simTime, MsgServer_c) < 0) {
					if (WSAGetLastError() != WSAEINTR) {
						printf("warning: receive from client fails\n");
					}
					Sock_c.socketShutdown();
					exit(-1);
				};

				if (ENABLE_VERBOSE) {
					FILE* f = fopen(MasterLogName.c_str(), "a");

					printf("receive simTime %f, simState %d\n", simTime, simState);

					fprintf(f, "receive simTime %f, simState %d\n", simTime, simState);

					for (auto& it : MsgServer_c.VehDataRecv_um) {
						printf("\treceive vehicle id %s\n", it.first.c_str());

						fprintf(f, "\treceive vehicle id %s\n", it.first.c_str());
					}
					for (auto& it : MsgServer_c.DetDataRecv_um) {
						printf("\treceive detector at intersection id %s\n", it.first.c_str());

						fprintf(f, "\treceive detector at intersection id %s\n", it.first.c_str());
					}
					for (auto& it : MsgServer_c.TlsDataRecv_um) {
						printf("\treceive signal intersection id %s\n", it.first.c_str());

						fprintf(f, "\treceive signal intersection id %s\n", it.first.c_str());
					}

					fclose(f);

				}
			}

			if (ENABLE_VERBOSE) {
				printf("receive complete \n");

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "receive complete \n");
				fclose(f);
			}

		}
		else {
			if (ENABLE_VERBOSE) {
				printf("no serve set, use dummy values\n");

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "no serve set, use dummy values\n");
				fclose(f);
			}
			printf("Error!!! CoordMerge has to be connected to a server!!!!\n");

			return -1;
		}

		/*==============================
		* 2) obtain commands for target vehicles
		==============================*/
		// will receive all vehicles subscribed
		VehDataLastStep_v.clear();
		VehDataLastStep_v = MsgServer_c.VehDataRecv_um;
		if (ENABLE_IPG) {
			if (IpgVehData.id.compare("1") == 0) {
				VehDataLastStep_v["1"] = IpgVehData;
			}
		}


		//for (auto& it : VehDataLastStep_v) {
		//	string id = any_cast<string> (it.second["id"]);
		//	float spd = any_cast<float> (it.second["speed"]);

		//	printf("recv from traffic layer id %s, speed %f\n", id, spd);
		//}

		if (ENABLE_VERBOSE) {
			printf("\nCentral control starts\n");

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "\nCentral control starts\n");
			fclose(f);
		}

		// desired speed of those vehicles in the control zone will be overwritten by the centralized controller
		CentralCtrl_g.runtimeStepCall(simTime, egoIdVec, VehDataLastStep_v);

		int aaaa = 1;

		//+++++++++
		// 3) Reparser vehicle message and then send out traffic states and ego commands to client
		//+++++++++
		// !! NOTE: order of vehicle data is different for each client. the ego vehicle of each client is at the first location
		// !!	SEND ALL TRAFFIC INFORMATION 
		if (Sock_c.ENABLE_CLIENT) {
			MsgServer_c.clearSendStorage();

			for (unsigned int iC = 0; iC < Sock_c.NCLIENT; iC++) {

				if (!Sock_c.ENABLE_SERVER) {
					simState = 1;
					simTime = simTime + 0.1;
				}

				if (ENABLE_VERBOSE) {
					printf("send client at port %d\n", selfServerPortUserInput[iC]);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "send client at port %d\n", selfServerPortUserInput[iC]);
					fclose(f);
				}

				int iByte = 0;
				float simTimeSend = simTime;
				char sendClientBuffer[8096];

				// !! WE don't know the message size yet, pack a dummy header
				MsgServer_c.packHeader(simState, simTimeSend, MSG_HEADER_SIZE, sendClientBuffer, &iByte);
				Sock_c.sendClientByte[iC] = { MSG_HEADER_SIZE };

				//for (int i = 0; i < CentralCtrl_g.TargetVehId_v.size(); i++) {
				//	//if (VehDataLastStep_v.find(egoIdVec[iC]) != VehDataLastStep_v.end()) {
				//	if (CentralCtrl_g.TargetVehId_v[i].compare(egoIdVec[iC]) == 0) {
				//		//string linkId = any_cast<string> (VehDataLastStep_v[egoIdVec[iC]]["linkId"]);
				//		//if (linkId.compare("1") != 0 && linkId.compare("2") != 0) {
				//		//	//VehDataLastStep_v[egoIdVec[iC]]["speedDesired"] = (float) 13.89;
				//		//}
				if (VehDataLastStep_v.find(egoIdVec[iC]) != VehDataLastStep_v.end()) {
					if (CentralCtrl_g.egoCzState[egoIdVec[iC]] == 2) {
						VehDataLastStep_v[egoIdVec[iC]].speedDesired = (float) 13.89;
					}

					MsgServer_c.packVehData(VehDataLastStep_v[egoIdVec[iC]], sendClientBuffer, &Sock_c.sendClientByte[iC]);

					if (ENABLE_VERBOSE || 1) {
						string id = VehDataLastStep_v[egoIdVec[iC]].id;
						float spd = VehDataLastStep_v[egoIdVec[iC]].speedDesired;

						printf("\tsend id %s, speedDesired %f\n", id.c_str(), spd);

						FILE* f = fopen(MasterLogName.c_str(), "a");
						fprintf(f, "\tsend id %s, speedDesired %f\n", id.c_str(), spd);
						fclose(f);
					}
				}
				//	}
				//	else {
				//		int aa = 1;
				//	}
				//}

				//for (int iV = 0; iV < recvVehId_v.size(); iV++) {
				//	if (recvVehId_v[iV] != egoIdVec[iC]) {
				//		Sock_c.packVehFullData(VehDataLastStep_v[recvVehId_v[iV]], sendClientBuffer, &Sock_c.sendClientByte[iC]);
				//	}
				//}

				// repack the header with the correct size
				iByte = 0;
				MsgServer_c.packHeader(simState, simTimeSend, Sock_c.sendClientByte[iC], sendClientBuffer, &iByte);

				if (send(Sock_c.clientSock[iC], sendClientBuffer, Sock_c.sendClientByte[iC], 0) != Sock_c.sendClientByte[iC]) {
					fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
					exit(1);
				}

				if (ENABLE_VERBOSE || 1) {
					printf("send client size %d\n", Sock_c.sendClientByte[iC]);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "send client size %d\n", Sock_c.sendClientByte[iC]);
					fclose(f);
				}

			}

			if (ENABLE_VERBOSE) {
				printf("send client complete\n");

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "send client complete\n");
				fclose(f);
			}
		}

		/*==============================
		* 4) wait and recv for next ego states to come in
		==============================*/
		//+++++++++
		// Group target vehicle speeds send to Server
		//+++++++++
		// VehDataNextStep_v is essentially a copy of VehDataLastStep_v
		VehDataNextStep_v.clear();

		if (ENABLE_VERBOSE) {
			printf("parser control command\n");

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "parser control command\n");
			fclose(f);
		}

		for (int iV = 0; iV < CentralCtrl_g.TargetVehId_v.size(); iV++) {
			VehDataNextStep_v[CentralCtrl_g.TargetVehId_v[iV]] = VehDataLastStep_v[CentralCtrl_g.TargetVehId_v[iV]];

			if (ENABLE_VERBOSE) {
				string id = VehDataNextStep_v[CentralCtrl_g.TargetVehId_v[iV]].id;
				float spd = VehDataNextStep_v[CentralCtrl_g.TargetVehId_v[iV]].speedDesired;

				printf("\tparser control commands id %s, speed %f\n", id.c_str(), spd);

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\tparser control commands id %s, speed %f\n", id.c_str(), spd);
				fclose(f);
			}

			if (0) {
				fstream debuglog;
				debuglog.open("control.txt", fstream::in | fstream::out | fstream::app);
				//int8_t tempuint8;
				//memcpy(&tempuint8, sendbuffer + i, sizeof(uint8_t));
				string id = VehDataLastStep_v[CentralCtrl_g.TargetVehId_v[iV]].id;
				float speed = VehDataLastStep_v[CentralCtrl_g.TargetVehId_v[iV]].speedDesired;
				cout << id << ", " << speed << endl;
				//debuglog << unsigned(tempuint8) << ",";
				debuglog << endl;
				debuglog.close();
			}
		}
		if (ENABLE_VERBOSE) {
			printf("parser control command complete\n");

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "parser control command complete\n");
			fclose(f);
		}

		if (Sock_c.ENABLE_CLIENT) {
			MsgClient_c.clearRecvStorage();

			if (ENABLE_VERBOSE) {
				printf("receive from client\n");

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "receive from client\n");
				fclose(f);
			}

			for (int iC = 0; iC < Sock_c.NCLIENT; iC++) {
				/*char recvClientBuffer[RECVCLIENTBUFSIZE];
				if ((recvClientMsgSize = recv(Sock_c.clientSock[iC], recvClientBuffer, sizeof(uint16_t), 0)) == SOCKET_ERROR) {
					printf("recvfrom() failed with error code : %d", WSAGetLastError());
					exit(EXIT_FAILURE);
				}
				double speed = ((recvClientBuffer[0] << 0) & 0x00ff
					| (recvClientBuffer[1] << 8) & 0xff00) / 1000.0;*/

					//unordered_map <string, VehData_t> VehNext_um;
					//unordered_map <string, TrafficLightData_t> TlsNext_um;
					//unordered_map <string, DetectorData_t> DetNext_um;
					//vector <string> nextVehId_v;
					//vector <string> nextTlsId_v;
					//vector <string> nextDetId_v;

				char recvBuffer[200];

				int recvSize;


				Sock_c.recvData(Sock_c.clientSock[iC], &simState, &simTime, MsgClient_c);


				if (ENABLE_VERBOSE) {
					FILE* f = fopen(MasterLogName.c_str(), "a");

					printf("receive client at port %d\n", selfServerPortUserInput[iC]);

					fprintf(f, "receive client at port %d\n", selfServerPortUserInput[iC]);

					for (auto& it : MsgServer_c.VehDataRecv_um) {
						printf("\treceive vehicle id %s\n", it.first.c_str());

						fprintf(f, "\treceive vehicle id %s\n", it.first.c_str());
					}
					for (auto& it : MsgServer_c.DetDataRecv_um) {
						printf("\treceive detector at intersection id %s\n", it.first.c_str());

						fprintf(f, "\treceive detector at intersection id %s\n", it.first.c_str());
					}
					for (auto& it : MsgServer_c.TlsDataRecv_um) {
						printf("\treceive signal intersection id %s\n", it.first.c_str());

						fprintf(f, "\treceive signal intersection id %s\n", it.first.c_str());
					}
				
					fclose(f);
				}

				//+++++++++
				// replace VehData from server for those IDs in the client msg.
				//+++++++++	
				//if (VehDataLastStep_v.find(egoIdVec[iC]) != VehDataLastStep_v.end()) {
				if (MsgClient_c.VehDataRecv_um.find(egoIdVec[iC]) != MsgClient_c.VehDataRecv_um.end()) {
					//if (VehNext_um.count(egoIdVec[iC])) {
					//if (VehNext_um.size()>0){
						//VehDataNextStep_v[egoIdVec[iC]].speed = VehNext_um[egoIdVec[iC]].speed;
					if (ENABLE_VERBOSE || 1) {
						FILE* f = fopen(MasterLogName.c_str(), "a");

						// if ego in the control zone
						if (VehDataNextStep_v.find(egoIdVec[iC]) != VehDataNextStep_v.end()) {
							float spd11 = VehDataNextStep_v[egoIdVec[iC]].speedDesired;
							float spd22 = MsgClient_c.VehDataRecv_um[egoIdVec[iC]].speedDesired;
							printf("\tClient commands: change vehicle speed from %f to %f\n", spd11, spd22);

							fprintf(f, "\tClient commands: change vehicle speed from %f to %f\n", spd11, spd22);
						}

						fclose(f);
					}

					// no matter ego in control zone or not, its speed will be a feedback
					VehDataNextStep_v[egoIdVec[iC]] = MsgClient_c.VehDataRecv_um[egoIdVec[iC]];


					//if (ENABLE_VERBOSE) {
					//	string id = any_cast<string> (VehNext_um[egoIdVec[iC]]["id"]);
					//	float speed = any_cast<float> (VehNext_um[egoIdVec[iC]]["speedDesired"]);
					//	printf("recv ego %s, speed %f\n", id.c_str(), speed);
					//}
				}

				if (ENABLE_IPG) {
					if (MsgClient_c.VehDataRecv_um.find("1") != MsgClient_c.VehDataRecv_um.end()) {
						printf("receive IPG vehicle data\n");
						IpgVehData = MsgClient_c.VehDataRecv_um["1"];
					}

				}

				//}
			}
		}

		/*==============================
		* 5) forward next ego states and traffic commands to Server
		==============================*/
		if (Sock_c.ENABLE_SERVER) {
			MsgClient_c.clearSendStorage();


			if (ENABLE_VERBOSE) {
				printf("send to server\n");

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "send to server\n");
				fclose(f);
			}


			// pack header
			for (unsigned int iS = 0; iS < Sock_c.NSERVER; iS++) {

				if (ENABLE_VERBOSE) {
					printf("send to ip %s, port %d \n", serverAddr[iS].c_str(), serverPort[iS]);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "send to ip %s, port %d \n", serverAddr[iS].c_str(), serverPort[iS]);
					fclose(f);
				}

				uint8_t simStateSend = simState;

				int iByte = 0;

				char sendServerBuffer[SENDSERVERBUFSIZE];
				// !! WE don't know the message size yet, pack a dummy header
				MsgClient_c.packHeader(simStateSend, simTime, MSG_HEADER_SIZE, sendServerBuffer, &iByte);
				Sock_c.sendServerByte[iS] = { MSG_HEADER_SIZE };

				//for (int iV = 0; iV < egoIdVec.size(); iV++) {
				//	if (VehDataNextStep_v.find(egoIdVec[iV]) != VehDataNextStep_v.end()) {
				//		Sock_c.packVehFullData(VehDataNextStep_v[egoIdVec[iV]], sendServerBuffer, &Sock_c.sendServerByte[iS]);
				//	}
				//}

				//for (int iV = 0; iV < CentralCtrl_g.nTargetVeh; iV++) {
				//	if (VehDataNextStep_v.find(CentralCtrl_g.TargetVehId_v[iV]) != VehDataNextStep_v.end()) {
				for (auto& iter : VehDataNextStep_v) {
					/*for (int iE = 0; iE < egoIdVec.size(); iE++) {
						if (egoIdVec[iE].compare(CentralCtrl_g.TargetVehId_v[iV]) != 0) {*/
					VehFullData_t temp = iter.second;
					MsgClient_c.packVehData(temp, sendServerBuffer, &Sock_c.sendServerByte[iS]);

					if (ENABLE_VERBOSE) {
						float spd = temp.speedDesired;
						string id = temp.id;
						printf("\tsend id %s speed %f\n", id.c_str(), spd);

						FILE* f = fopen(MasterLogName.c_str(), "a");
						fprintf(f, "\tsend id %s speed %f\n", id.c_str(), spd);
						fclose(f);
					}
				}
				//	}
				//}
				// repack the header with the correct size
				iByte = 0;
				MsgClient_c.packHeader(simState, simTime, Sock_c.sendServerByte[iS], sendServerBuffer, &iByte);

				if (send(Sock_c.serverSock[iS], sendServerBuffer, Sock_c.sendServerByte[iS], 0) != Sock_c.sendServerByte[iS]) {
					fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
					exit(1);
				}
			}
			if (ENABLE_VERBOSE) {
				printf("send complete\n");

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "send complete\n");
				fclose(f);
			}

		}

	}
}