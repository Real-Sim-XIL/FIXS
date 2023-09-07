/***************************************************************************

   Source file RS_DS_CM11_SimulinkRS_dsrtvdf.c:

   Definition of function that initializes the global TRC pointers

   4.6 (02-Nov-2021)
   Fri May 19 17:14:26 2023

   Copyright 2023, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header file. */
#include "RS_DS_CM11_SimulinkRS_dsrtvdf.h"
#include "RS_DS_CM11_SimulinkRS.h"
#include "RS_DS_CM11_SimulinkRS_private.h"

/* Compiler options to turn off optimization. */
#if !defined(DS_OPTIMIZE_INIT_TRC_POINTERS)
#ifdef _MCCPPC

#pragma options -nOt -nOr -nOi -nOx

#endif

#ifdef __GNUC__

#pragma GCC optimize ("O0")

#endif

#ifdef _MSC_VER

#pragma optimize ("", off)

#endif
#endif

/* Definition of Global pointers to data type transitions (for TRC-file access) */
volatile VehDataBus *p_0_RS_DS_CM11_SimulinkRS_VehDataBus_0 = NULL;
volatile real_T *p_0_RS_DS_CM11_SimulinkRS_real_T_1 = NULL;
volatile uint32_T *p_0_RS_DS_CM11_SimulinkRS_uint32_T_2 = NULL;
volatile int32_T *p_0_RS_DS_CM11_SimulinkRS_int32_T_3 = NULL;
volatile uint8_T *p_0_RS_DS_CM11_SimulinkRS_uint8_T_4 = NULL;
volatile boolean_T *p_0_RS_DS_CM11_SimulinkRS_boolean_T_5 = NULL;
volatile struct_GphLW54WCcJjc0lEaqz83B
  *p_1_RS_DS_CM11_SimulinkRS_struct_GphLW54WCcJjc0lEaqz83B_0 = NULL;
volatile real_T *p_1_RS_DS_CM11_SimulinkRS_real_T_1 = NULL;
volatile boolean_T *p_1_RS_DS_CM11_SimulinkRS_boolean_T_2 = NULL;
volatile real_T *p_1_RS_DS_CM11_SimulinkRS_real_T_3 = NULL;
volatile uint8_T *p_1_RS_DS_CM11_SimulinkRS_uint8_T_4 = NULL;
volatile real_T *p_2_RS_DS_CM11_SimulinkRS_real_T_3 = NULL;
volatile uint32_T *p_2_RS_DS_CM11_SimulinkRS_uint32_T_5 = NULL;
volatile boolean_T *p_2_RS_DS_CM11_SimulinkRS_boolean_T_6 = NULL;
volatile uint8_T *p_2_RS_DS_CM11_SimulinkRS_uint8_T_7 = NULL;
volatile boolean_T *p_2_RS_DS_CM11_SimulinkRS_boolean_T_8 = NULL;
volatile real_T *p_3_RS_DS_CM11_SimulinkRS_real_T_0 = NULL;
volatile real_T *RS_DS_CM11_SimulinkRS_RTIDerivativesPtr = 0;

/*
 *  Declare the functions, that initially assign TRC pointers
 */
static void rti_init_trc_pointers_0(void);

/* Global pointers to data type transitions are separated in different functions to avoid overloading */
static void rti_init_trc_pointers_0(void)
{
  p_0_RS_DS_CM11_SimulinkRS_VehDataBus_0 =
    &RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1;
  p_0_RS_DS_CM11_SimulinkRS_real_T_1 = &RS_DS_CM11_SimulinkRS_B.IoIn;
  p_0_RS_DS_CM11_SimulinkRS_uint32_T_2 = &RS_DS_CM11_SimulinkRS_B.ReceivedBytes;
  p_0_RS_DS_CM11_SimulinkRS_int32_T_3 =
    &RS_DS_CM11_SimulinkRS_B.ConnectionState_f;
  p_0_RS_DS_CM11_SimulinkRS_uint8_T_4 = &RS_DS_CM11_SimulinkRS_B.DataVector[0];
  p_0_RS_DS_CM11_SimulinkRS_boolean_T_5 = &RS_DS_CM11_SimulinkRS_B.Status;
  p_1_RS_DS_CM11_SimulinkRS_struct_GphLW54WCcJjc0lEaqz83B_0 =
    &RS_DS_CM11_SimulinkRS_P.RealSimPara;
  p_1_RS_DS_CM11_SimulinkRS_real_T_1 =
    &RS_DS_CM11_SimulinkRS_P.VehicleMessageFieldDefInputVec[0];
  p_1_RS_DS_CM11_SimulinkRS_boolean_T_2 =
    &RS_DS_CM11_SimulinkRS_P.DetectFallNegative_vinit;
  p_1_RS_DS_CM11_SimulinkRS_real_T_3 = &RS_DS_CM11_SimulinkRS_P.Constant_Value;
  p_1_RS_DS_CM11_SimulinkRS_uint8_T_4 =
    &RS_DS_CM11_SimulinkRS_P.ManualSwitch_CurrentSetting;
  p_2_RS_DS_CM11_SimulinkRS_real_T_3 =
    &RS_DS_CM11_SimulinkRS_DW.Memory_2_PreviousInput;
  p_2_RS_DS_CM11_SimulinkRS_uint32_T_5 =
    &RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1;
  p_2_RS_DS_CM11_SimulinkRS_boolean_T_6 =
    &RS_DS_CM11_SimulinkRS_DW.DelayInput1_DSTATE;
  p_2_RS_DS_CM11_SimulinkRS_uint8_T_7 =
    &RS_DS_CM11_SimulinkRS_DW.is_active_c2_RS_DS_CM11_SimulinkRS;
  p_2_RS_DS_CM11_SimulinkRS_boolean_T_8 = &RS_DS_CM11_SimulinkRS_DW.objisempty;
  p_3_RS_DS_CM11_SimulinkRS_real_T_0 =
    &RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE;
}

void RS_DS_CM11_SimulinkRS_rti_init_trc_pointers(void)
{
  rti_init_trc_pointers_0();
}
