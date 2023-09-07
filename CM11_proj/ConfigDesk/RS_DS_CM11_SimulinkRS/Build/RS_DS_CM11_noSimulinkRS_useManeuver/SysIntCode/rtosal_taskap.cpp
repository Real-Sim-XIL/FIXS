/****************************************************************************\ 
 *** 
 *** File                  : rtosal_taskap.cpp
 *** 
 *** Generated by dSPACE Tools.
 *** (c) Copyright 2013, dSPACE GmbH. All rights reserved.
\****************************************************************************/

#if !defined(RTOSAL_TASKAP_CPP_)
#  define    RTOSAL_TASKAP_CPP_


/* -- Include directives. -- */
#  include "rtosal_task.h"
#  include "rtosal_simengineap.h"
#  include "IOCode_Data.h"
#  include "rtosal_taskap.h"

#  if defined(EXTERN_C_DECL)
#    undef EXTERN_C_DECL
#  endif
#  if defined(__cplusplus)
#    define EXTERN_C_DECL extern "C"
#  else
#    define EXTERN_C_DECL extern
#  endif


/* -- Declarations of extern functions. -- */

 EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForSimulationStopCondition(
    void);

/* -- Definitions of exported functions. -- */

  void
RtosAlTask_02_TaskEntry(
    void) 
{

DsBusAccessRP_updateAllInputs(ErrorList);

}

  void
RtosAlTask_02_TaskExit(
    void) 
{
RS_DS_CM11_noSimulinkRS_useManeuver_DSRTCheckForSimulationStopCondition();

DsBusAccessRP_updateAllOutputs(ErrorList);

    /* Update task simulation time */
    StepCounter_Periodic_Task_1++;
    RtosAlTaskSimTime_Periodic_Task_1 = StepCounter_Periodic_Task_1*0.001;

}

#endif                                 /* RTOSAL_TASKAP_CPP_ */
/* [EOF] */
