/*****************************************************************************

   Include file RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.h:

   Definition of model functions.

   Tue Sep 26 10:58:04 2023

   Copyright 2020, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

#ifndef _DSRT_RS_DS_CM11_noSimulinkRS_useManeuver_DSRTMDLFCN_HEADER_
#define _DSRT_RS_DS_CM11_noSimulinkRS_useManeuver_DSRTMDLFCN_HEADER_
#if defined(EXTERN_C_DECL)
#undef EXTERN_C_DECL
#endif

#if defined(__cplusplus)
#define EXTERN_C_DECL                  extern "C"
#else
#define EXTERN_C_DECL                  extern
#endif

#include "rtwtypes.h"
#include <string.h>

/* Model functions declarations */
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTInitMdl(void);
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTStartMdl(void);
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTStopMdl(void);
EXTERN_C_DECL void
  RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForSimulationStopCondition(void);
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForErrorStatus
  (void);
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_APTerminateSimulation
  (void);
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_APStopSimulation(void);
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_APLastApplStateStopped
  (uint8_T*);
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_APPrintMessage(const
  char_T*);

#endif        /* _DSRT_RS_DS_CM11_noSimulinkRS_useManeuver_DSRTMDLFCN_HEADER_ */
