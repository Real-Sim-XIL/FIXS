/*==========================================================================*/
/*  SC_DLL_MAIN.CPP                                                         */
/*                                                                          */
/*  Main module of a signal controller DLL for VISSIM.                      */
/*                                                                          */
/*  Version of 2007-01-17                                   Lukas Kautzsch  */
/*==========================================================================*/

#include "lsa_fehl.h"
#include "sc_dll_functions.h"
#include "sc_dll_main.h"

#include <assert.h>
#include <stdio.h>
#include <tchar.h>

#pragma unmanaged


#include <unordered_set>
#include <tuple>
#include "SocketHelper.h"
#include "lsa_puff.h"

using namespace std;

// Master Switches
bool ENABLE_REALSIM = false;

unordered_set <unsigned long> scNo_s;
int nSC_g = 0;
int iSC_g = 0;

double simTime = 0.0;

const vector <int> selfServerPortUserInput = { 1338 };
SocketHelper Sock_c;
MsgHelper Msg_c;

const int realSimFreq = 1; // communicate with RealSim 1 times per second
const int baseFreq = 10; // base frequency, how frequency vissim signal controller should update

bool isVeryFirstStep = true;

typedef int(__stdcall* f_funci)(MsgHelper);
f_funci funci;

// map signal controller number/name to the current status
// then need to pass these signal controller values to the traffic layer
//vector <TlsDetector_t> DetStorage;
//vector <TrafficLightData_t> SigStorage;
//int scNo, string scName, 
//vector <TrafficLightData_t> Sig_v;

/*--------------------------------------------------------------------------*/

void  SC_DLL_ReadDataFiles(unsigned long sc_no, const TCHAR* filename1, const TCHAR* filename2)
{
	/* Reads the data file(s) of the signal controller number <sc_no>. */
	/* The filenames (as user defined in VISSIM) are <filename1> and   */
	/* <filename2> (NULL if empty in VISSIM, i.e. not used).           */
	/* This function is called from VISSIM once per SC at the start of */
	/* a simulation (or test) run, before SC_DLL_Init() is called for  */
	/* any SC.                                                         */

	/* ### */

} /* SC_DLL_ReadDataFiles */

/*--------------------------------------------------------------------------*/

void  SC_DLL_Init(unsigned long sc_no)
{
	/* Initializes the controller logic of SC number <sc_no>.          */
	/* This function is called from VISSIM once per SC at the start of */
	/* a simulation (or test) run, after SC_DLL_ReadDataFiles() has    */
	/* been called for all SC's.                                       */
	/* It should call SC_SetFrequency() if controller <sc_no> doesn't  */
	/* run at one controller time step per simulation second.          */


	SC_SetFrequency(sc_no, baseFreq); // 

	// add all sc numbers to a set
	scNo_s.insert(sc_no);
	nSC_g = scNo_s.size();


	/*!!!initialize socket here*/
	Sock_c.socketSetup(selfServerPortUserInput); // no Server 

	Sock_c.disableWaitClientTrigger();

	if (ENABLE_REALSIM) {
		Sock_c.initConnection();
	}

} /* SC_DLL_Init */

/*--------------------------------------------------------------------------*/

void  SC_DLL_Calculate(unsigned long sc_no)
{
	/* Executes one pass through the controller logic of SC no. <sc_no>.  */
	/* This function is called from VISSIM once per SC at the end of each */
	/* signal control interval, after the (detector) data for all SC's    */
	/* has been passed to the controller DLL.                             */

	/* ### */

	// Cycle length typically 1 seconds, that means this will be triggered every 1 seconds

	if (iSC_g == 0) {
		simTime = Sim_Time();

		// ===========================================================================
		// 			RECV DATA FROM TRAFFIC LAYER <<<<<<<<<=======
		// ===========================================================================
		// only receive once when its the FIRST signal light
		if (ENABLE_REALSIM) {
			if (isVeryFirstStep) {
				isVeryFirstStep = false;
			}
			else {
				// reset receive buffer
				Msg_c.clearRecvStorage();

				// recv from Traffic Layer
				int simStateRecv;
				float simTimeRecv;
				Sock_c.recvData(Sock_c.clientSock[0], &simStateRecv, &simTimeRecv, Msg_c);
			}
			
		}
	}

	// ===========================================================================
	// 			Update Signal and Detector Data to Send
	// ===========================================================================
	// Only update signal group data and detector data once per realSimFreq
	if (abs(round(simTime * realSimFreq) - (simTime * realSimFreq)) < 1e-5) {
		sg_iterator sg_iter = SignalGroups();
		detector_iterator det_iter = Detectors();

		// each signal light will have multiple detectors, but only one signal group
		vector <DetectorData_t> Det_v;

		int nDet = 0;
		while (det_iter.IsValid()) {
			T_Det iDet = (*det_iter);

			DetectorData_t det;

			det.id = iDet.VISSIM_Nr; //this is port number

			det.name = Det_Name(sc_no, det.id);
			det.state = Det_Detection(sc_no, det.id); // state 0, 1

			Det_v.push_back(det);

			++det_iter;
		}
		TlsDetector_t det_tuple{ sc_no, "", Det_v };


		TrafficLightData_t Sig{ sc_no, "", "" };
		int nSig = 0;
		while (sg_iter.IsValid()) {
			T_SigGr iSig = (*sg_iter);

			long id = iSig.VISSIM_Nr; // this is phase of each signal
			string name = SG_Name(sc_no, id); // this is name
			int state = SG_CurrentState(sc_no, id); // this is state, 

			//!!!!
			// AT THIS MOMENT, ONLY SUPPORT BASIC RED, GREEN, AMBER lights, and OFF
			//if (Sig.state != SG_STATE_GREEN && Sig.state != SG_STATE_AMBER && Sig.state != SG_STATE_RED && Sig.state != SG_STATE_OFF) {
			//	
			//	return;
			//}
			if (state == SG_STATE_GREEN) {
				Sig.state.append("G");
			}
			else if (state == SG_STATE_AMBER) {
				Sig.state.append("y");
			}
			else if (state == SG_STATE_RED) {
				Sig.state.append("r");
			}
			else if (state == SG_STATE_OFF) {
				Sig.state.append("O");
			}
			else {
				Sig.state.append("z");
			}

			++sg_iter;
		}

		// only communicate with Traffic Layer once at the last SC. then after this, VISSIM will proceed one step of simulation
		// iSC_g is 0 index. do not send if none SC is controlled here

		if (Msg_c.DetDataSend_v.size() == 0) {
			Msg_c.DetDataSend_v.push_back(vector <TlsDetector_t>{det_tuple});
		}
		else {
			Msg_c.DetDataSend_v[0].push_back(det_tuple);
		}
		if (Msg_c.TlsDataSend_v.size() == 0) {
			Msg_c.TlsDataSend_v.push_back(vector <TrafficLightData_t>{Sig});
		}
		else {
			Msg_c.TlsDataSend_v[0].push_back(Sig);
		}
	}


	if (iSC_g == nSC_g - 1 && nSC_g >= 1) {
		// ===========================================================================
		// 			SEND DATA TO TRAFFIC LAYER ====>>>>>>>>>
		// ===========================================================================
		// only send out once when its the LAST intersection
		if (ENABLE_REALSIM) {
			// send out to Traffic Layer
			float simTimeSend = simTime;
			uint8_t simStateSend = 1;

			Msg_c.VehDataSend_um[Sock_c.clientSock[0]] = {};
			if (Msg_c.DetDataSend_v.size() == 0) {
				Msg_c.TlsDataSend_um[Sock_c.clientSock[0]] = {};
			}
			else {
				Msg_c.TlsDataSend_um[Sock_c.clientSock[0]] = Msg_c.TlsDataSend_v[0];
			}
			if (Msg_c.TlsDataSend_v.size() == 0) {
				Msg_c.DetDataSend_um[Sock_c.clientSock[0]] = {};
			}
			else {
				Msg_c.DetDataSend_um[Sock_c.clientSock[0]] = Msg_c.DetDataSend_v[0];
			}

			Sock_c.sendData(Sock_c.clientSock[0], 0, simTimeSend, simStateSend, Msg_c);

			Msg_c.clearSendStorage();
		}  

		// reset signal controller index counter if last intersection
		iSC_g = 0;

	}
	else {
		iSC_g++;
	}

} /* SC_DLL_Calculate */

/*--------------------------------------------------------------------------*/

void  SC_DLL_Cleanup(unsigned long sc_no)
{
	/* Closes the controller logic of SC number <sc_no>.             */
	/* This function is called from VISSIM once per SC at the end of */
	/* a simulation (or test) run.                                   */

	/* ### */

} /* SC_DLL_Cleanup */

/*--------------------------------------------------------------------------*/

void  SC_DLL_WriteSnapshotFile(unsigned long sc_no, const TCHAR* filename)
{
	/* Writes the current internal data of SC number <sc_no> to the file */
	/* <filename> in the current directory.                              */
	/* This function is called from VISSIM once per SC when the user     */
	/* selects Save Snapshot from the Simulation menu during a           */
	/* simulation run.                                                   */

	/* ### */

} /* SC_DLL_WriteSnapshotFile */

/*--------------------------------------------------------------------------*/

void  SC_DLL_ReadSnapshotFile(unsigned long sc_no, const TCHAR* filename)
{
	/* Reads the current internal data of SC number <sc_no> from the file */
	/* <filename> in the current directory.                               */
	/* This function is called from VISSIM once per SC when the user      */
	/* selects Load Snapshot from the Simulation menu.                    */

	/* ### */

} /* SC_DLL_ReadSnapshotFile */

/*==========================================================================*/
/*  End of SC_DLL_MAIN.CPP                                                  */
/*==========================================================================*/
