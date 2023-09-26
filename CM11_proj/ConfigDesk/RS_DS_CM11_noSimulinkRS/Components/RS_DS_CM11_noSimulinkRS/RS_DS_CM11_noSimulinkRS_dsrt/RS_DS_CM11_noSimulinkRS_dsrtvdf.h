/*********************** dSPACE target specific file *************************

   Header file RS_DS_CM11_noSimulinkRS_dsrtvdf.h:

   Declaration of function that initializes the global TRC pointers

   4.6 (02-Nov-2021)
   Tue Sep 19 14:33:07 2023

   Copyright 2023, dSPACE GmbH. All rights reserved.

 *****************************************************************************/
#ifndef _DSRT_RS_DS_CM11_noSimulinkRS_DSRTVDF_HEADER_
#define _DSRT_RS_DS_CM11_noSimulinkRS_DSRTVDF_HEADER_

/* Include the model header file. */
#include "RS_DS_CM11_noSimulinkRS.h"
#include "RS_DS_CM11_noSimulinkRS_private.h"
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
EXTERN_C volatile real_T *p_0_RS_DS_CM11_noSimulinkRS_real_T_0;
EXTERN_C volatile boolean_T *p_0_RS_DS_CM11_noSimulinkRS_boolean_T_1;
EXTERN_C volatile struct_GphLW54WCcJjc0lEaqz83B
  *p_1_RS_DS_CM11_noSimulinkRS_struct_GphLW54WCcJjc0lEaqz83B_0;
EXTERN_C volatile real_T *p_1_RS_DS_CM11_noSimulinkRS_real_T_1;
EXTERN_C volatile boolean_T *p_1_RS_DS_CM11_noSimulinkRS_boolean_T_2;
EXTERN_C volatile real_T *p_1_RS_DS_CM11_noSimulinkRS_real_T_3;
EXTERN_C volatile real_T *p_2_RS_DS_CM11_noSimulinkRS_real_T_0;
EXTERN_C volatile uint32_T *p_2_RS_DS_CM11_noSimulinkRS_uint32_T_2;
EXTERN_C volatile boolean_T *p_2_RS_DS_CM11_noSimulinkRS_boolean_T_3;
EXTERN_C volatile uint8_T *p_2_RS_DS_CM11_noSimulinkRS_uint8_T_4;
EXTERN_C volatile real_T *p_3_RS_DS_CM11_noSimulinkRS_real_T_0;

/*
 *  Declare the general function for TRC pointer initialization
 */
EXTERN_C void RS_DS_CM11_noSimulinkRS_rti_init_trc_pointers(void);

#endif                       /* _DSRT_RS_DS_CM11_noSimulinkRS_DSRTVDF_HEADER_ */
