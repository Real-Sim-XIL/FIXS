/**************************************************************************** *
 * FILE :
 *  RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.c
 *
 *
 *
 * Copyright 2017, dSPACE GmbH. All rights reserved
 *
   \****************************************************************************/

#ifndef TEMP_DSRT_RTI
#include <rtmodel.h>
#include "rtwtypes.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.h"
#include <stdio.h>

int32_T DSRTStopSimulation = 0;
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_initialize();
EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_terminate();

/* Initialization of model */
void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTInitMdl(void)
{
  /* Initialize model */
  RS_DS_CM11_noSimulinkRS_useManeuver_initialize();

  /* Check for initialization errors */
  RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForErrorStatus();

  /* Reinit Stop Simulation flag*/
  DSRTStopSimulation = 0;
}

/* Model start function */
void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTStartMdl(void)
{
  uint8_T lastApplStateStopped;
  RS_DS_CM11_noSimulinkRS_useManeuver_APLastApplStateStopped
    (&lastApplStateStopped);
  if (lastApplStateStopped) {
    /* Reinit main simulation structure */
    RS_DS_CM11_noSimulinkRS_useManeuver_DSRTInitMdl();
  }
}

/* Model stop function */
void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTStopMdl(void)
{
  /* Call terminate function */
  RS_DS_CM11_noSimulinkRS_useManeuver_terminate();

  /* Check for model error status */
  RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForErrorStatus();
}

/* Check for model error status */
void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForErrorStatus()
{
  const char_T* errorStatus = rtmGetErrorStatus
    (RS_DS_CM11_noSimulinkRS_useManeuver_M);
  if (errorStatus != NULL && strcmp(errorStatus, "Simulation finished") == 0) {
    /* The RTM errorStatus field has been set */
    RS_DS_CM11_noSimulinkRS_useManeuver_APTerminateSimulation();
    RS_DS_CM11_noSimulinkRS_useManeuver_APPrintMessage(
      "Model 'RS_DS_CM11_noSimulinkRS_useManeuver' error status:");
    RS_DS_CM11_noSimulinkRS_useManeuver_APPrintMessage(errorStatus);
  }
}

/* Check for simulation stop conditions */
void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForSimulationStopCondition
  (void)
{
  const char_T* errorStatus = rtmGetErrorStatus
    (RS_DS_CM11_noSimulinkRS_useManeuver_M);
  if (errorStatus != NULL) {
    if (DSRTStopSimulation != 1) {
      /* The RTM errorStatus field was set */
      RS_DS_CM11_noSimulinkRS_useManeuver_APTerminateSimulation();
      RS_DS_CM11_noSimulinkRS_useManeuver_APPrintMessage(
        "Model 'RS_DS_CM11_noSimulinkRS_useManeuver' error status:");
      RS_DS_CM11_noSimulinkRS_useManeuver_APPrintMessage(errorStatus);
      DSRTStopSimulation = 1;
    }
  } else if (rtmGetStopRequested(RS_DS_CM11_noSimulinkRS_useManeuver_M)) {
    if (DSRTStopSimulation != 1) {
      RS_DS_CM11_noSimulinkRS_useManeuver_APStopSimulation();
      RS_DS_CM11_noSimulinkRS_useManeuver_APPrintMessage(
        "Model 'RS_DS_CM11_noSimulinkRS_useManeuver': Executed a Simulink Stop Simulation block or request.");
      DSRTStopSimulation = 1;
    }
  }
}

#endif
