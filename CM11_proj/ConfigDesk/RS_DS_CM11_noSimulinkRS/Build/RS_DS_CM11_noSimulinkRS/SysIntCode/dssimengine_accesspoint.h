/****************************************************************************\ 
 *** 
 *** File                  : dssimengine_accesspoint.h
 *** Description           : 
 *** This file contains the declarations of all access point functions
 *** provided by the simulation engine. The access point functions are
 *** called by the simulation engine when the application state changes
 *** or when the background task is executed.
 *** 
 *** Generated by dSPACE Tools.
 *** (c) Copyright 2013, dSPACE GmbH. All rights reserved.
\****************************************************************************/

#if !defined(DSSIMENGINE_ACCESSPOINT_H_)
#  define    DSSIMENGINE_ACCESSPOINT_H_


/* -- Include directives. -- */
#  include <DsApplicationInterface.h>

#  if defined(EXTERN_C_DECL)
#    undef EXTERN_C_DECL
#  endif
#  if defined(__cplusplus)
#    define EXTERN_C_DECL extern "C"
#  else
#    define EXTERN_C_DECL extern
#  endif


/* -- Declarations of exported functions. -- */

 EXTERN_C_DECL void ApSimEngineOnInit(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStart(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStop(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnTerminate(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnUnload(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnInitMdl(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStartMdl(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStopMdl(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnTerminateMdl(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnUnloadMdl(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnInitImc(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStartImc(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStopImc(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnTerminateImc(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnUnloadImc(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnInitIoPreRtosInit(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnInitIoPostRtosInit(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnInitHwDiagnostics(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStartIo(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStopIo(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnTerminateIo(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnUnloadIo(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnInitRtos(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStartRtos(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnStopRtos(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnTerminateRtos(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineOnUnloadRtos(
    DsTErrorList ErrorList);

 EXTERN_C_DECL void ApSimEngineIdle(
    DsTErrorList ErrorList);

#endif                                 /* DSSIMENGINE_ACCESSPOINT_H_ */
/* [EOF] */