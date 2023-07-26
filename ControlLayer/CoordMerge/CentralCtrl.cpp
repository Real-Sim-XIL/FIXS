#include "CentralCtrl.h"

using namespace std;

//CentralCtrl CentralCtrl_g;

CentralCtrl::CentralCtrl(string cz_filename) {
	
	PAR.time_step = 0.1;
	PAR.ind_control_type = 1;
	PAR.dist_merg1 = 21;
	PAR.dist_merg2 = 23;
	PAR.dist_merg3 = 26;
	//PAR.dist_merg1 = 31;
	//PAR.dist_merg2 = 31;
	//PAR.dist_merg3 = 34;
	PAR.smin_safe1 = 4.76;
	PAR.smin_safe2 = 7.49;
	PAR.smin_safe3 = 10.22;
	PAR.s0_gap = 1.5;
	PAR.t_gap = 0.9;
	PAR.TYPE_CAV_LDV = 100;
	PAR.TYPE_CAV_HGV = 200;
	PAR.TYPE_HV_HGV = 300;
	PAR.TYPE_HV_LDV = 600;

	//string cz_filename = "ControlZoneDefault.txt";
	ifstream czConfig(cz_filename, ifstream::in);
	string line;
	if (czConfig.is_open())
	{
		while (getline(czConfig, line)) {
			line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
			if (line.empty() || line[0] == '#')
			{
				continue;
			}
			auto delimiterPos = line.find("=");
			string name = line.substr(0, delimiterPos);
			string value = line.substr(delimiterPos + 1);
			stringstream ssValue(value);

			string delimiter = ",";
			size_t pos = 0;

			cout << name << ": ";
			do {
				pos = value.find(delimiter);
				string token = value.substr(0, pos);
				//cout << stod(token) << endl;
				value.erase(0, pos + delimiter.length());

				if (name == "playerId") {
				}
				else if (name == "veryFirstPlayerId")
					veryFirstVehId = (token);
				else if (name == "Link_M_no")
					PAR.Link_M_no.push_back(token);
				else if (name == "Lane_M_no")
					PAR.Lane_M_no.push_back(stoi(token));
				else if (name == "Pos_CZ_M_en")
					PAR.Pos_CZ_M_en.push_back(stod(token));
				else if (name == "Pos_CZ_M_ex")
					PAR.Pos_CZ_M_ex.push_back(stod(token));
				else if (name == "CZ_M_len")
					PAR.CZ_M_len.push_back(stod(token));
				else if (name == "Link_R_no")
					PAR.Link_R_no.push_back(token);
				else if (name == "Lane_R_no")
					PAR.Lane_R_no.push_back(stoi(token));
				else if (name == "Pos_CZ_R_en")
					PAR.Pos_CZ_R_en.push_back(stod(token));
				else if (name == "Pos_CZ_R_ex")
					PAR.Pos_CZ_R_ex.push_back(stod(token));
				else if (name == "CZ_R_len")
					PAR.CZ_R_len.push_back(stod(token));
				else if (name == "v_des")
					PAR.v_des.push_back(stod(token));
				else if (name == "CZ_en_coords_x")
					PAR.CZ_en_coords_x.push_back(stod(token));
				else if (name == "CZ_en_coords_y")
					PAR.CZ_en_coords_y.push_back(stod(token));
				else if (name == "t_des")
					t_des_config = stod(token);
				else if (name == "typeLDV")
					typeLDV = token;
				else if (name == "typeHDV")
					typeHDV = token;
				else if (name == "t_des_truck")
					t_des_truck = stod(token);
				else if (name == "t_des_truck_accel")
					t_des_truck_accel = stod(token);
				else if (name == "dist_merg1")
					PAR.dist_merg1 = stod(token);
				else if (name == "dist_merg2")
					PAR.dist_merg2 = stod(token);
				else if (name == "dist_merg3")
					PAR.dist_merg3 = stod(token);
				else if (name == "enable_vissim_ipg")
					ENABLE_IPG = stoi(token);

				cout << token << ", ";
			} while (pos != string::npos);
			cout << endl;
		} // reach last line of config file
	} // end Reading Config file

	iCzVeh = 0;
	nCzVeh = 0;

	ifstream f("tempLog.txt");
	if (f.good()) {
		f.close();
		remove("tempLog.txt");
	}

}


void CentralCtrl::setVerbose(bool ENABLE_VERBOSE_MASTER) {

	ENABLE_VERBOSE = ENABLE_VERBOSE_MASTER;

}


void CentralCtrl::coordinate_ind(const INP_Struct* INP, const PAR_Struct* PAR, double t_0, OutStruct* out, double t_des)
{

	double dt = 1e-3;
	double vf = PAR->v_des[INP->czNumber];
	double t_set = PAR->time_step;
	double Pos_CZ_ex;
	double Pos_CZ_en;

	double sfs;
	double v_des_set;
	double a_des_set;

	double t0s = t_0;
	double s0s = INP->Pos_veh;
	double v0s = INP->spd_veh;
	double a0s = INP->acc_veh;

	double sf = INP->CZ_len; // Pos_CZ_ex;

	double tfs_fix = INP->t0_veh_arr_data + sf / vf;
	double tfs_up = t0s + (sf - s0s) / vf;

	double tfs;
	if ((INP->Type_veh == PAR->TYPE_CAV_LDV) || (INP->Type_veh == PAR->TYPE_CAV_HGV)) {
		if (tfs_fix < t_0)
			tfs = tfs_up;
		else
			tfs = min(tfs_up, tfs_fix);
	}
	else {
		tfs = tfs_up;
	}

	//tfs = round(tfs / t_set) * t_set;

	int ind_isLDV_prc = 0;
	int ind_isLDV = 0;
	double smin_MZ;
	double tfs_th = 0;
	if (INP->no_veh.compare(INP->leadVehNo)!=0) { // not leading vehicle
		if (INP->veh_type_prc == PAR->TYPE_CAV_LDV || INP->veh_type_prc == PAR->TYPE_HV_LDV)
			ind_isLDV_prc = 1;
		//if (INP->veh_type_prc == PAR->TYPE_CAV_LDV || INP->veh_type_prc == PAR->TYPE_HV_LDV)
		if (INP->Type_veh == PAR->TYPE_CAV_LDV || INP->Type_veh == PAR->TYPE_HV_LDV)
			ind_isLDV = 1;
		if ((ind_isLDV_prc + ind_isLDV) == 2)
			smin_MZ = PAR->dist_merg1;
		else if ((ind_isLDV_prc + ind_isLDV) == 0)
			smin_MZ = PAR->dist_merg3;
		else
			smin_MZ = PAR->dist_merg2;

		double dt_min_MZ = smin_MZ / vf;
		dt_min_MZ = round(dt_min_MZ / t_set) * t_set;
		tfs_th = INP->tfs_prc + dt_min_MZ;

		tfs = max(tfs, tfs_th);
	}
	else {
		//if (INP->no_veh.compare(veryFirstVehId)==0) {
		//	tfs_th = INP->tfs_prv_veh_arr;
		//	tfs = max(tfs, tfs_th);
		//}
		tfs_th = INP->tfs_prv_veh_arr;
		tfs = max(tfs, tfs_th);
	}

	if ((INP->Type_veh == PAR->TYPE_HV_HGV) || (INP->Type_veh == PAR->TYPE_HV_LDV)) {
		double n_arr = round((max(t_0 + t_set, tfs) - t_0) / dt + 1);
		sfs = INP->Pos_veh + (n_arr - 1) * dt * INP->spd_veh;
		v_des_set = INP->spd_d_veh;
	}
	else {

		//OptSolTemp = v3Unconstrained_soln(v0s, s0s, vf, sf, t_0, max(t_0 + t_set, tfs), dt, t_set);
		//(INP.spd_veh / 3.6, INP.Pos_veh, vf, Pos_CZ_ex, t_0, max(t_0 + t_set, tfs), dt);
		//v_des_set = OptSolTemp.v_des_set;
		//sfs = OptSolTemp.sfs;

		double tf = max(dt, max(t0s + t_set, tfs) - t0s);
		double k1 = -12 * (sf - s0s) / pow(tf, 3) + 6 * (v0s + vf) / pow(tf, 2);
		double k2 = 6 * (sf - s0s) / pow(tf, 2) - 2 * (2 * v0s + vf) / tf;

		double t_arr = min(tf, t_des);
		//a_des_set = k1 * t_arr + k2;
		v_des_set = k1 / 2 * pow(t_arr, 2) + k2 * t_arr + v0s;
		//a_des_set = k1 * t_arr + k2;
		a_des_set = (k1 * t_arr + k2 + a0s) / 2;

		//=================
		// HIL treat
		//=================
		//if (t_des == t_des_truck && t_des_config != t_des_truck) {
		//// if t_des_config is not t_des_truck, means it is HIL
		//// if t_des is t_des_truck, means it is ego vehicle
		//	if (v_des_set > v0s ) {
		//		// if acceleration, use t_des_truck_accel
		//		t_arr = min(tf, t_des_truck_accel);
		//		v_des_set = k1 / 2 * pow(t_arr, 2) + k2 * t_arr + v0s;
		//		a_des_set = (k1 * t_arr + k2 + a0s) / 2;
		//	}
		//}

		tf = round((tf - 0) / dt) * dt + 0;

		sfs = k1 / 6 * pow(tf, 3) + k2 / 2 * pow(tf, 2) + v0s * tf + s0s;

		if (tfs - t_0 < 2) {
			v_des_set = min(vf, v_des_set);
			if (tfs - t_0 < 0.3)
				v_des_set = vf;
		}

		if (s0s > sf - 3)
			v_des_set = vf;
	}

	v_des_set = round(max(1.0, v_des_set ) * 1e2) / 1e2; // m/s

	out->v_des_arr = v_des_set;
	out->tfs_veh_arr = tfs;
	out->sfs_veh_arr = sfs;
	//out->a_des_arr = a_des_set;


	//string tempStr = "flow_1.9";
	//if (INP->no_veh.compare(tempStr.c_str()) == 0) {
	//	fstream tempLog;
	//	tempLog.open("tempLog.txt", fstream::in | fstream::out | fstream::app);
	//	tempLog << t_0 << "; " << INP->Type_veh << "; " << v_des_set << "; " << INP->Pos_veh << "; "
	//		<< max(t_0 + t_set, tfs) << "; " << tfs << "; " << tfs_fix << "; "
	//		<< tfs_up << "; " << v0s << "; " << s0s << "; " << vf << "; " << sf << "; "
	//		<< tfs_th << "; " << INP->leadVehNo << "; " << INP->tfs_prv_veh_arr << "; " << INP->tfs_prc << endl;
	//	tempLog.close();
	//}
	if (INP->no_veh.compare("1") == 0) {
		fstream tempLog;
		tempLog.open("tempLog.txt", fstream::in | fstream::out | fstream::app);
		tempLog << INP->no_veh.c_str() << "; " << t_0 << "; " << INP->Type_veh << "; " << v_des_set << "; " << INP->Pos_veh << "; "
			<< max(t_0 + t_set, tfs) << "; " << tfs << "; " << tfs_fix << "; "
			<< tfs_up << "; " << v0s << "; " << s0s << "; " << vf << "; " << sf << "; "
			<< tfs_th << "; " << INP->leadVehNo << "; " << INP->tfs_prv_veh_arr << "; " << INP->tfs_prc << endl;
		tempLog.close();
	}
}


void CentralCtrl::getSpeedGuidance(VehStructCtrl* Veh, double simTime, double t_des) {

	//if (MainOrRamp.compare("Main") == 0) {
	//	this->getPosOnCzMain(Veh);
	//}
	//else if (MainOrRamp.compare("Ramp") == 0) {
	//	this->getPosOnCzRamp(Veh);
	//}
	//else {
	//	DebugBreak();
	//}

	int idxPos = 0;
	
	if (Veh->IS_IN_CZ) {
		CzVeh.idStrArr[iCzVeh] = Veh->idStr;
		CzVeh.distToExitArr[iCzVeh] = Veh->distToExit;
		CzVeh.typeArr[iCzVeh] = Veh->type;

		string curIdStr[100];
		unsigned int curType[100];
		int iId = 0;
		while (iId < nCzVeh) {
			curIdStr[iId] = CzVeh.idStrArrSort[iId];
			curType[iId] = CzVeh.typeArrSort[iId];
			if (iId == 0) {
				INP.leadVehNo = curIdStr[iId];
			}
			if (curIdStr[iId] == Veh->idStr) {
				// if leading vehicle
				if (iId == 0) {
					INP.veh_type_prc = 0;
					INP.tfs_prc = 0;
					INP.sGap = 0;
				}
				else {
					INP.veh_type_prc = curType[iId - 1];
					INP.tfs_prc = ArrivalInfo.prevTfsArr[curIdStr[iId - 1]];
					INP.sGap = CzVeh.distToExitArr[iId] - CzVeh.distToExitArr[iId - 1];
				}

				INP.czNumber = idxPos;

				//INP.Link_no = Veh->link;
				//INP.Lane_no = Veh->lane;

				INP.Pos_veh = Veh->distOnCz;
				INP.spd_veh = Veh->speed;
				INP.no_veh = Veh->idStr;
				INP.acc_veh = Veh->acceleration;
				INP.Type_veh = Veh->type;
				INP.spd_d_veh = Veh->speed;

				INP.t0_veh_arr_data = ArrivalInfo.arrivalTimeArr[Veh->idStr];
				INP.v0_veh_arr_data = ArrivalInfo.arrivalSpeedArr[Veh->idStr];

				INP.tfs_prv_veh_arr = ArrivalInfo.prevTfsArr[Veh->idStr];

				coordinate_ind(&INP, &PAR, simTime, &Out, t_des);

				Veh->desiredSpeed = Out.v_des_arr;
				Veh->desiredAcceleration = (Veh->desiredSpeed - Veh->speed) / PAR.time_step;

				ArrivalInfo.prevTfsArr[Veh->idStr] = Out.tfs_veh_arr;

				Veh->ENABLE_CTRL = true;

			}

			iId++;
		}

		++iCzVeh;
	}
}

void CentralCtrl::sortCzVeh() {

	//*******************************************************************
	// Sorting vehicle sequence based on their distance to control zone exit
	//*******************************************************************
	int j;

	// sort array and get the sequence of vehicle ids
	vector<tuple<double, string, int>> vp;
	for (j = 0; j < iCzVeh; j++) {
		vp.push_back(make_tuple(CzVeh.distToExitArr[j], CzVeh.idStrArr[j], CzVeh.typeArr[j]));
	}
	sort(vp.begin(), vp.end());

	for (j = 0; j < iCzVeh; j++) {
		CzVeh.idStrArrSort[j] = get<1>(vp[j]);
		CzVeh.typeArrSort[j] = get<2>(vp[j]);
	}

	//for (j = 0; j < iCzVeh; j++) {
	//	printf("%s\t", CzVeh.idStrArrSort[j].c_str());
	//}
	//printf("\n");

	nCzVeh = iCzVeh;
	iCzVeh = 0;

	/*for (j = nCzVeh - 1; j < 200; j++) {
		CzVeh.idStrArr[j] = 
	}*/

}


//void CentralCtrl::getPosOnCzMain(VehStructCtrl* Veh) {
//	bool IS_IN_CZ = false;
//	double distToExit = 0;
//	double distOnCz = 0;
//	double CZ_len = 0;
//
//	if (Veh->linkPosition >= PAR.Pos_CZ_M_en[0] && Veh->linkPosition <= PAR.Pos_CZ_M_ex[0]) {
//		distOnCz = Veh->linkPosition - PAR.Pos_CZ_M_en[0];
//		distToExit = PAR.CZ_M_len[0] - distOnCz;
//		IS_IN_CZ = true;
//	}
//
//	Veh->distToExit = distToExit;
//	Veh->distOnCz = distOnCz;
//	Veh->IS_IN_CZ = IS_IN_CZ;
//
//	INP.CZ_len = PAR.CZ_M_len[0];
//}
//
//void CentralCtrl::getPosOnCzRamp(VehStructCtrl* Veh) {
//	bool IS_IN_CZ = false;
//	double distToExit = 0;
//	double distOnCz = 0;
//	double CZ_len = 0;
//
//	if (Veh->linkPosition >= PAR.Pos_CZ_R_en[0] && Veh->linkPosition <= PAR.Pos_CZ_R_ex[0]) {
//		distOnCz = Veh->linkPosition - PAR.Pos_CZ_R_en[0];
//		distToExit = PAR.CZ_R_len[0] - distOnCz;
//		IS_IN_CZ = true;
//	}
//
//	Veh->distToExit = distToExit;
//	Veh->distOnCz = distOnCz;
//	Veh->IS_IN_CZ = IS_IN_CZ;
//
//	INP.CZ_len = PAR.CZ_R_len[0];
//}

void CentralCtrl::runtimeStepCall(double simTime, vector <string> egoIdVec, unordered_map <string, VehFullData_t>& VehDataLastStep_v) {

	double t_des = t_des_config;

	// find control zone vehicles
	vector <string> vehIdCzM;
	vector <string> vehIdCzR;
	vector <string> vehTypeCzM;
	vector <string> vehTypeCzR;
	vector <double> vehSpeedCzM;
	vector <double> vehSpeedCzR;
	vector <double> vehDistCzM;
	vector <double> vehDistCzR;
	//vector <string> TargetVehId_v;

	TargetVehId_v.clear();

	
	for (auto& it: VehDataLastStep_v) {
		//string linkId = any_cast<string>(VehDataLastStep_v[it.first]["linkId"]);
		string id = VehDataLastStep_v[it.first].id;
		float speed = VehDataLastStep_v[it.first].speed;
		float distanceTravel = VehDataLastStep_v[it.first].distanceTravel;
		string type = VehDataLastStep_v[it.first].type;
		float posX = VehDataLastStep_v[it.first].positionX;
		float posY = VehDataLastStep_v[it.first].positionY;


		unsigned int iMin = 0;
		double coordsMin = 1e5;
		double curCoords;
		// only search from the beginning of current control zone. therefore can do a full track loop with 
		for (int iC = 0; iC < 2; iC++) {
			curCoords = sqrt(pow((PAR.CZ_en_coords_x[iC] - posX), 2) + pow((PAR.CZ_en_coords_y[iC] - posY), 2));
			if (curCoords < coordsMin) {
				coordsMin = curCoords;
				iMin = iC;
				// stop search as soon as found the first coordinates
				if (coordsMin < 1.5) {
					break;
				}
			}
		}
		// cz index 1: main road, cz index 2: ramp road
		if (coordsMin < 1.5) {
			ArrivalInfo.czIndexArr[id] = iMin + 1;
			ArrivalInfo.distanceInitCzArr[id] = distanceTravel;
		}

		double distOnCz = -1.0;
		if (ArrivalInfo.czIndexArr.find(id) != ArrivalInfo.czIndexArr.end()) {
			distOnCz = distanceTravel - ArrivalInfo.distanceInitCzArr[id];

			if (ArrivalInfo.czIndexArr[id] == 1 && distOnCz < PAR.CZ_M_len[0]) {
				if (ENABLE_VERBOSE) {
					printf("\tvehicle %s on main road\n", id.c_str());

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "\tvehicle %s on main road\n", id.c_str());
					fclose(f);
				}
				
				//string id = any_cast<string>(VehDataLastStep_v[it.first]["id"]);
				//float speed = any_cast<float>(VehDataLastStep_v[it.first]["speed"]);
				//float distanceTravel = any_cast<float>(VehDataLastStep_v[it.first]["distanceTravel"]);
				//string type = any_cast<string>(VehDataLastStep_v[it.first]["type"]);

				vehIdCzM.push_back(id);
				vehSpeedCzM.push_back(speed);
				vehDistCzM.push_back(distanceTravel);
				vehTypeCzM.push_back(type);
				//TargetVehId_v.push_back(iV);
			}
			if (ArrivalInfo.czIndexArr[id] == 2 && distOnCz < PAR.CZ_R_len[0]) {
				if (ENABLE_VERBOSE) {
					printf("\tvehicle %s on ramp road\n", id.c_str());

					FILE* f = fopen(MasterLogName.c_str(), "a");
					fprintf(f, "\tvehicle %s on ramp road\n", id.c_str());
					fclose(f);
				}
				
				//string id = any_cast<string>(VehDataLastStep_v[it.first]["id"]);
				//float speed = any_cast<float>(VehDataLastStep_v[it.first]["speed"]);
				//float distanceTravel = any_cast<float>(VehDataLastStep_v[it.first]["distanceTravel"]);
				//string type = any_cast<string>(VehDataLastStep_v[it.first]["type"]);

				vehIdCzR.push_back(id);
				vehSpeedCzR.push_back(speed);
				vehDistCzR.push_back(distanceTravel);
				vehTypeCzR.push_back(type);
				//TargetVehId_v.push_back(iV);
			}



		}

		


		/*	vehIdCzM = traci.edge.getLastStepVehicleIDs(czMainLinkId);
		vehIdCzR = traci.edge.getLastStepVehicleIDs(czRampLinkId);*/
	}


	if (ENABLE_VERBOSE || 1) {
		FILE* f = fopen(MasterLogName.c_str(), "a");

		printf("\tcontrol zone vehicle order: ");

		fprintf(f, "\tcontrol zone vehicle order: ");

		for (int i = 0; i < nCzVeh; i++) {
			printf("%s\t", CzVeh.idStrArrSort[i].c_str());

			fprintf(f, "%s\t", CzVeh.idStrArrSort[i].c_str());
		}
		printf("\n");

		fprintf(f, "\n");

		fclose(f);

	}


	//nTargetVeh = TargetVehId_v.size();

	//vecIdCzMerge = traci.edge.getLastStepVehicleIDs(czMergeLinkId);

	for (int iV = 0; iV < vehIdCzM.size(); iV++) {
		VehStructCtrl Veh;

		string idStr = vehIdCzM[iV];
		//unsigned int id = stoi(idStr.substr(12));

		Veh.idStr = idStr;
		Veh.distance = vehDistCzM[iV];
		Veh.speed = vehSpeedCzM[iV];
		//Veh.link = CZ_MAIN_LINK_ID; // traci.vehicle.getRoadID(idStr);
		//Veh.lane = 1;
		if (!typeLDV.compare(vehTypeCzM[iV])) {
			Veh.type = PAR.TYPE_CAV_LDV;
		}
		else if (!typeHDV.compare(vehTypeCzM[iV])){
			Veh.type = PAR.TYPE_CAV_HGV;
			//Veh.type = PAR.TYPE_CAV_LDV;
		}
		Veh.ENABLE_CTRL = false;


		// if vehicle has passed the control zone entry point
		if (ArrivalInfo.czIndexArr.find(idStr) != ArrivalInfo.czIndexArr.end()) {
			Veh.distOnCz = Veh.distance - ArrivalInfo.distanceInitCzArr[idStr];
			Veh.distToExit = PAR.CZ_M_len[0] - Veh.distOnCz;
			if (Veh.distToExit > 0) {
				Veh.IS_IN_CZ = true;
			}
			else {
				Veh.IS_IN_CZ = false;
			}
		}
		else {
			Veh.distOnCz = -1.0;
			Veh.distToExit = -1.0;
			Veh.IS_IN_CZ = false;
		}
		INP.CZ_len = PAR.CZ_M_len[0];

		if (Veh.distOnCz >= 0 && Veh.distOnCz <= 3) {
			ArrivalInfo.arrivalTimeArr[Veh.idStr] = simTime;
			ArrivalInfo.arrivalSpeedArr[Veh.idStr] = Veh.speed;
		}

		for (int i = 0; i < egoIdVec.size(); i++) {
			if (Veh.idStr.compare(egoIdVec[i]) == 0) {
				t_des = t_des_truck;
				egoCzState[egoIdVec[i]] = 1;
				break;
			}
			else {
				t_des = t_des_config;
			}
		}

		this->getSpeedGuidance(&Veh, simTime, t_des);

		if (Veh.ENABLE_CTRL && ENABLE_CAV_APP) {
			/*traci.vehicle.setSpeed(idStr, Veh.desiredSpeed);
			traci.vehicle.setSpeedMode(idStr, 0);*/
			VehDataLastStep_v[Veh.idStr].speedDesired = (float) Veh.desiredSpeed;
			TargetVehId_v.push_back(Veh.idStr);

			if (ENABLE_VERBOSE) {
				printf("\tcontrol vehicle %s, speed %f\n", Veh.idStr.c_str(), Veh.desiredSpeed);

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\tcontrol vehicle %s, speed %f\n", Veh.idStr.c_str(), Veh.desiredSpeed);
				fclose(f);
			}

		}

	}
	
	
	for (int iV = 0; iV < vehIdCzR.size(); iV++) {
		VehStructCtrl Veh;

		string idStr = vehIdCzR[iV];
		//unsigned int id = stoi(idStr.substr(12));

		Veh.idStr = idStr;
		Veh.distance = vehDistCzR[iV];
		Veh.speed = vehSpeedCzR[iV];
		//Veh.link = CZ_RAMP_LINK_ID; // traci.vehicle.getRoadID(idStr);
		//Veh.lane = 1;
		if (!typeLDV.compare(vehTypeCzR[iV])) {
			Veh.type = PAR.TYPE_CAV_LDV;
		}
		else if (!typeHDV.compare(vehTypeCzR[iV])) {
			Veh.type = PAR.TYPE_CAV_HGV;
			//Veh.type = PAR.TYPE_CAV_LDV;
		}
		Veh.ENABLE_CTRL = false;


		// if vehicle has passed the control zone entry point
		if (ArrivalInfo.czIndexArr.find(idStr) != ArrivalInfo.czIndexArr.end()) {
			Veh.distOnCz = Veh.distance - ArrivalInfo.distanceInitCzArr[idStr];
			Veh.distToExit = PAR.CZ_R_len[0] - Veh.distOnCz;
			if (Veh.distToExit > 0) {
				Veh.IS_IN_CZ = true;
			}
			else {
				Veh.IS_IN_CZ = false;
			}
		}
		else {
			Veh.distOnCz = -1.0;
			Veh.distToExit = -1.0;
			Veh.IS_IN_CZ = false;
		}
		INP.CZ_len = PAR.CZ_R_len[0];

		if (Veh.distOnCz >= 0 && Veh.distOnCz <= 3) {
			ArrivalInfo.arrivalTimeArr[Veh.idStr] = simTime;
			ArrivalInfo.arrivalSpeedArr[Veh.idStr] = Veh.speed;
		}

		for (int i = 0; i < egoIdVec.size(); i++) {
			if (Veh.idStr.compare(egoIdVec[i]) == 0) {
				t_des = t_des_truck;
				egoCzState[egoIdVec[i]] = 1;
				break;
			}
			else {
				t_des = t_des_config;
			}
		}
		this->getSpeedGuidance(&Veh, simTime, t_des);

		if (Veh.ENABLE_CTRL && ENABLE_CAV_APP) {
			/*traci.vehicle.setSpeed(idStr, Veh.desiredSpeed);
			traci.vehicle.setSpeedMode(idStr, 0);*/
			VehDataLastStep_v[Veh.idStr].speedDesired = (float) Veh.desiredSpeed;
			TargetVehId_v.push_back(Veh.idStr);

			if (ENABLE_VERBOSE) {
				printf("\tcontrol vehicle %s, speed %f\n", Veh.idStr.c_str(), Veh.desiredSpeed);

				FILE* f = fopen(MasterLogName.c_str(), "a");
				fprintf(f, "\tcontrol vehicle %s, speed %f\n", Veh.idStr.c_str(), Veh.desiredSpeed);
				fclose(f);
			}
		}

	}

	// for all egoVeh, if not in control zone, and egoCzState = 1 (means previously in control zone), then assign state = 2
	for (int iV = 0; iV < egoIdVec.size(); iV++) {
		bool egoInCz = false;
		for (int i = 0; i < vehIdCzM.size(); i++) {
			if (egoIdVec[iV].compare(vehIdCzM[i]) == 0) {
				egoInCz = true;
				break;
			}
		}
		for (int i = 0; i < vehIdCzR.size(); i++) {
			if (egoIdVec[iV].compare(vehIdCzR[i]) == 0) {
				egoInCz = true;
				break;
			}
		}
		if (!egoInCz && egoCzState[egoIdVec[iV]] == 1) {
			egoCzState[egoIdVec[iV]] = 2;
		}
	}

	this->sortCzVeh();

	nTargetVeh = TargetVehId_v.size();

	//cout << " controlling " << nTargetVeh << " vehicles " << endl;
	//printf(" controlling %d vehicles\n", nTargetVeh);
}
