/****************************************************************************\ 
 *** 
 *** File                  : rtosal_task.cpp
 *** 
 *** Generated by dSPACE Tools.
 *** (c) Copyright 2013, dSPACE GmbH. All rights reserved.
\****************************************************************************/

#if !defined(RTOSAL_TASK_CPP_)
#  define    RTOSAL_TASK_CPP_


/* -- Include directives. -- */
#  include <DsApplicationInterface.h>
#  include "dssimengine_api.h"
#  include "rtosal_msg.h"
#  include "rtosal_taskap.h"
#  include "IOCode_Data.h"
#  include <dsrtt.h>
#  include "rtosal_task.h"

#  if defined(EXTERN_C_DECL)
#    undef EXTERN_C_DECL
#  endif
#  if defined(__cplusplus)
#    define EXTERN_C_DECL extern "C"
#  else
#    define EXTERN_C_DECL extern
#  endif


/* -- Defines and directives in module scope. After include directives. -- */
DS_DEFINE_TASK(DsCRtosAlPeriodic_Task_1);

/* -- Variable declarations in module scope. -- */
DsCTask* pRtosAlPeriodic_Task_1 = 0;
DsCTimer* pRtosAlTimer_Periodic_Task_1 = 0;

/* Task simulation time variable */
Float64 RtosAlTaskSimTime_Periodic_Task_1 = 0;
ULong64 StepCounter_Periodic_Task_1 = 0;

#ifdef TASK_ENABLE_PROFILING
DsTProfilerSource ProfilerSrc_RtosAlTask_01_TaskEntry;
DsTProfilerSource ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0;
DsTProfilerSource ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0;
DsTProfilerSource ProfilerSrc_RtosAlTask_01_TaskExit;
DsTProfilerSource ProfilerSrc_pRtosAlPeriodic_Task_1_daqService_1;
#endif

/* -- Declarations of extern functions. -- */

 EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0(
    void);

 EXTERN_C_DECL void RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0(
    void);

/* -- Declarations of module local functions. -- */

  void RtosAlHandleOverruns(
    DsCTask* pTask, int numberOfAcceptedOverruns);

/* -- Definitions of exported functions. -- */

  void
RtosAlDefaultDeadlineViolationHandler(
    DsTErrorList ErrorList, DsCTask* pTask) 
{
    SimEngineApplStop();
    DsCApplication* pAppl = SimEngineApplGet();
    char CallCounterStr[32];
    snprintf(CallCounterStr, 32, "%llu", pTask->getCallCounter());
    pAppl->errorMessage(RTOSAL_DEADLINE_VIOLATION, "%s%s%s", "RS_DS_CM11_noSimulinkRS", pTask->getDescription(), CallCounterStr);

}

  void
RtosAlProfilerSourcesInitialization(
    ) 
{
#ifdef TASK_ENABLE_PROFILING
    ProfilerSrc_RtosAlTask_01_TaskEntry =  dsProfilerCreateSource("RtosAlTask_01_TaskEntry");
    ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0 =  dsProfilerCreateSource("RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0");
    ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0 =  dsProfilerCreateSource("RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0");
    ProfilerSrc_RtosAlTask_01_TaskExit =  dsProfilerCreateSource("RtosAlTask_01_TaskExit");
    ProfilerSrc_pRtosAlPeriodic_Task_1_daqService_1 =  dsProfilerCreateSource("pRtosAlPeriodic_Task_1_daqService_1");
#endif

}

/* -- Definitions of module local functions. -- */

  void
DsCRtosAlPeriodic_Task_1::onRun(
    DsTErrorList ErrorList) 
{
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RtosAlTask_01_TaskEntry, UserTimespanStart , 0, 0);
#   endif
    RtosAlTask_01_TaskEntry();
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RtosAlTask_01_TaskEntry, UserTimespanEnd , 0, 0);
#   endif

    DS_MODEL_ACCESS_WRITE(1,pRtosAlPeriodic_Task_1);
    dsrtt_script_service_pre_computation();
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0, UserTimespanStart , 0, 0);
#   endif
    RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0();
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlOutputs0, UserTimespanEnd , 0, 0);
#   endif
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_pRtosAlPeriodic_Task_1_daqService_1, UserTimespanStart , 0, 0);
#   endif
    pRtosAlPeriodic_Task_1->daqService(1);
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_pRtosAlPeriodic_Task_1_daqService_1, UserTimespanEnd , 0, 0);
#   endif

    DS_MODEL_ACCESS_READ(1,pRtosAlPeriodic_Task_1);
    dsrtt_script_service_post_computation();
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0, UserTimespanStart , 0, 0);
#   endif
    RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0();
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RS_DS_CM11_noSimulinkRS_DSRTMdlUpdate0, UserTimespanEnd , 0, 0);
#   endif
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RtosAlTask_01_TaskExit, UserTimespanStart , 0, 0);
#   endif
    RtosAlTask_01_TaskExit();
#   ifdef TASK_ENABLE_PROFILING
    dsProfilerWriteEvent(ProfilerSrc_RtosAlTask_01_TaskExit, UserTimespanEnd , 0, 0);
#   endif

}

  void
DsCRtosAlPeriodic_Task_1::onOverrun(
    DsTErrorList ErrorList) 
{
    DsCTask* pTask = DsCRtosAlPeriodic_Task_1::getInstance();
    RtosAlHandleOverruns(pTask, -1);

}

  void
RtosAlHandleOverruns(
    DsCTask* pTask, int numberOfAcceptedOverruns) 
{
    /* Value -1 of numberOfAcceptedOverruns represents positive infinity */ 
    if (numberOfAcceptedOverruns < 0)
    {
        return;
    }
    if (pTask->getOverrunCounter() > (unsigned int)numberOfAcceptedOverruns)
    {
        SimEngineApplStop();
        DsCApplication* pAppl = SimEngineApplGet();
        char CallCounterStr[32];
        snprintf(CallCounterStr, 32, "%llu", pTask->getCallCounter());
        pAppl->errorMessage(RTOSAL_TASK_OVERRUN, "%s%s%s%d", "RS_DS_CM11_noSimulinkRS", pTask->getDescription(), CallCounterStr, numberOfAcceptedOverruns);
    }

}

#endif                                 /* RTOSAL_TASK_CPP_ */
/* [EOF] */
