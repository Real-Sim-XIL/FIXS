#include <iostream>
#include <fstream>
#include <unordered_map>

#include <event2/bufferevent.h>
#include <event2/util.h>
#include <event2/event.h>

#include "SocketHelper.h"

#include "DeviceV2X.h"

using namespace std;

// use vector so that easier for future expansion
vector <string> rsuServerAddr = {};
vector <int> rsuServerPort = {};

// use vector so that easier for future expansion
// ego vehicle/CAVE dSPACE need to connect to this port
vector <int> selfServerPortUserInput = {880}; // client connect to this port

// turn this flag to 'true' when using RSU OBU
// turn this flag to 'false' when directly to ego vehicle
// if 'false', rsuServerAddr and rsuServerPort will be ignored, selfServerPortUserInput will be used
// if 'true', selfServerPortUserInput will be ignored, rsuServerAddr and rsuServerPort will be used
bool XIL_AS_SERVER = false;


// location of the merging point/where RSU will locate
// distance between ego location and this location will be used to simulate communication model
DeviceV2X RSU_c("RSU", 0);
COORD_t rsuPos = { 1523, 755.4, 0 }; // x, y, z position

// specify the configuration file here
string configPath = "..\\..\\RealSimRelease\\config_SUMO_ACM.yaml";


static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << endl
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-f,--file PATH\\FILENAME\tSpecify the path and filename of configuration yaml file"
		<< std::endl;
}

int main(int argc, char* argv[]) {

	// ===========================================================================
	//							Read Configuration File
	// ===========================================================================
	ConfigHelper Config_c;

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
		else {
			printf("Check options\n");
			show_usage(argv[0]);
			return 0;
		}
	}

	if (Config_c.getConfig(configPath) < 0) {
		return -1;
	}

	// ===========================================================================
	//				 Convert Config to flags and variables
	// ===========================================================================
	bool ENABLE_VERBOSE = false;

	if (Config_c.SimulationSetup.EnableVerboseLog) {
		ENABLE_VERBOSE = true;
	}
	else {
		ENABLE_VERBOSE = false;
	}

	// -------------------
	//  Get Address of Controller/Application Layer
	// -------------------
	// these containers have unique address and port
	vector <string> serverAddr = {};
	vector <int> serverPort = {};

	// some times user can specifiy same port address multiple times for different message subscription. These containers store these repeated addresses
	vector <string> serverAddrAll = {};
	vector <int> serverPortAll = {};
	for (int i = 0; i < Config_c.XilSetup.VehicleSubscription.size(); i++) {
		vector <int> port_v;
		vector <string> ip_v;
		ip_v = get<2>(Config_c.XilSetup.VehicleSubscription[i]);
		port_v = get<3>(Config_c.XilSetup.VehicleSubscription[i]);
		for (int iP = 0; iP < ip_v.size(); iP++) {
			serverAddrAll.push_back(ip_v[iP]);
		}
		for (int iP = 0; iP < port_v.size(); iP++) {
			serverPortAll.push_back(port_v[iP]);
		}
	}
	serverPort = serverPortAll;
	auto it = unique(serverPort.begin(), serverPort.end());
	serverPort.resize(distance(serverPort.begin(), it));

	serverAddr = serverAddrAll;
	serverAddr.resize(distance(serverPort.begin(), it));


	// ===========================================================================
	//							Connection Setups
	// ===========================================================================
	bool ENABLE_DOWNSTREAM = true;

	SocketHelper Sock_c;
	// -------------------
	//  handle case based on RSU connected or not
	// -------------------
	vector <int> upstreamSock;
	vector <int> downstreamSock;
	if (XIL_AS_SERVER) {
	// connect to RSU, this Comms is client for both Controller/Application Layer and RSU
		serverAddr.push_back(rsuServerAddr[0]);
		serverPort.push_back(rsuServerPort[0]);

		Sock_c.socketSetup(serverAddr, serverPort);
	}
	else {
	// not connect to RSU, this Comms is client for Controller/Application Layer, server for ego vehicle
		if (ENABLE_DOWNSTREAM) {
			Sock_c.socketSetup(serverAddr, serverPort, selfServerPortUserInput);
		}
		else {
			Sock_c.socketSetup(serverAddr, serverPort);
		}

	}
	Sock_c.disableWaitClientTrigger();
	Sock_c.disableServerTrigger();

	Sock_c.initConnection();

	if (XIL_AS_SERVER) {
		// sanity check
		if (Sock_c.serverSock.size() != 2) {
			printf("Error Setup, Comms should be clients for both RSU and Controller/Application Layer! \n");
			return -1;
		}
		upstreamSock.push_back(Sock_c.serverSock[0]);
		downstreamSock.push_back(Sock_c.serverSock[1]);
	}
	else {
		upstreamSock.push_back(Sock_c.serverSock[0]);
		if (ENABLE_DOWNSTREAM) {
			downstreamSock.push_back(Sock_c.clientSock[0]);
		}
	}


	// ===========================================================================
	//								Message Setups
	// ===========================================================================
	MsgHelper MsgUpstream_c;
	MsgHelper MsgDownstream_c;

	MsgUpstream_c.getConfig(Config_c);
	MsgDownstream_c.getConfig(Config_c);

	int simState = 0;
	float simTime = 0;

	// ===========================================================================
	//								Initialize COMMS
	// ===========================================================================
	RSU_c.pos.x = rsuPos.x;
	RSU_c.pos.y = rsuPos.y;
	RSU_c.pos.z = rsuPos.z;

	// ===========================================================================
	//								Main LOOP
	// ===========================================================================

	while (1) {
		// -------------------
		// 1) receive from Controller/Application Layer
		// -------------------
		try {
			MsgUpstream_c.clearRecvStorage();
			if (Sock_c.recvData(upstreamSock[0], &simState, &simTime, MsgUpstream_c) < 0) {
				if (WSAGetLastError() != WSAEINTR) {
					printf("ERROR: receive from server fails\n");
				}
				Sock_c.socketShutdown();
				exit(-1);
			}
		}
		catch (const std::exception& e) {
			Sock_c.socketShutdown();
			std::cout << e.what();
			exit(-1);
		}
		catch (...) {
			Sock_c.socketShutdown();
			printf("UNKNOWN ERROR: send to client fails\n");
			exit(-1);
		}
		// -------------------
		// 2) send to RSU or Ego
		// -------------------
		if (ENABLE_DOWNSTREAM) {
			try {
				// stores BSM
				MsgDownstream_c.clearSendStorage();
				for (auto iter : MsgUpstream_c.VehDataRecv_um) {
					RSU_c.pushBsmBuf(simTime, iter.second);
				}

				// retrieve BSM
				VehData_t currentBsm;
				RSU_c.getCurrentBsm(simTime, currentBsm);

				// send out
				MsgDownstream_c.VehDataSend_um[downstreamSock[0]] = {};
				if (currentBsm.size() > 0) {
					MsgDownstream_c.VehDataSend_um[downstreamSock[0]].push_back(currentBsm);
				}
				float simTimeSend = simTime;
				uint8_t simStateSend = 1;
				if (Sock_c.sendData(downstreamSock[0], 0, simTimeSend, simStateSend, MsgDownstream_c) < 0) {
					printf("ERROR: send to client fails\n");
					Sock_c.socketShutdown();
					exit(-1);
				};
			}
			catch (const std::exception& e) {
				Sock_c.socketShutdown();
				std::cout << e.what();
				exit(-1);
			}
			catch (...) {
				Sock_c.socketShutdown();
				printf("UNKNOWN ERROR: send to client fails\n");
				exit(-1);
			}
		}

		// -------------------
		// 3) receive from RSU or Ego, NO COMMS MODEL
		// -------------------
		if (ENABLE_DOWNSTREAM) {
			try {
				MsgDownstream_c.clearRecvStorage();

				int simStateRecv;
				float simTimeRecv;

				// save received message into Msg_c recv storages
				if (Sock_c.recvData(downstreamSock[0], &simStateRecv, &simTimeRecv, MsgDownstream_c) < 0) {
					if (WSAGetLastError() != WSAEINTR) {
						printf("ERROR: receive from client fails\n");
					}
					Sock_c.socketShutdown();
					exit(-1);
				};

				for (auto it : MsgDownstream_c.VehDataRecv_um) {
					float speed = std::any_cast<float>(it.second["speed"]);
					float speedDes = std::any_cast<float>(it.second["speedDesired"]);
					printf("receive client simTime %f, speed %f, speedDes %f\n", simTimeRecv, speed, speedDes);
				}

			}
			catch (const std::exception& e) {
				Sock_c.socketShutdown();
				std::cout << e.what();
				exit(-1);
			}
			catch (...) {
				Sock_c.socketShutdown();
				printf("UNKNOWN ERROR: receive client fails\n");
				exit(-1);
			}
		}

		// -------------------
		// 4) send to Controller/Application Layer, NO COMMS MODEL
		// -------------------
		try {
			MsgUpstream_c.clearSendStorage();

			MsgUpstream_c.VehDataSend_um[upstreamSock[0]] = {};
			for (auto it : MsgDownstream_c.VehDataRecv_um) {
				MsgUpstream_c.VehDataSend_um[upstreamSock[0]].push_back(it.second);
			}

			float simTimeSend = simTime;
			uint8_t simStateSend = 1;
			if (Sock_c.sendData(upstreamSock[0], 0, simTimeSend, simStateSend, MsgUpstream_c) < 0) {
				printf("ERROR: send to server fails\n");
				Sock_c.socketShutdown();
				exit(-1);
			};
		}
		catch (const std::exception& e) {
			Sock_c.socketShutdown();
			std::cout << e.what();
			exit(-1);
		}
		catch (...) {
			Sock_c.socketShutdown();
			printf("UNKNOWN ERROR: send to server fails\n");
			exit(-1);
		}
	}

}