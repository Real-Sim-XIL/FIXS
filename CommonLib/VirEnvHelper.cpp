#include "VirEnvHelper.h"

using namespace std;

VirEnvHelper::VirEnvHelper() {

}


int VirEnvHelper::CM_Log(const char* MsgChar){
	Log(MsgChar);
	return 0;
}
int VirEnvHelper::CM_LogErrF(const char* MsgChar){
	LogErrF(EC_General, MsgChar);
	return 0;
}

void VirEnvHelper::shutdown() {
	Log("RealSim shutdown \n");

#ifdef DSRTLX
	//Sock_c.socketShutdown();
#endif
	veryFirstStep = 1;

	TrafficSimulatorId2CarMakerId.clear();
	CmAvailableCarId_queue = queue<int>();
	CmAvailableTruckId_queue = queue<int>();
	CmAvailableBusId_queue = queue<int>();
	TrafficSimulatorId2Remove.clear();
}

int VirEnvHelper::initialization(const char** errorMsg) {
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

#ifndef RS_DSPACE
	if (Config_c.getConfig(configPath) < 0) {
		errorMsgStr = "RealSim: Read Configuration Yaml File Failed";
		*errorMsg = errorMsgStr.c_str();
		return ERROR_INIT_READ_CONFIG;
	}

	Msg_c.getConfig(Config_c);

	ENABLE_REALSIM = Config_c.CarMakerSetup.EnableCosimulation;
	ENABLE_SEPARATE_EGO_TRAFFIC = Config_c.CarMakerSetup.EnableEgoSimulink;
#else
	Log("RealSim read config\n");

	// reparser to common variables as the non dSPACE version
	ENABLE_REALSIM = Config_s.EnableCosimulation;
	ENABLE_SEPARATE_EGO_TRAFFIC = Config_s.EnableEgoSimulink;
	
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


	try {
#ifndef RS_DSPACE
		serverAddr[0] = Config_c.CarMakerSetup.CarMakerIP;
		serverPort[0] = Config_c.CarMakerSetup.CarMakerPort;
#else
		serverAddr[0] = Config_s.TrafficLayerIP;
		serverPort[0] = Config_s.CarMakerPort;
#endif

		Sock_c.socketSetup(serverAddr, serverPort); // connect to server Traffic Layer
		Sock_c.disableServerTrigger();
		Sock_c.disableWaitClientTrigger();
#ifdef RS_DEBUG
		Log("RealSim serverAddr[0] %s\n", serverAddr[0].c_str());
		Log("RealSim serverPort[0] %d\n", serverPort[0]);
#endif
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

	// ===========================================================================
	// 			receiving and handling
	// ===========================================================================
	 // run real sim step every 0.1 seconds and not do this step at simTime=0
	int simStateRecv = 0;
	float simTimeRecv = 0;
	int iS = 0;

	if (simTime < 0.05){
		try {
			for (int iObj = 0; iObj < Traffic.nObjs; iObj++) {
				VehFullData_t curTrfObjData;

				tTrafficObj* TrfObj = Traffic_GetByTrfId(iObj);

				string name = TrfObj->Cfg.Name; //

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

		try {

			Msg_c.clearRecvStorage();

			if (ENABLE_REALSIM && simTime) {
				//Log("RealSim start receive data t=%f\n",simTime);
				if (Sock_c.recvData(Sock_c.serverSock[iS], &simStateRecv, &simTimeRecv, Msg_c) < 0) {
					*errorMsg = "RealSim: Receive from traffic simulator failed";
					return ERROR_STEP_RECV_REALSIM;
				}else {
					//Log("RealSim received data t=%f\n",simTime);
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


		try {
			// remove those ids that not exists since last time step
			for (string idTs : TrafficSimulatorId2Remove) {

				tTrafficObj* TrfObj = Traffic_GetByTrfId(TrafficSimulatorId2CarMakerId[idTs]);

				 string name = TrfObj->Cfg.Name; //

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
				 vehDataNext.positionZ = Msg_c.VehDataRecv_um[idTs].positionZ + TrfObj->Cfg.h / 2 + TrfObj->Cfg.zOff;
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
			 }

			if (ENABLE_REALSIM) {
				 uint8_t simStateSend = simStateRecv;

				 int iByte = 0;

				 char sendServerBuffer[8096];
				 // !! WE don't know the message size yet, pack a dummy header
				Msg_c.packHeader(simStateSend, simTimeRecv, MSG_HEADER_SIZE, sendServerBuffer, &iByte);
				Sock_c.sendServerByte[iS] = { MSG_HEADER_SIZE };

				// only pack ego information if one connection to RealSim
				// if two connections, still send ego at beginning to add this ego car in SUMO/VISSIM
				if (!ENABLE_SEPARATE_EGO_TRAFFIC || simTime < 1e-5) {
					Msg_c.packVehData(VehDataSend, sendServerBuffer, &Sock_c.sendServerByte[iS]);
				}

				// repack the header with the correct size
				iByte = 0;
				Msg_c.packHeader(simStateSend, simTimeRecv, Sock_c.sendServerByte[iS], sendServerBuffer, &iByte);

				if (send(Sock_c.serverSock[iS], sendServerBuffer, Sock_c.sendServerByte[iS], 0) != Sock_c.sendServerByte[iS]) {
					char buff[1000];
#ifdef WIN32
					snprintf(buff, sizeof(buff), "RealSim: Send ego states failed, %d", WSAGetLastError());
					fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
#else
					snprintf(buff, sizeof(buff), "RealSim: Send ego states failed, %d, %s", errno, strerror(errno));
					fprintf(stderr, "%s: \n", "send() failed");
#endif
					*errorMsg = buff;
					return ERROR_STEP_SEND_EGO;
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

				 tTrafficObj* TrfObj = Traffic_GetByTrfId(idCm);


				 TrfObj->t_0[0] = posX;
				 TrfObj->t_0[1] = posY;
				 TrfObj->t_0[2] = posZ;

				 TrfObj->r_zyx[1] = pitch;
				 TrfObj->r_zyx[2] = yaw;
			 }

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
