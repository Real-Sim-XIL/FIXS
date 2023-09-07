/***************************************************************************

   Source file RS_DS_CM11_noSimulinkRS_dsrtvdf.c:

   Definition of function that initializes the global TRC pointers

   4.6 (02-Nov-2021)
   Fri May 19 12:30:37 2023

   Copyright 2023, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header file. */
#include "RS_DS_CM11_noSimulinkRS_dsrtvdf.h"
#include "RS_DS_CM11_noSimulinkRS.h"
#include "RS_DS_CM11_noSimulinkRS_private.h"

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
volatile real_T *p_0_RS_DS_CM11_noSimulinkRS_real_T_0 = NULL;
volatile boolean_T *p_0_RS_DS_CM11_noSimulinkRS_boolean_T_1 = NULL;
volatile struct_GphLW54WCcJjc0lEaqz83B
  *p_1_RS_DS_CM11_noSimulinkRS_struct_GphLW54WCcJjc0lEaqz83B_0 = NULL;
volatile real_T *p_1_RS_DS_CM11_noSimulinkRS_real_T_1 = NULL;
volatile boolean_T *p_1_RS_DS_CM11_noSimulinkRS_boolean_T_2 = NULL;
volatile real_T *p_1_RS_DS_CM11_noSimulinkRS_real_T_3 = NULL;
volatile real_T *p_2_RS_DS_CM11_noSimulinkRS_real_T_0 = NULL;
volatile uint32_T *p_2_RS_DS_CM11_noSimulinkRS_uint32_T_2 = NULL;
volatile boolean_T *p_2_RS_DS_CM11_noSimulinkRS_boolean_T_3 = NULL;
volatile uint8_T *p_2_RS_DS_CM11_noSimulinkRS_uint8_T_4 = NULL;
volatile real_T *p_3_RS_DS_CM11_noSimulinkRS_real_T_0 = NULL;

/*
 *  Declare the functions, that initially assign TRC pointers
 */
static void rti_init_trc_pointers_0(void);

/* Global pointers to data type transitions are separated in different functions to avoid overloading */
static void rti_init_trc_pointers_0(void)
{
  p_0_RS_DS_CM11_noSimulinkRS_real_T_0 = &RS_DS_CM11_noSimulinkRS_B.IoIn;
  p_0_RS_DS_CM11_noSimulinkRS_boolean_T_1 = &RS_DS_CM11_noSimulinkRS_B.Equal;
  p_1_RS_DS_CM11_noSimulinkRS_struct_GphLW54WCcJjc0lEaqz83B_0 =
    &RS_DS_CM11_noSimulinkRS_P.RealSimPara;
  p_1_RS_DS_CM11_noSimulinkRS_real_T_1 =
    &RS_DS_CM11_noSimulinkRS_P.CompareToConstant1_const;
  p_1_RS_DS_CM11_noSimulinkRS_boolean_T_2 =
    &RS_DS_CM11_noSimulinkRS_P.DetectFallNegative_vinit;
  p_1_RS_DS_CM11_noSimulinkRS_real_T_3 =
    &RS_DS_CM11_noSimulinkRS_P.Constant_Value;
  p_2_RS_DS_CM11_noSimulinkRS_real_T_0 =
    &RS_DS_CM11_noSimulinkRS_DW.Memory_2_PreviousInput;
  p_2_RS_DS_CM11_noSimulinkRS_uint32_T_2 =
    &RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1;
  p_2_RS_DS_CM11_noSimulinkRS_boolean_T_3 =
    &RS_DS_CM11_noSimulinkRS_DW.DelayInput1_DSTATE;
  p_2_RS_DS_CM11_noSimulinkRS_uint8_T_4 =
    &RS_DS_CM11_noSimulinkRS_DW.is_active_c2_RS_DS_CM11_noSimulinkRS;
  p_3_RS_DS_CM11_noSimulinkRS_real_T_0 =
    &RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE;
}

void RS_DS_CM11_noSimulinkRS_rti_init_trc_pointers(void)
{
  rti_init_trc_pointers_0();
}
