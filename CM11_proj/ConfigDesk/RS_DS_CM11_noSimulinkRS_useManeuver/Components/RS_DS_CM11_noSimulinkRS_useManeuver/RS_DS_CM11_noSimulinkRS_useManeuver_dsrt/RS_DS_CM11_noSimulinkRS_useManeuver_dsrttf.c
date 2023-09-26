/*****************************************************************************

   Include file RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.c:

   Definition of task functions.

   Tue Sep 26 10:58:04 2023

   Copyright 2019, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header files */
#include "RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver_private.h"

/* Task function for TID0 */
void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTMdlOutputs0()
{
  /* Call to Simulink model output function */
  RS_DS_CM11_noSimulinkRS_useManeuver_output();
}

void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTMdlUpdate0()
{
  /* Call to Simulink model upadte function */
  RS_DS_CM11_noSimulinkRS_useManeuver_update();
}
