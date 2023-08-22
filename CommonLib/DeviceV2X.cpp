#include "DeviceV2X.h"

using namespace std;

// default 
DeviceV2X::DeviceV2X()
    : type("RSU")
    , id(999)
{

}


DeviceV2X::DeviceV2X(string typeInput, int idInput) {
	type = typeInput;
	id = idInput;
}

void DeviceV2X::simulateComm(COORD_t pos1, COORD_t pos2, bool* IS_COMM_SUCCESS, double* commLatency) {

    double distToComm = sqrt(pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2));

    // This ideally should be a probability model. 
    // Currently just simiplify the range to be any distance below 300m
    if (distToComm < 500) {
        *IS_COMM_SUCCESS = true;
    }
    else {
        *IS_COMM_SUCCESS = false;
    }

    // for simplicity, latency set to 0 for now
    *commLatency = 0*0.5;
}

void DeviceV2X::getCurrentBsm(double simTime, VehData_t& currentBsm) {
	// bsmBuf ordered by receiving time, so can loop through one by one to handle it.

	double tRecvMax = 0;

	// if there is bsm in the RSU
	if (bsmBuf.size() > 0) {
		// since bsmBuf is ordered, loop through until find the last message that can receive at current sim_time
		for (auto iter = bsmBuf.begin(); iter != bsmBuf.end(); iter++) {
			// if current bsm cannnot be received 
			if (iter->first > simTime + 1e-5 || iter == prev(bsmBuf.end())) {
				bsmBuf.erase(bsmBuf.begin(), iter);
				break;
			}

			// if current bsm can be received 
			//currentBsm_v.push_back(iter->second);
			if (iter->first > tRecvMax) {
				currentBsm = iter->second;
				tRecvMax = iter->first;
			}
		}
	}

	// only send out one BSM that is latest
	

}


void DeviceV2X::pushBsmBuf(double simTime, VehData_t VehData) {

	bool IS_COMM_SUCCESS;
	double commLatency;

	COORD_t posVeh;
	posVeh.x = (double) any_cast<float> (VehData["positionX"]);
	posVeh.y = (double)any_cast<float> (VehData["positionY"]);
	posVeh.z = (double)any_cast<float> (VehData["positionZ"]);

	simulateComm(posVeh, pos, &IS_COMM_SUCCESS, &commLatency);
	double timeRecv = simTime + commLatency;

	if (IS_COMM_SUCCESS) {

		bsmBuf.insert({ timeRecv, VehData });
	}

}