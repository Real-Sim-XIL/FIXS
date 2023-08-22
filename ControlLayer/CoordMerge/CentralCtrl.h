#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <tuple>
#include <unordered_map>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>
//#include "VehDataPOD.h"
#include "VehDataMsgDefs.h"


// Vehicle Data Structure Specially Designed for Centralized Controller
typedef struct VehStructCtrl {
    //unsigned int id;
    std::string idStr;
    double distOnCz;
    double distToExit;
    double distance;
    //double linkPosition;
    double speed;
    double acceleration;
    std::string link;
    int lane;
    unsigned int type;
    bool ENABLE_CTRL;
    double desiredSpeed;
    double desiredAcceleration;
    bool IS_IN_CZ;
};

typedef struct INP_Struct {
    std::string leadVehNo;
    int veh_type_prc;
    int czNumber;
    double tfs_prc;
    //std::string Link_no;
    //std::string Lane_no;
    double CZ_len;
    double Pos_veh;
    double spd_veh;
    std::string no_veh;
    double acc_veh;
    int Type_veh;
    double spd_d_veh;
    double t0_veh_arr_data;
    double v0_veh_arr_data;
    double tfs_prv_veh_arr;
    double sGap;
};

typedef struct PAR_Struct {

    std::vector <std::string> Link_M_no;
    std::vector <int> Lane_M_no;
    std::vector <double> Pos_CZ_M_en;
    std::vector <double> Pos_CZ_M_ex;
    std::vector <double> CZ_M_len;
    std::vector <std::string> Link_R_no; // array of control zone
    std::vector <int> Lane_R_no;
    std::vector <double> Pos_CZ_R_en;
    std::vector <double> Pos_CZ_R_ex;
    std::vector <double> CZ_R_len;
    std::vector <double> CZ_en_coords_x;
    std::vector <double> CZ_en_coords_y; //(x, y)
    std::vector <double> v_des;

    double time_step; // in seconds
    int ind_control_type;
    double dist_merg1;
    double dist_merg2;
    double dist_merg3;
    double smin_safe1;
    double smin_safe2;
    double smin_safe3;
    double s0_gap;
    double t_gap;
    unsigned int TYPE_CAV_LDV;
    unsigned int TYPE_CAV_HGV;
    unsigned int TYPE_HV_HGV;
    unsigned int TYPE_HV_LDV;
};

// struct that saves current states of vehicles within the control zone
typedef struct CzVehStruct {
    // here by default we allow maximum of 200 vehicles in the control zone
    //unsigned int idArr[200];
    std::string idStrArr[200];
    //double posXArr[200];
    //double posYArr[200];
    unsigned int typeArr[200];
    double distToExitArr[200];
    std::string idStrArrSort[200]; // sorted array by the coordinator controller
    unsigned int typeArrSort[200];
};

// saves information of all vehicles ever appeared in the simulation
typedef struct ArrivalInfoStruct {
    // here by default assume maximum there will be 1000 vehicles in the simulation
    std::unordered_map<std::string,double> arrivalTimeArr;
    std::unordered_map<std::string, double> arrivalSpeedArr;
    std::unordered_map<std::string, double> prevTfsArr; // previous 
    std::unordered_map<std::string, int> czIndexArr; // specify which control zone it is in
    //std::unordered_map<std::string, double> distanceOnCzArr;
    // initial travel distance when enter control zone
    std::unordered_map<std::string, double> distanceInitCzArr;
};

typedef struct OutStruct {
    double v_des_arr;
    double tfs_veh_arr;
    double sfs_veh_arr;
};

class CentralCtrl
{

public:
	// constructor
	CentralCtrl(std::string cz_filename);

	void coordinate_ind(const INP_Struct* INP, const PAR_Struct* PAR, double t_0, OutStruct* out, double t_des);
    void getSpeedGuidance(VehStructCtrl* Veh, double simTime, double t_des);
    void sortCzVeh();
    //void getPosOnCzMain(VehStructCtrl* Veh);
    //void getPosOnCzRamp(VehStructCtrl* Veh);
    void runtimeStepCall(double simTime, std::vector <std::string> egoIdVec, std::unordered_map <std::string, VehFullData_t>& VehDataLastStep_v);
    void setVerbose(bool ENABLE_VERBOSE_MASTER);

    PAR_Struct PAR;

    INP_Struct INP;

    unsigned int VERY_FIRST_VEH_ID;
  
    CzVehStruct CzVeh;

    ArrivalInfoStruct ArrivalInfo;

    OutStruct Out;

    int iCzVeh;
	int nCzVeh;

    std::string veryFirstVehId;

    int nTargetVeh = 0;
    std::vector <std::string> TargetVehId_v;

    bool ENABLE_CAV_APP = 1;

    double t_des_truck = 0.1;

    double t_des_truck_accel = 0.1;

    bool ENABLE_VERBOSE = 0;
    int ENABLE_IPG = 0;


    std::string MasterLogName = "";


    std::unordered_map <std::string, int> egoCzState;

private:
    //std::string CZ_MAIN_LINK_ID = "1";
    //std::string CZ_RAMP_LINK_ID = "2";
    double t_des_config = 0.1;
    

    std::string typeLDV = "CAV_LDV";
    std::string typeHDV = "CAV_HGV";
};

//extern CentralCtrl CentralCtrl_g;

