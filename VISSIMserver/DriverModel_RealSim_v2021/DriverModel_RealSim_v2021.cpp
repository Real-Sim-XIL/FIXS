/*==========================================================================*/
/*  DriverModel.cpp                                  DLL Module for VISSIM  */
/*                                                                          */
/*  Interface module for external driver models.                            */
/*  Dummy version that does nothing (uses Vissim's internal model).         */
/*                                                                          */
/*  Version of 2018-11-15                                   Lukas Kautzsch  */
/*==========================================================================*/

#include "SocketHelper.h"

#include "DriverModel.h"
#include "NetworkTableHelper.h"

#include <string>

#include <math.h>

#include <sstream>
#include <fstream>

#include <ctime>
#include <chrono>

using namespace std;


// Master Switches
bool ENABLE_REALSIM = true;

// if no signal control but vehicle control depends on signal information
bool NEED_SIGNAL = false;

bool ENABLE_LOG = false;

bool ENABLE_EXT_DYN = false;

//bool ENABLE_VERBOSE = false;

bool NEW_TIMESTEP = false;

bool isVeryFirstStep = true;

bool USE_ESTIMATED_SLOPE = true;

bool hasWriteRecvError = false;


bool NEED_SPEED_LIMIT = true;


string configFile = "..\\config.yaml";

vector <int> selfServerPortUserInput = {  };
SocketHelper Sock_c;

int VissimSock;

// This Contains all vehicle data
VehFullData_t VissimVehDataCommon;

std::vector <std::vector <VehFullData_t> > VehDataSend_v;

// define a struct with variables for debuggging purposes only
struct DebugVar_t {
	int id;
	int intacState;
	int intactTargetType;
	int speedDesired;
} DebugVar;



VehicleDataAuxiliary_t VehicleDataAuxiliary;



unordered_map <int, SignalHead_t > SignalHeadInfo_um; // signal head index => signal head configurations

unordered_map <int, TrafficLightData_t> SignalData; // signal controller index => trafficlightdata


double simTime;
long nVeh_g = 0;
long iVeh_g = 0;

// for VISSIM layer, only has one MsgHelper instance, since the communication is only with Traffic Layer
MsgHelper Msg_c;



ConfigHelper Config_c;

NetworkTableHelper Net_c;


std::unordered_set <std::string> edgeSubscribeId_v;

// vehicle id -> radius
std::unordered_map <std::string, double > vehicleSubscribeId_v;

std::pair <bool, double> subscribeAllVehicle = { std::make_pair(false, 0) };

// tuple x, y, z, radius
// name of the point -> x, y, z, r
std::unordered_map <std::string, std::tuple<double, double, double, double> > pointSubscribeId_v;

// for each subscribed vehicle, store its latest position, so that can find those vehicles within radius of it
std::unordered_map <std::string, tuple<double, double, double> > vehicleSubscribePosition_um;

std::unordered_map <std::string, double> vehicleTypeSubscribedId_v;
std::unordered_map <std::string, tuple<double, double, double> > vehicleTypeSubscribePosition_um;


std::unordered_map <std::string, VehFullData_t > VehicleDataPrevious;

int laneChangeStatus = 0;


// define common functions
int receiveMessage();
int sendMessage();
void RealSimShutdown();
void ResetVehicleData();



int receiveMessage() {
	try {
		//if (isVeryFirstStep) {
		//	isVeryFirstStep = false;

		//	//FILE* f = fopen("DriverModelLog.txt", "a");
		//	//fprintf(f, "Set NotFirstStep\n");
		//	//fclose(f);
		//}
		//else {
		if (ENABLE_LOG) {
			FILE* f = fopen("DriverModelLog.txt", "a");
			fprintf(f, "\n=============NEW TIME STEP===============\n");
			fprintf(f, "simTime %f\n", simTime);
			fclose(f);
		}

		// reset receive buffer
		Msg_c.clearRecvStorage();

		// receive messages
		int simStateRecv;
		float simTimeRecv;
		if (Sock_c.recvData(VissimSock, &simStateRecv, &simTimeRecv, Msg_c) < 0) {
			// only write this error once
			if (!hasWriteRecvError) {
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "ERROR: receive from client fails\n");
				fclose(f);

				hasWriteRecvError = true;
			}
			//exit(-1); // this will close the VISSIM after disconnect from RealSim
			RealSimShutdown();
			return 1; // return 1 to suppress error messages from VISSIM
		};


		if (ENABLE_LOG) {
			//printf("receive simTime %f, simState %d\n", simTimeRecv, simStateRecv);
			FILE* f = fopen("DriverModelLog.txt", "a");

			for (auto& it : Msg_c.VehDataRecv_um) {
				fprintf(f, "\treceive vehicle id %s\n", it.first.c_str());
			}
			for (auto& it : Msg_c.DetDataRecv_um) {
				fprintf(f, "\treceive detector at intersection id %s\n", it.first.c_str());
			}
			for (auto& it : Msg_c.TlsDataRecv_um) {
				fprintf(f, "\treceive signal intersection id %s\n", it.first.c_str());
			}
			fclose(f);

		}

		//}
	}
	catch (const std::exception& e) {
		FILE* f = fopen("DriverModelError.txt", "a");
		fprintf(f, "ERROR: %s\n", e.what());
		fclose(f);
		RealSimShutdown();
		return 1; // return 1 to suppress error messages from VISSIM
	}
	catch (...) {
		FILE* f = fopen("DriverModelError.txt", "a");
		fprintf(f, "UNKNOWN ERROR: receive from client fails\n");
		fclose(f);
		RealSimShutdown();
		return 1; // return 1 to suppress error messages from VISSIM
	}
}


int sendMessage() {
	try {
		// send out to Traffic Layer
		float simTimeSend = simTime;
		uint8_t simStateSend = 1;

		if (VehDataSend_v.size() == 0) {
			Msg_c.VehDataSend_um[VissimSock] = {};
		}
		else {
			Msg_c.VehDataSend_um[VissimSock] = VehDataSend_v[0];
		}
		Msg_c.TlsDataSend_um[VissimSock] = {};
		Msg_c.DetDataSend_um[VissimSock] = {};

		if (ENABLE_LOG) {
			FILE* f = fopen("DriverModelLog.txt", "a");
			fprintf(f, "sending out\n");
			fprintf(f, "\tvehicle data size %d\n", (int)Msg_c.VehDataSend_um[VissimSock].size());
			fprintf(f, "\tsignal light data size %d\n", (int)Msg_c.TlsDataSend_um[VissimSock].size());
			fprintf(f, "\tdetector data size %d\n", (int)Msg_c.DetDataSend_um[VissimSock].size());
			fclose(f);
		}

		if (Sock_c.sendData(VissimSock, 0, simTimeSend, simStateSend, Msg_c) < 0) {
			if (!hasWriteRecvError) {
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "ERROR: send to client fails\n");
				fclose(f);
				//exit(-1);
				RealSimShutdown();
				hasWriteRecvError = true;
			}
			return 1; // return 1 to suppress error messages from VISSIM
		};

		Msg_c.clearSendStorage();

		VehDataSend_v.clear();

	}
	catch (const std::exception& e) {
		FILE* f = fopen("DriverModelError.txt", "a");
		fprintf(f, "ERROR: %s\n", e.what());
		fclose(f);
		RealSimShutdown();
		return 1; // return 1 to suppress error messages from VISSIM
	}
	catch (...) {
		FILE* f = fopen("DriverModelError.txt", "a");
		fprintf(f, "UNKNOWN ERROR: send to TrafficLayer failed\n");
		fclose(f);
		RealSimShutdown();
		return 1; // return 1 to suppress error messages from VISSIM
	}
}

int checkSubscription(VehFullData_t VissimVehDataCommon) {
	int isSubscription = 0;

	// -------------------
	//  check vehicle subscription
	// -------------------
	// if need all vehicles, return immediately
	if (get<0>(subscribeAllVehicle)) {
		isSubscription = 1;
		return isSubscription;
	}

	if (vehicleSubscribeId_v.find(VissimVehDataCommon.id) != vehicleSubscribeId_v.end()) {
		// if vehicle is the one subscribed, store vehicle information
		// also return as subscription
		vehicleSubscribePosition_um[VissimVehDataCommon.id] = make_tuple(VissimVehDataCommon.positionX, VissimVehDataCommon.positionY, VissimVehDataCommon.positionZ);

		isSubscription = 1;
		//return isSubscription;
	}
	else {
		// otherwise loop over all subscribed position see if it is within radius
		for (auto& iter : vehicleSubscribePosition_um) {
			double r = vehicleSubscribeId_v[iter.first];
			double x = get<0>(iter.second);
			double y = get<1>(iter.second);
			double z = get<2>(iter.second);

			// only check if radius is positive
			// otherwise it is possible two vehicles collapse to each other if ego not controlled well
			if (r >= 1e-5) {
				if (sqrt(pow(VissimVehDataCommon.positionX - x, 2) + pow(VissimVehDataCommon.positionY - y, 2)) <= r) {
					isSubscription = 1;

					break;
					//return isSubscription;
				}
			}
		}
	}

	// -------------------
	//  check link subscription
	// -------------------
	if (edgeSubscribeId_v.find(VissimVehDataCommon.linkId) != edgeSubscribeId_v.end()) {
		isSubscription = 1;
		//return isSubscription;
	}

	// -------------------
	//  check point subscription
	// -------------------
	for (auto& iter : pointSubscribeId_v) {
		double x = get<0>(iter.second);
		double y = get<1>(iter.second);
		double z = get<2>(iter.second);
		double r = get<3>(iter.second);

		if (sqrt(pow(VissimVehDataCommon.positionX - x, 2) + pow(VissimVehDataCommon.positionY - y, 2)) <= r) {
			isSubscription = 1;
			//return isSubscription;
		}
	}

	// -------------------
	//  check vehicle type subscription
	// -------------------
	if (vehicleTypeSubscribedId_v.find(VissimVehDataCommon.type) != vehicleTypeSubscribedId_v.end()) {
		// if vehicle is the one subscribed, store vehicle information
		// also return as subscription
		vehicleTypeSubscribePosition_um[VissimVehDataCommon.id] = make_tuple(VissimVehDataCommon.positionX, VissimVehDataCommon.positionY, VissimVehDataCommon.positionZ);

		isSubscription = 1;
		//return isSubscription;
	}
	else {
		// otherwise loop over all subscribed position see if it is within radius
		for (auto& iter : vehicleTypeSubscribePosition_um) {
			double r = vehicleTypeSubscribedId_v[iter.first];
			double x = get<0>(iter.second);
			double y = get<1>(iter.second);
			double z = get<2>(iter.second);

			if (sqrt(pow(VissimVehDataCommon.positionX - x, 2) + pow(VissimVehDataCommon.positionY - y, 2)) <= r) {
				isSubscription = 1;

				break;
				//return isSubscription;
			}
		}
	}

	return isSubscription;

}

void RealSimShutdown() {
	Sock_c.socketShutdown();
	// Reset flags not defined in Config file
	//NEW_TIMESTEP = false;
	//isVeryFirstStep = true;
	//hasWriteRecvError = false;
	// Reset containers used in dll
	VehDataSend_v.clear();
	SignalHeadInfo_um.clear(); // signal head index => signal head configurations
	SignalData.clear(); // signal controller index => trafficlightdata
	// Reset vehicle counter
	//nVeh_g = 0;
	//iVeh_g = 0;
	// Reset Msg containers
	Msg_c.clearRecvStorage();
	Msg_c.clearSendStorage();
	// Reset Config containers
	Config_c.resetConfig();
	// Reset local subscription containers
	edgeSubscribeId_v.clear();
	vehicleSubscribeId_v.clear();
	//subscribeAllVehicle = std::make_pair(false, 0);
	pointSubscribeId_v.clear();
	vehicleSubscribePosition_um.clear();
}


void ResetVehicleData() {
	// reset data for current driver
	VissimVehDataCommon.precedingVehicleId = "N/A";
	VissimVehDataCommon.hasPrecedingVehicle = 0;
	VissimVehDataCommon.precedingVehicleSpeed = -1.0;
	VissimVehDataCommon.precedingVehicleDistance = -1.0;

	VissimVehDataCommon.speedLimit = -1;
	VissimVehDataCommon.speedLimitNext = -1;
	VissimVehDataCommon.speedLimitChangeDistance = -1;

	VissimVehDataCommon.signalLightId = to_string(-1);
	VissimVehDataCommon.signalLightHeadId = -1;
	VissimVehDataCommon.signalLightColor = -1;
	VissimVehDataCommon.signalLightDistance = -1.0;

	laneChangeStatus = 0;
	//desired_lane_angle = 0.0;
	//active_lane_change = 0;
	//rel_target_lane = 0;
	//vehPreferredRelLane = 0;
	//vehUsePreferredLane = 0;

	//VehicleDataAuxiliary.isNextSpeedLimitFound = 0;
	//VehicleDataAuxiliary.nextSpeedLimit = -1;
	//VehicleDataAuxiliary.nextSpeedLimitDistanceCalc = -1;
	VehicleDataAuxiliary.nextLink_v.clear();
}

/*==========================================================================*/
double  desired_acceleration = 0.0;
double  desired_velocity = 0.0;

double  accelDes = 0.0;
double  desired_lane_angle = 0.0;
int    active_lane_change = 0;
int    rel_target_lane = 0;
double  speedDes = 0.0;
int    turning_indicator = 0;
int    color = RGB(0, 0, 0);

int vehPreferredRelLane = 0;
int vehUsePreferredLane = 0;


int numOfLanes = 1;

/*==========================================================================*/

BOOL APIENTRY DllMain(HANDLE  hModule,
	DWORD   ul_reason_for_call,
	LPVOID  lpReserved)
{
	FILE* f = fopen("DriverModelError.txt", "a");

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		//fprintf(f, "DLL Thread Detached\n");
		//fclose(f);
		//if (Sock_c.clientSock.size() > 0) {
		//	fprintf(f, "DLL Thread Detached, RealSimReset\n");
		//	fclose(f);
		//	RealSimShutdown();
		//}

		break;
	case DLL_PROCESS_DETACH:
		fprintf(f, "DLL Process Detached\n");
		fclose(f);

		RealSimShutdown();

		break;
	}
	return TRUE;
}

/*==========================================================================*/
// this is retrieve from VISSIM
DRIVERMODEL_API  int  DriverModelSetValue(int   type,
	int   index1,
	int   index2,
	int   int_value,
	double double_value,
	char* string_value)
{
	/* Sets the value of a data object of type <type>, selected by <index1> */
	/* and possibly <index2>, to <int_value>, <double_value> or            */
	/* <*string_value> (object and value selection depending on <type>).    */
	/* Return value is 1 on success, otherwise 0.                           */


	int sigController;
	int sigHead;
	int sigState;
	int sigGroupIndex;

	switch (type) {
	case DRIVER_DATA_TIMESTEP:
		return 1;
	case DRIVER_DATA_TIME:
		// this command will exactly be called once per simulation time step
		if (type == DRIVER_DATA_TIME) {
			// if time step changes, then need to check nVeh_g. if nVeh_g == 0, send empty message to Traffic Layer, otherwise, do nothing
			// need to skip 0.1 second to accomodate for signal controller
			if (NEW_TIMESTEP) {
				if (nVeh_g < 1) {
					if (ENABLE_REALSIM) {
						if (isVeryFirstStep) {
							isVeryFirstStep = false;
							//FILE* f = fopen("DriverModelLog.txt", "a");
							//fprintf(f, "Set notFirstStep, no vehicle yet\n");
							//fclose(f);
						}
						else {
							// send out to Traffic Layer
							// use previous time
							float simTimeSend = simTime;
							uint8_t simStateSend = 1;
							if (Sock_c.sendData(VissimSock, 0, simTimeSend, simStateSend, Msg_c) < 0) {
								FILE* f = fopen("DriverModelError.txt", "a");
								fprintf(f, "ERROR: send to client fails\n");
								fclose(f);
								//exit(-1);
								RealSimShutdown();
								return 1; // return 1 to suppress error messages from VISSIM
							}
							else {
								if (ENABLE_LOG) {
									FILE* f = fopen("DriverModelLog.txt", "a");
									fprintf(f, "send, no vehicle yet\n");
									fclose(f);
								}
							};

							Msg_c.clearSendStorage();

							// recv from Traffic Layer
							Msg_c.clearRecvStorage();

							int simStateRecv;
							float simTimeRecv;
							if (Sock_c.recvData(VissimSock, &simStateRecv, &simTimeRecv, Msg_c) < 0) {
								// only write this error once
								if (!hasWriteRecvError) {
									FILE* f = fopen("DriverModelError.txt", "a");
									fprintf(f, "ERROR: receive from client fails\n");
									fclose(f);

									hasWriteRecvError = true;
								}
								RealSimShutdown();
								return 1; // return 1 to suppress error messages from VISSIM
							}
							else {
								if (ENABLE_LOG) {
									FILE* f = fopen("DriverModelLog.txt", "a");
									fprintf(f, "receive, no vehicle yet\n");
									fclose(f);
								}
							};

						}

						// !!!should change vehicle speed in the next timestep
						int aa = 1;
					}
				}
				else {
					int sendStatus = sendMessage();
					int receiveStatus = receiveMessage();
				}
			}
			if (double_value > simTime + 1e-5 && double_value > 0.1 - 1e-5) {
				NEW_TIMESTEP = true;
			}
			else {
				NEW_TIMESTEP = false;
			}
			simTime = double_value;
		}
		return 1;
	case DRIVER_DATA_PARAMETERFILE:
		configFile = string_value;
		return 1;
	case DRIVER_DATA_USE_UDA:
		return 1; /* doesn't use any UDAs */
				  /* must return 1 for desired values of index1 if UDA values are to be sent from/to Vissim */
	case DRIVER_DATA_VEH_ID:
		/* int:   vehicle number */
		VissimVehDataCommon.id = to_string(int_value);
		DebugVar.id = int_value;

		return 1;
	case DRIVER_DATA_VEH_LANE:
		/* int:   current lane number (rightmost = 1) */
		VissimVehDataCommon.laneId = (int)int_value;
		return 1;
	case DRIVER_DATA_VEH_ODOMETER:
		/* double: total elapsed distance in the network [m] */
		VissimVehDataCommon.distanceTravel = double_value;
		return 1;
	case DRIVER_DATA_VEH_LANE_ANGLE:
	case DRIVER_DATA_VEH_LATERAL_POSITION:
	case DRIVER_DATA_VEH_VELOCITY:
		/* double: current speed [m/s] */
		VissimVehDataCommon.speed = double_value;
		return 1;
	case DRIVER_DATA_VEH_ACCELERATION:
		/* double: current acceleration [m/s²] */
		VissimVehDataCommon.acceleration = double_value;
		return 1;
	case DRIVER_DATA_VEH_LENGTH:
		/* double: vehicle length [m] */
		VissimVehDataCommon.length = double_value;
		return 1;
	case DRIVER_DATA_VEH_WIDTH:
		/* double: vehicle width [m] */
		VissimVehDataCommon.width = double_value;
		return 1;
	case DRIVER_DATA_VEH_WEIGHT:
		/* double: vehicle weight [kg] */
		VissimVehDataCommon.weight = double_value;
		return 1;
	case DRIVER_DATA_VEH_MAX_ACCELERATION:
		return 1;
	case DRIVER_DATA_VEH_TURNING_INDICATOR:
		turning_indicator = int_value;
		return 1;
	case DRIVER_DATA_VEH_CATEGORY:
		return 1;
	case DRIVER_DATA_VEH_PREFERRED_REL_LANE:
		/* int:   positive = left, 0 = current lane, negative = right */
		vehPreferredRelLane = int_value;
		return 1;
	case DRIVER_DATA_VEH_USE_PREFERRED_LANE:
		/* int:   0 = only preferable (e.g. European highway) */
		/*         1 = necessary (e.g. before a connector)     */
		vehUsePreferredLane = int_value;
		return 1;
	case DRIVER_DATA_VEH_DESIRED_VELOCITY:
		VissimVehDataCommon.speedDesired = double_value;
		DebugVar.speedDesired = double_value;
		desired_velocity = double_value;
		return 1;
	case DRIVER_DATA_VEH_X_COORDINATE:
		VissimVehDataCommon.positionX = double_value;
		return 1;
	case DRIVER_DATA_VEH_Y_COORDINATE:
		VissimVehDataCommon.positionY = double_value;
		return 1;
	case DRIVER_DATA_VEH_Z_COORDINATE:
		VissimVehDataCommon.positionZ = double_value;
		return 1;
	case DRIVER_DATA_VEH_REAR_X_COORDINATE:
		VehicleDataAuxiliary.positionXrear = double_value;
		return 1;
	case DRIVER_DATA_VEH_REAR_Y_COORDINATE:
		VehicleDataAuxiliary.positionYrear = double_value;
		return 1;
	case DRIVER_DATA_VEH_REAR_Z_COORDINATE:
		if (USE_ESTIMATED_SLOPE) {
			VissimVehDataCommon.grade = asin((VissimVehDataCommon.positionZ - double_value) / VissimVehDataCommon.length);

			if (isnan(VissimVehDataCommon.grade)) {
				VissimVehDataCommon.grade = 0;
			}
		}

		return 1;
	case DRIVER_DATA_VEH_TYPE:
		VissimVehDataCommon.type = to_string(int_value);
		return 1;
	case DRIVER_DATA_VEH_COLOR:
		VissimVehDataCommon.color = (uint32_t)int_value;
		return 1;
	case DRIVER_DATA_VEH_CURRENT_LINK:
		VissimVehDataCommon.linkId = to_string(int_value);

		return 1; /* (To avoid getting sent lots of DRIVER_DATA_VEH_NEXT_LINKS messages) */
				  /* Must return 1 if these messages are to be sent from VISSIM!         */
	case DRIVER_DATA_VEH_NEXT_LINKS:
		// there will be next link only when the vehicle is in static decision areas
		// the next links will contain all links within the static decision areas
		// if used combine route, then will be able to see longer future routes

		VehicleDataAuxiliary.nextLink_v.push_back(to_string(int_value));

		VissimVehDataCommon.linkIdNext = to_string(int_value);
		return 1;
	case DRIVER_DATA_VEH_ACTIVE_LANE_CHANGE:

		return 1;
	case DRIVER_DATA_VEH_REL_TARGET_LANE:

		return 1;
	case DRIVER_DATA_VEH_INTAC_STATE:
		/* int:   interaction state from internal model                 */
		/*         (sent only if DRIVER_DATA_WANTS_SUGGESTION is set!)   */
		/*         FREE = 1, CLOSEUP = 2, FOLLOW = 3, BRAKEAX = 4,       */
		/*         BRAKEBX = 5,BRAKEZX = 6, BRAKESPW = 7, BRAKEKOOP = 8, */
		/*         PELOPS = 9,PASS = 10, SLEEP = 11, DWELL = 12          */
		DebugVar.intacState = int_value;
		return 1;
	case DRIVER_DATA_VEH_INTAC_TARGET_TYPE:
		/* int:   type of relevant interaction target from internal model */
		/*         (sent only if DRIVER_DATA_WANTS_SUGGESTION is set!)     */
		/*         no target = 0, real vehicle = 1, signal head = 2,       */
		/*         priority rule = 3, conflict area = 4, reduced speed     */
		/*         area = 5, stop sign = 6, parking lot = 7, PT stop = 8   */
		DebugVar.intactTargetType = int_value;
		return 1;
	case DRIVER_DATA_VEH_INTAC_TARGET_ID:
		return 1;
		/* int:   number of relevant interaction target from internal model */
		/*         (sent only if DRIVER_DATA_WANTS_SUGGESTION is set!)       */
	case DRIVER_DATA_VEH_INTAC_HEADWAY:
		return 1;
	case DRIVER_DATA_VEH_UDA:
		// this is RealSim_PositionOnLink
		if (index1 == 23333) {
			VehicleDataAuxiliary.positionOnLink = double_value;
			//Net_c.VehicleId2PositionOnLink[VissimVehDataCommon.id] = double_value;
		}
		return 1;
	case DRIVER_DATA_NVEH_ID:
		/* index1 relative lane
		+2 second lane to left, +1 next lane to left
		+0 current lane
		-2 second lane to right, -1 next lane to right
		*/
		/* index2 relative position
		positive downstream, +1 next, +2 second next
		negative upstream, -1 next, -2 second next
		*/
		// if preceding vehicle
		//if (VissimVehDataCommon.id.compare("8") == 0) {
		//	int aa = 1;
		//}
		if (index1 == 0 && index2 == 1) {
			if (int_value > 0) {
				VissimVehDataCommon.precedingVehicleId = to_string(int_value);
				VissimVehDataCommon.hasPrecedingVehicle = 1;
			}
			else {
				VissimVehDataCommon.precedingVehicleId = "N/A";
				VissimVehDataCommon.hasPrecedingVehicle = 0;
				VissimVehDataCommon.precedingVehicleDistance = -1.0;
				VissimVehDataCommon.precedingVehicleSpeed = -1.0;
			}
		}
		return 1;
	case DRIVER_DATA_NVEH_LANE_ANGLE:
	case DRIVER_DATA_NVEH_LATERAL_POSITION:
		return 1;
	case DRIVER_DATA_NVEH_DISTANCE:
		// front end to front end, negative = nveh is upstream
		if (index1 == 0 && index2 == 1) {
			VissimVehDataCommon.precedingVehicleDistance = double_value;
		}
		return 1;
	case DRIVER_DATA_NVEH_REL_VELOCITY:
		// speed difference[m / s](veh.speed - nveh.speed)
		if (index1 == 0 && index2 == 1) {
			VissimVehDataCommon.precedingVehicleSpeed = VissimVehDataCommon.speed - double_value;
		}
		return 1;
	case DRIVER_DATA_NVEH_ACCELERATION:
		return 1;
	case DRIVER_DATA_NVEH_LENGTH:
		// manually minus the length of the preceding vehicle to get front end to rear end
		if (index1 == 0 && index2 == 1) {
			VissimVehDataCommon.precedingVehicleDistance -= double_value;
		}
		return 1;
	case DRIVER_DATA_NVEH_WIDTH:
	case DRIVER_DATA_NVEH_WEIGHT:
	case DRIVER_DATA_NVEH_TURNING_INDICATOR:
	case DRIVER_DATA_NVEH_CATEGORY:
	case DRIVER_DATA_NVEH_LANE_CHANGE:
	case DRIVER_DATA_NVEH_TYPE:
	case DRIVER_DATA_NVEH_UDA:
	case DRIVER_DATA_NVEH_X_COORDINATE:
	case DRIVER_DATA_NVEH_Y_COORDINATE:
	case DRIVER_DATA_NVEH_Z_COORDINATE:
	case DRIVER_DATA_NVEH_REAR_X_COORDINATE:
	case DRIVER_DATA_NVEH_REAR_Y_COORDINATE:
	case DRIVER_DATA_NVEH_REAR_Z_COORDINATE:
	case DRIVER_DATA_NO_OF_LANES:
		numOfLanes = int_value;
		return 1;
	case DRIVER_DATA_LANE_WIDTH:
	case DRIVER_DATA_LANE_END_DISTANCE:
	case DRIVER_DATA_CURRENT_LANE_POLY_N:
	case DRIVER_DATA_CURRENT_LANE_POLY_X:
	case DRIVER_DATA_CURRENT_LANE_POLY_Y:
	case DRIVER_DATA_CURRENT_LANE_POLY_Z:
	case DRIVER_DATA_RADIUS:
	case DRIVER_DATA_MIN_RADIUS:
	case DRIVER_DATA_DIST_TO_MIN_RADIUS:
	case DRIVER_DATA_SLOPE:
		/* double: current slope (negative = drop) */
		/* (e.g. -0.026 = -2.6%) */
		// this requires gradient to be defined for each link. 
		// NOTE: it is different than simply set z-offset in VISSIM
		if (!USE_ESTIMATED_SLOPE) {
			VissimVehDataCommon.grade = atan(double_value);
		}
		return 1;
	case DRIVER_DATA_SLOPE_AHEAD:
	case DRIVER_DATA_SIGNAL_DISTANCE:

		return 1;
	case DRIVER_DATA_SIGNAL_STATE:
		// prefer to set DRIVER_DATA_WANTS_ALL_SIGNALS == 1. Then here can obtain ALL signal state in the VISSIM. VISSIM will do it smartly, so each time step, each signal head states will only be retreived once. 
		sigController = index1;
		sigHead = index2;
		sigState = int_value;

		if (NEED_SIGNAL) {
			int nSigGroup = 0;

			// if sigHead is one of those defined in signal table, then retreive signal group index
			if (SignalHeadInfo_um.find(sigHead) != SignalHeadInfo_um.end()) {
				sigGroupIndex = SignalHeadInfo_um[sigHead].signalGroupIndex - 1;
				nSigGroup = SignalData[SignalHeadInfo_um[sigHead].signalControllerIndex].state.size() - 1;
			}
			// otherwise, set signal group index to be 0
			else {
				sigGroupIndex = -1;
				nSigGroup = 0;
			}

			// if in any case sigGroupIndex < 0, then save to a log and return
			if (sigGroupIndex < 0 || sigGroupIndex > nSigGroup) {

				if (sigController > 0) {
					fstream fLog;
					fLog.open("DriverModelLog.txt", fstream::in | fstream::out | fstream::app);
					fLog << simTime << " seconds, signal group index out of range, signal controller " << sigController << ", signal head " << sigHead << endl;
					fLog.close();

					int msgboxID = MessageBox(
						NULL,
						_T("Signal group index out of range\nCheck log files for details"),
						_T(""),
						MB_ICONWARNING | MB_OK
					);

					switch (msgboxID)
					{
					case IDOK:
						// TODO: add code
						break;
					}
				}

				return 1;
			}

			if (sigState == 3) {
				SignalData[SignalHeadInfo_um[sigHead].signalControllerIndex].state[sigGroupIndex] = 'G';
			}
			else if (sigState == 2) {
				SignalData[SignalHeadInfo_um[sigHead].signalControllerIndex].state[sigGroupIndex] = 'y';
			}
			else if (sigState == 1) {
				SignalData[SignalHeadInfo_um[sigHead].signalControllerIndex].state[sigGroupIndex] = 'r';
			}
			else if (sigState == 6) {
				SignalData[SignalHeadInfo_um[sigHead].signalControllerIndex].state[sigGroupIndex] = 'O';
			}
			else {
				SignalData[SignalHeadInfo_um[sigHead].signalControllerIndex].state[sigGroupIndex] = 'z';
			}
		}
		else {
			//fstream f("DriverModelLog.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			//f << "signalLightId " << index1 << " signalLightHeadId " << index2 << " signalLightColor " << int_value << endl;
			//f.close();

			VissimVehDataCommon.signalLightId = to_string(index1);
			VissimVehDataCommon.signalLightHeadId = index2;
			VissimVehDataCommon.signalLightColor = int_value;
		}
		return 1;
	case DRIVER_DATA_SIGNAL_STATE_START:
		return 1;
	case DRIVER_DATA_SPEED_LIMIT_DISTANCE:
		/* distance[m] to "speed limit sign" (reduced speed area : real distance,
		 desired speed decision : 1.0 m when just passed, negative : no sign visible)*/
		//VissimVehDataCommon.speedLimitChangeDistance = double_value;

		return 1;
	case DRIVER_DATA_SPEED_LIMIT_VALUE:
		//// speed limit[km / h](0 = end of reduced speed area)
		//if (double_value < 0) {
		//	VissimVehDataCommon.speedLimit = -1.0;
		//	VissimVehDataCommon.speedLimitNext = -1.0;
		//}
		//else {
		//	VissimVehDataCommon.speedLimit = double_value / 3.6; // convert to m/s
		//	VissimVehDataCommon.speedLimitNext = VissimVehDataCommon.speedLimit; // !!! make it the same for now
		//}
		return 1;
	case DRIVER_DATA_PRIO_RULE_DISTANCE:
	case DRIVER_DATA_PRIO_RULE_STATE:
	case DRIVER_DATA_ROUTE_SIGNAL_DISTANCE:
		if (double_value < 0) {
			VissimVehDataCommon.signalLightColor = -1;
		}
		VissimVehDataCommon.signalLightDistance = double_value;
		return 1;
	case DRIVER_DATA_ROUTE_SIGNAL_STATE:
		// here is the signal state specific in front of each vehicle, will be called once per vehicle
		sigController = index1;
		sigHead = index2;
		sigState = int_value;
		// red = 1, amber = 2, green = 3, red / amber = 4, amber flashing = 5, off = 6, other = 0
		if (!NEED_SIGNAL) {
			VissimVehDataCommon.signalLightId = to_string(index1);
			VissimVehDataCommon.signalLightHeadId = index2;
			VissimVehDataCommon.signalLightColor = int_value;
		}
		return 1;
	case DRIVER_DATA_ROUTE_SIGNAL_CYCLE:
		return 1;
	case DRIVER_DATA_ROUTE_SIGNAL_SWITCH:
		return 0;  /* (to avoid getting sent lots of signal switch data) */
	case DRIVER_DATA_CONFL_AREAS_COUNT:
		return 0;  /* (to avoid getting sent lots of conflict area data) */
	case DRIVER_DATA_CONFL_AREA_TYPE:
	case DRIVER_DATA_CONFL_AREA_YIELD:
	case DRIVER_DATA_CONFL_AREA_DISTANCE:
	case DRIVER_DATA_CONFL_AREA_LENGTH:
	case DRIVER_DATA_CONFL_AREA_VEHICLES:
	case DRIVER_DATA_CONFL_AREA_TIME_ENTER:
	case DRIVER_DATA_CONFL_AREA_TIME_IN:
	case DRIVER_DATA_CONFL_AREA_TIME_EXIT:
		return 1;
	case DRIVER_DATA_DESIRED_ACCELERATION:
		VissimVehDataCommon.accelerationDesired = double_value;
		VehicleDataAuxiliary.accelerationDesired = double_value;
		desired_acceleration = double_value;
		return 1;
	case DRIVER_DATA_DESIRED_LANE_ANGLE:
		desired_lane_angle = double_value;
		return 1;
	case DRIVER_DATA_ACTIVE_LANE_CHANGE:
		active_lane_change = int_value;
		VissimVehDataCommon.activeLaneChange = int_value;
		return 1;
	case DRIVER_DATA_REL_TARGET_LANE:
		if (int_value == 0 && (active_lane_change != 0 || abs(desired_lane_angle) > 1e-5) ) {
			// if rel_target_lane has been set to 0 from nonzero previous value
			// this means middle of the front end of the vehicle has reached the target lane
			// then send activeLaneChange with -128 or 127 to notify the TrafficLayer
			if (rel_target_lane > 0) {
				laneChangeStatus = 1;
			}
			else {
				laneChangeStatus = -1;
			}
		}
		else {
			laneChangeStatus = 0;
		}
		rel_target_lane = int_value;	
		return 1;
	default:
		return 0;
	}
}

/*--------------------------------------------------------------------------*/

DRIVERMODEL_API  int  DriverModelSetValue3(int   type,
	int   index1,
	int   index2,
	int   index3,
	int   int_value,
	double double_value,
	char* string_value)
{
	/* Sets the value of a data object of type <type>, selected by <index1>, */
	/* <index2> and <index3>, to <int_value>, <double_value> or             */
	/* <*string_value> (object and value selection depending on <type>).     */
	/* Return value is 1 on success, otherwise 0.                            */
	/* DriverModelGetValue (DRIVER_DATA_MAX_NUM_INDICES, ...) needs to set   */
	/* *int_value to 3 or greater in order to activate this function!       */

	switch (type) {
	case DRIVER_DATA_ROUTE_SIGNAL_SWITCH:
		return 0; /* don't send any more switch values */
	default:
		return 0;
	}
}

/*--------------------------------------------------------------------------*/
// Send value to VISSIM
DRIVERMODEL_API  int  DriverModelGetValue(int   type,
	int   index1,
	int   index2,
	int* int_value,
	double* double_value,
	char** string_value)
{
	/* Gets the value of a data object of type <type>, selected by <index1> */
	/* and possibly <index2>, and writes that value to <*int_value>,       */
	/* <*double_value> or <**string_value> (object and value selection      */
	/* depending on <type>).                                                */
	/* Return value is 1 on success, otherwise 0.                           */

	switch (type) {
	case DRIVER_DATA_STATUS:
		*int_value = 0;
		return 1;
	case DRIVER_DATA_WANTS_ALL_SIGNALS:
		if (NEED_SIGNAL) {
			*int_value = 1; /* needs to be set to zero if no global signal data is required */
		}
		else {
			*int_value = 0;
		}
		return 1;
	case DRIVER_DATA_MAX_NUM_INDICES:
		*int_value = 3; /* because DriverModelSetValue3() and DriverModelSetValue3() exist in this DLL */
		return 1;
	case DRIVER_DATA_VEH_TURNING_INDICATOR:
		*int_value = turning_indicator;
		return 1;
	case DRIVER_DATA_VEH_DESIRED_VELOCITY:
		*double_value = VissimVehDataCommon.speedDesired;
		//*double_value = desired_velocity;
		return 1;
	case DRIVER_DATA_VEH_COLOR:
		*int_value = VissimVehDataCommon.color;
		return 1;
	case DRIVER_DATA_VEH_UDA:
		return 1; /* doesn't set any UDA values */ // can only overwrite to UDA DataSrcType Data
	case DRIVER_DATA_WANTS_SUGGESTION:
		*int_value = 1;
		return 1;
	case DRIVER_DATA_DESIRED_ACCELERATION:
		*double_value = VissimVehDataCommon.accelerationDesired;
		//*double_value = desired_acceleration;
		return 1;
	case DRIVER_DATA_DESIRED_LANE_ANGLE:
		*double_value = desired_lane_angle;
		return 1;
	case DRIVER_DATA_ACTIVE_LANE_CHANGE:
		//*int_value = active_lane_change;
		//if (VissimVehDataCommon.type.compare("1000") == 0) {
		//	VissimVehDataCommon.activeLaneChange = 0;
		//}
		*int_value = VissimVehDataCommon.activeLaneChange;
		
		//if (ENABLE_LOG) {
		//	FILE* f = fopen("DriverModelLog.txt", "a");
		//	fprintf(f, "set vehicle %s, actual active lane change %d\n", VissimVehDataCommon.id, VissimVehDataCommon.activeLaneChange);
		//	fclose(f);
		//}

		return 1;
	case DRIVER_DATA_REL_TARGET_LANE:
		*int_value = rel_target_lane;
		return 1;
	case DRIVER_DATA_SIMPLE_LANECHANGE:
		*int_value = 1;
		return 1;
	case DRIVER_DATA_USE_INTERNAL_MODEL:
		if (!ENABLE_REALSIM) {
			*int_value = 1; /* must be set to 0 if external model is to be applied */
		}
		else {
			*int_value = 0;
		}
		return 1;
	case DRIVER_DATA_WANTS_ALL_NVEHS:
		*int_value = 1; /* must be set to 1 if data for more than 2 nearby vehicles per lane and upstream/downstream is to be passed from Vissim */
		return 1;
	case DRIVER_DATA_ALLOW_MULTITHREADING:
		*int_value = 0; /* must be set to 1 to allow a simulation run to be started with multiple cores used in the simulation parameters */
		return 1;
	default:
		return 0;
	}
}

/*--------------------------------------------------------------------------*/

DRIVERMODEL_API  int  DriverModelGetValue3(int   type,
	int   index1,
	int   index2,
	int   index3,
	int* int_value,
	double* double_value,
	char** string_value)
{
	/* Gets the value of a data object of type <type>, selected by <index1>, */
	/* <index2> and <index3>, and writes that value to <*int_value>,        */
	/* <*double_value> or <**string_value> (object and value selection       */
	/* depending on <type>).                                                 */
	/* Return value is 1 on success, otherwise 0.                            */
	/* DriverModelGetValue (DRIVER_DATA_MAX_NUM_INDICES, ...) needs to set   */
	/* *int_value to 3 or greater in order to activate this function!       */

	switch (type) {
	default:
		return 0;
	}
}

/*==========================================================================*/

DRIVERMODEL_API  int  DriverModelExecuteCommand(int number)
{
	/* Executes the command <number> if that is available in the driver */
	/* module. Return value is 1 on success, otherwise 0.               */

	int aa;
	int isSubscription = 0;
	//int iClientSub = -1;
	vector <string> serverAddr = {};
	serverAddr.push_back("127.0.0.1");


	switch (number) {
	case DRIVER_COMMAND_INIT:

		NEW_TIMESTEP = false;
		isVeryFirstStep = true;
		hasWriteRecvError = false;
		// Reset vehicle counter
		nVeh_g = 0;
		iVeh_g = 0;

		ResetVehicleData();

		// ===========================================================================
		// 			Read Config
		// ===========================================================================

		if (1) {
			auto simStartTimestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
			fstream f("DriverModelError.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			f << endl << "=============================================" << endl;
			f << "Simulation Starts at  " << std::ctime(&simStartTimestamp) << endl;
			f.close();
		}


		if (Config_c.getConfig(configFile) < 0) {
			fstream f("DriverModelError.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			f << "Error: get configuration file failed" << endl;
			f.close();

			return 0;
		}
		Msg_c.getConfig(Config_c);


		// ===========================================================================
		// 			RESET LOG 
		// ===========================================================================
		// remove existing log file
		//if (Config_c.SimulationSetup.EnableVerboseLog) {
		if (1) {
			ifstream f("DriverModelLog.txt");
			if (f.good()) {
				f.close();
				remove("DriverModelLog.txt");
			}
		}
		//}

		// ===========================================================================
		// 			Retrieve Config as local variables
		// ===========================================================================
		if (Config_c.SimulationSetup.EnableVerboseLog) {
			ENABLE_LOG = true;
		}
		else {
			ENABLE_LOG = false;
		}
		if (Config_c.SimulationSetup.EnableExternalDynamics) {
			ENABLE_EXT_DYN = true;
		}
		else {
			ENABLE_EXT_DYN = false;
		}

		if (Config_c.SimulationSetup.EnableRealSim) {
			ENABLE_REALSIM = true;
			fstream f("DriverModelLog.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			f << "Real-Sim enabled" << endl;
			f.close();
		}
		else {
			ENABLE_REALSIM = false;
			fstream f("DriverModelLog.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			f << "Real-Sim disabled" << endl;
			f.close();
		}

		// get subscription information
		// variable to store subscription that need to check
		// if application layer is disabled, xil is enabled then use subscription of xil, this means traffic layer directly connects to xil
		if (!Config_c.ApplicationSetup.EnableApplicationLayer && Config_c.XilSetup.EnableXil) {
			Config_c.getVehSubscriptionList(Config_c.XilSetup.VehicleSubscription, edgeSubscribeId_v, vehicleSubscribeId_v, subscribeAllVehicle, pointSubscribeId_v, vehicleTypeSubscribedId_v);
		}
		// otherwise find out the subscription of application layer
		else {
			Config_c.getVehSubscriptionList(Config_c.ApplicationSetup.VehicleSubscription, edgeSubscribeId_v, vehicleSubscribeId_v, subscribeAllVehicle, pointSubscribeId_v, vehicleTypeSubscribedId_v);
		}


		// ===========================================================================
		// 			SETUP REAL-SIM INTERFACE CONNECTIONS 
		// ===========================================================================
		//// if use VISSIM as Server
		//Sock_c.socketSetup(selfServerPortUserInput); // no server
		//Sock_c.enableWaitClientTrigger();

		// if use VISSIM as Client
		selfServerPortUserInput.push_back(Config_c.SimulationSetup.TrafficSimulatorPort);
		Sock_c.socketSetup(serverAddr, selfServerPortUserInput); // connect to server Traffic Layer
		Sock_c.disableServerTrigger();


		if (ENABLE_REALSIM) {
			if (Sock_c.initConnection("DriverModelError.txt") < 0) {
				fstream f("DriverModelError.txt", std::fstream::in | std::fstream::out | std::fstream::app);
				f << "Error: initialize connection to Traffic Layer failed" << endl;
				f.close();

				int msgboxID = MessageBox(
					NULL,
					_T("Connect to RealSim failed, VISSIM might start without RealSim interface\nCheck DriverModelError.txt for details"),
					_T("RealSim Initialization Failed"),
					MB_ICONERROR | MB_OK
				);

				switch (msgboxID)
				{
				case IDOK:
					// TODO: add code
					break;
				}

				return 1;
			}
			else {
				VissimSock = Sock_c.serverSock[0];

				fstream f("DriverModelLog.txt", std::fstream::in | std::fstream::out | std::fstream::app);
				f << "Real-Sim connected" << endl;
				f.close();
			}
		}

		aa = 1;

		// ===========================================================================
		// 			READ Signal Table
		// ===========================================================================
		// -------------------
		//  Comment
		// -------------------
		// read signal table which is needed to map index to intersection and phase
		// only do this when need signal information
		if (NEED_SIGNAL) {
			if (Net_c.readSignalTable(".\\SignalTable.csv") < 0) {
				fstream fLog;
				fLog.open("DriverModelLog.txt", fstream::in | fstream::out | fstream::app);
				fLog << "0.0 seconds, signal table cannot be found or opened" << endl;
				fLog.close();


				int msgboxID = MessageBox(
					NULL,
					_T("Cannot find or open signal table\nNo signal data will be transmitted!"),
					_T("DriverModelDll"),
					MB_ICONWARNING | MB_OK
				);

				switch (msgboxID)
				{
				case IDOK:
					// TODO: add code
					break;
				}

				return 1;
			}
			else {
				SignalHeadInfo_um = Net_c.SignalHeadInfo_um;
				SignalData = Net_c.SignalData;
			}

		}


		if (Msg_c.VehicleMessageField_set.find("speedLimit") != Msg_c.VehicleMessageField_set.end() || Msg_c.VehicleMessageField_set.find("speedLimitNext") != Msg_c.VehicleMessageField_set.end() || Msg_c.VehicleMessageField_set.find("speedLimitChangeDistance") != Msg_c.VehicleMessageField_set.end()) {
			NEED_SPEED_LIMIT = true;
		}
		else {
			NEED_SPEED_LIMIT = false;
		}
		// ===========================================================================
		// 			READ Speed Limit Table
		// ===========================================================================
		if (NEED_SPEED_LIMIT) {
			int realSpeedLimitTableStatus = 0;
			try {
				realSpeedLimitTableStatus = Net_c.readSpeedLimitTable(".\\SpeedLimitTable.csv");
			}
			catch (const std::exception& e) {
				realSpeedLimitTableStatus = -1;
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "ERROR: read SpeedLimitTable.csv failed, %s\n", e.what());
				fclose(f);
			}
			catch (...) {
				realSpeedLimitTableStatus = -1;
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "UNKNOWN ERROR: read SpeedLimitTable.csv failed\n");
				fclose(f);
			}
			if (realSpeedLimitTableStatus < 0) {
				fstream fLog;
				fLog.open("DriverModelLog.txt", fstream::in | fstream::out | fstream::app);
				fLog << "SpeedLimitTable.csv cannot be found or opened properly" << endl;
				fLog.close();


				int msgboxID = MessageBox(
					NULL,
					_T("Cannot find or open SpeedLimitTable\nSpeed limit data will not be properly transmitted!"),
					_T("DriverModelDll"),
					MB_ICONWARNING | MB_OK
				);

				switch (msgboxID)
				{
				case IDOK:
					// TODO: add code
					break;
				}

				return 1;
			}
			else if (realSpeedLimitTableStatus == 10000) {
				int msgboxID = MessageBox(
					NULL,
					_T("There are empty items in SpeedLimitTable.csv, please check the file!\nSpeed limit data may not be properly transmitted!"),
					_T("DriverModelDll"),
					MB_ICONWARNING | MB_OK
				);

				switch (msgboxID)
				{
				case IDOK:
					// TODO: add code
					break;
				}
			}
			else {
				// do nothing
			}
		}

		// ===========================================================================
		// 			READ Link Geometry Table
		// ===========================================================================
		if (NEED_SPEED_LIMIT) {
			int realLinkTableStatus = 0;
			try {
				realLinkTableStatus = Net_c.readLinkTable(".\\LinkGeometryTable.csv");
			}
			catch (const std::exception& e) {
				realLinkTableStatus = -1;
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "ERROR: read LinkGeometryTable.csv failed, %s\n", e.what());
				fclose(f);
			}
			catch (...) {
				realLinkTableStatus = -1;
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "UNKNOWN ERROR: read LinkGeometryTable.csv failed\n");
				fclose(f);
			}
			if (realLinkTableStatus < 0) {
				fstream fLog;
				fLog.open("DriverModelLog.txt", fstream::in | fstream::out | fstream::app);
				fLog << "LinkGeometryTable.csv cannot be found or opened properly" << endl;
				fLog.close();


				int msgboxID = MessageBox(
					NULL,
					_T("Cannot find or open LinkGeometryTable\nSpeed limit data will not be properly transmitted!"),
					_T("DriverModelDll"),
					MB_ICONWARNING | MB_OK
				);

				switch (msgboxID)
				{
				case IDOK:
					// TODO: add code
					break;
				}

				return 1;
			}
			else {
				// do nothing
			}
		}

		// ===========================================================================
		// 			READ Routes
		// ===========================================================================
		if (NEED_SPEED_LIMIT) {
			int readRoutesStauts = 0;
			try {
				ifstream f(".\\Routes.yaml");
				if (!f.good()) {
					readRoutesStauts = -10000;
				}
				else {
					readRoutesStauts = Net_c.readRoutes(".\\Routes.yaml");
				}
			}
			catch (const std::exception& e) {
				readRoutesStauts = -1;
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "ERROR: read Routes.yaml failed, %s\n", e.what());
				fclose(f);
			}
			catch (...) {
				readRoutesStauts = -1;
				FILE* f = fopen("DriverModelError.txt", "a");
				fprintf(f, "UNKNOWN ERROR: read Routes.yaml failed\n");
				fclose(f);
			}
			if (readRoutesStauts < 0) {
				// error message
				string errorSpecs = "";

				// handle error codes
				switch (readRoutesStauts){
				case -10000:
					errorSpecs = "cannot find or open Routes.yaml, make sure its in same path as .inpx";
					break;
				case -10001:
					errorSpecs = "lane and link should be same length OR lane should be size 1";
					break;
				case -10002:
					errorSpecs = "id and vehicleType should be same length";
					break;
				default:
					errorSpecs = "unknown error";
					break;
				}

				fstream fLog;
				fLog.open("DriverModelLog.txt", fstream::in | fstream::out | fstream::app);
				fLog << "Routes.yaml cannot be found or opened properly" << endl;
				fLog << "\tError: " << errorSpecs << endl;
				fLog.close();

				char buff[200];
				snprintf(buff, sizeof buff, "Read Routes.yaml failed\n%s\nSpeed limit data will not be properly transmitted!", errorSpecs.c_str());
				int msgboxID = MessageBox(
					NULL,
					_T(buff),
					_T("DriverModelDll"),
					MB_ICONWARNING | MB_OK
				);

				switch (msgboxID)
				{
				case IDOK:
					// TODO: add code
					break;
				}

				return 1;
			}
			else {
				// do nothing
			}
		}

		return 1;
	case DRIVER_COMMAND_CREATE_DRIVER:
		nVeh_g++;

		return 1;
	case DRIVER_COMMAND_KILL_DRIVER:
		nVeh_g--;

		Net_c.VehicleId2SpeedLimitCurrent.erase(VissimVehDataCommon.id);
		Net_c.VehicleId2SpeedLimitIntervals.erase(VissimVehDataCommon.id);

		if (VehicleDataPrevious.find(VissimVehDataCommon.id) != VehicleDataPrevious.end()) {
			VehicleDataPrevious.erase(VissimVehDataCommon.id);
		}

		return 1;
	case DRIVER_COMMAND_MOVE_DRIVER:

		// ===========================================================================
		// 			RECV DATA FROM TRAFFIC LAYER <<<<<<<<<=======
		// ===========================================================================
		// only receive once when its the FIRST vehicle
		if (iVeh_g == 0 && nVeh_g >= 1) {
			if (ENABLE_REALSIM) {
				//int receiveStatus = receiveMessage();
			}
		}

		// ===========================================================================
		// 			Update Vehicle Data to Send
		// ===========================================================================
		// only obtain vehicles that are within subscription list
		// therefore, only these subscribed vehicles will be saved to the send out buffer. 
		//checkSubscription(&isSubscription, &iClientSub, VissimVehDataCommon);
		try {
			isSubscription = checkSubscription(VissimVehDataCommon);
			// !!! need to update heading calculation


			if (isSubscription > 0) {

				// ===========================================================================
				// 			GET SPEED LIMIT INFORMATION
				// ===========================================================================

				if (NEED_SPEED_LIMIT) {
					try {

						Net_c.getSpeedLimit(simTime, VehicleDataAuxiliary, VissimVehDataCommon);

					}
					catch (const std::exception& e) {
						RealSimShutdown();
						return 1; // return 1 to suppress error messages from VISSIM
					}
					catch (...) {
						RealSimShutdown();
						return 1; // return 1 to suppress error messages from VISSIM
					}
				}


				// ===========================================================================
				// 			Get heading
				// ===========================================================================
				{
					const double M_PI = 3.14159265358979323846;
					double atanyx = atan2((VissimVehDataCommon.positionY - VehicleDataAuxiliary.positionYrear), (VissimVehDataCommon.positionX - VehicleDataAuxiliary.positionXrear));
					VissimVehDataCommon.heading = 0;
					if (atanyx >= -M_PI && atanyx <= 0.5 * M_PI) {
						VissimVehDataCommon.heading = (-atanyx + 0.5 * M_PI) * 180 / M_PI;
					}
					else {
						VissimVehDataCommon.heading = (-atanyx + 2.5 * M_PI) * 180 / M_PI;
					}
				}


				if (ENABLE_LOG) {
					FILE* f = fopen("DriverModelLog.txt", "a");
					fprintf(f, "vehicle id %s is subscribed\n", VissimVehDataCommon.id.c_str());
					fclose(f);
				}

				VehFullData_t CurVehData = VissimVehDataCommon;

				// create buffer to store all vehicle data
				// VissimVehDataCommon contains all possible messages can be obtained from VISSIM/supported by RealSim
				//	use a separate vehicle data struct VehData_t to store data subscribed
				//Msg_c.VehFullDataToSubData(VissimVehDataCommon, CurVehData);


				// if vehicle has finished lane change, then reset activeLaneChange
				// if the controller is making lane change commands, then the controller will receive 0 and need to reset the commands 
				if (laneChangeStatus == 1) {
					if (Msg_c.VehicleMessageField_set.find("activeLaneChange") != Msg_c.VehicleMessageField_set.end()) {
						CurVehData.activeLaneChange = 0;
					}
				}
				else if (laneChangeStatus == -1) {
					if (Msg_c.VehicleMessageField_set.find("activeLaneChange") != Msg_c.VehicleMessageField_set.end()) {
						CurVehData.activeLaneChange = 0;
					}
				}

				if (ENABLE_EXT_DYN) {
					if (VehicleDataPrevious.find(VissimVehDataCommon.id) == VehicleDataPrevious.end()) {
						CurVehData.speedDesired = (float)(VissimVehDataCommon.speed + 0.2 * VehicleDataAuxiliary.accelerationDesired);
					}
					else {
						CurVehData.speedDesired = (float)(VehicleDataPrevious[VissimVehDataCommon.id].speed + 0.1 * VehicleDataPrevious[VissimVehDataCommon.id].accelerationDesired + 0.2 * VehicleDataAuxiliary.accelerationDesired);
						//CurVehData["speedDesired"] = (float)(VissimVehDataCommon.speed + 0.2 * VehicleDataAuxiliary.accelerationDesired);
					}
				}

				//if (laneChangeStatus == 1) {
				//	VissimVehDataCommon.activeLaneChange = 0;
				//	CurVehData["activeLaneChange"] = (int8_t) SCHAR_MAX;
				//}
				//else if (laneChangeStatus == -1) {
				//	VissimVehDataCommon.activeLaneChange = 0;
				//	CurVehData["activeLaneChange"] = (int8_t) SCHAR_MIN;
				//}

				if (ENABLE_LOG) {
					FILE* f = fopen("DriverModelLog.txt", "a");
					fprintf(f, "send lane change time %f activeLaneChange %d, laneChangeStatus %d\n", simTime, VissimVehDataCommon.activeLaneChange, laneChangeStatus);
					fclose(f);
				}

				if (VehDataSend_v.size() == 0) {
					VehDataSend_v.push_back(vector <VehFullData_t>{CurVehData});
					//Msg_c.VehIdSend_v.push_back(vector <string> {VissimVehDataCommon.id});

					int aa = 1;
				}
				else {
					VehDataSend_v[0].push_back(CurVehData);
					//Msg_c.VehIdSend_v[0].push_back(VissimVehDataCommon.id);
				}

				if (ENABLE_EXT_DYN) {
					VehicleDataPrevious[VissimVehDataCommon.id] = VissimVehDataCommon;
				}

				if (VissimVehDataCommon.distanceTravel < 6) {
					// avoid initial lane change when entering the network
					VissimVehDataCommon.activeLaneChange = 0;
				}

			}
		}
		catch (const std::exception& e) {
			FILE* f = fopen("DriverModelError.txt", "a");
			fprintf(f, "ERROR: %s\n", e.what());
			fclose(f);
			RealSimShutdown();
			return 1; // return 1 to suppress error messages from VISSIM
		}
		catch (...) {
			FILE* f = fopen("DriverModelError.txt", "a");
			fprintf(f, "UNKNOWN ERROR: get subscribed vehicle data failed\n");
			fclose(f);
			RealSimShutdown();
			return 1; // return 1 to suppress error messages from VISSIM
		}

		// ===========================================================================
		// 			Set Desired Acceleration If Vehicle is Controlled
		// ===========================================================================
		// set the desired acceleration (desired speed should be bypassed if acceleration is set)
		// only set desired acceleration for vehicles received from Traffic Layer
		try {
			if (Msg_c.VehDataRecv_um.find(VissimVehDataCommon.id) != Msg_c.VehDataRecv_um.end()) {
				/*VissimVehDataCommon.accelerationDesired = Msg_c.VehDataRecv_um[VissimVehDataCommon.id].accelerationDesired;
				VissimVehDataCommon.speedDesired = VissimVehDataCommon.speed + 0.1 * VissimVehDataCommon.accelerationDesired;*/
				if ((Msg_c.VehicleMessageField_set.find("accelerationDesired") != Msg_c.VehicleMessageField_set.end()) || (Msg_c.VehicleMessageField_set.find("speedDesired") != Msg_c.VehicleMessageField_set.end())) {

					float speedDes = VissimVehDataCommon.speedDesired;
					double accelDes = VissimVehDataCommon.accelerationDesired;
					if (Msg_c.VehicleMessageField_set.find("accelerationDesired") != Msg_c.VehicleMessageField_set.end()) {
						// if defined acceleration, then use acceleration, otherwise, use speed
						accelDes = Msg_c.VehDataRecv_um[VissimVehDataCommon.id].accelerationDesired;
						speedDes = accelDes * 0.1 + VissimVehDataCommon.speed;
						//speedDes = std::any_cast<float>(Msg_c.VehDataRecv_um[VissimVehDataCommon.id]["speedDesired"]);
						//accelDes = ((double) speedDes - VissimVehDataCommon.speed) / 0.1;

					}
					else if (Msg_c.VehicleMessageField_set.find("speedDesired") != Msg_c.VehicleMessageField_set.end()) {
						//int msgboxID = MessageBox(
						//	NULL,
						//	(LPCSTR)L"VISSIM must have \"accelerationDesired\" data field selected!",
						//	(LPCSTR)L"DriverModelDll",
						//	MB_ICONERROR | MB_OK
						//);

						//switch (msgboxID)
						//{
						//case IDOK:
						//	// TODO: add code
						//	break;
						//}
						speedDes = Msg_c.VehDataRecv_um[VissimVehDataCommon.id].speedDesired;

						accelDes = ((double)speedDes - VissimVehDataCommon.speed) / 0.1;
						//accelDes = ((double)speedDes - (VissimVehDataCommon.speed + 0.1 * VissimVehDataCommon.accelerationDesired)) / 0.1;

					}

					if (ENABLE_LOG) {
						FILE* f = fopen("DriverModelLog.txt", "a");
						fprintf(f, "Control vehicle id %s: speed desired from %f to %f, \n\t\t acceleration desired from %f to %f\n",
							VissimVehDataCommon.id.c_str(), VissimVehDataCommon.speedDesired, VissimVehDataCommon.speed + 0.1 * accelDes,
							VissimVehDataCommon.accelerationDesired, accelDes);
						fclose(f);
					}

					VissimVehDataCommon.accelerationDesired = accelDes;
					if (!ENABLE_EXT_DYN) {
						VissimVehDataCommon.speedDesired = VissimVehDataCommon.speed + 0.1 * VissimVehDataCommon.accelerationDesired;
					}
				}

				// if want to set color of the vehicle
				if (Msg_c.VehicleMessageField_set.find("color") != Msg_c.VehicleMessageField_set.end() && VissimVehDataCommon.id.compare("1") == 0) {
					// RealSim is rgba
					//uint32_t colorDes = std::any_cast<uint32_t>(Msg_c.VehDataRecv_um[VissimVehDataCommon.id]["color"]);
					////uint32_t colorDes = 2155872511; //808000FF
					//uint8_t r = (colorDes >> 24) & 0xFF;
					//uint8_t g = (colorDes >> 16) & 0xFF;
					//uint8_t b = (colorDes >> 8) & 0xFF;
					//uint8_t a = (colorDes) & 0xFF;

					uint8_t r = 64;
					uint8_t g = 255;
					uint8_t b = 64;
					uint8_t a = 255;
					// VISSIM is argb code
					VissimVehDataCommon.color = (a << 24) + (r << 16) + (g << 8) + b;
				}
			}
		}
		catch (const std::exception& e) {
			FILE* f = fopen("DriverModelError.txt", "a");
			fprintf(f, "ERROR: %s\n", e.what());
			fclose(f);
			RealSimShutdown();
			return 1; // return 1 to suppress error messages from VISSIM
		}
		catch (...) {
			FILE* f = fopen("DriverModelError.txt", "a");
			fprintf(f, "UNKNOWN ERROR: set ego vehicle speed/acceleration failed\n");
			fclose(f);
			RealSimShutdown();
			return 1; // return 1 to suppress error messages from VISSIM
		}

		// ===========================================================================
		// 			Set Lane Change Command
		// ===========================================================================
		try {
			if (Msg_c.VehDataRecv_um.find(VissimVehDataCommon.id) != Msg_c.VehDataRecv_um.end()) {
				if (Msg_c.VehicleMessageField_set.find("activeLaneChange") != Msg_c.VehicleMessageField_set.end()) {
					int8_t activeLaneChange = 0;
					activeLaneChange = Msg_c.VehDataRecv_um[VissimVehDataCommon.id].activeLaneChange;

					// if vehicle need to make lane change due to route or path, then lane change command from controller will be ignored
					if (vehPreferredRelLane != 0 && activeLaneChange != vehPreferredRelLane) {
						activeLaneChange = min(max(vehPreferredRelLane, -1), 1);
					}
					// activeLaneChange command has to be -1, 1, 0, otherwise record error message
					else if (activeLaneChange > 1 || activeLaneChange < -1) {
						FILE* f = fopen("DriverModelError.txt", "a");
						fprintf(f, "ERROR: simTime %f, incorrect activeLaneChange %d (has to be one of -1,1,0), use VISSIM default value\n", simTime, activeLaneChange);
						fclose(f);
						activeLaneChange = min(max(vehPreferredRelLane, -1), 1);;
					}
					// if already finish lane change for current step, then reset to 0
					else if (laneChangeStatus != 0) {
						activeLaneChange = 0;
					}
					// if at leftmost lane but want to change to left, or rightmost lane but want to change to right
					// then record error message
					// rightmost == 1, activeLaneChange +1 = to left
					else if ((VissimVehDataCommon.laneId == numOfLanes && activeLaneChange > 0) || (VissimVehDataCommon.laneId == 1 && activeLaneChange < 0)) {
						FILE* f = fopen("DriverModelError.txt", "a");
						fprintf(f, "ERROR: simTime %f, incorrect activeLaneChange %d, already at rightmost or leftmost lanes, use VISSIM default value\n", simTime, activeLaneChange);
						fclose(f);
						activeLaneChange = 0;
					}
					// else, feel free to listen to command
					else {
						// do nothing
					}

					// set activeLaneChange command that will be sent to VISSIM
					VissimVehDataCommon.activeLaneChange = activeLaneChange;

					if (ENABLE_LOG) {
						FILE* f = fopen("DriverModelLog.txt", "a");
						fprintf(f, "enters lane change loop at time %f activeLaneChange %d, vehPreferredRelLane %d\n", simTime, VissimVehDataCommon.activeLaneChange, vehPreferredRelLane);
						fclose(f);
					}

				}

			}

		}
		catch (const std::exception& e) {
			FILE* f = fopen("DriverModelError.txt", "a");
			fprintf(f, "ERROR: %s\n", e.what());
			fclose(f);
			RealSimShutdown();
			return 1; // return 1 to suppress error messages from VISSIM
		}
		catch (...) {
			FILE* f = fopen("DriverModelError.txt", "a");
			fprintf(f, "UNKNOWN ERROR: set ego vehicle lane change failed\n");
			fclose(f);
			RealSimShutdown();
			return 1; // return 1 to suppress error messages from VISSIM
		}

		/*VissimVehDataCommon.accelerationDesired = 1;
		VissimVehDataCommon.speedDesired = VissimVehDataCommon.speed + 0.1*VissimVehDataCommon.accelerationDesired;*/

		// ===========================================================================
		// 			SEND DATA TO TRAFFIC LAYER ====>>>>>>>>>
		// ===========================================================================
		// only send out once when its the LAST vehicle
		// afterwards, VISSIM will proceed one step of simulation t->t+dt
		//if (ENABLE_LOG) {
		//	FILE* f = fopen("DriverModelLog.txt", "a");
		//	fprintf(f, "iVeh_g %d nVeh_g %d\n", iVeh_g, nVeh_g);
		//	fclose(f);
		//}
		if (iVeh_g == nVeh_g - 1 && nVeh_g >= 1) {
			if (ENABLE_REALSIM) {
				//int sendStatus = sendMessage();
			}

			// reset vehicle index counter if last vehicle
			iVeh_g = 0;
		}
		else {
			iVeh_g++;
		}

		ResetVehicleData();

		// End of Current Switch-Case: DRIVER_COMMAND_MOVE_DRIVER
		//=========================
		//=========================
		return 1;


	default:
		return 0;
	}
}

//DRIVERMODEL_API void test(MsgHelper Msg_c) {
//
//	int aa = 1;
//
//}
//
//DRIVERMODEL_API void test2(int   type,
//						int   index1,
//						int   index2,
//						int   index3,
//						int int_value,
//						double double_value) {
//
//	int aa = 1;
//
//	TrafficLightData_t Sig{ index1, "", "" };
//
//	int state = int_value;
//
//	if (state == 1) {
//		Sig.state.append("G");
//	}
//	else if (state == 8) {
//		Sig.state.append("y");
//	}
//	else if (state == 2) {
//		Sig.state.append("r");
//	}
//	else {
//		Sig.state.append("z");
//	}
//
//	if (Msg_c.TlsDataSend_v.size() == 0) {
//		Msg_c.TlsDataSend_v.push_back(vector <TrafficLightData_t>{Sig});
//	}
//	else {
//		Msg_c.TlsDataSend_v[0].push_back(Sig);
//	}
//}


/*==========================================================================*/
/*  End of DriverModel.cpp                                                  */
/*==========================================================================*/
