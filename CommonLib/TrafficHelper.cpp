#include "TrafficHelper.h"


//const unsigned short selfServerPort[NSERVER] = { 420 };

using namespace std;

using namespace libtraci;

//CentralCtrl CentralCtrl_g;

// The convention is that, Server always provide service to the Client. 
// i.e. Manager will be the Server to provide traffic data service to all possible clients, e.g. Controller, Communication, Vehicle. 
// The Controller can be another service provider to Vehicle to command desired speed
// The Communication will be another service provider to Controller to provide traffic information. 
// 
// [              ] - [Controller] - [      ]
// [Another Server] - [Controller] - [Client]
// [              ] - [Controller] - [      ]
//
// First, connect to other server
// Then, wait all clients connect
// Next, signal other server to start service
// Last, start own service => return to the caller of Socket initConnection
TrafficHelper::TrafficHelper(){
}

void TrafficHelper::connectionSetup(string trafficIp, int trafficPort, int nClientInput) {

	nClient = nClientInput;

	edgeHasSubscribed = false;
	//nEdgeSubscribe = 0;

	//system("sumo -c \"C:\Users\y0n\Dropbox (ORNL)\2_projects\1_2_sumo\1_4_speedHarmTest\speedHarmTest.sumocfg\" --remote-port 1337 ");

	Simulation::init(trafficPort, 60, trafficIp);
	Simulation::setOrder(1);

	//traci.connect(trafficIp, trafficPort);

	//traci.setOrder(1);

	/********************************************
	* GET VEH SUBSCRIPTION
	*********************************************/
	//VehDataSubscribeList.push_back(libsumo::TRACI_ID_LIST);
	VehDataSubscribeList.push_back(libsumo::VAR_TYPE);
	VehDataSubscribeList.push_back(libsumo::VAR_SPEED);
	VehDataSubscribeList.push_back(libsumo::VAR_POSITION3D);
	VehDataSubscribeList.push_back(libsumo::VAR_ANGLE); // north 0 deg, clockwise
	VehDataSubscribeList.push_back(libsumo::VAR_COLOR);
	VehDataSubscribeList.push_back(libsumo::VAR_ROAD_ID);
	VehDataSubscribeList.push_back(libsumo::VAR_LANE_INDEX);
	VehDataSubscribeList.push_back(libsumo::VAR_DISTANCE);
	VehDataSubscribeList.push_back(libsumo::VAR_LANEPOSITION);

	VehDataSubscribeList.push_back(libsumo::VAR_LANE_ID);
	VehDataSubscribeList.push_back(libsumo::VAR_VEHICLECLASS);
	VehDataSubscribeList.push_back(libsumo::VAR_ROUTE_INDEX);

	// testing new data
	VehDataSubscribeList.push_back(libsumo::VAR_ACCELERATION);
	//// no speedDesired accelerationDesired
	//// will return id and distance
	//VehDataSubscribeList.push_back(libsumo::VAR_LEADER);
	//// then need to query speed of that vehicle
	//// or use followSpeed??
	//VehDataSubscribeList.push_back(libsumo::VAR_FOLLOW_SPEED);
	// signal light
	// will return tlsID (signal light id), tlsIndex (signal head id), distance, state
	//VehDataSubscribeList.push_back(libsumo::VAR_NEXT_TLS);
	// speed limit
	VehDataSubscribeList.push_back(libsumo::VAR_ALLOWED_SPEED);
	VehDataSubscribeList.push_back(libsumo::VAR_SPEED_FACTOR);
	// get next speed limit
	// handle it at beginning when this vehicle enters
	VehDataSubscribeList.push_back(libsumo::VAR_VIA);
	// next link
	// retrieved as part of the speed limit information
	
	//// lane change
	//// need to check if this even works
	//VehDataSubscribeList.push_back(libsumo::CMD_CHANGELANE);

	// get slope of the road
	VehDataSubscribeList.push_back(libsumo::VAR_SLOPE);

	// get signals
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
	//VehDataSubscribeList.push_back(libsumo::VAR_SIGNALS);

	// -------------------
	// These variables are subscribed for testing purposes
	// -------------------
	VehDataSubscribeList.push_back(libsumo::VAR_SPEED_WITHOUT_TRACI);



	//VehDataSubscribeList.push_back(libsumo::VAR_NEXT_TLS); // Returns upcoming traffic lights, along with distanceand state [(tlsID, tlsIndex, distance, state), ...]

	//VehDataSubscribeList.push_back(libsumo::VAR_LEADER);/*Returns the id of the leading vehicleand its distance, if the string is empty, no leader was found within the given range.Only vehicles ahead on the currently list of best lanes are considered(see above).This means, the leader is only valid until the next lane - change maneuver.The returned distance is measured from the ego vehicle front bumper + minGap to the back bumper of the leader vehicle.*/

	//VehDataSubscribeList.push_back(libsumo::VAR_ALLOWED_SPEED); //Returns the maximum allowed speed on the current lane regarding speed factor in m/s for this vehicle.
	//
	//VehDataSubscribeList.push_back(libsumo::VAR_SPEED_FACTOR); // Returns the road speed multiplier for this vehicle[double]

	//VehDataSubscribeList.push_back(libsumo::VAR_SLOPE); // Retrieves the slope at the current vehicle position in degrees





	/********************************************
	* GET DETECTOR SUBSCRIPTION
	*********************************************/
	// obtain detector ids of the selected intersection and subscribe to results
	if (ENABLE_DET_SUB) {
		//string detSelectPattern = DET_SEL_PATTERN; // select which intersection we want to do signal control in the loop SCIP
		////===================================================
		//// Retreive DETECTOR configuration for the scenario BEFORE simulation starts
		////===================================================
		//vector <string> detAreaAllId_v = traci.lanearea.getIDList();
		//vector <string> detInductAllId_v = traci.inductionloop.getIDList();

		//vector <int> detSubscribeList;

		//detSubscribeList.push_back(libsumo::LAST_STEP_VEHICLE_NUMBER);
		//for (int iD = 0; iD < detAreaAllId_v.size(); iD++) {
		//	if (detAreaAllId_v[iD].find(detSelectPattern) != std::string::npos) {
		//		detSelectId_v.push_back(detAreaAllId_v[iD]);
		//		traci.lanearea.subscribe(detAreaAllId_v[iD], detSubscribeList, 0, tSimuEnd);
		//	}
		//}
	}



	/********************************************
	* GET Speed Limit of every edge, lane
	*********************************************/
	vector <string> laneList = Lane::getIDList();

	vector <string> vehClassList;

	vector <string> vehTypeList = VehicleType::getIDList();
	for (int i = 0; i < vehTypeList.size(); i++) {
		string vehType = vehTypeList[i];
		string vehClass = VehicleType::getVehicleClass(vehType);

		vehClassList.push_back(vehClass);
	}

	for (int i = 0; i < laneList.size(); i++) {
		string laneId = laneList[i];
		string edgeId = Lane::getEdgeID(laneId);
		
		vector <string> allowClassList = Lane::getAllowed(laneId);
		vector <string> disallowClassList = Lane::getDisallowed(laneId);

		if (allowClassList.size() == 0 && disallowClassList.size() == 0) {
			for (int iC = 0; iC < vehClassList.size(); iC++) {
				string vClass = vehClassList[iC];
				LaneVehClass2SpeedLimit_um[make_pair(laneId, vClass)] = Lane::getMaxSpeed(laneId);

				EdgeVehClass2SpeedLimit_um[make_pair(edgeId, vClass)] = Lane::getMaxSpeed(laneId);
			}
		}
		else {
			for (int iC = 0; iC < allowClassList.size(); iC++) {
				string vClass = allowClassList[iC];
				LaneVehClass2SpeedLimit_um[make_pair(laneId, vClass)] = Lane::getMaxSpeed(laneId);

				EdgeVehClass2SpeedLimit_um[make_pair(edgeId, vClass)] = Lane::getMaxSpeed(laneId);
			}
		}
	}

	vector <string> edgeList = Edge::getIDList();
	for (int i = 0; i < edgeList.size(); i++) {
		AllEdgeList.insert(edgeList[i]);
	}

}

void TrafficHelper::connectionSetup(int nClientInput) {

	nClient = nClientInput;

	edgeHasSubscribed = false;
	//nEdgeSubscribe = 0;

}

void TrafficHelper::enableVehSub() {
	ENABLE_VEH_SUB = true;
}

void TrafficHelper::disableVehSub() {
	ENABLE_VEH_SUB = false;
}

void TrafficHelper::enableDetSub() {
	ENABLE_DET_SUB = true;
}

void TrafficHelper::disableDetSub() {
	ENABLE_DET_SUB = false;
}

void TrafficHelper::selectVISSIM(vector <int> vissimSockInput, vector <string> vissimSockNames) {
	SUMO_OR_VISSIM = "VISSIM";

	for (int i = 0; i < vissimSockInput.size(); i++) {
		vissimSock.push_back(vissimSockInput[i]);
		vissimSockName_um[vissimSockInput[i]] = vissimSockNames[i];
	}
	//for (int i = 0; i < clientSockInput.size(); i++) {
	//	clientSock.push_back(clientSockInput[i]);
	//}
}

void TrafficHelper::selectSUMO() {
	SUMO_OR_VISSIM = "SUMO";
}

void TrafficHelper::close() {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		Simulation::close();
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {

	}
	else {

	}
}


void TrafficHelper::getConfig() {

	if (Config_c->SimulationSetup.EnableVerboseLog) {
		ENABLE_VERBOSE = true;
	}
	else {
		ENABLE_VERBOSE = false;
	}

	if (Config_c->ApplicationSetup.VehicleSubscription.size() > 0) {
		ENABLE_VEH_SUB = true;
	}
	else {
		if (Config_c->XilSetup.EnableXil) {
			ENABLE_VEH_SUB = true;
		}
		else {
			ENABLE_VEH_SUB = false;
		}
	}
	if (Config_c->ApplicationSetup.DetectorSubscription.size() > 0) {
		ENABLE_DET_SUB = true;
	}
	else {
		if (Config_c->XilSetup.DetectorSubscription.size() > 0) {
			ENABLE_DET_SUB = true;
		}
		else{
			ENABLE_DET_SUB = false;
		}
	}
	if (Config_c->ApplicationSetup.SignalSubscription.size() > 0) {
		ENABLE_SIG_SUB = true;
	}
	else {
		if (Config_c->XilSetup.SignalSubscription.size() > 0) {
			ENABLE_SIG_SUB = true;
		}
		else {
			ENABLE_SIG_SUB = false;
		}
	}
	if (Config_c->SimulationSetup.EnableExternalDynamics) {
		ENABLE_EXT_DYN = true;
	}
	else {
		ENABLE_EXT_DYN = false;
	}
	tSimuEnd = Config_c->SimulationSetup.SimulationEndTime;

	// get vehicle message that needs to be sent out
	VehicleMessageField_v = Config_c->SimulationSetup.VehicleMessageField;

	for (int i = 0; i < VehicleMessageField_v.size(); i++) {
		VehicleMessageField_set.insert(VehicleMessageField_v[i]);
	}

	// get subscription information
	// variable to store subscription that need to check
	// if application layer is disabled, xil is enabled then use subscription of xil, this means traffic layer directly connects to xil
	if (!Config_c->ApplicationSetup.EnableApplicationLayer && Config_c->XilSetup.EnableXil) {
		Config_c->getVehSubscriptionList(Config_c->XilSetup.VehicleSubscription, edgeSubscribeId_v, vehicleSubscribeId_v, subscribeAllVehicle, pointSubscribeId_v, vehicleTypeSubscribedId_v);
	}
	// otherwise find out the subscription of application layer
	else {
		Config_c->getVehSubscriptionList(Config_c->ApplicationSetup.VehicleSubscription, edgeSubscribeId_v, vehicleSubscribeId_v, subscribeAllVehicle, pointSubscribeId_v, vehicleTypeSubscribedId_v);
	}
	
	vehicleHasSubscribed_v.clear();
	vehicleHasSubscribed_v.resize(vehicleSubscribeId_v.size());
	fill(vehicleHasSubscribed_v.begin(), vehicleHasSubscribed_v.end(), false);

	Config_c->CarMakerSetup.TrafficRefreshRate = 0.123;

	int aa = 1;
}


int TrafficHelper::sendToTrafficSimulator(double simTime, MsgHelper Msg_c) {
	
	int sendStatus = 0;

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		sendStatus = this->sendToSUMO(simTime, Msg_c);
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {
		sendStatus = this->sendToVISSIM(simTime, Msg_c);
	}
	else {

	}

	return sendStatus;
}


int TrafficHelper::addEgoVehicle(double simTime) {

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		if (ENABLE_VEH_SIMULATOR) {
			// !!!!check if what received is ego vehicle 
			// use default type if not specified!!
			string idStr = Config_c->CarMakerSetup.EgoId;
			// if ego not exist yet, add it
			string typeStr = Config_c->CarMakerSetup.EgoType;

			// if is empty
			if (typeStr.size() == 0) {
				Vehicle::add(idStr, "");
			}
			else {
				Vehicle::add(idStr, "", typeStr);
			}
			Vehicle::setColor(idStr, libsumo::TraCIColor(255, 0, 0));
		}

		return 1;
	}
	else {
		return 0;
	}

}


int TrafficHelper::checkIfEgoExist(double* simTime) {

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		*simTime = Simulation::getTime();

		vector <string> VehIdInSimulator = Vehicle::getIDList();

		// check if subscribed vheicle is in the network
		for (auto& iter : vehicleSubscribeId_v) {
			string idStr = iter.first;

			// if any one of vehicle has not been subscribed yet
			if (find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) != VehIdInSimulator.end()) {
				return 1;
			}
			else {
				return 0;
			}

			// only check the first vehicle, which considered as the ego vehicle
			// break;
		}
	}
	else {
		return 0;
	}

}

int TrafficHelper::getSimulationTime(double* simTime) {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		*simTime = Simulation::getTime();
		return 1;
	}
	else {
		return 0;
	}
}

int TrafficHelper::runSimulation(double endTime) {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		Simulation::step(endTime);
		return 1;
	}
	else {
		return 0;
	}
}


int TrafficHelper::sendToSUMO(double simTime, MsgHelper Msg_c) {

	int sendStatus = 0;

	if (ENABLE_VERBOSE) {
		printf("sending to SUMO\n ");

		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "sending to SUMO\n ");
		fclose(f);

	}

	//int nVeh = VehIdInSimulator.size();

	//if (nVeh > 0) {
	//	for (int iV = 0; iV < nVeh; iV++) {
	//		string idStr = VehIdInSimulator[iV];
	//		//unsigned int id = stoi(idStr.substr(12));

	//		//if (idStr.compare("flow_0.10") == 0) {
	//		//	Vehicle::setColor(idStr, libsumo::TraCIColor(255, 0, 0, 255));
	//		//}
	//		//if (idStr.compare("flow_1.9") == 0) {
	//		//	Vehicle::setColor(idStr, libsumo::TraCIColor(0, 166, 255, 255));
	//		//}

	//		//if (std::find(Msg_c.VehIdRecv_v.begin(), Msg_c.VehIdRecv_v.end(), idStr) != Msg_c.VehIdRecv_v.end()) {
	//		if (Msg_c.VehDataRecv_um.find(idStr) != Msg_c.VehDataRecv_um.end()) {
	//			Vehicle::setSpeed(idStr, Msg_c.VehDataRecv_um[idStr].speed);
	//			Vehicle::setSpeedMode(idStr, 0); // most checks off


	//			if (ENABLE_VERBOSE) {
	//				//cout << "setSpeed ego # " << idStr << " speed " << VehCmdRecv_um[idStr].speed << endl;
	//				printf("setSpeed ego %s speed %.4f\n", idStr.c_str(), Msg_c.VehDataRecv_um[idStr].speed);
	//				//cout << "setSpeed ego # " << idStr << " speed " << VehCmdRecv_um[idStr].speed << '\n';
	//			}
	//		}
	//		else {
	//			//Vehicle::setSpeedMode(idStr, 31);
	//		}
	//	}

	//	if (Msg_c.TlsIdRecv_v.size() > 0) {
	//		if (Msg_c.TlsDataRecv_um.find(tlsSelect) != Msg_c.TlsDataRecv_um.end()) {
	//			string setTlsState = Msg_c.TlsDataRecv_um[tlsSelect].state;
	//			traci.trafficlights.setRedYellowGreenState(tlsSelect, setTlsState);

	//			if (ENABLE_VERBOSE) {
	//				//cout << "set " << " time " << simTime << " traffic light " << tlsSelect << " state " << setTlsState << endl;
	//				printf("set time %.1f traffic light %s state %s \n", simTime, tlsSelect.c_str(), setTlsState.c_str());
	//			}
	//		}
	//	}
	//}

	try {
		vector <string> VehIdInSimulator = Vehicle::getIDList();
		
		//uint32_t color = 4278190335;
		//uint8_t r = (color >> 24) & 0xFF;
		//uint8_t g = (color >> 16) & 0xFF;
		//uint8_t b = (color >> 8) & 0xFF;
		//uint8_t a = (color) & 0xFF;
		//Vehicle::setColor("flow_0.0", libsumo::TraCIColor(r, g, b, a));

		for (int iV = 0; iV < VehIdInSimulator.size(); iV++) {
			//Vehicle::setSpeedMode(VehIdInSimulator[iV], 31); // default speed mode
		}

		for (int iV = 0; iV < Msg_c.VehDataSend_um[0].size(); iV++) {
			string idStr = Msg_c.VehDataSend_um[0][iV].id;
			
			double speed;
			if (VehicleMessageField_set.find("speedDesired") != VehicleMessageField_set.end()) {
				speed = Msg_c.VehDataSend_um[0][iV].speedDesired;
			}
			else {
				//double accel = std::any_cast<float>(Msg_c.VehDataSend_um[0][iV]["accelerationDesired"]);
				//speed = 
				printf("ERROR: SUMO does not support control by accelerationDesired yet\n");
				printf("\tPlease select speedDesired in VehicleMessageField instead\n");
				exit(-1);
			}


			if (ENABLE_VERBOSE) {
				if (!ENABLE_VEH_SIMULATOR && find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) != VehIdInSimulator.end()) {
					double speedOld = Vehicle::getSpeed(idStr);
					printf("Set SUMO id %s from speed %.4f to speed %.4f\n", idStr.c_str(), speedOld, speed);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "Set SUMO id %s from speed %.4f to speed %.4f\n", idStr.c_str(), speedOld, speed);
					fclose(f);
				}
			}

			// if vehicle simulator and is ego
			if (ENABLE_VEH_SIMULATOR && idStr.compare(Config_c->CarMakerSetup.EgoId) == 0) {
				// !!!!check if what received is ego vehicle 
				// use default type if not specified!!
				
				//// if ego not exist yet, add it
				//if (find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) == VehIdInSimulator.end()) {
				//	string typeStr = Msg_c.VehDataSend_um[0][iV].type;

				//	// if is empty
				//	if (typeStr.size() == 0) {
				//		Vehicle::add(idStr, "");
				//	}
				//	else {
				//		Vehicle::add(idStr, "", typeStr);
				//	}
				//	Vehicle::setColor(idStr, libsumo::TraCIColor(255, 0, 0));
				//}
				// otherwise, move it
				{
					if (ENABLE_EXT_DYN) {
						Vehicle::setPreviousSpeed(idStr, speed); // setting speed at (k) will be reflected at (k) "immediately", i.e., be considered in the next integration
					}
					else {

						double positionX = (double)Msg_c.VehDataSend_um[0][iV].positionX;
						double positionY = (double)Msg_c.VehDataSend_um[0][iV].positionY;
						double positionZ = (double)Msg_c.VehDataSend_um[0][iV].positionZ;
						double heading = (double)Msg_c.VehDataSend_um[0][iV].heading;

						Vehicle::moveToXY(idStr, "", -1, positionX, positionY, heading, 6); // keepRoute 110 => 6
						//bit0(keepRoute = 1 when only this bit is set)
							//1: The vehicle is mapped to the closest edge within it's existing route. If no suitable position is found within 100m mapping fails with an error.
							//0 : The vehicle is mapped to the closest edge within the network.If that edge does not belong to the original route, the current route is replaced by a new route which consists of that edge only.If no suitable position is found within 100m mapping fails with an error.When using the sublane model the best lateral position that is fully within the lane will be used.Otherwise, the vehicle will drive in the center of the closest lane.
						//bit1(keepRoute = 2 when only this bit is set)
							//1 : The vehicle is mapped to the exact position in the network(including the exact lateral position).If that position lies outside the road network, the vehicle stops moving on it's own accord until it is placed back into the network with another TraCI command. (if keeproute = 3, the position must still be within 100m of the vehicle route)
							//0 : The vehicle is always on a road
						//bit2(keepRoute = 4 when only this bit is set)
							//1 : lane permissions are ignored when mapping
							//0 : The vehicle is mapped only to lanes that allow it's vehicle class

					}

				}
			}
			else {
				if (1 && find(VehIdInSimulator.begin(), VehIdInSimulator.end(), idStr) != VehIdInSimulator.end()) {
					if (ENABLE_EXT_DYN) {
						Vehicle::setPreviousSpeed(idStr, speed); // setting speed at (k) will be reflected at (k) "immediately", i.e., be considered in the next integration
					}
					else {
						Vehicle::setSpeed(idStr, speed); // speed set at (k) essentially will be reflected at (k+1), not considered in the integration

						/*
						bit0: Regard safe speed
						bit1 : Regard maximum acceleration
						bit2 : Regard maximum deceleration
						bit3 : Regard right of way at intersections(only applies to approaching foe vehicles outside the intersection)
						bit4 : Brake hard to avoid passing a red light
						bit5 : Disregard right of way within intersections(only applies to foe vehicles that have entered the intersection).
						*/

						Vehicle::setSpeedMode(idStr, Config_c->SumoSetup.SpeedMode); // 000000 most checks off
						//Vehicle::setSpeedMode(idStr, 0); // 000000 most checks off
						//Vehicle::setSpeedMode(idStr, 24); // 011000
						//Vehicle::setSpeedMode(idStr, 8); // 001000

						//Vehicle::setSpeedFactor(idStr, 1);
					}

					// change vehicle color if needed
					if (VehicleMessageField_set.find("color") != VehicleMessageField_set.end()) {
						uint32_t color = Msg_c.VehDataSend_um[0][iV].color;
						uint8_t r = (color >> 24) & 0xFF;
						uint8_t g = (color >> 16) & 0xFF;
						uint8_t b = (color >> 8) & 0xFF;
						uint8_t a = (color) & 0xFF;
						Vehicle::setColor(idStr, libsumo::TraCIColor(r, g, b, a));
					}

				}
			}


		}


		if (ENABLE_SIG_SUB) {
			for (int iS = 0; iS < Msg_c.TlsDataSend_um[0].size(); iS++) {
				string idStr = Msg_c.TlsDataSend_um[0][iS].name;

				TrafficLight::setRedYellowGreenState(idStr, Msg_c.TlsDataSend_um[0][iS].state);
			}
		}


		if (ENABLE_VERBOSE) {
			printf("send SUMO complete\n ");

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "send SUMO complete\n ");
			fclose(f);
		}

	}
	catch (const std::exception& e) {
		std::cout << e.what();
		return -1;
	}
	catch (...) {
		printf("UNKNOWN ERROR: send to SUMO fails\n");
		return -1;
	}

		//if (Msg_c.TlsIdRecv_v.size() > 0) {
		//	if (Msg_c.TlsDataRecv_um.find(tlsSelect) != Msg_c.TlsDataRecv_um.end()) {
		//		string setTlsState = Msg_c.TlsDataRecv_um[tlsSelect].state;
		//		traci.trafficlights.setRedYellowGreenState(tlsSelect, setTlsState);

		//		if (ENABLE_VERBOSE) {
		//			//cout << "set " << " time " << simTime << " traffic light " << tlsSelect << " state " << setTlsState << endl;
		//			printf("set time %.1f traffic light %s state %s \n", simTime, tlsSelect.c_str(), setTlsState.c_str());
		//		}
		//	}
		//}


	return sendStatus;
}

int TrafficHelper::sendToVISSIM(double simTime, MsgHelper Msg_c) {

	int sendStatus = 0;

	SocketHelper Sock_temp;
	
	try {
		for (int iClient = 0; iClient < vissimSock.size(); iClient++) {
			if (ENABLE_VERBOSE) {
				printf("send to VISSIM %s\n", vissimSockName_um[vissimSock[iClient]].c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "send to VISSIM %s\n", vissimSockName_um[vissimSock[iClient]].c_str());
				fclose(f);

			}

			//if (vissimSock.size() > 1 && vissimSockName_um[vissimSock[i]].find(SIGNAL_SOCK_PATTERN) != string::npos) {
			//	// if is signal controller
			//	if (abs(round(simTime / (vissimSignalStep * vissimBaseDt)) - simTime / (vissimSignalStep * vissimBaseDt)) < 1e-5) {
			//		Sock_temp.sendData(vissimSock[i], i, simTime, 1, Msg_c);
			//	}
			//}
			//else {
			//	Sock_temp.sendData(vissimSock[i], i, simTime, 1, Msg_c);
			//}
			sendStatus = Sock_temp.sendData(vissimSock[iClient], iClient, simTime, 1, Msg_c);
		}

		if (ENABLE_VERBOSE) {
			printf("send VISSIM complete\n");

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "send VISSIM complete\n");
			fclose(f);

		}

	}
	catch (const std::exception& e) {
		std::cout << e.what();
		return -1;
	}
	catch (...) {
		printf("UNKNOWN ERROR: send to VISSIM fails\n");
		return -1;
	}

	return sendStatus;

}

void TrafficHelper::parseSendMsg(MsgHelper MsgIn_c, MsgHelper& MsgOut_c) {

	// MsgClient_c/MsgIn recv => MsgServer_c/MsgOut send

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		// SUMO use a dummy socket index 0
		MsgOut_c.VehDataSend_um[0] = {}; 
		MsgOut_c.TlsDataSend_um[0] = {}; 
		MsgOut_c.DetDataSend_um[0] = {}; 
		for (auto it : MsgIn_c.VehDataRecv_um) {
			MsgOut_c.VehDataSend_um[0].push_back(it.second);
		}
		for (auto it : MsgIn_c.TlsDataRecv_um) {
			MsgOut_c.TlsDataSend_um[0].push_back(it.second);
		}
		for (auto it : MsgIn_c.DetDataRecv_um) {
			MsgOut_c.DetDataSend_um[0].push_back(it.second);
		}
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {
		for (int i = 0; i < vissimSock.size(); i++) {
			// if current socket is signal, then only send signal
			if (vissimSockName_um[vissimSock[i]].find(SIGNAL_SOCK_PATTERN) != string::npos) {
				MsgOut_c.VehDataSend_um[vissimSock[i]] = {};
				MsgOut_c.TlsDataSend_um[vissimSock[i]] = {};
				MsgOut_c.DetDataSend_um[vissimSock[i]] = {};
				for (auto it : MsgIn_c.TlsDataRecv_um) {
					MsgOut_c.TlsDataSend_um[vissimSock[i]].push_back(it.second);
				}
				for (auto it : MsgIn_c.DetDataRecv_um) {
					MsgOut_c.DetDataSend_um[vissimSock[i]].push_back(it.second);
				}
			}
			else {
				MsgOut_c.VehDataSend_um[vissimSock[i]] = {};
				MsgOut_c.TlsDataSend_um[vissimSock[i]] = {};
				MsgOut_c.DetDataSend_um[vissimSock[i]] = {};
				for (auto it : MsgIn_c.VehDataRecv_um) {
					MsgOut_c.VehDataSend_um[vissimSock[i]].push_back(it.second);
				}
			}
		}
	}
	
}

void TrafficHelper::runOneStepSimulation() {
	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		Simulation::step();
		//traci.simulationStep();
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {

	}
	else {

	}
}

int TrafficHelper::recvFromTrafficSimulator(double* simTime, MsgHelper& Msg_c) {
	int recvStatus = 0;

	if (SUMO_OR_VISSIM.compare("SUMO") == 0) {
		recvStatus = this->recvFromSUMO(simTime, Msg_c);
	}
	else if (SUMO_OR_VISSIM.compare("VISSIM") == 0) {
		recvStatus = this->recvFromVISSIM(simTime, Msg_c);
	}
	else {

	}

	return recvStatus;
}

int TrafficHelper::recvFromSUMO(double* simTime, MsgHelper& Msg_c) {
	int recvStatus = 0;

	Msg_c.clearSendStorage();

	VehIdInSimulator.clear();
	
	*simTime = Simulation::getTime();
	VehIdInSimulator = Vehicle::getIDList();
	
	int nVeh = VehIdInSimulator.size(); // number of vehicles

	if (ENABLE_VERBOSE) {
		printf("SUMO time step %f\n", *simTime);

		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "SUMO time step %f\n", *simTime);
		fclose(f);

	}

	//=================
	// SUMO will have vehicles except for the first few time steps
	//=================
	int nVehSend = 0;

	if (ENABLE_VEH_SUB && nVeh > 0) {

		// only subscribe once
		if (!edgeHasSubscribed) {
			// (self, objectID, domain, dist, varIDs=None, begin=-1073741824.0, end=-1073741824.0, parameters=None)


			// -------------------
			//  subscribe edge
			// -------------------
			for (auto& iter: edgeSubscribeId_v) {
				double radius = 0;
				string id = iter;

				Edge::subscribeContext(id, libsumo::CMD_GET_VEHICLE_VARIABLE, 100, VehDataSubscribeList, 0, tSimuEnd);
			}

			edgeHasSubscribed = true;

		}

		if (!pointHasSubscribed) {
			// (self, objectID, domain, dist, varIDs=None, begin=-1073741824.0, end=-1073741824.0, parameters=None)

			// -------------------
			//  subscribe point
			// -------------------
			//pointNamePoi_v.clear();
			int i = 0; 
			for (auto& iter: pointSubscribeId_v) {
				double x = get<0>(iter.second);
				double y = get<1>(iter.second);
				double z = get<2>(iter.second);
				double r = get<3>(iter.second);

				/*Both polygonsand points of interest may be located within a "layer".Shapes with lowest layer values are below those with a higher layer number.The network itself is drawn as layer 0. An additional file may contain definitions for both points of interestand polygons.*/
				//imgFile	string	A bitmap to use for rendering this polygon
				//angle	float	angle of rendered image in degree
				//lineWidth	double	Drawing width of unfilled polygons in m, default 1

				//string poiName = "RealSimPOI_"+to_string(i);
				string poiName = iter.first;
				/*int 	r,
					int 	g,
					int 	b,
					int 	a = 255*/
				libsumo::TraCIColor color(255,0,255); // use magenta as color 
				string type = "RealSim";
				int layer = 999; // put it as a very high layer
				string imgFile = ""; // no image file
				double width = 0; // width	float	width of rendered image in meters
				double height = 0; // height	float	height of rendered image in meters
				double angle = 0; // angle	float	angle of rendered image in degree
				POI::add(poiName, x, y, color, type, layer, imgFile, width, height, angle);
				POI::subscribeContext(poiName, libsumo::CMD_GET_VEHICLE_VARIABLE, r, VehDataSubscribeList, 0, tSimuEnd);

				//pointNamePoi_v.push_back(poiName);
				i++;
			}

			pointHasSubscribed = true;

		}

		if (!allVehicleHasSubscribed) {
			// -------------------
			//  subscribe vehicle
			// -------------------
			
			// get list of all vehicles entered network
			vector <string> vehDepartedId_v = Simulation::getDepartedIDList();

			allVehicleHasSubscribed = true;
			// only able to get vehicle subscription for vehicles already in the network
			int i = 0;
			for (auto & iter: vehicleSubscribeId_v) {
				// if any one of vehicle has not been subscribed yet
				if (!vehicleHasSubscribed_v[i]) {
					allVehicleHasSubscribed = false;
					// id of the vehicle to be subscribed
					string id = iter.first;
					// if the vehicle to subscribe just entered the network
					if (find(vehDepartedId_v.begin(), vehDepartedId_v.end(), id)!=vehDepartedId_v.end()) {
						double radius = iter.second;

						Vehicle::subscribeContext(id, libsumo::CMD_GET_VEHICLE_VARIABLE, radius, VehDataSubscribeList, 0, tSimuEnd);

						vehicleHasSubscribed_v[i] = true;
					}

					i++;
				}
				
			}

			// !!! need to sub all vehicles

			//while Simulation::getMinExpectedNumber() > 0:
			//for veh_id in Simulation::getDepartedIDList() :
			//	Vehicle::subscribe(veh_id, [traci.constants.VAR_POSITION])
			//	positions = Vehicle::getAllSubscriptionResults()
			//	traci.simulationStep()


		}
		else {
			int aa = 1;

		}

		// this might make it slightly faster to not get repeated vehicles
		unordered_set <string> processedVehId_us;


		// ===========================================================================
		// 			GET SUBSCRIBED VEHICLE
		// ===========================================================================
		libsumo::ContextSubscriptionResults VehicleSubscribeRaw;
		VehicleSubscribeRaw = Vehicle::getAllContextSubscriptionResults();

		//{
		//int i = 0;
		//for (auto& it : vehicleSubscribeId_v) {
		//	if (vehicleHasSubscribed_v[i]) {
		for (auto& it : VehicleSubscribeRaw) {
			//string id = it.first;
			libsumo::SubscriptionResults VehDataSubscribeResults = it.second;

			for (auto& iter : VehDataSubscribeResults) {

				string tempvehId = iter.first;

				VehFullData_t CurVehData;

				if (processedVehId_us.find(tempvehId) == processedVehId_us.end()) {
					processedVehId_us.insert(tempvehId);
				}
				else {
					continue;
				}

				this->parserSumoSubscription(iter.second, tempvehId, CurVehData);
				//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId]

				//=================
				// save to Msg_c recv buffer
				//=================
				//Msg_c.VehDataRecvAll_v.push_back(CurVehData);
				if (Msg_c.VehDataRecv_um.size() < 30) {
					Msg_c.VehDataRecv_um[tempvehId] = CurVehData;
				}


				if (ENABLE_VERBOSE) {
					float speed = CurVehData.speed;
					printf("recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);
					fclose(f);

				}

			}

		}

	/*		i++;

		}
		}*/

		// ===========================================================================
		// 			GET SUBSCRIBED point
		// ===========================================================================
		libsumo::ContextSubscriptionResults PointSubscribeRaw;
		PointSubscribeRaw = POI::getAllContextSubscriptionResults();

		for (auto& it : PointSubscribeRaw) {
			string poiName = it.first;
			libsumo::SubscriptionResults VehDataSubscribeResults = it.second;

			for (auto& iter : VehDataSubscribeResults) {

				string tempvehId = iter.first;

				VehFullData_t CurVehData;

				if (processedVehId_us.find(tempvehId) == processedVehId_us.end()) {
					processedVehId_us.insert(tempvehId);
				}
				else {
					continue;
				}

				this->parserSumoSubscription(iter.second, tempvehId, CurVehData);
				//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId]

				//=================
				// save to Msg_c recv buffer
				//=================
				//Msg_c.VehDataRecvAll_v.push_back(CurVehData);
				Msg_c.VehDataRecv_um[tempvehId] = CurVehData;


				if (ENABLE_VERBOSE) {
					float speed = CurVehData.speed;
					printf("recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);
					fclose(f);
				}
			}
		}


		// ===========================================================================
		// 			GET SUBSCRIBED EDGE
		// ===========================================================================
		libsumo::ContextSubscriptionResults EdgeSubscribeRaw;
		EdgeSubscribeRaw = Edge::getAllContextSubscriptionResults();


		if (edgeHasSubscribed) {
			nVehSend = 0;

			int temp = 1;

			for (auto & it: EdgeSubscribeRaw) {
				libsumo::SubscriptionResults VehDataSubscribeResults = it.second;

				auto iter = VehDataSubscribeResults.begin();
				nVehSend = min((int)VehDataSubscribeResults.size(), 200);

				vector <string> tempVehIdList;
				//tempVehIdList.push_back(egoIdVec[iC]);
				for (int iV = 0; iV < VehDataSubscribeResults.size(); iV++) {
					//if (iter->first != egoIdVec[iC]) {
					tempVehIdList.push_back(iter->first);
					//}
					iter++;
				}

				for (int iV = 0; iV < nVehSend; iV++) {
					VehFullData_t CurVehData;

					string tempvehId = tempVehIdList[iV];

					if (processedVehId_us.find(tempvehId) == processedVehId_us.end()) {
						processedVehId_us.insert(tempvehId);
					}
					else {
						continue;
					}

					this->parserSumoSubscription(VehDataSubscribeResults[tempvehId], tempvehId, CurVehData);
					//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId]



					//Msg_c.packVehData(CurVehData, tempVehDataBuffer[iC], &tempVehDataByte[iC]);
					//tempVehDataSend_v.push_back(CurVehData);

					//tempVehIdSend_v.push_back(tempvehId);

					//=================
					// save to Msg_c recv buffer          
					//=================
					//Msg_c.VehDataRecvAll_v.push_back(CurVehData);
					Msg_c.VehDataRecv_um[tempvehId] = CurVehData;


					if (ENABLE_VERBOSE) {
						float speed = CurVehData.speed;
						printf("recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);

						FILE* f = fopen(MasterLogName.c_str(), "a");
						fprintf(f, "recv SUMO veh id %s veh speed %.4f\n", tempvehId.c_str(), speed);
						fclose(f);

					}

					//}
					/*VehData_t iVehFullData;
					Sock_c.depackVehData(tempVehDataBuffer[iC]+3, &iVehFullData);*/
				}
			}
			//nVehSend = tempVehIdSend_v.size();
		}

		//=================
		// remove vehicle from list
		//=================
		vector <string> vehArrivedIdList = Simulation::getArrivedIDList();
		for (int i = 0; i < vehArrivedIdList.size(); i++) {
			VehicleId2EdgeList_um.erase(vehArrivedIdList[i]);
		}

	}

	//char tempDetDataBuffer[NCLIENT_MACRO][8096];
	//int tempDetDataByte[NCLIENT_MACRO] = { 0 };

	if (ENABLE_DET_SUB) {
		if (!DetectorSubscriptionFlags.patternHasSubscribed) {
			//===================================================
			// Retreive DETECTOR configuration for the scenario BEFORE simulation starts
			//===================================================
			vector <string> detAreaAllId_v = LaneArea::getIDList();
			vector <string> detInductAllId_v = InductionLoop::getIDList();

			// obtain detector ids of the selected intersection and subscribe to results

			vector <int> detSubscribeList = {libsumo::LAST_STEP_VEHICLE_NUMBER};

			for (auto it: Config_c->SubscriptionDetectorList.pattern_v){
				for (int iD = 0; iD < detAreaAllId_v.size(); iD++) {
					if (detAreaAllId_v[iD].find(it) != std::string::npos) {
						LaneArea::subscribe(detAreaAllId_v[iD], detSubscribeList, 0, tSimuEnd);
					}
				}
			}

			DetectorSubscriptionFlags.patternHasSubscribed = true;

		}

		libsumo::SubscriptionResults DetSubscribeRaw;
		DetSubscribeRaw = LaneArea::getAllSubscriptionResults();


		vector <DetectorData_t> tempDetData_v;
		
		std::shared_ptr<libsumo::TraCIInt> tempIntPtr;

		for (auto& it : DetSubscribeRaw) {
			DetectorData_t curDet;

			curDet.id = 0;
			curDet.name = it.first;
			tempIntPtr = static_pointer_cast<libsumo::TraCIInt> (it.second[libsumo::LAST_STEP_VEHICLE_NUMBER]);
			if (tempIntPtr->value > 0) {
				curDet.state = 1;
			}
			else {
				curDet.state = 0;
			};

			tempDetData_v.push_back(curDet);
		}

		Msg_c.DetDataRecv_um["NA"] = make_tuple(0, "NA", tempDetData_v);

	}

	if (ENABLE_SIG_SUB) {

	}

	return recvStatus;
}


void TrafficHelper::parserSumoSubscription(libsumo::TraCIResults VehDataSubscribeTraciResults, std::string vehId, VehFullData_t& CurVehData) {

	// if does not have this vehicle yet
	if (VehicleId2EdgeList_um.find(vehId) == VehicleId2EdgeList_um.end()) {
		vector <string> edgeList = Vehicle::getRoute(vehId);
		VehicleId2EdgeList_um[vehId] = edgeList;
		//vector <libsumo::TraCIConnection> nextLinkList = Vehicle::getNextLinks(vehId);
		//int aa = 1;
	}

	//libsumo::TraCIResults VehDataSubscribeTraciResults = VehDataSubscribeResults[tempvehId];
	std::shared_ptr<libsumo::TraCIString> tempStringPtr;
	std::shared_ptr<libsumo::TraCIInt> tempIntPtr;
	std::shared_ptr<libsumo::TraCIDouble> tempDoublePtr;
	std::shared_ptr<libsumo::TraCIPosition> tempPositionPtr;
	std::shared_ptr<libsumo::TraCIColor> tempColorPtr;
	std::shared_ptr<libsumo::TraCIDouble> tempDoublePtr2;

	CurVehData.id = vehId;

	//for (int iD = 0; iD < VehDataSubscribeList.size(); iD++) {
	tempStringPtr = static_pointer_cast<libsumo::TraCIString> (VehDataSubscribeTraciResults[libsumo::VAR_TYPE]);
	CurVehData.type = tempStringPtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SPEED]);
	CurVehData.speed = tempDoublePtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SPEED_WITHOUT_TRACI]);
	float speedWithoutTraci = tempDoublePtr->value;


	tempPositionPtr = static_pointer_cast<libsumo::TraCIPosition> (VehDataSubscribeTraciResults[libsumo::VAR_POSITION3D]);
	CurVehData.positionX = tempPositionPtr->x;
	CurVehData.positionY = tempPositionPtr->y;
	CurVehData.positionZ = tempPositionPtr->z;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_ANGLE]);
	CurVehData.heading = tempDoublePtr->value;

	tempColorPtr = static_pointer_cast<libsumo::TraCIColor> (VehDataSubscribeTraciResults[libsumo::VAR_COLOR]);
	uint8_t r = tempColorPtr->r;
	uint8_t g = tempColorPtr->g;
	uint8_t b = tempColorPtr->b;
	uint8_t a = tempColorPtr->a;
	CurVehData.color = (r << 24) + (g << 16) + (b << 8) + a;

	tempStringPtr = static_pointer_cast<libsumo::TraCIString> (VehDataSubscribeTraciResults[libsumo::VAR_ROAD_ID]);
	CurVehData.linkId = tempStringPtr->value;

	tempIntPtr = static_pointer_cast<libsumo::TraCIInt> (VehDataSubscribeTraciResults[libsumo::VAR_LANE_INDEX]);
	CurVehData.laneId = (int32_t) tempIntPtr->value;

	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_DISTANCE]);
	//tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_LANEPOSITION]);
	CurVehData.distanceTravel = (float) tempDoublePtr->value;


	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_ACCELERATION]);
	CurVehData.acceleration = tempDoublePtr->value;

	CurVehData.speedDesired = CurVehData.speed;
	CurVehData.accelerationDesired = 0.0;

	//=================
	// get preceding vehicle
	//=================
	pair<string, double> leaderIdNSpeed = Vehicle::getLeader(vehId, 1000);
	CurVehData.precedingVehicleId = get<0>(leaderIdNSpeed);
	CurVehData.precedingVehicleDistance = get<1>(leaderIdNSpeed);
	CurVehData.hasPrecedingVehicle = 0;
	CurVehData.precedingVehicleSpeed = -1.0;
	if (CurVehData.precedingVehicleId.compare("") != 0) {
		CurVehData.hasPrecedingVehicle = 1;
		CurVehData.precedingVehicleSpeed = Vehicle::getSpeed(CurVehData.precedingVehicleId);
	}

	//=================
	// get signal information
	//=================
	vector <libsumo::TraCINextTLSData> nextTlsList = Vehicle::getNextTLS(vehId);

	if (nextTlsList.size() > 0) {
		CurVehData.signalLightId = nextTlsList[0].id;
		CurVehData.signalLightHeadId = nextTlsList[0].tlIndex;
		CurVehData.signalLightDistance = nextTlsList[0].dist;
		char tlsState = nextTlsList[0].state;
		CurVehData.signalLightColor = 0;

		if (tlsState == 'r') {
			CurVehData.signalLightColor = 1;
		}
		else if (tlsState == 'y') {
			CurVehData.signalLightColor = 2;
		}
		else if (tlsState == 'g' || tlsState == 'G') {
			CurVehData.signalLightColor = 3;
		}
		else if (tlsState == 'u') {
			CurVehData.signalLightColor = 4;
		}
		else if (tlsState == 'o') {
			CurVehData.signalLightColor = 5;
		}
		else if (tlsState == 'O') {
			CurVehData.signalLightColor = 6;
		}
		else if (tlsState == 's') {
			CurVehData.signalLightColor = 7;
		}

		if (nextTlsList.size() > 1) {
			if (ENABLE_VERBOSE) {

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "received more than 1 nextTLS vehId %s \n", vehId.c_str());
				fclose(f);

			}
		}
	}
	else {
		CurVehData.signalLightId = "";
		CurVehData.signalLightHeadId = -1;
		CurVehData.signalLightDistance = -1.0;
		CurVehData.signalLightColor = -1;
	}

	//=================
	// get speed limit
	//=================
	// retrieve current speed limit
	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_ALLOWED_SPEED]);
	tempDoublePtr2 = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SPEED_FACTOR]);
	CurVehData.speedLimit = tempDoublePtr->value / tempDoublePtr2->value;

	// retrieve next speed limit
	vector <string> edgeRouteList = VehicleId2EdgeList_um[vehId];

	tempIntPtr = static_pointer_cast<libsumo::TraCIInt> (VehDataSubscribeTraciResults[libsumo::VAR_ROUTE_INDEX]);
	int edgeListIdx = tempIntPtr->value;
	CurVehData.linkIdNext = "";
	if (edgeListIdx + 1 < edgeRouteList.size()) {
		CurVehData.linkIdNext = edgeRouteList[edgeListIdx + 1];
	}

	tempStringPtr = static_pointer_cast<libsumo::TraCIString> (VehDataSubscribeTraciResults[libsumo::VAR_VEHICLECLASS]);
	CurVehData.vehicleClass = tempStringPtr->value;

	CurVehData.speedLimitNext = -1;
	// if there is speed limit for next link and vehclass
	if (EdgeVehClass2SpeedLimit_um.find(make_pair(CurVehData.linkIdNext, CurVehData.vehicleClass)) != EdgeVehClass2SpeedLimit_um.end()) {
		CurVehData.speedLimitNext = EdgeVehClass2SpeedLimit_um[make_pair(CurVehData.linkIdNext, CurVehData.vehicleClass)];
	}

	// for following information, need to get them one by one rather than through subscription
	CurVehData.speedLimitChangeDistance = -1;
	if (AllEdgeList.find(CurVehData.linkIdNext) != AllEdgeList.end()) {
		CurVehData.speedLimitChangeDistance = max(Vehicle::getDrivingDistance(vehId, CurVehData.linkIdNext, 0), -1.0);
	}

	//=================
	// grade
	//=================
	tempDoublePtr = static_pointer_cast<libsumo::TraCIDouble> (VehDataSubscribeTraciResults[libsumo::VAR_SLOPE]);
	CurVehData.grade = tempDoublePtr->value * M_PI/180;

	//=================
	// get lane change
	//=================
	//vector <libsumo::TraCIBestLanesData> bestLanesData = Vehicle::getBestLanes(vehId);
	CurVehData.activeLaneChange = 0;


}



int TrafficHelper::recvFromVISSIM(double* simTime, MsgHelper& Msg_c) {
	int recvStatus = 0;

	SocketHelper Sock_temp;
	
	int simStateRecv;
	float simTimeRecv;

	if (ENABLE_VERBOSE) {
		printf("receive VISSIM ......\n");
	
		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "recv VISSIM\n");
		fclose(f);
	}

	for (int i = 0; i < vissimSock.size(); i++) {
		if (ENABLE_VERBOSE) {
			printf("receive VISSIM %s \n", vissimSockName_um[vissimSock[i]].c_str());
		
			//FILE* f = fopen(MasterLogName.c_str(), "a");
			//fprintf(f, "receive VISSIM %s \n", vissimSockName_um[vissimSock[i]].c_str());
			//fclose(f);
		}

		//if (vissimSock.size() > 1 && vissimSockName_um[vissimSock[i]].find(SIGNAL_SOCK_PATTERN) != string::npos) {
		//	// if is signal controller
		//	// if simTime has reached steps for SC
		//	if (abs(round((*simTime- vissimBaseDt) / (vissimSignalStep * vissimBaseDt)) - (*simTime - vissimBaseDt) / (vissimSignalStep * vissimBaseDt)) < 1e-5) {
		//		// if not simTime = 0
		//		if (*simTime > (vissimSignalStep * vissimBaseDt) - 1e-5) {
		//			Sock_temp.recvData(vissimSock[i], &simStateRecv, &simTimeRecv, Msg_c);
		//			*simTime = simTimeRecv;
		//		}
		//	}
		//}
		//else {
		//	Sock_temp.recvData(vissimSock[i], &simStateRecv, &simTimeRecv, Msg_c);
		//	*simTime = simTimeRecv;
		//}
		recvStatus = Sock_temp.recvData(vissimSock[i], &simStateRecv, &simTimeRecv, Msg_c);
		*simTime = simTimeRecv;

		if (recvStatus < 0) {
			return -1;
		}

		if (ENABLE_VERBOSE) {
			printf("\treceive simTime %f\n", *simTime);

			FILE* f = fopen(MasterLogName.c_str(), "a");
			fprintf(f, "\treceive simTime %f\n", *simTime);
			fclose(f);

			for (auto& it : Msg_c.VehDataRecv_um) {
				printf("\treceive vehicle id %s\n", it.first.c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\trecv veh id %s\n", it.first.c_str());
				fclose(f);
			}
			for (auto& it : Msg_c.DetDataRecv_um) {
				printf("\treceive detector at intersection id %s\n", it.first.c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\treceive detector at intersection id %s\n", it.first.c_str());
				fclose(f);
			}
			for (auto& it : Msg_c.TlsDataRecv_um) {
				printf("\treceive signal intersection id %s\n", it.first.c_str());

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\treceive signal intersection id %s\n", it.first.c_str());
				fclose(f);
			}
		}
	}

	if (ENABLE_VERBOSE && recvStatus >= 0) {
		printf("receive VISSIM complete \n");
	
		//FILE* f = fopen(MasterLogName.c_str(), "a");
		//fprintf(f, "receive VISSIM complete \n");
		//fclose(f);
	}

	int aa = 1;

	return recvStatus;
}