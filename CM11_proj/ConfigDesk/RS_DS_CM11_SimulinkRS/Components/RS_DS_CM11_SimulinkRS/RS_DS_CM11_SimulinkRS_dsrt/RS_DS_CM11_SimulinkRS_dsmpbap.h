/*********************** dSPACE target specific header file ********************
   Include file RS_DS_CM11_SimulinkRS_dsmpbap.h:

   Definitions used for access points

   Fri May 19 17:14:26 2023

   (c) Copyright 2020, dSPACE GmbH. All rights reserved.

 *******************************************************************************/

#ifndef _DSMPB_RS_DS_CM11_SimulinkRS_HEADER_
#define _DSMPB_RS_DS_CM11_SimulinkRS_HEADER_
#include "rtwtypes.h"
#ifdef EXTERN_C
#undef EXTERN_C
#endif

#ifdef __cplusplus
#define EXTERN_C                       extern "C"
#else
#define EXTERN_C                       extern
#endif

/* External declarations for access points prototypes */

/*                                                                            */
/* Declarations of read/write and trigger access points                       */
/*                                                                            */
/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)], non-bus port 1 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P1_S1(uint8_T
  * OutputPortSignalPtr);

/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)], non-bus port 2 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P2_S1(boolean_T
  * OutputPortSignalPtr);

/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)], non-bus port 3 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P3_S1(uint32_T
  * OutputPortSignalPtr);

/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)], non-bus port 4 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P4_S1(uint32_T
  * OutputPortSignalPtr);

/* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_Out [TCP (1)], non-bus port 1 */
EXTERN_C void ap_write_RS_DS_CM11_SimulinkRS_DataOutport1_P1_S1(const uint32_T
  * InputPortSignalPtr);

/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Status//Control_In [TCP (1)], non-bus port 1 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport2_P1_S1(int32_T
  * OutputPortSignalPtr);

/* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/Status//Control_Out [TCP (1)], non-bus port 1 */
EXTERN_C void ap_write_RS_DS_CM11_SimulinkRS_DataOutport2_P1_S1(const boolean_T*
  InputPortSignalPtr);

/* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP Out/Transmit_Out [TCP (1)], non-bus port 1 */
EXTERN_C void ap_write_RS_DS_CM11_SimulinkRS_DataOutport3_P1_S1(const uint8_T
  * InputPortSignalPtr);

/* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP Out/Transmit_Out [TCP (1)], non-bus port 2 */
EXTERN_C void ap_write_RS_DS_CM11_SimulinkRS_DataOutport3_P2_S1(const uint32_T
  * InputPortSignalPtr);

/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Ethernet Setup (1), non-bus port 1 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport3_P1_S1(real_T
  * OutputPortSignalPtr);

/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Transmit_In [TCP (1)], non-bus port 1 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport4_P1_S1(boolean_T
  * OutputPortSignalPtr);

/* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Transmit_In [TCP (1)], non-bus port 2 */
EXTERN_C void ap_read_RS_DS_CM11_SimulinkRS_DataInport4_P2_S1(uint32_T
  * OutputPortSignalPtr);

/*                                                                            */
/* Declarations of function module access points                              */
/*                                                                            */
/* Function module access point of system <Root>/CarMaker    */
EXTERN_C void ap_entry_RS_DS_CM11_SimulinkRS_SIDS1_TID1();
EXTERN_C void ap_exit_RS_DS_CM11_SimulinkRS_SIDS1_TID1();

#ifdef HAVE_RS_DS_CM11_SimulinkRS_DSMPBAP_CUSTOM
#include "RS_DS_CM11_SimulinkRS_dsmpbap_custom.h"
#endif
#endif
