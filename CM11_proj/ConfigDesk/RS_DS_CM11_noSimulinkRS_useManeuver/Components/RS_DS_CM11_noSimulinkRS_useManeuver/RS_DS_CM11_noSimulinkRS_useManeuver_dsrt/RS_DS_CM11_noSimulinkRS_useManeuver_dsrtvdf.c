/***************************************************************************

   Source file RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.c:

   Definition of function that initializes the global TRC pointers

   4.6 (02-Nov-2021)
   Tue Sep 26 10:58:04 2023

   Copyright 2023, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header file. */
#include "RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver_private.h"

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
volatile real_T *p_0_RS_DS_CM11_noSimulinkRS_useManeuver_real_T_0 = NULL;
volatile boolean_T *p_0_RS_DS_CM11_noSimulinkRS_useManeuver_boolean_T_1 = NULL;
volatile real_T *p_1_RS_DS_CM11_noSimulinkRS_useManeuver_real_T_0 = NULL;

/*
 *  Declare the functions, that initially assign TRC pointers
 */
static void rti_init_trc_pointers_0(void);

/* Global pointers to data type transitions are separated in different functions to avoid overloading */
static void rti_init_trc_pointers_0(void)
{
  p_0_RS_DS_CM11_noSimulinkRS_useManeuver_real_T_0 =
    &RS_DS_CM11_noSimulinkRS_useManeuver_B.IoIn;
  p_0_RS_DS_CM11_noSimulinkRS_useManeuver_boolean_T_1 =
    &RS_DS_CM11_noSimulinkRS_useManeuver_B.Compare;
  p_1_RS_DS_CM11_noSimulinkRS_useManeuver_real_T_0 =
    &RS_DS_CM11_noSimulinkRS_useManeuver_P.VehicleSource_BuiltIn_const;
}

void RS_DS_CM11_noSimulinkRS_useManeuver_rti_init_trc_pointers(void)
{
  rti_init_trc_pointers_0();
}
