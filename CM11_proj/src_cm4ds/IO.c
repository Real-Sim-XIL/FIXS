/*
******************************************************************************
**  CarMaker - Version 11.0.1
**  Vehicle Dynamics Simulation Toolkit
**
**  Copyright (C)   IPG Automotive GmbH
**                  Bannwaldallee 60             Phone  +49.721.98520.0
**                  76185 Karlsruhe              Fax    +49.721.98520.99
**                  Germany                      WWW    www.ipg-automotive.com
******************************************************************************
**
** Connection to I/O hardware of the CarMaker/HIL test stand
**
** Connected test rig: ???
**
******************************************************************************
**
** Functions
** ---------
**
** - iGetCal ()
** - CalIn ()
** - CalInF ()
** - CalOut ()
** - CalOutF ()
** - LimitInt ()
** - IO_Init_First ()
** - IO_Init_Finalize ()
** - IO_Init ()
** - IO_Param_Get ()
** - IO_BeginCycle ()
** - IO_In ()
** - IO_Out ()
** - IO_Cleanup ()
**
******************************************************************************
*/

#include <Global.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include <CarMaker.h>

#include <dsio.h>
#include <ioconf.h>
#include <can_interface.h>
#include <FailSafeTester.h>
#include <flex.h>
#include <rbs.h>
#include <CM_XCP.h>
#include <CM_CCP.h>

#include "IOVec.h"


/*** I/O vector */
tIOVec IO;


/*** I/O configuration */

/* int IO_None; DON'T - Variable is predefined by CarMaker! */
int IO_CAN_IF;
int IO_FlexRay;

static struct tIOConfig IOConfiguration[] = {
/* This table should contain one line for each IO_xyz-flag in IOVec.h */
/*  { <Flagvar>,	<Name for -io>,	<Description for -help> },     */
    { &IO_None,		"none",		"No I/O" }, /* Always keep this first line! */
    { &IO_CAN_IF,	"can",		"CAN communication" },
    { &IO_FlexRay,	"flexray",	"FlexRay communication" },
    { NULL, NULL, NULL } /* End of table */
};



/**** Additional useful functions *********************************************/


/*
** iGetCal()
**
** Read calibration parameters.
*/

void
iGetCal (tInfos *Inf, const char *key, tCal *cal, int optional)
{
    cal->Min       =  1e37;
    cal->Max       = -1e37;
    cal->LimitLow  =  1e37;
    cal->LimitHigh = -1e37;
    cal->Factor    =  1.0;
    cal->Offset    =  0.0;
    cal->Rezip     =  0;

    const char *item = iGetStrOpt(Inf, key, NULL);
    if (item != NULL) {
	int n = sscanf(item, "%g %g %g %g %d",
		       &cal->LimitLow, &cal->LimitHigh,
		       &cal->Factor,   &cal->Offset, &cal->Rezip);
	if (n != 5)
	    LogErrF(EC_Init, "Invalid calibration parameter entry '%s'", key);
    } else {
	if (!optional) {
	    LogErrF(EC_Init, "Missing calibration parameter entry '%s'", key);
	    return;
	}
	cal->LimitLow  = -1e37;
	cal->LimitHigh =  1e37;
    }

    cal->Min = cal->LimitHigh;
    cal->Max = cal->LimitLow;
}


/*
** CalInF() / CalIn()
**
** Analog input -> calibration infos -> physical quantity
** Converts an I/O value (e.g. the voltage from an analog input module) to
** the corresponding physical value, delimited by LimitLow and LimitHigh.
*/

float
CalInF (tCal *cal, float Value)
{
    float Result = (Value - cal->Offset) * cal->Factor;

    if (cal->Rezip)
	Result = 1.0 / Result;

    if      (Result < cal->Min)  cal->Min = Result;
    else if (Result > cal->Max)  cal->Max = Result;

    if      (Result < cal->LimitLow)   Result = cal->LimitLow;
    else if (Result > cal->LimitHigh)  Result = cal->LimitHigh;

    return Result;
}

float
CalIn (tCal *cal, int Value)
{
    return CalInF(cal, Value);
}


/*
** CalOutF() / CalOut()
**
** Physical quantity -> calibration infos -> analog output
** The physical value is delimited by LimitLow and LimitHigh and then converted
** to the corresponding I/O value (e.g. voltage for an analog output module).
*/

float
CalOutF (tCal *cal, float Value)
{
    if      (Value < cal->Min) cal->Min = Value;
    else if (Value > cal->Max) cal->Max = Value;

    if      (Value < cal->LimitLow)  Value = cal->LimitLow;
    else if (Value > cal->LimitHigh) Value = cal->LimitHigh;

    if (cal->Rezip) {
	return 1.0 / (Value*cal->Factor) + cal->Offset;
    } else {
	return Value/cal->Factor + cal->Offset;
    }
}

int
CalOut (tCal *cal, float Value)
{
    return (int)CalOutF(cal, Value);
}


int
LimitInt (float fValue, int Min, int Max)
{
    int Value = (int)fValue;
    if      (Value < Min) return Min;
    else if (Value > Max) return Max;
    return   Value;
}



/*****************************************************************************/


/*
** IO_Init_First ()
**
** First, low level initialization of the IO module
**
** Call:
** - one times at start of program
** - no realtime conditions
*/

int
IO_Init_First (void)
{
    memset(&IO,  0, sizeof(IO));

    IO_SetConfigurations(IOConfiguration);

    CANIf_Init_First();
    FC_Init_First();
    RBS_Init_First();
    if (DSCAN_Init(16) < 0) {
	LogErrF(EC_Init, "Failed to initialize CarMaker CAN communication.");
	return -1;
    }
    if (DSIO_Init_CAN_Support() < 0 || DSIO_Init() < 0) {
	LogErrF(EC_Init, "Failed to initialize CarMaker DSIO module.");
	return -2;
    }

    return 0;
}



/*
** IO_Init ()
**
** initialization
** - i/o hardware
** - add variables to data dictionary
**
** call:
** - single call at program start
*/

int
IO_Init (void)
{
    Log("I/O Configuration: %s\n", IO_ListNames(NULL, 1));


    /* hardware configuration "none" */
    if (IO_None)
	return 0;

#define FST_CAN_QUEUE 0
    /*** CAN message queue for FailSafeTester */
    if (DSCAN_QueueConfig(FST_CAN_QUEUE, 16) < 0) {
	LogErrF(EC_General, "Failed to initialize FST CAN msg queue");
	return -1;
    }


    if (IO_CAN_IF) {
	if (CANIf_Init() < 0)
	    return -1;
    }
    if (IO_FlexRay) {
	if (FC_Init())
	    return -1;
    }
    if (RBS_Init())
        return -1;

    return 0;
}


/*
** IO_Init_Finalize ()
**
** last (deferred) I/O initialization step
**
** call:
** - single call at program start in CarMaker_FinishStartup()
*/

int
IO_Init_Finalize (void)
{
    RBS_MapQuants();
    if (IO_FlexRay) {
	if (FC_Start())
	    return -1;
    }
    if (IO_CAN_IF) {
	if (CANIf_Init_Finalize() < 0)
	    return -1;
    }


    if (!IO_None) {
	/* CAN message queue for FailSafeTester */
	DSIO_CAN_SetDSCAN_Qu(FST_CAN_Slot, FST_CAN_Ch, FST_CAN_QUEUE);
	DSIO_CAN_SetAcceptCodeMask(FST_CAN_Slot, FST_CAN_Ch, 0, 0xffffffff);
	if (RBS_Start())
            return -1;
    }

    return 0;
}


/*
** IO_Param_Get ()
**
** Get i/o configuration parameters
** - calibration
** - constant values
** - ids
*/

int
IO_Param_Get (tInfos *inf)
{
    unsigned nError = GetInfoErrorCount ();

    /* ignition off */
    SetKl15 (0);

    if (IO_None)
    	return 0;

    if (IO_CAN_IF)
	CANIf_Param_Get(inf, NULL);
    if (IO_FlexRay)
	FC_Param_Get(inf, NULL);
    RBS_Param_Get(inf, NULL);

    return nError != GetInfoErrorCount() ? -1 : 0;
}


void
IO_BeginCycle (void)
{
}


/*
** IO_In ()
**
** reading signals from hardware / ECU
**
** CycleNo: simulation cycle counter, incremented every loop/millisecond
**
** call:
** - in the main loop
** - first function call in main loop, after waiting for next loop
** - just before User_In()
** - pay attention to realtime condition
*/

void
IO_In (unsigned CycleNo)
{
    CAN_Msg Msg;

    IO.DeltaT = SimCore.DeltaT;
    IO.T      = TimeGlobal;
    if (IO_None)
	return;

    /*** FailSafeTester messages */
    if (FST_IsActive()) {
	while (DSIO_CAN_Recv(FST_CAN_Slot, FST_CAN_Ch, &Msg) == 0)
	    FST_MsgIn (CycleNo, &Msg);
    }

    if (IO_CAN_IF)
	CANIf_In(CycleNo); 
    if (IO_FlexRay)
	FC_In(CycleNo);
    RBS_In(CycleNo);
}



/*
** IO_Out ()
**
** writing signals to hardware / ECU
**
** CycleNo: simulation cycle counter, incremented every loop/millisecond
**
** call:
** - in the main loop
** - last function call in main loop
** - just after User_Out()
** - pay attention to realtime condition
*/

void
IO_Out (unsigned CycleNo)
{
    if (IO_None)
	return;

    /*** Messages to the FailSafeTester */
    FST_MsgOut(CycleNo);

    RBS_Out(CycleNo);
}



/*
** IO_Cleanup ()
**
** Uninits all MIO hardware:
** - puts M-Modules into reset state
** - frees unneeded memory
*/

void
IO_Cleanup (void)
{
    if (IO_None)
	goto EndReturn;

    RBS_Cleanup();
    if (IO_FlexRay)
	FC_Cleanup();
    if (IO_CAN_IF)
	CANIf_Cleanup();

  EndReturn:
    DSIO_Cleanup();
    DSCAN_Cleanup();
    return;
}
