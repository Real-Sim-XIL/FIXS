/*****************************************************************************

   Include file RS_DS_CM11_SimulinkRS_dsrttf.c:

   Definition of task functions.

   Fri May 19 17:14:26 2023

   Copyright 2019, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

/* Include header files */
#include "RS_DS_CM11_SimulinkRS_dsrttf.h"
#include "RS_DS_CM11_SimulinkRS.h"
#include "RS_DS_CM11_SimulinkRS_private.h"

extern volatile real_T *RS_DS_CM11_SimulinkRS_RTIDerivativesPtr;

/* Task function for TID0 */
void RS_DS_CM11_SimulinkRS_DSRTMdlOutputs0()
{
  /* Call to Simulink model output function */
  RS_DS_CM11_SimulinkRS_output();

  /* Set derivatives pointer */
  if (!RS_DS_CM11_SimulinkRS_RTIDerivativesPtr) {
    RS_DS_CM11_SimulinkRS_RTIDerivativesPtr = (real_T *) rtmGetdX
      (RS_DS_CM11_SimulinkRS_M);
  }
}

void RS_DS_CM11_SimulinkRS_DSRTMdlUpdate0()
{
  /* Call to Simulink model upadte function */
  RS_DS_CM11_SimulinkRS_update();
}
