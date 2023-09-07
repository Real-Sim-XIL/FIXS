/*****************************************************************************

   Include file RS_DS_CM11_SimulinkRS_dsrtmdlfcn.h:

   Definition of model functions.

   Fri May 19 17:14:26 2023

   Copyright 2020, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

#ifndef _DSRT_RS_DS_CM11_SimulinkRS_DSRTMDLFCN_HEADER_
#define _DSRT_RS_DS_CM11_SimulinkRS_DSRTMDLFCN_HEADER_
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
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_DSRTInitMdl(void);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_DSRTStartMdl(void);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_DSRTStopMdl(void);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_DSRTCheckForSimulationStopCondition
  (void);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_DSRTCheckForErrorStatus(void);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_APTerminateSimulation(void);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_APStopSimulation(void);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_APLastApplStateStopped(uint8_T*);
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_APPrintMessage(const char_T*);

#endif                      /* _DSRT_RS_DS_CM11_SimulinkRS_DSRTMDLFCN_HEADER_ */
