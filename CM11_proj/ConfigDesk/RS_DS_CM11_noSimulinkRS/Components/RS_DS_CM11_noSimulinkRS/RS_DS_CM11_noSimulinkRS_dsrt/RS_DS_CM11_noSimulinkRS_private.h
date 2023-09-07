/*
 * RS_DS_CM11_noSimulinkRS_private.h
 *
 * Code generation for model "RS_DS_CM11_noSimulinkRS".
 *
 * Model version              : 5.1
 * Simulink Coder version : 9.6 (R2021b) 14-May-2021
 * C source code generated on : Fri May 19 12:30:37 2023
 *
 * Target selection: dsrt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_RS_DS_CM11_noSimulinkRS_private_h_
#define RTW_HEADER_RS_DS_CM11_noSimulinkRS_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#include "zero_crossing_types.h"

/* Private macros used by the generated code to access rtModel */
#ifndef rtmIsMajorTimeStep
#define rtmIsMajorTimeStep(rtm)        (((rtm)->Timing.simTimeStep) == MAJOR_TIME_STEP)
#endif

#ifndef rtmIsMinorTimeStep
#define rtmIsMinorTimeStep(rtm)        (((rtm)->Timing.simTimeStep) == MINOR_TIME_STEP)
#endif

#ifndef rtmSetTPtr
#define rtmSetTPtr(rtm, val)           ((rtm)->Timing.t = (val))
#endif

#undef CM_APPNAME
#define CM_APPNAME                     ""
#undef CM_CMDLINE
#define CM_CMDLINE                     ""
#undef CM_CMDENV
#define CM_CMDENV                      ""
#undef CM_APPNAME
#define CM_APPNAME                     ""
#undef CM_CMDLINE
#define CM_CMDLINE                     ""
#undef CM_CMDENV
#define CM_CMDENV                      ""

extern real_T look1_binlcpw(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T maxIndex);
extern real_T look1_plinlcapw(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T prevIndex[], uint32_T maxIndex);

/* private model entry point functions */
extern void RS_DS_CM11_noSimulinkRS_derivatives(void);

#endif                       /* RTW_HEADER_RS_DS_CM11_noSimulinkRS_private_h_ */
