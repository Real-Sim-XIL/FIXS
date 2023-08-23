#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>


//#include "TraCIAPI.h"


//#include <event2/bufferevent.h>
//#include <event2/util.h>
//#include <event2/event.h>

//#define TRACI_ENABLED 1
#include <ctime>
#include <chrono>


#include "TrafficHelper.h"

#include "RealSimVersion.h"

using namespace std;

//!!!!! NEED TO
// -multithread
// -add lane position
// -might need to put a timeout on recv or something. because the message can be larger than buffer size. 
// -limit number of vehicles to send
// -need to define if want to directly overwrite SUMO speed or send as speed guidance
// -handle vehicle enter the network and exiting the network
// -timing log
//
// !!! NEED TO RECV UNTIL EMPTY

const bool ENABLE_TIMING = false;

typedef struct Timer_t {
	LARGE_INTEGER StartingTime;
	LARGE_INTEGER EndingTime;
	LARGE_INTEGER ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
};

ConfigHelper Config_c;
SocketHelper Sock_c;



BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal.
	case CTRL_C_EVENT:
		printf("Ctrl-C event caught\n\n");
		printf("RealSim Exits, Please stop VISSIM/SUMO manually\n\n");
		Sock_c.socketShutdown();
		exit(-1);

		return TRUE;

	default:
		return FALSE;
	}
}

static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << endl
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-f,--file PATH\\FILENAME\tSpecify the path and filename of configuration yaml file"
		<< std::endl;
}

int main(int argc, char* argv[]) {

	printf("==================================================\n");
	printf("\t\tRealSim Interface\n");
	printf("\t\t    v%s\n",REALSIM_VERSION_STRING);
	printf("==================================================\n");

	// control-c handles
	if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		//printf("\nThe control-c handler is set.\n");
	}
	else
	{
		printf("\nERROR: Could not set control-c handler");
		exit(-1);
	}


	string TrafficLayerErrorFile = "TrafficLayer.err";

	auto simStartTimestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
	char simStartTimestampChar[100];
	ctime_s(simStartTimestampChar, sizeof simStartTimestampChar, &simStartTimestamp);
	fstream f(TrafficLayerErrorFile, std::fstream::in | std::fstream::out | std::fstream::app);
	f << endl << "=============================================" << endl;
	f << "Traffic Layer Starts at  " << simStartTimestampChar << endl;
	f.close();

	
	string LogPath = ".\\RealSim_tmp";
	if (CreateDirectoryA(LogPath.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
		printf("cannot create RealSim_tmp folder, exiting...");
		return -1;
	}

	char MasterLogNameChar[100];

	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char formatTimeBuffer[100];
	strftime(formatTimeBuffer, 100, "%Y%m%d_%H%M%S", timeinfo);
	snprintf(MasterLogNameChar, sizeof(char) * 100, ".\\RealSim_tmp\\TrafficLayer_%s.log", formatTimeBuffer);
	DWORD dwAttrib = GetFileAttributesA(MasterLogNameChar);

	bool logNameExist = (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

	int iFile = 1;
	while (logNameExist) {
		snprintf(MasterLogNameChar, sizeof(char) * 100, ".\\RealSim_tmp\\TrafficLayer_%s_%d.log", formatTimeBuffer,iFile);
		DWORD dwAttrib = GetFileAttributesA(MasterLogNameChar);
		iFile++;
		logNameExist = (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));	
	}
	
	string MasterLogName(MasterLogNameChar);

	// reset logs
	if (1) {
		ifstream f(MasterLogName.c_str());
		if (f.good()) {
			f.close();
			remove(MasterLogName.c_str());
		}
	}

	string configPath = "..\\..\\RealSimToyota\\config.yaml";
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
				std::cerr << "--path option requires one argument." << std::endl;
				return -1;
			}
		}
		else {
			printf("Check options\n");
			show_usage(argv[0]);
			return 0;
		}
	}

	// ===========================================================================
	// 			READ Config File
	// ===========================================================================
	printf("Reading Configuration file %s\n", configPath.c_str());
	if (Config_c.getConfig(configPath) < 0) {
		printf("Please check path and filename of the configuration yaml\n");
		show_usage(argv[0]);
		exit(-1);
	}
	else {
		printf("Read configuration file success\n");
	}
	
	{
		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "\nConfigFile %s\n", configPath.c_str());
		fclose(f);
	}

	// initialize Traffic Layer setup variables
	bool ENABLE_REALSIM = true;
	bool ENABLE_VISSIM = false;
	bool ENABLE_CLIENT = false;
	bool ENABLE_VERBOSE = false;
	bool ENABLE_EXT_DYN = false;
	bool ENABLE_VEH_SIMULATOR = false;

	vector <int> selfServerPortAll = {};
	vector <string> serverAddr = {};
	vector <int> serverPort = {};
	vector <string> serverNames = {};

	// each application, map port to socket
	unordered_map <int, int> selfServerPort2Sock_um;
	vector <int> selfServerPortUserInput = {};

	vector <int> vissimSock;
	vector <int> actualClientSock;

	// config Traffic Layer setup variables
	if (Config_c.SimulationSetup.EnableRealSim) {
		ENABLE_REALSIM = true;
	}
	else {
		ENABLE_REALSIM = false;
	}
	if (Config_c.SimulationSetup.SelectedTrafficSimulator.compare("VISSIM") == 0) {
		ENABLE_VISSIM = true;
		printf("Selected Traffic Simulator VISSIM\n\n");
	}
	else {
		ENABLE_VISSIM = false;
		printf("Selected Traffic Simulator SUMO\n\n");
	}
	if (Config_c.SimulationSetup.EnableVerboseLog) {
		ENABLE_VERBOSE = true;
		printf("Enable verbose logging\n\n");
	}
	else {
		ENABLE_VERBOSE = false;
		printf("No verbose logging\n\n");
	}
	if (Config_c.ApplicationSetup.EnableApplicationLayer) {
		ENABLE_CLIENT = true;
	}
	else {
		if (Config_c.XilSetup.EnableXil) {
			ENABLE_CLIENT = true;
		}
		else {
			ENABLE_CLIENT = false;
		}
	}

	if (Config_c.SimulationSetup.EnableExternalDynamics) {
		ENABLE_EXT_DYN = true;
	}
	else {
		ENABLE_EXT_DYN = false;
	}

	ENABLE_VEH_SIMULATOR = Config_c.CarMakerSetup.EnableCosimulation;

	if (ENABLE_VERBOSE) {
		//FILE* f = fopen(MasterLogName.c_str(), "a");
		//fprintf(f, "\n============================================");
		//fprintf(f, "Traffic Layer Starts at %s\n", simStartTimestampChar);
		//fclose(f);
	}

	if (!Config_c.ApplicationSetup.EnableApplicationLayer && Config_c.XilSetup.EnableXil){
		if (Config_c.XilSetup.VehicleSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimDriver");
				printf("VISSIM driver model dll selected as server \n");
			}
		}
		if (Config_c.XilSetup.DetectorSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort+1);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimSignal");
				printf("VISSIM signal dll selected as server \n");
			}
		}
	}
	else {
		if (Config_c.ApplicationSetup.VehicleSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimDriver");
				printf("VISSIM driver model dll selected as server \n");
			}
		}
		if (Config_c.ApplicationSetup.DetectorSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort+1);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimSignal");
				printf("VISSIM signal dll selected as server \n");
			}
		}
	}


	// ===========================================================================
	// 			Check Configuration
	// ===========================================================================
	if (serverAddr.size() == 0 || serverPort.size() == 0) {
		printf("ERROR: must define at least a VehicleSubscription or DetectorSubscription!\n\n");
		exit(-1);
	}

	if (ENABLE_CLIENT) {
		if (!Config_c.ApplicationSetup.EnableApplicationLayer && Config_c.XilSetup.EnableXil) {

			for (int i = 0; i < Config_c.XilSetup.VehicleSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.XilSetup.VehicleSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
					printf("\nTrafficLayer broadcast to client at port %d \n", port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.XilSetup.SignalSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.XilSetup.SignalSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.XilSetup.DetectorSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.XilSetup.DetectorSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
		}
		else {
			for (int i = 0; i < Config_c.ApplicationSetup.VehicleSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.ApplicationSetup.VehicleSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
					printf("\nTrafficLayer broadcast to client at port %d \n", port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.ApplicationSetup.SignalSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.ApplicationSetup.SignalSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.ApplicationSetup.DetectorSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.ApplicationSetup.DetectorSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
		}

	}

	// number of ports equal to size of Sock_c.clientSock
	// map each port to an element of Sock_c.clientSock
	selfServerPortUserInput = selfServerPortAll;
	auto it = unique(selfServerPortUserInput.begin(), selfServerPortUserInput.end());
	selfServerPortUserInput.resize(distance(selfServerPortUserInput.begin(), it));
	for (int i = 0; i < selfServerPortUserInput.size(); i++) {
		int port = selfServerPortUserInput[i];
		// if not exist
		if (selfServerPort2Sock_um.find(port) == selfServerPort2Sock_um.end()) {
			selfServerPort2Sock_um[port] = selfServerPort2Sock_um.size();
		}
	}


	for (int i = 0; i < selfServerPortUserInput.size(); i++) {
		if (selfServerPortUserInput[i] == Config_c.SimulationSetup.TrafficSimulatorPort || selfServerPortUserInput[i] == Config_c.SimulationSetup.TrafficSimulatorPort+1) {
			printf("ERROR: %d and %d are reserved port, please select other ports for Application Layer!!\n\n", Config_c.SimulationSetup.TrafficSimulatorPort, Config_c.SimulationSetup.TrafficSimulatorPort+1);
			exit(-1);
		}
	}

	double simTime = 0;
	int ii = 0;

	/********************************************
	* Timing Analysis
	*********************************************/
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);

	/********************************************
	* Connection Setups
	*********************************************/
	//if (selfServerPortUserInput.size() > 1 && !ENABLE_VEH_SIMULATOR) {
	//	printf("\nERROR: currently only support one application layer\n");
	//	exit(-1);
	//}

	TrafficHelper Traffic_c;
	if (ENABLE_VISSIM) {
		//// has client
		//if (ENABLE_CLIENT && selfServerPortUserInput.size() > 0) {
		//	//Sock_c.socketSetup(serverAddr, serverPort, selfServerPortUserInput);
		//	Sock_c.socketSetup(selfServerPortUserInput);
		//}
		//else {
		//	Sock_c.socketSetup(serverAddr, serverPort);
		//}
		//if (!ENABLE_CLIENT) {
		//	Sock_c.disableClient();
		//}
		int N_ACT_CLIENT = selfServerPortUserInput.size();

		vector <int> vissimSockIdx;
		for (int i = 0; i < serverAddr.size(); i++) {
			vissimSockIdx.push_back(selfServerPortUserInput.size());
			if (ENABLE_REALSIM) {
				selfServerPortUserInput.push_back(serverPort[i]);
			}
		}
		Sock_c.socketSetup(selfServerPortUserInput);

		Sock_c.N_ACT_CLIENT = N_ACT_CLIENT;

		Sock_c.disableWaitClientTrigger();
		Sock_c.disableServerTrigger();
		if (Sock_c.initConnection(TrafficLayerErrorFile) < 0) {
			printf("Connect to VISSIM failed! Make sure start one instance of VISSIM/SUMO \n");
			exit(-1);
		}

		if (vissimSockIdx.size() == 0) {
			printf("\nERROR: must define at least a VehicleSubscription or DetectorSubscription!\n\n");
			exit(-1);
		}
		for (int i = 0; i < vissimSockIdx[0]; i++) {
			actualClientSock.push_back(Sock_c.clientSock[i]);
		}
		for (int i = 0; i < vissimSockIdx.size(); i++) {
			if (ENABLE_REALSIM) {
				vissimSock.push_back(Sock_c.clientSock[vissimSockIdx[i]]);
			}
		}
	}
	else {

		if (ENABLE_VEH_SIMULATOR) {
			bool needAddPort = true;
			for (int curPort : selfServerPortUserInput) {
				if (curPort == Config_c.CarMakerSetup.CarMakerPort) {
					needAddPort = false;
					break;
				}
			}
			if (needAddPort) {
				selfServerPortUserInput.push_back(Config_c.CarMakerSetup.CarMakerPort);
			}
		}

		Sock_c.socketSetup(selfServerPortUserInput); // no Server 
		if (!ENABLE_CLIENT) {
			Sock_c.disableClient();
		}

		Sock_c.N_ACT_CLIENT = selfServerPortUserInput.size();
		
		//Sock_c.enableWaitClientTrigger();
		Sock_c.disableWaitClientTrigger();
		if (Sock_c.initConnection(TrafficLayerErrorFile) > 0) {
			printf("Connect to SUMO failed! Make sure start Traffic Simulator first and start one instance of VISSIM/SUMO \n");
			exit(-1);
		}

		for (int i = 0; i < Sock_c.clientSock.size(); i++) {
			actualClientSock.push_back(Sock_c.clientSock[i]);
		}

	}


	/********************************************
	* Traffic Simulator send and recv setups
	*********************************************/
	try {
		if (ENABLE_VISSIM) {
			Traffic_c.connectionSetup(Sock_c.NCLIENT);
			Traffic_c.selectVISSIM(vissimSock, serverNames);
		}
		else {
			string trafficIp = Config_c.SimulationSetup.TrafficSimulatorIP;
			int trafficPort = Config_c.SimulationSetup.TrafficSimulatorPort;
			Traffic_c.connectionSetup(trafficIp, trafficPort, Sock_c.NCLIENT);
			Traffic_c.selectSUMO();
		}
	}
	catch (const std::exception& e) {
		printf("Error: connect to traffic simulator failed\n");
		std::cout << e.what();
		exit(-1);
	}
	catch (...) {
		printf("Error: connect to traffic simulator failed\n");
		exit(-1);
	}


	// pass configuration to Traffic_c
	Traffic_c.Config_c = &Config_c;
	Traffic_c.getConfig();

	Traffic_c.MasterLogName = MasterLogName;

	Traffic_c.ENABLE_VEH_SIMULATOR = ENABLE_VEH_SIMULATOR;

	/********************************************
	* Message Setups
	*********************************************/
	MsgHelper MsgServer_c;
	MsgHelper MsgClient_c;

	MsgServer_c.getConfig(Config_c);
	MsgClient_c.getConfig(Config_c);

	Timer_t TimerDelay = { 0 };
	QueryPerformanceFrequency(&TimerDelay.Frequency);

	///********************************************
	//* Start connection
	//*********************************************/
	//Sock_c.initConnection();

	bool isVeryFirstStep = true; 

	bool isEgoExist = false;
	bool isInitialTimeFinished = false;

	//while (simTime <= tSimuEnd && ii < nT) {
	while (1) {

		///****************************************************
		// RUN one-step simulation
		///****************************************************
		
		Traffic_c.runOneStepSimulation();

		if (ENABLE_VERBOSE) {
			printf("\n===========New time step==============\n");
			printf("===========SimTime %f==============\n", simTime);

			FILE* f = fopen(MasterLogName.c_str(), "a");
			//fprintf(f, "\n===========New time step==============\n");
			fprintf(f, "\n===SimTime %f\n", simTime);
			fclose(f);
		}
		// print every 10 simulation seconds
		else if (abs(simTime / 10 - round(simTime / 10)) < 1e-5) {
			printf("===========SimTime %f==============\n", simTime);
		}

		if ((Config_c.SimulationSetup.SimulationMode == 4 || Config_c.SimulationSetup.SimulationMode == 5) && !isInitialTimeFinished) {
			Traffic_c.runSimulation(Config_c.SimulationSetup.SimulationModeParameter);
			isInitialTimeFinished = true;
		}

		if ((Config_c.SimulationSetup.SimulationMode == 1 || Config_c.SimulationSetup.SimulationMode == 2) && !isEgoExist) {
			if (Traffic_c.checkIfEgoExist(&simTime)) {
				// continue the code to sync VISSIM/SUMO with clients
				isEgoExist = true;
			}else{
				// otherwise just continue running the simulation
				continue;
			}
		}

		// if veryFirstStep and vehicle simulator, add the ego vehicle
		if (isVeryFirstStep && ENABLE_VEH_SIMULATOR) {
			Traffic_c.addEgoVehicle(simTime);
		}

		///****************************************************
		// VISSIM/SUMO =====>>>>> Traffic Layer
		// get Traffic Simulator simulation INFO
		///****************************************************
		try {
			MsgServer_c.clearRecvStorage();

			if (ENABLE_REALSIM) {
				if (Traffic_c.recvFromTrafficSimulator(&simTime, MsgServer_c) < 0) {
					if (WSAGetLastError() != WSAEINTR) {

						printf("WARNING: receive from traffic simulator fails\n");

						printf("\tVISSIM or SUMO may already be closed\n");
						printf("\texit......\n");
					}
					Sock_c.socketShutdown();
					exit(-1);
				};

				// end simulation if simulation time is greater than simulation end time setup
				if (simTime > Config_c.SimulationSetup.SimulationEndTime) {
					exit(1);
				}
			}

			if (ENABLE_VERBOSE) {
				for (auto& it : MsgServer_c.VehDataRecv_um) {
					MsgServer_c.printVehData(it.second);

					MsgServer_c.printVehDataToFile(MasterLogName, it.second);
				}
			}
		}
		catch (const std::exception& e) {
			Sock_c.socketShutdown();
			std::cout << e.what();
			exit(-1);
		}
		catch (...) {
			Sock_c.socketShutdown();
			printf("UNKNOWN ERROR: receive from traffic simulator fails\n");
			exit(-1);
		}

		///****************************************************
		// Traffic Layer =====>>>>> Clients (Controller, Vehicle Simulator, Models)
		// send traffic to clients, e.g. Simulink/dSPACE
		///****************************************************
		if (ENABLE_TIMING) {

			QueryPerformanceCounter(&EndingTime);
			ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
			ElapsedMicroseconds.QuadPart *= 1000000;
			ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

			fstream debugLog;
			debugLog.open("timing_sumo.txt", fstream::in | fstream::out | fstream::app);
			debugLog << simTime << ";" << ElapsedMicroseconds.QuadPart / 1000000.0 << endl;
			debugLog.close();
		}

		if (MsgServer_c.VehDataRecv_um.size() > 0) {
			int aa = 1;
		}

		if (ENABLE_CLIENT) {
			try {
				MsgClient_c.clearSendStorage();

				for (unsigned int iC = 0; iC < actualClientSock.size(); iC++) {
				//for (unsigned int iC = actualClientSock.size()-1; iC > 0; iC--) {
					//!!!! NEED TO DISTRIBUTE MsgServer_c data to MsgClient_c
					//!!!! MsgServer_c VehDataRecv_um TlsDataRecvAll_v DetDataRecvAll_v ==>>> MsgClient_c VehDataSend_v[iclient]  VehIdSend_v[iclient] DetDataSend_v[iclient] TlsDataSend_v[iclient]

					//MsgClient_c.VehDataSend_um[Sock_c.clientSock[iC]] = MsgServer_c.VehDataRecvAll_v;

					int curPort = selfServerPortUserInput[iC];

					MsgClient_c.VehDataSend_um[actualClientSock[iC]] = {};
					MsgClient_c.TlsDataSend_um[actualClientSock[iC]] = {};
					MsgClient_c.DetDataSend_um[actualClientSock[iC]] = {};
					for (auto it : MsgServer_c.VehDataRecv_um) {
						if (ENABLE_VEH_SIMULATOR) {
							// if id is ego, and is the first socket (XIL), send it 
							if (it.second.id.compare(Config_c.CarMakerSetup.EgoId) == 0 && selfServerPortUserInput[iC] != Config_c.CarMakerSetup.CarMakerPort && Config_c.CarMakerSetup.EnableEgoSimulink) {
								MsgClient_c.VehDataSend_um[actualClientSock[iC]].push_back(it.second);
							}
							// if is the CarMakerPort or only have one socket (both vehicle simualtor and XIL), send it
							else if (selfServerPortUserInput[iC] == Config_c.CarMakerSetup.CarMakerPort || actualClientSock.size() == 1) {
								MsgClient_c.VehDataSend_um[actualClientSock[iC]].push_back(it.second);
							}
						}
						else {
							MsgClient_c.VehDataSend_um[actualClientSock[iC]].push_back(it.second);
						}
					}
					for (auto it : MsgServer_c.TlsDataRecv_um) {
						// assign Tls data to each client
						// if all signal, then add it to send list
						if (Config_c.SocketPort2SubscriptionList_um[curPort].SignalList.subAllSignalFlag) {
							MsgClient_c.TlsDataSend_um[actualClientSock[iC]].push_back(it.second);
						}
						// else, only assign Tls that is needed for current port/socket
						else {
							// check if current signal light is included
							if (Config_c.SocketPort2SubscriptionList_um[curPort].SignalList.signalId_v.find(it.first) != Config_c.SocketPort2SubscriptionList_um[curPort].SignalList.signalId_v.end()) {
								MsgClient_c.TlsDataSend_um[actualClientSock[iC]].push_back(it.second);
							}
						}
						
					}
					for (auto it : MsgServer_c.DetDataRecv_um) {
						MsgClient_c.DetDataSend_um[actualClientSock[iC]].push_back(it.second);
					}

					if (ENABLE_VERBOSE) {
						printf("sending client at port %d\n", selfServerPortUserInput[iC]);
						printf("\tvehicle data size %d\n", (int)MsgClient_c.VehDataSend_um[actualClientSock[iC]].size());
						printf("\tsignal light data size %d\n", (int)MsgClient_c.TlsDataSend_um[actualClientSock[iC]].size());
						printf("\tdetector data size %d\n", (int)MsgClient_c.DetDataSend_um[actualClientSock[iC]].size());

						FILE* f = fopen(MasterLogName.c_str(), "a");
						//fprintf(f, "send complete\n");
						fprintf(f, "send client: %d\n", selfServerPortUserInput[iC]);
						if ((int)MsgClient_c.VehDataSend_um[actualClientSock[iC]].size() > 0) {
							fprintf(f, "\tveh: %d\n", (int)MsgClient_c.VehDataSend_um[actualClientSock[iC]].size());
						}
						if ((int)MsgClient_c.TlsDataSend_um[actualClientSock[iC]].size()) {
							fprintf(f, "\ttls: %d\n", (int)MsgClient_c.TlsDataSend_um[actualClientSock[iC]].size());
						}
						if ((int)MsgClient_c.DetDataSend_um[actualClientSock[iC]].size()) {
							fprintf(f, "\tdet: %d\n", (int)MsgClient_c.DetDataSend_um[actualClientSock[iC]].size());
						}
						fclose(f);

					}

					float simTimeSend = simTime;
					uint8_t simStateSend = 1;

					if (ENABLE_EXT_DYN) {
						if (isVeryFirstStep) {
							if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
								printf("ERROR: send to client fails\n");
								Sock_c.socketShutdown();
								exit(-1);
							};
						}
						else {
							if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
								printf("ERROR: send to client fails\n");
								Sock_c.socketShutdown();
								exit(-1);
							};
						}
					}
					else if (ENABLE_VEH_SIMULATOR) {
						//if (isVeryFirstStep && selfServerPortUserInput[iC] == Config_c.CarMakerSetup.CarMakerPort && Config_c.CarMakerSetup.EnableEgoSimulink) {
						//	continue;
						//}
						//else {
							if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
								printf("ERROR: send to client fails\n");
								Sock_c.socketShutdown();
								exit(-1);
							};
						//}
					}
					else {
						if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
							printf("ERROR: send to client fails\n");
							Sock_c.socketShutdown();
							exit(-1);
						};
					}

					if (ENABLE_VERBOSE) {
						printf("send complete\n");

						//FILE* f = fopen(MasterLogName.c_str(), "a");
						//fprintf(f, "send complete\n");
						//fclose(f);
					}

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
		}

		///****************************************************
		// Traffic Layer <<<<<===== Clients (Controller, Vehicle Simulator, Models)
		// RECV message from client (CAV CONTROLLER or VEHICLE)
		///****************************************************

		if (ENABLE_CLIENT) {
			try {
				MsgClient_c.clearRecvStorage();

				for (unsigned int iC = 0; iC < actualClientSock.size(); iC++) {

					int simStateRecv;
					float simTimeRecv;

					if (ENABLE_VERBOSE) {
						printf("receiving client at port %d\n", selfServerPortUserInput[iC]);

						FILE* f = fopen(MasterLogName.c_str(), "a");
						fprintf(f, "recv client: %d\n", selfServerPortUserInput[iC]);
						fclose(f);
					}

					// save received message into Msg_c recv storages
					if (Sock_c.recvData(actualClientSock[iC], &simStateRecv, &simTimeRecv, MsgClient_c) < 0) {
						if (WSAGetLastError() != WSAEINTR && WSAGetLastError() != WSAEFAULT) {
							printf("ERROR: receive from client fails\n");
						}
						Sock_c.socketShutdown();
						exit(-1);
					};

					if (ENABLE_VERBOSE) {

						for (auto& it : MsgClient_c.VehDataRecv_um) {
							string id = it.second.id;
							// check desired speed or desired acceleration
							if (MsgClient_c.VehicleMessageField_set.find("speedDesired") != MsgClient_c.VehicleMessageField_set.end()) {
								float spd = it.second.speedDesired;
								printf("\trecv id %s speed %f\n", id.c_str(), spd);

								FILE* f = fopen(MasterLogName.c_str(), "a");
								fprintf(f, "\tid %s spd %f\n", id.c_str(), spd);
								fclose(f);
							}
							else {
								float accel = it.second.accelerationDesired;
								printf("\trecv id %s accel %f\n", id.c_str(), accel);

								FILE* f = fopen(MasterLogName.c_str(), "a");
								fprintf(f, "\id %s accel %f\n", id.c_str(), accel);
								fclose(f);
							}


						}

						printf("recv complete\n");

						//FILE* f = fopen(MasterLogName.c_str(), "a");
						//fprintf(f, "recv complete\n");
						//fclose(f);
					}


					//!!!! if no current vehicle in SUMO, remove it from the recv list
					if (!ENABLE_VEH_SIMULATOR) {
						vector <string> idToRemove;
						for (auto& it : MsgClient_c.VehDataRecv_um) {
							//string id = it.first;
							//printf("current id %s\n", id);
							if (MsgServer_c.VehDataRecv_um.find(it.first) == MsgServer_c.VehDataRecv_um.end()) {
								idToRemove.push_back(it.first);

							}
						}
						for (int i = 0; i < idToRemove.size(); i++) {
							MsgClient_c.VehDataRecv_um.erase(idToRemove[i]);
						}
					}

					int aa = 1;

				}
			}
			catch (const std::exception& e) {
				Sock_c.socketShutdown();
				std::cout << e.what();
				exit(-1);
			}
			catch (...) {
				Sock_c.socketShutdown();
				printf("UNKNOWN ERROR: receive from client fails\n");
				exit(-1);
			}
		}
		else {
			//Sleep(100);
		}

		if (ENABLE_TIMING) {
			QueryPerformanceCounter(&StartingTime);
		}

		///****************************************************
		// VISSIM/SUMO <<<<<===== Traffic Layer
		// SEND to Traffic Simulator
		///****************************************************
		try {
			MsgServer_c.clearSendStorage();
			// this function distribute combined message to different server/clients
			if (ENABLE_CLIENT) {
				Traffic_c.parseSendMsg(MsgClient_c, MsgServer_c); //MsgClient_c recv storage => MsgServer_c send storage
			}
			else {
				Traffic_c.parseSendMsg(MsgServer_c, MsgServer_c);
			}

			if (ENABLE_REALSIM) {
				if (Traffic_c.sendToTrafficSimulator(simTime, MsgServer_c) < 0) {
					printf("ERROR: send to traffic simulator fails\n");
					Sock_c.socketShutdown();
					exit(-1);
				};
			}


			if (ENABLE_VERBOSE) {
				for (auto& it : MsgServer_c.VehDataSend_um) {
					for (int i = 0; i < it.second.size(); i++) {
						MsgServer_c.printVehData(it.second[i]);

						MsgServer_c.printVehDataToFile(MasterLogName, it.second[i]);

					}
				}
			}
		}
		catch (const std::exception& e) {
			Sock_c.socketShutdown();
			std::cout << e.what();
			exit(-1);
		}
		catch (...) {
			Sock_c.socketShutdown();
			printf("UNKNOWN ERROR: send to traffic simulator fails\n");
			exit(-1);
		}

		///****************************************************
		// go to next step
		///****************************************************
		if (isVeryFirstStep) {
			isVeryFirstStep = false;
		}

		ii = ii + 1;

	}

	Traffic_c.close();

}