/**************************************************************************** *
 * FILE :
 *  RS_DS_CM11_SimulinkRS_dsrtmdlfcn.c
 *
 *
 *
 * Copyright 2017, dSPACE GmbH. All rights reserved
 *
   \****************************************************************************/

#ifndef TEMP_DSRT_RTI
#include <rtmodel.h>
#include "rtwtypes.h"
#include "RS_DS_CM11_SimulinkRS_dsrtmdlfcn.h"
#include <stdio.h>

int32_T DSRTStopSimulation = 0;
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_initialize();
EXTERN_C_DECL void RS_DS_CM11_SimulinkRS_terminate();

/* Initialization of model */
void RS_DS_CM11_SimulinkRS_DSRTInitMdl(void)
{
  /* Initialize model */
  RS_DS_CM11_SimulinkRS_initialize();

  /* Check for initialization errors */
  RS_DS_CM11_SimulinkRS_DSRTCheckForErrorStatus();

  /* Reinit Stop Simulation flag*/
  DSRTStopSimulation = 0;
}

/* Model start function */
void RS_DS_CM11_SimulinkRS_DSRTStartMdl(void)
{
  uint8_T lastApplStateStopped;
  RS_DS_CM11_SimulinkRS_APLastApplStateStopped(&lastApplStateStopped);
  if (lastApplStateStopped) {
    /* Reinit main simulation structure */
    RS_DS_CM11_SimulinkRS_DSRTInitMdl();
  }
}

/* Model stop function */
void RS_DS_CM11_SimulinkRS_DSRTStopMdl(void)
{
  /* Call terminate function */
  RS_DS_CM11_SimulinkRS_terminate();

  /* Check for model error status */
  RS_DS_CM11_SimulinkRS_DSRTCheckForErrorStatus();
}

/* Check for model error status */
void RS_DS_CM11_SimulinkRS_DSRTCheckForErrorStatus()
{
  const char_T* errorStatus = rtmGetErrorStatus(RS_DS_CM11_SimulinkRS_M);
  if (errorStatus != NULL && strcmp(errorStatus, "Simulation finished") == 0) {
    /* The RTM errorStatus field has been set */
    RS_DS_CM11_SimulinkRS_APTerminateSimulation();
    RS_DS_CM11_SimulinkRS_APPrintMessage(
      "Model 'RS_DS_CM11_SimulinkRS' error status:");
    RS_DS_CM11_SimulinkRS_APPrintMessage(errorStatus);
  }
}

/* Check for simulation stop conditions */
void RS_DS_CM11_SimulinkRS_DSRTCheckForSimulationStopCondition(void)
{
  const char_T* errorStatus = rtmGetErrorStatus(RS_DS_CM11_SimulinkRS_M);
  if (errorStatus != NULL) {
    if (DSRTStopSimulation != 1) {
      /* The RTM errorStatus field was set */
      RS_DS_CM11_SimulinkRS_APTerminateSimulation();
      RS_DS_CM11_SimulinkRS_APPrintMessage(
        "Model 'RS_DS_CM11_SimulinkRS' error status:");
      RS_DS_CM11_SimulinkRS_APPrintMessage(errorStatus);
      DSRTStopSimulation = 1;
    }
  } else if (rtmGetStopRequested(RS_DS_CM11_SimulinkRS_M)) {
    if (DSRTStopSimulation != 1) {
      RS_DS_CM11_SimulinkRS_APStopSimulation();
      RS_DS_CM11_SimulinkRS_APPrintMessage(
        "Model 'RS_DS_CM11_SimulinkRS': Executed a Simulink Stop Simulation block or request.");
      DSRTStopSimulation = 1;
    }
  }
}

#endif
