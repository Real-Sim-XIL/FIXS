/*****************************************************************************

   Include file RS_DS_CM11_noSimulinkRS_dsrttf.c:

   Definition of task functions.

   Fri May 19 12:30:37 2023

   Copyright 2019, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header files */
#include "RS_DS_CM11_noSimulinkRS_dsrttf.h"
#include "RS_DS_CM11_noSimulinkRS.h"
#include "RS_DS_CM11_noSimulinkRS_private.h"

/* Task function for TID0 */
void RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0()
{
  /* Call to Simulink model output function */
  RS_DS_CM11_noSimulinkRS_output();
}

void RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0()
{
  /* Call to Simulink model upadte function */
  RS_DS_CM11_noSimulinkRS_update();
}
