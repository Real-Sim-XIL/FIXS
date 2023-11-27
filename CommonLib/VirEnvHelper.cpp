#include "VirEnvHelper.h"

using namespace std;

VirEnvHelper::VirEnvHelper() {

}


int VirEnvHelper::CM_Log(const char* MsgChar) {
	Log(MsgChar);
	return 0;
}
int VirEnvHelper::CM_LogErrF(const char* MsgChar) {
	LogErrF(EC_General, MsgChar);
	return 0;
}

void VirEnvHelper::shutdown() {
	Log("RealSim shutdown \n");

#ifdef RS_DSPACE
	try {
		Sock_c.socketShutdown();
		Sock_c.socketReset();
		//Log("RealSim shutdown socket size %d\n", Sock_c.serverSock.size());
		serverAddr.clear();
		serverPort.clear();
		Config_s.VehicleMessageField_v.clear();
	}
	catch (const std::exception& e) {
		std::cout << e.what();
		Log("Warning: RealSim shutdown failed\n");
	}
	catch (...) {
		Log("Warning: RealSim shutdown failed\n");
	}
#else
	//try {
	//	Sock_c.socketShutdown();
	//	Sock_c.socketReset();
	//	//Log("RealSim shutdown socket size %d\n", Sock_c.serverSock.size());
	//	serverAddr.clear();
	//	serverPort.clear();
	//	Config_c.resetConfig();
	//}
	//catch (const std::exception& e) {
	//	std::cout << e.what();
	//	Log("Warning: RealSim shutdown failed\n");
	//}
	//catch (...) {
	//	Log("Warning: RealSim shutdown failed\n");
	//}
#endif
	veryFirstStep = 1;

	TrafficSimulatorId2CarMakerId.clear();
	CmAvailableCarId_queue = queue<int>();
	CmAvailableTruckId_queue = queue<int>();
	CmAvailableBusId_queue = queue<int>();
	TrafficSimulatorId2Remove.clear();
}


//===================================================
//
//
//===================================================

//#ifndef RS_DSPACE
int VirEnvHelper::initialization(const char** errorMsg, const char* configPathInput, const char* signalTablePathInput) {
	//#else
	//int VirEnvHelper::initialization(const char** errorMsg, const char* signalTablePathInput) {
	//#endif
		//TCHAR NPath[MAX_PATH];
		//GetCurrentDirectory(MAX_PATH, NPath);

		// auto simStartTimestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
		// char simStartTimestampChar[100];
		// ctime_s(simStartTimestampChar, sizeof simStartTimestampChar, &simStartTimestamp);
		// fstream f(CmErrorFile, std::fstream::in | std::fstream::out | std::fstream::app);
		// f << endl << "=============================================" << endl;
		// f << "RealSim CarMaker Starts at  " << simStartTimestampChar << endl;
		// f.close();


		// char cwd[1024];
		// if (getcwd(cwd, sizeof(cwd)) != NULL) {
		   // Log("RealSim Current working dir: %s\n", cwd);
		// } else {
		   // Log("RealSim getcwd() error");
		// }

	veryFirstStep = 0;

	string errorMsgStr;

	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
	std::string str(buffer);

	fstream f(CmErrorFile, std::fstream::in | std::fstream::out | std::fstream::app);
	f << endl << "=============================================" << endl;
	f << "RealSim CarMaker Starts at  " << str << endl;
	f.close();

	configPath = configPathInput;
#ifndef RS_DSPACE
	if (Config_c.getConfig(configPath) < 0) {
		errorMsgStr = "RealSim: Read Configuration Yaml File Failed";
		*errorMsg = errorMsgStr.c_str();
		return ERROR_INIT_READ_CONFIG;
	}

	Msg_c.getConfig(Config_c);

	ENABLE_REALSIM = Config_c.CarMakerSetup.EnableCosimulation;
	ENABLE_SEPARATE_EGO_TRAFFIC = Config_c.CarMakerSetup.EnableEgoSimulink;
	SYNCHRONIZE_TRAFFIC_SIGNAL = Config_c.CarMakerSetup.SynchronizeTrafficSignal;
#else

	Log("RealSim read config\n");

	// reparser to common variables as the non dSPACE version
	ENABLE_REALSIM = Config_s.EnableCosimulation;
	ENABLE_SEPARATE_EGO_TRAFFIC = Config_s.EnableEgoSimulink;
	SYNCHRONIZE_TRAFFIC_SIGNAL = Config_s.SynchronizeTrafficSignal;

	Msg_c.VehicleMessageField_v = Config_s.VehicleMessageField_v;

	Log("RealSim message field: ");
	for (unsigned int i = 0; i < Msg_c.VehicleMessageField_v.size(); i++) {
		Msg_c.VehicleMessageField_set.insert(Msg_c.VehicleMessageField_v[i]);
		Log("%s, ", Msg_c.VehicleMessageField_v[i].c_str());
	}
	Log("\n");

#endif

	// check if vehicle class is defined
	if (Msg_c.VehicleMessageField_set.find("vehicleClass") == Msg_c.VehicleMessageField_set.end() || Msg_c.VehicleMessageField_set.find("heading") == Msg_c.VehicleMessageField_set.end() || Msg_c.VehicleMessageField_set.find("grade") == Msg_c.VehicleMessageField_set.end()) {
		*errorMsg = "RealSim: Must subscribe: id, speed, vehicleClass, heading, grade, speedDesired/accelerationDesired";
		//*errorMsg = const_cast<char*>(errorMsgStr.c_str());
		return ERROR_INIT_MSG_FIELD;
	}

	if (SYNCHRONIZE_TRAFFIC_SIGNAL) {
		// 
#ifdef RS_DSPACE
		string pattern = "RS_tmp";
		string rsTmpPath = configPath.substr(0, configPath.find(pattern));
		string siganlTableFullPath = rsTmpPath + "RS_tmp/" + signalTablePathInput;
#else
		string siganlTableFullPath = signalTablePathInput;
#endif
		Log("RS signal path %s\n", siganlTableFullPath.c_str());
		// read signal table
		readSignalTable(siganlTableFullPath.c_str());
	}

	//Log("RealSim init socket size %d\n", Sock_c.serverSock.size());

	// try to start RealSim socket connection
	try {
#ifndef RS_DSPACE
		// vehicle data port
		serverAddr.push_back(Config_c.SimulationSetup.TrafficLayerIP);
		serverPort.push_back(Config_c.CarMakerSetup.CarMakerPort);

		// if signal data, then use a separate port
		if (SYNCHRONIZE_TRAFFIC_SIGNAL) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficLayerIP);
			serverPort.push_back(Config_c.CarMakerSetup.TrafficSignalPort);
		}
#else
		serverAddr.push_back(Config_s.TrafficLayerIP);
		serverPort.push_back(Config_s.CarMakerPort);

		// if signal data, then use a separate port
		if (SYNCHRONIZE_TRAFFIC_SIGNAL) {
			serverAddr.push_back(Config_s.TrafficLayerIP);
			serverPort.push_back(Config_s.TrafficSignalPort);
		}
#endif

		Sock_c.socketSetup(serverAddr, serverPort); // connect to server Traffic Layer
		Sock_c.disableServerTrigger();
		Sock_c.disableWaitClientTrigger();
#ifdef RS_DEBUG
		Log("RealSim serverAddr[0] %s\n", serverAddr[0].c_str());
		Log("RealSim serverPort[0] %d\n", serverPort[0]);
#endif
		//Log("RealSim init socket size exit %d\n", Sock_c.serverSock.size());

		if (ENABLE_REALSIM) {
#ifdef RS_DEBUG
			Log("RealSim socket initConnection\n");
#endif
			if (Sock_c.initConnection(CmErrorFile) > 0) {
				printf("Connect to RealSim failed! Make sure start TrafficLayer first\n");
				*errorMsg = "RealSim: Initialize Socket Failed";
#ifdef RS_DEBUG
				Log("RealSim socket initConnection failed");
#endif
				return ERROR_INIT_SOCKET;
			}
		}
#ifdef RS_DEBUG
		Log("RealSim socket initConnection succeed\n");
#endif
	}
	catch (const std::exception& e) {
		Sock_c.socketShutdown();
		std::cout << e.what();
		printf("ERROR: initialize RealSim socket failed!\n");
		*errorMsg = "RealSim: Initialize Socket Failed";
		return ERROR_INIT_SOCKET;
	}
	catch (...) {
		Sock_c.socketShutdown();
		printf("UNKNOWN ERROR: initialize RealSim socket failed!\n");
		*errorMsg = "RealSim: Initialize Socket Failed";
		return ERROR_INIT_SOCKET;
	}

	return 0;
}

int VirEnvHelper::readSignalTable(const char* signalTablePathInput) {

	// Open an existing file
	string signalTablePathInput_str = signalTablePathInput;
	if (signalTablePathInput_str.substr(signalTablePathInput_str.size() - 4).compare(".csv") == 0) {

	}
	else {
		signalTablePathInput_str.append(".csv");
	}
	Log("SignalTable name %s\n", signalTablePathInput_str.c_str());

	ifstream SignalTableFile(signalTablePathInput_str);

	if (!SignalTableFile) {
		return -1;
	}

	string curLine;
	getline(SignalTableFile, curLine); // ignore the first line
	while (getline(SignalTableFile, curLine)) {

		istringstream lineString(curLine);

		string element;

		SignalTable_t Sig;

		// 'signalControllerName' : string
		getline(lineString, element, ',');
		Sig.signalControllerName = (element);

		//'signalGroupId' : int,
		getline(lineString, element, ',');
		Sig.signalGroupId = atoi(element.c_str());

		//'signalHeadId' : int,
		getline(lineString, element, ',');
		Sig.signalHeadId = atoi(element.c_str());

		//'CmTrafficLightIndex' : int,
		getline(lineString, element, ',');
		Sig.cmTrafficLightIndex = atoi(element.c_str());

		//'CmControllerId' : string,
		getline(lineString, element, ',');
		Sig.cmControllerId = (element);

		// if has CM traffic light
		if (Sig.cmTrafficLightIndex > -1) {
			if (SignalController2HeadIdTrfLightIndex.find(Sig.signalControllerName) != SignalController2HeadIdTrfLightIndex.end()) {
				SignalController2HeadIdTrfLightIndex[Sig.signalControllerName].push_back(make_pair(Sig.signalHeadId, Sig.cmTrafficLightIndex));
			}
			else {
				SignalController2HeadIdTrfLightIndex[Sig.signalControllerName] = { make_pair(Sig.signalHeadId, Sig.cmTrafficLightIndex) };
			}
		}
	}

	return 0;
}

int VirEnvHelper::runStep(double simTime, const char** errorMsg) {
	//FOR veh in RealSimReceived
	//  IF veh.id NOT in sumo2ipg :
	//      IF no available space
	//          throw error, ignore current vehicle
	//      ELSE
	//          pop CmAvailableId_queue
	//          add new sumo2ipg
	//      END
	//  ELSE
	//      pop CmVehicleToRemove set
	//  END
	//END
	//FOR veh.id in CmVehicleToRemove
	//  set position to nonsense
	//  add CmAvailableId_queue
	//  remove from sumo2ipg
	//END
	//FOR veh in RealSimReceived
	//  update position, speed, acceleration, etc.
	//END
	//FOR veh.id in sumo2ipg
	//  add to CmVehicleToRemove for next timestep
	//END

	string errorMsgStr;


	// run real sim step every 0.1 seconds and not do this step at simTime=0
	int simStateRecv = 0;
	float simTimeRecv = 0;
	unsigned int iS = 0;

	// ===========================================================================
	// 			initialize available CM id queue and move traffic objects far away
	// ===========================================================================
	if (simTime < 0.05) {
		try {
			for (int iObj = 0; iObj < Traffic.nObjs; iObj++) {
				VehFullData_t curTrfObjData;

				tTrafficObj* TrfObj = Traffic_GetByTrfId(iObj);

				string name = TrfObj->Cfg.Name; //

				// do not touch Traffic objects that are not RS related
				if (name.find(RealSimCarNamePattern) == string::npos && name.find(RealSimTruckNamePattern) == string::npos) {
					continue;
				}

				/*
				should check what character is in the name then determine vehile class/type
				*/
				if (name.find(RealSimCarNamePattern) != string::npos) {
					CmAvailableCarId_queue.push(iObj);
				}
				else if (name.find(RealSimTruckNamePattern) != string::npos) {
					CmAvailableTruckId_queue.push(iObj);
				}

				// set position to be nonsense
				TrfObj->t_0[0] = 0;
				TrfObj->t_0[1] = 0;
				TrfObj->t_0[2] = -5000;

			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			printf("ERROR: initialize traffic object position failed!\n");
			errorMsgStr = "RealSim: Initialize Traffic Objects Failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_INIT_TRAFFIC;
		}
		catch (...) {
			printf("UNKNOWN ERROR: initialize traffic object position failed!\n");
			errorMsgStr = "RealSim: Initialize Traffic Objects Failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_INIT_TRAFFIC;
		}


	}

	if (simTime > 1e-5 && abs((simTime) * 10 - ceil((simTime) * 10 - 0.5)) < 1e-5) {
		// ===========================================================================
		// 			receive from RealSim
		// ===========================================================================
		try {

			Msg_c.clearRecvStorage();

			if (ENABLE_REALSIM && simTime) {
				//Log("RealSim start receive data t=%f\n",simTime);
				//Log("RealSim serverSock size %d\n", Sock_c.serverSock.size());
				for (iS = 0; iS < Sock_c.serverSock.size(); iS++) {
					if (Sock_c.recvData(Sock_c.serverSock[iS], &simStateRecv, &simTimeRecv, Msg_c) < 0) {
						*errorMsg = "RealSim: Receive from traffic simulator failed";
						return ERROR_STEP_RECV_REALSIM;
					}
					else {
						//Log("RealSim received data t=%f\n",simTime);
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			errorMsgStr = "RealSim: Receive from traffic simulator failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_RECV_REALSIM;
		}
		catch (...) {
			errorMsgStr = "RealSim: Receive from traffic simulator failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_RECV_REALSIM;
		}

		// ===========================================================================
		// 			map received car Id to Cm id
		// ===========================================================================
		try {
			// for each received vehicle, map to cm id if needed
			for (auto it : Msg_c.VehDataRecv_um) {
				string idTs = it.second.id;
				string vehClass = it.second.vehicleClass;

#ifndef RS_DSPACE
				// skip if receive egoId states
				if (idTs.compare(Config_c.CarMakerSetup.EgoId) == 0) {
					continue;
				}
#else
				// skip if receive egoId states
				if (idTs.compare(Config_s.EgoId) == 0) {
					continue;
				}
#endif

				// if current received id already mapped, then remove from CmVehicleToRemove set
				// otherwise, need to map it to a new vehicle id
				if (TrafficSimulatorId2CarMakerId.find(idTs) == TrafficSimulatorId2CarMakerId.end()) {
					if (vehClass.find("car") != string::npos || vehClass.find("passenger") != string::npos || vehClass.find("private") != string::npos) {
						if (CmAvailableCarId_queue.size() == 0) {
							continue;
						}
						else {
							int idCm = CmAvailableCarId_queue.front();
							CmAvailableCarId_queue.pop();
							TrafficSimulatorId2CarMakerId[idTs] = idCm;
						}
					}
					else if (vehClass.find("truck") != string::npos) {
						if (CmAvailableTruckId_queue.size() == 0) {
							continue;
						}
						else {
							int idCm = CmAvailableTruckId_queue.front();
							CmAvailableTruckId_queue.pop();
							TrafficSimulatorId2CarMakerId[idTs] = idCm;
						}
					}

				}
				else {
					if (TrafficSimulatorId2Remove.find(idTs) != TrafficSimulatorId2Remove.end()) {
						TrafficSimulatorId2Remove.erase(TrafficSimulatorId2Remove.find(idTs));
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			errorMsgStr = "RealSim: Map received traffic simualtor id to CM id failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_MAP_ID;
		}
		catch (...) {
			errorMsgStr = "RealSim: Map received traffic simualtor id to CM id failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_MAP_ID;
		}

		// ===========================================================================
		// 			clean up vehicles that already gone
		// ===========================================================================
		try {
			// remove those ids that not exists since last time step
			for (string idTs : TrafficSimulatorId2Remove) {

				tTrafficObj* TrfObj = Traffic_GetByTrfId(TrafficSimulatorId2CarMakerId[idTs]);

				string name = TrfObj->Cfg.Name; //

				// do not touch Traffic objects that are not RS related
				if (name.find(RealSimCarNamePattern) == string::npos && name.find(RealSimTruckNamePattern) == string::npos) {
					continue;
				}

				// set position to be nonsense
				TrfObj->t_0[0] = 0;
				TrfObj->t_0[1] = 0;
				TrfObj->t_0[2] = -5000;

				// now these CmId becomes available
				if (name.find(RealSimCarNamePattern) != string::npos) {
					CmAvailableCarId_queue.push(TrafficSimulatorId2CarMakerId[idTs]);
				}
				else if (name.find(RealSimTruckNamePattern) != string::npos) {
					CmAvailableTruckId_queue.push(TrafficSimulatorId2CarMakerId[idTs]);
				}

				TrafficSimulatorId2CarMakerId.erase(idTs);

				// remove from TrafficState um for interpolation
				TrafficStateNext_um.erase(idTs);
				TrafficStatePrevious_um.erase(idTs);

			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			errorMsgStr = "RealSim: Remove arrived vehicle id failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_REMOVE_ID;
		}
		catch (...) {
			errorMsgStr = "RealSim: Remove arrived vehicle id failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_REMOVE_ID;
		}

		// ===========================================================================
		// 			move traffic position
		// ===========================================================================
		try {
			// update state
			TrafficSimulatorId2Remove.clear();
			for (auto iter : TrafficSimulatorId2CarMakerId) {
				string idTs = iter.first;
				int idCm = iter.second;

				tTrafficObj* TrfObj = Traffic_GetByTrfId(idCm);

				TrafficSimulatorId2Remove.insert(idTs);

				// -------------------
				// interpolation to smooth position change as CM position updates faster than Traffic Simulaotr 0.1second
				// -------------------
				// position, heading, grade in VehDataAuxiliary are defined according to CM convention
				VehDataAuxiliary_t vehDataNext;
				VehDataAuxiliary_t vehDataPrevious;

				double heading = Msg_c.VehDataRecv_um[idTs].heading;
				// RS position is at front of vehicle. CM is rearmost surface
				// RS z is ground of vehicle, CM is CoM
				vehDataNext.positionX = Msg_c.VehDataRecv_um[idTs].positionX - TrfObj->Cfg.l * sin(heading * M_PI / 180);
				vehDataNext.positionY = Msg_c.VehDataRecv_um[idTs].positionY - TrfObj->Cfg.l * cos(heading * M_PI / 180);
				vehDataNext.positionZ = Msg_c.VehDataRecv_um[idTs].positionZ + TrfObj->Cfg.h / 2 + TrfObj->Cfg.zOff - TrfObj->Cfg.l * sin(Msg_c.VehDataRecv_um[idTs].grade); // 
				// RS heading in degree, north is 0 degree, then increasing clockwise. i.e., east is 90 degree.
				// convert to east 0 radian, north pi/2 radian, south -pi/2 radian system
				vehDataNext.yaw = 0;
				if (heading >= 0 && heading <= 1.5 * 180) {
					vehDataNext.yaw = 0.5 * M_PI - heading * M_PI / 180;
				}
				else {
					vehDataNext.yaw = 2.5 * M_PI - heading * M_PI / 180;
				}
				// RS positive grade climbing hill, but CM negative climbing hill
				vehDataNext.pitch = -Msg_c.VehDataRecv_um[idTs].grade;

				vehDataNext.lightIndicators = Msg_c.VehDataRecv_um[idTs].lightIndicators;

				// the current received state is for next 0.1 time step
				double simTimeNext = ceil(simTime * 10 + 0.001) / 10;
				TrafficStateNext_um[idTs] = make_pair(simTimeNext, vehDataNext);
				// if this vehicle enters first time, no interpolation yet
				if (TrafficStatePrevious_um.find(idTs) == TrafficStatePrevious_um.end()) {
					vehDataPrevious = vehDataNext;
				}
				else
				{
					// as we received one new state from RealSim, current state essentially now becomes previous state
					vehDataPrevious.positionX = TrfObj->t_0[0];
					vehDataPrevious.positionY = TrfObj->t_0[1];
					vehDataPrevious.positionZ = TrfObj->t_0[2];
					vehDataPrevious.yaw = TrfObj->r_zyx[2];
					vehDataPrevious.pitch = TrfObj->r_zyx[1];
				}
				TrafficStatePrevious_um[idTs] = make_pair(simTime, vehDataPrevious);
			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			errorMsgStr = "RealSim: Update traffic object states failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_UPDATE_STATE;
		}
		catch (...) {
			errorMsgStr = "RealSim: Update traffic object states failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_UPDATE_STATE;
		}

	}


	// ===========================================================================
	// 			sync traffic signal light
	// ===========================================================================
	if (SYNCHRONIZE_TRAFFIC_SIGNAL) {
		try {
			// loop over each signal light
			for (auto it : Msg_c.TlsDataRecv_um) {
				string tlsId = it.second.name;
				string tlsState = it.second.state;

				// if can find the tlsId, then synchronize it
				if (SignalController2HeadIdTrfLightIndex.find(tlsId) != SignalController2HeadIdTrfLightIndex.end()) {
					// the unordered_map contains a list of head id and TrfLight index pair, so loop over each to synchronize
					for (auto it : SignalController2HeadIdTrfLightIndex[tlsId]) {
						TrfLight.Objs[get<1>(it)].State = tlsChar2CmState(tlsState.at(get<0>(it)));
					}
				}

			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			errorMsgStr = "RealSim: Sync traffic signal light failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_SYNC_TRAFFIC_SIGNAL;
		}
		catch (...) {
			errorMsgStr = "RealSim: Sync traffic signal light failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_SYNC_TRAFFIC_SIGNAL;
		}
	}

	// ===========================================================================
	// 			sending out
	// ===========================================================================
	if (abs((simTime) * 10 - ceil((simTime) * 10 - 0.5)) < 1e-5) {
		try {
			// send ego states
			VehFullData_t VehDataSend;

			if (!ENABLE_SEPARATE_EGO_TRAFFIC || simTime < 1e-5) {
#ifndef RS_DSPACE
				VehDataSend.id = Config_c.CarMakerSetup.EgoId;
				VehDataSend.type = Config_c.CarMakerSetup.EgoType;
#else
				VehDataSend.id = Config_s.EgoId;
				VehDataSend.type = Config_s.EgoType;
#endif
				VehDataSend.speed = (float)Vehicle.v;

				if (Vehicle.Yaw >= -M_PI && Vehicle.Yaw <= 0.5 * M_PI) {
					VehDataSend.heading = (float)((-Vehicle.Yaw + 0.5 * M_PI) * 180 / M_PI);
				}
				else {
					VehDataSend.heading = (float)((-Vehicle.Yaw + 2.5 * M_PI) * 180 / M_PI);
				}

				VehDataSend.positionX = (float)Vehicle.PoI_Pos[0] + Vehicle.Cfg.Bdy1_CoM[0] * sin(VehDataSend.heading * M_PI / 180);
				VehDataSend.positionY = (float)Vehicle.PoI_Pos[1] + Vehicle.Cfg.Bdy1_CoM[0] * cos(VehDataSend.heading * M_PI / 180);
				VehDataSend.positionZ = (float)Vehicle.PoI_Pos[2] - Vehicle.Cfg.Bdy1_CoM[2];

				VehDataSend.acceleration = (float)0;
				VehDataSend.color = (uint32_t)0;
				VehDataSend.linkId = (string)"None";
				VehDataSend.laneId = (int32_t)0;
				VehDataSend.distanceTravel = (float)0;
				VehDataSend.speedDesired = (float)Vehicle.v;
				VehDataSend.accelerationDesired = (float)0;

				//Name	Bit
				//VEH_SIGNAL_BLINKER_RIGHT	0
				//VEH_SIGNAL_BLINKER_LEFT	1
				//VEH_SIGNAL_BLINKER_EMERGENCY	2
				//VEH_SIGNAL_BRAKELIGHT	3
				//VEH_SIGNAL_FRONTLIGHT	4
				//VEH_SIGNAL_FOGLIGHT	5
				//VEH_SIGNAL_HIGHBEAM	6
				//VEH_SIGNAL_BACKDRIVE	7
				//VEH_SIGNAL_WIPER	8
				//VEH_SIGNAL_DOOR_OPEN_LEFT	9
				//VEH_SIGNAL_DOOR_OPEN_RIGHT	10
				//VEH_SIGNAL_EMERGENCY_BLUE	11
				//VEH_SIGNAL_EMERGENCY_RED	12
				//VEH_SIGNAL_EMERGENCY_YELLOW	13
				VehDataSend.lightIndicators = (uint16_t) (VehicleControl.Lights.Brake * 8 + VehicleControl.Lights.IndL * 2 + VehicleControl.Lights.IndR);
			}

			if (ENABLE_REALSIM) {

				for (iS = 0; iS < Sock_c.serverSock.size(); iS++) {

					uint8_t simStateSend = simStateRecv;

					int iByte = 0;

					char sendServerBuffer[8096];
					// !! WE don't know the message size yet, pack a dummy header
					Msg_c.packHeader(simStateSend, simTimeRecv, MSG_HEADER_SIZE, sendServerBuffer, &iByte);
					Sock_c.sendServerByte[iS] = { MSG_HEADER_SIZE };

					// only pack ego information if one connection to RealSim
					// if two connections, still send ego at beginning to add this ego car in SUMO/VISSIM
					if ((!ENABLE_SEPARATE_EGO_TRAFFIC || simTime < 1e-5) && iS == 0) {
						Msg_c.packVehData(VehDataSend, sendServerBuffer, &Sock_c.sendServerByte[iS]);
					}

					// repack the header with the correct size
					iByte = 0;
					Msg_c.packHeader(simStateSend, simTimeRecv, Sock_c.sendServerByte[iS], sendServerBuffer, &iByte);

					if (send(Sock_c.serverSock[iS], sendServerBuffer, Sock_c.sendServerByte[iS], 0) != Sock_c.sendServerByte[iS]) {
						char buff[1000];
#ifdef WIN32
						snprintf(buff, sizeof(buff), "RealSim: Send failed, %d", WSAGetLastError());
						fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
#else
						snprintf(buff, sizeof(buff), "RealSim: Send failed, %d, %s", errno, strerror(errno));
						fprintf(stderr, "%s: \n", "send() failed");
#endif
						* errorMsg = buff;
						return ERROR_STEP_SEND_EGO;
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cout << e.what();
			errorMsgStr = "RealSim: Send ego states failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_SEND_EGO;
		}
		catch (...) {
			errorMsgStr = "RealSim: Send ego states failed";
			*errorMsg = errorMsgStr.c_str();
			return ERROR_STEP_SEND_EGO;
		}
	}

	try {
		// refresh XXXX Hz
		// // int refreshRate = 0.1;
#ifndef RS_DSPACE
		int refreshRate = (int)1 / Config_c.CarMakerSetup.TrafficRefreshRate;
#else
		int refreshRate = (int)1 / Config_s.TrafficRefreshRate;
#endif
		if (abs(simTime * refreshRate - ceil(simTime * refreshRate - 0.5)) < 1e-5) {

			for (auto iter : TrafficSimulatorId2CarMakerId) {
				string idTs = iter.first;
				int idCm = iter.second;

				double tPrevious = get<0>(TrafficStatePrevious_um[idTs]);
				VehDataAuxiliary_t vehDataPrevious = get<1>(TrafficStatePrevious_um[idTs]);
				double tNext = get<0>(TrafficStateNext_um[idTs]);
				VehDataAuxiliary_t vehDataNext = get<1>(TrafficStateNext_um[idTs]);

				// do interpolation
				double posX = (vehDataNext.positionX - vehDataPrevious.positionX) / (tNext - tPrevious) * (simTime - tPrevious) + vehDataPrevious.positionX;
				double posY = (vehDataNext.positionY - vehDataPrevious.positionY) / (tNext - tPrevious) * (simTime - tPrevious) + vehDataPrevious.positionY;
				double posZ = (vehDataNext.positionZ - vehDataPrevious.positionZ) / (tNext - tPrevious) * (simTime - tPrevious) + vehDataPrevious.positionZ;
				// don't do any interpolation on yaw and pitch, just use that of vehDataNext
				double pitch = vehDataNext.pitch;
				double yaw = vehDataNext.yaw;

				// need grade to find out elevation and rotation angle

				// find vehicle turning indicators
				int lightIndicators = vehDataNext.lightIndicators;
				//Name	Bit
				//VEH_SIGNAL_BLINKER_RIGHT	0
				//VEH_SIGNAL_BLINKER_LEFT	1
				//VEH_SIGNAL_BLINKER_EMERGENCY	2
				//VEH_SIGNAL_BRAKELIGHT	3
				//VEH_SIGNAL_FRONTLIGHT	4
				bool indRight = (lightIndicators & (1 << 0)) >> 0;
				bool indLeft = (lightIndicators & (1 << 1)) >> 1;
				bool indBrake = (lightIndicators & (1 << 3)) >> 3;

				//if (indRight || indLeft || indBrake) {
				//	int aa = 1;
				//}

				tTrafficObj* TrfObj = Traffic_GetByTrfId(idCm);


				TrfObj->t_0[0] = posX;
				TrfObj->t_0[1] = posY;
				TrfObj->t_0[2] = posZ;

				TrfObj->r_zyx[1] = pitch;
				TrfObj->r_zyx[2] = yaw;


				tLights* pLights = Traffic_Lights_GetByObjId(TrfObj->Cfg.ObjId);

				// Brake light on (boolean)
				//Turn indicator
				   // - 1 = Right; 0 = Off; 1 = Left
				Lights_Set_CtrlElem_Ignition(pLights, 1);

				Lights_Set_LightElem_Brake(pLights, indBrake);
				if (indRight) {
					Lights_Set_CtrlElem_Indicator(pLights, -1);
				}
				else if (indLeft) {
					Lights_Set_CtrlElem_Indicator(pLights, 1);
				}
				else {
					Lights_Set_CtrlElem_Indicator(pLights, 0);
				}

				//int ind = Lights_Get_CtrlElem_Indicator(pLights);
				//int indL = Lights_Get_LightElem_IndL(pLights);
				//int indR = Lights_Get_LightElem_IndR(pLights);
				//int indB = Lights_Get_LightElem_Brake(pLights);
				//int ig = Lights_Get_CtrlElem_Ignition(pLights);
				//Log("obj %d, sumo L %d R %d B %d, ipg L %d R %d B %d\n", idCm, indLeft, indRight, indBrake, indL, indR, indB);

			}

			//{
			//	// test turning indicator      
			//	tTrafficObj* TrfObj = Traffic_GetByTrfId(40);

			//	tLights* pLights = Traffic_Lights_GetByObjId(TrfObj->Cfg.ObjId);

			//	//Lights_Get_LightElem_Brake(pLights)
			//	   // Lights_Get_CtrlElem_Indicator(pLights)

			//	// Brake light on (boolean)
			//	//Turn indicator
			//	   // - 1 = Right; 0 = Off; 1 = Left
			//	Lights_Set_CtrlElem_Ignition(pLights, 1);

			//	if (simTime < 30) {
			//		Lights_Set_LightElem_Brake(pLights, 1);
			//		//Lights_Set_LightElem_IndL(pLights, 1);
			//		Lights_Set_CtrlElem_Indicator(pLights, 1);
			//	}
			//	else if (simTime >= 30 && simTime < 60) {
			//		Lights_Set_LightElem_Brake(pLights, 0);
			//		//Lights_Set_LightElem_IndL(pLights, 0);
			//		//Lights_Set_LightElem_IndR(pLights, 0);
			//		Lights_Set_CtrlElem_Indicator(pLights, 0);
			//	}
			//	else {
			//		Lights_Set_LightElem_Brake(pLights, 1);
			//		//Lights_Set_LightElem_IndR(pLights, 1);
			//		Lights_Set_CtrlElem_Indicator(pLights, -1);
			//	}

			//	int ind = Lights_Get_CtrlElem_Indicator(pLights);
			//	int indL = Lights_Get_LightElem_IndL(pLights);
			//	int indR = Lights_Get_LightElem_IndR(pLights);
			//	int ig = Lights_Get_CtrlElem_Ignition(pLights);
			//	Log("ind %d, indL %d, indR %d, ig %d\n", ind, indL, indR, ig);

			//}
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what();
		errorMsgStr = "RealSim: Refresh traffic visualization failed";
		*errorMsg = errorMsgStr.c_str();
		return ERROR_STEP_REFRESH_TRAFFIC;
	}
	catch (...) {
		errorMsgStr = "RealSim: Refresh traffic visualization failed";
		*errorMsg = errorMsgStr.c_str();
		return ERROR_STEP_REFRESH_TRAFFIC;
	}

	return 0;
}



tTLState VirEnvHelper::tlsChar2CmState(char charState) {
	//0: All lights off
	//1 : Green light on
	//2 : Yellow light on
	//3 : Red light on
	//4 : Red - Yellow light on

	//r	FOO	'red light' for a signal - vehicles must stop
	//y	FOO	'amber (yellow) light' for a signal - vehicles will start to decelerate if far away from the junction, otherwise they pass
	//g	FOO	'green light' for a signal, no priority - vehicles may pass the junction if no vehicle uses a higher priorised foe stream, otherwise they decelerate for letting it pass.They always decelerate on approach until they are within the configured visibility distance
	//G	FOO	'green light' for a signal, priority - vehicles may pass the junction
	//s	FOO	'green right-turn arrow' requires stopping - vehicles may pass the junction if no vehicle uses a higher priorised foe stream.They always stop before passing.This is only generated for junction type traffic_light_right_on_red.
	//u	FOO	'red+yellow light' for a signal, may be used to indicate upcoming green phase but vehicles may not drive yet(shown as orange in the gui)
	//o	FOO	'off - blinking' signal is switched off, blinking light indicates vehicles have to yield
	//O	FOO	'off - no signal' signal is switched off, vehicles have the right of way

	tTLState CmState = State_Off;
	// r->3, y->2, g/G->1, u->4, O->0
	if (charState == 'r') {
		CmState = State_Red;
	}
	else if (charState == 'y') {
		CmState = State_Yellow;
	}
	else if (charState == 'g' || charState == 'G') {
		CmState = State_Green;
	}
	else if (charState == 'u') {
		CmState = State_RedYellow;
	}
	else if (charState == 'O') {
		CmState = State_Off;
	}

	return CmState;
}
