/*********************** dSPACE target specific file *************************

   Header file RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.h:

   Declaration of function that initializes the global TRC pointers

   4.6 (02-Nov-2021)
   Tue Sep 26 10:58:04 2023

   Copyright 2023, dSPACE GmbH. All rights reserved.

 *****************************************************************************/
#ifndef _DSRT_RS_DS_CM11_noSimulinkRS_useManeuver_DSRTVDF_HEADER_
#define _DSRT_RS_DS_CM11_noSimulinkRS_useManeuver_DSRTVDF_HEADER_

/* Include the model header file. */
#include "RS_DS_CM11_noSimulinkRS_useManeuver.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver_private.h"
#ifdef EXTERN_C
#undef EXTERN_C
#endif

#ifdef __cplusplus
#define EXTERN_C                       extern "C"
#else
#define EXTERN_C                       extern
#endif

/*
 *  Declare the global TRC pointers
 */
EXTERN_C volatile real_T *p_0_RS_DS_CM11_noSimulinkRS_useManeuver_real_T_0;
EXTERN_C volatile boolean_T *p_0_RS_DS_CM11_noSimulinkRS_useManeuver_boolean_T_1;
EXTERN_C volatile real_T *p_1_RS_DS_CM11_noSimulinkRS_useManeuver_real_T_0;

/*
 *  Declare the general function for TRC pointer initialization
 */
EXTERN_C void RS_DS_CM11_noSimulinkRS_useManeuver_rti_init_trc_pointers(void);

#endif           /* _DSRT_RS_DS_CM11_noSimulinkRS_useManeuver_DSRTVDF_HEADER_ */
