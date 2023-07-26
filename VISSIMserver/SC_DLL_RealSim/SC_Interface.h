/*==========================================================================*/
/*  SC_Interface.h                                   DLL Module for VISSIM  */
/*                                                                          */
/*  Interface module for signal controller DLLs.                            */
/*                                                                          */
/*  Version of 2009-10-19                                   Lukas Kautzsch  */
/*==========================================================================*/

#ifndef __SC_INTERFACE_H
#define __SC_INTERFACE_H

#include "lsapuffr.h"

#ifndef _CONSOLE
#include <windows.h>
#endif

/* In the creation of a controller DLL all files must be compiled */
/* with the preprocessor definition SC_DLL_EXPORTS.               */
/* Programs that use the controller DLL must not be compiled      */
/* with that preprocessor definition.                             */

#ifdef SC_DLL_EXPORTS
#define SC_DLL_API extern "C" __declspec(dllexport)
#else
#define SC_DLL_API extern "C" __declspec(dllimport)
#endif

/*==========================================================================*/

/* termination types: */
#define  TERM_SUCCESSFUL                        1
#define  TERM_ERRORFILE_CREATED                 2
#define  TERM_ABORT                             3

/* global VISSIM data: */
#define  SC_DLL_DATA_NEMA_LEVEL                 1
           /* long: NEMA level from VISSIM license */
#define  SC_DLL_DATA_SIM_TIME                   2
           /* double: current simulation time [s] */
#define  SC_DLL_DATA_SNAPSHOT_FILE              3
           /* string: VISSIM snapshot file name (including .snp) */
#define  SC_DLL_DATA_LICENSE_CONTROLLERS        4
           /* long: Some license relevant informations */
#define  SC_DLL_DATA_SIM_TIME_OF_DAY            5
           /* string: current simulation time of day [hh:mm:ss,s] */
#define  SC_DLL_DATA_SIM_DATE                   6
           /* long: simulation start date (from Simulation Parameters) [YYYYMMDD] */
#define  SC_DLL_DATA_CONSOLE_MODE               7
           /* long: whether Vissim was started in console mode or not */
#define  SC_DLL_DATA_PROCESS_ID                 8
           /* long: the process id, if VISSIM was started in parallel mode. Otherwise */
           /*       it is NO_PROCESS_ID.                                              */
#define  SC_DLL_DATA_TERMINATION               10
           /* long: program termination type:                             */
           /*       TERM_SUCCESSFUL, TERM_ERRORFILE_CREATED or TERM_ABORT */
           /* string: the name of the error file if it was created and    */
           /*         TERM_ERRORFILE_CREATED was set.                     */
#define  SC_DLL_DATA_SIM_CANCELED              11
           /* long: Set from VISSIM for each controller before      */
           /* SC_DLL_COMMAND_EXIT_CONTROLLER is called: to 1 if the */
           /* simulation has been cancelled by the user before the  */
           /* end of the simulation period, else to 0.              */
#define  SC_DLL_DATA_RBC_LEVEL                 12
           /* long: RBC level from the VISSIM license */
#define  SC_DLL_CHECK_SIGNAL_SEQUENCE          13
           /* long: 1 if the signal sequence should be validated, otherwise 0. */
#define  SC_DLL_DATA_SIM_SECONDS_SINCE_MIDNIGHT 14
           /* double: current simulation seconds since midnight [s] */

/* global signal controller data: */
/* (index1 = data file number [1..2]) */
#define  SC_DLL_DATA_DATA_FILE                101
           /* string: data file name (including .extension) */
/* (no index) */
#define  SC_DLL_DATA_CYCLE_TIME               102
           /* double: full cycle time (period) [s] */
#define  SC_DLL_DATA_CYCLE_SECOND             103
           /* double: current cycle second [s] */
#define  SC_DLL_DATA_INDEX                    104
           /* long: global unique index of the current signal controller [0..n] */

/* signal group data: */
/* (index1 = signal group number) */
#define  SC_DLL_DATA_SG_TRANSITION            201
           /* long: 1 if automatic transition (amber, red/amber) */
           /*       is to be done for signal group, else 0       */
#define  SC_DLL_DATA_SG_T_GREEN_MIN           202
           /* double: minimum green time [s] */
#define  SC_DLL_DATA_SG_T_RED_MIN             203
           /* double: minimum red time [s] */
#define  SC_DLL_DATA_SG_T_AMBER               204
           /* double: amber time [s] */
#define  SC_DLL_DATA_SG_T_RED_AMBER           205
           /* double: red/amber time [s] */
#define  SC_DLL_DATA_SG_CURR_DURATION         206
           /* double: elapsed time [s] with the current signal state */
#define  SC_DLL_DATA_SG_GREENEND              207
/* double: the green end time [s] */
#define  SC_DLL_DATA_SG_REDEND                208
/* double: the red end time [s] */
#define  SC_DLL_DATA_SG_GREENEND2             209
/* double: the green end time 2 [s] */
#define  SC_DLL_DATA_SG_REDEND2               210
/* double: the red end time 2 [s] */

/* SC/Det. Record data: */
#define  SC_DLL_DATA_NO_OF_RECORD_DATA        301
           /* long: number of values available for SC/Det. Record */
/* (index1 = data record index [1..SC_DLL_DATA_NO_OF_RECORD_VALUES]) */
#define  SC_DLL_DATA_RECORD_DATA_TYPE         302
           /* long:  type code of data record [index1]  */
#define  SC_DLL_DATA_RECORD_DATA_NUMBER       303
           /* long: value of data record [index1]      */
#define  SC_DLL_DATA_RECORD_DATA_VALUE        304
           /* long: value of data record [index1]      */
#define  SC_DLL_DATA_DETECTOR_NAME           3103
           /* string: detector name                    */
           /*         The name will be sent only once  */
           /*         before the first step.           */
#define  SC_DLL_DATA_SG_NAME                 3104
           /* string: signal group name                */
           /*         The name will be sent only once  */
           /*         before the first step.           */

#define SC_DLL_DATA_OPTIMIZATION_NO_SIGNALGROUPS         10001
           /* long: 1 no signalization, otherwise 0                    */
           /*       Whether signalization data should be sent or not   */

#define SC_DLL_DATA_OPTIMIZATION_NO_DETECTORS            10002
           /* long: 1 no detector values, otherwise 0              */
           /*       Whether detector data should be sent or not    */

#define SC_DLL_DATA_OPTIMIZATION_NO_SIMTIMEOFDAY          10003
           /* long: 1 no sim time of day as string, otherwise 0               */
           /*       Whether sim time of day as string should be sent or not   */



/* All types from lsapuffr.h [1000..11000] have long values. */

/*--------------------------------------------------------------------------*/
#ifdef _UNICODE
SC_DLL_API  int  ControllerSetValueW(long   type,
                                    long   sc_no,
                                    long   index1,
                                    long   index2,
                                    long   long_value,
                                    double double_value,
                                    const wchar_t   *string_value);

/* Called from VISSIM to set the value of a data object of type <type> */
/* (see definition of type numbers above) of controller no. <sc_no>,   */
/* selected by <index1> and possibly <index2>, to <long_value>,        */
/* <double_value> or <*string_value> (object and value selection       */
/* depending on <type>).                                               */
/* Return value is 1 on success, otherwise 0.                          */

/*--------------------------------------------------------------------------*/

SC_DLL_API  int  ControllerGetValueW(long   type,
                                     long   sc_no,
                                     long   index1,
                                     long   index2,
                                     long   *long_value,
                                     double *double_value,
                                     wchar_t   *string_value);

/* Called from VISSIM to get the value of a data object of type <type>  */
/* (see definition of type numbers above) of controller no. <sc_no>,    */
/* selected by <index1> and possibly <index2>.                          */
/* This function writes that value to <*long_value>, <*double_value>    */
/* or <*string_value> (object and value selection depending on <type>). */
/* Return value is 1 on success, otherwise 0.                           */

#endif

/*--------------------------------------------------------------------------*/

SC_DLL_API  int  ControllerSetValue (long   type,
                                     long   sc_no,
                                     long   index1,
                                     long   index2,
                                     long   long_value,
                                     double double_value,
                                     char   *string_value);

/* Called from VISSIM to set the value of a data object of type <type> */
/* (see definition of type numbers above) of controller no. <sc_no>,   */
/* selected by <index1> and possibly <index2>, to <long_value>,        */
/* <double_value> or <*string_value> (object and value selection       */
/* depending on <type>).                                               */
/* Return value is 1 on success, otherwise 0.                          */

/*--------------------------------------------------------------------------*/

SC_DLL_API  int  ControllerGetValue (long   type,
                                     long   sc_no,
                                     long   index1,
                                     long   index2,
                                     long   *long_value,
                                     double *double_value,
                                     char   *string_value);

/* Called from VISSIM to get the value of a data object of type <type>  */
/* (see definition of type numbers above) of controller no. <sc_no>,    */
/* selected by <index1> and possibly <index2>.                          */
/* This function writes that value to <*long_value>, <*double_value>    */
/* or <*string_value> (object and value selection depending on <type>). */
/* Return value is 1 on success, otherwise 0.                           */

/*==========================================================================*/

#define  SC_DLL_COMMAND_CREATE_CONTROLLER   0
           /* Called from VISSIM at the start of a simulation run   */
           /* once for each controller.                             */
           /* Values set before: SC_DLL_DATA_PROCESS_ID             */
           /*                    SC_DLL_DATA_CONSOLE_MODE           */
           /* Values got after:  ---                                */

#define  SC_DLL_COMMAND_READ_DATA_FILES     1
           /* Called from VISSIM at the start of a simulation run   */
           /* once for each controller (after                       */
           /* SC_DLL_COMMAND_CREATE_CONTROLLER) and possibly later  */
           /* if a user explicitly selects "Reload Data" in VISSIM. */
           /* Values set before: SC_DLL_DATA_DATA_FILE[0] + [1]     */
           /*                    PROGRAMMNUMMER                     */
           /*                    SC_DLL_DATA_NEMA_LEVEL             */
           /*                      (only for SC of type NEMA)       */
           /*                    SC_DLL_DATA_LICENSE_CONTROLLERS    */
           /*                    SC_DLL_DATA_RBC_LEVEL              */
           /*                      (only for SC of type RBC)        */
           /* Values got after:  ---                                */

#define  SC_DLL_COMMAND_INIT_CONTROLLER     2
           /* Called from VISSIM at the start of a simulation run   */
           /* once for each controller (after                       */
           /* SC_DLL_COMMAND_READ_DATA_FILES).                      */
           /* Values set before: ---                                */
           /* Values got after:  SCHALTFREQUENZ                     */
           /*                    SC_DLL_CHECK_SIGNAL_SEQUENCE       */

#define  SC_DLL_COMMAND_RUN_CONTROLLER      3
           /* Called from VISSIM for each controller once per       */
           /* controller time step but not more frequently than     */
           /* once per simulation time step.                        */
           /* Values set before: many...                            */
           /* Values got after:  many...                            */

#define  SC_DLL_COMMAND_EXIT_CONTROLLER     4
           /* Called from VISSIM at the end of a simulation run     */
           /* once for each controller.                             */
           /* Values set before: SC_DLL_DATA_SIM_CANCELED           */
           /* Values got before: SC_DLL_DATA_TERMINATION            */

#define  SC_DLL_COMMAND_WRITE_SNAPSHOT      5
           /* Called from VISSIM for each controller when the user  */
           /* selects Save Snapshot from the Simulation menu during */
           /* a simulation run. The controller saves its internal   */
           /* data in a separate snapshot file, appending the SC    */
           /* number to the snapshot filename passed from VISSIM.   */
           /* Values set before: SC_DLL_DATA_SNAPSHOT_FILE          */

#define  SC_DLL_COMMAND_READ_SNAPSHOT       6
           /* Called from VISSIM for each controller (after         */
           /* SC_DLL_COMMAND_CREATE_CONTROLLER and                  */
           /* SC_DLL_COMMAND_READ_DATA_FILES) instead of            */
           /* SC_DLL_COMMAND_INIT_CONTROLLER when the user          */
           /* selects Load Snapshot from the Simulation menu        */
           /* to start a simulation run from a previously saved     */
           /* state (snapshot).                                     */
           /* Values set before: SC_DLL_DATA_SNAPSHOT_FILE          */

/*--------------------------------------------------------------------------*/

SC_DLL_API  int  ControllerExecuteCommand (long number, long sc_no);

/* Called from VISSIM to execute the command <number> if that is */
/* available in the DLL (see definitions above) for controller   */
/* no. <sc_no>.                                                  */
/* Return value is 1 on success, otherwise 0.                    */

/*==========================================================================*/

#endif /* __SC_INTERFACE_H */

/*==========================================================================*/
/*  Ende of SC_Interface.h                                                  */
/*==========================================================================*/
