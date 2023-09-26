/*
 * RS_DS_CM11_noSimulinkRS.c
 *
 * Code generation for model "RS_DS_CM11_noSimulinkRS".
 *
 * Model version              : 5.2
 * Simulink Coder version : 9.6 (R2021b) 14-May-2021
 * C source code generated on : Tue Sep 19 14:33:07 2023
 *
 * Target selection: dsrt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "RS_DS_CM11_noSimulinkRS_dsrtvdf.h"
#include "RS_DS_CM11_noSimulinkRS.h"
#include "RS_DS_CM11_noSimulinkRS_private.h"

/* Named constants for Chart: '<S1>/RealSimHILCycle' */
#define RS_DS_CM11_noSimulinkRS_IN_CycleOn ((uint8_T)1U)
#define RS_DS_CM11_noSimulinkRS_IN_Driving ((uint8_T)1U)
#define RS_DS_CM11_noSimulinkRS_IN_Idle ((uint8_T)2U)
#define RS_DS_CM11_noSimulinkRS_IN_InitialWait ((uint8_T)2U)
#define RS_DS_CM11_noSimulinkRS_IN_NO_ACTIVE_CHILD ((uint8_T)0U)
#define RS_DS_CM11_noSimulinkRS_IN_RampDown ((uint8_T)3U)
#define RS_DS_CM11_noSimulinkRS_IN_RampUp ((uint8_T)3U)
#define RS_DS_CM11_noSimulinkRS_IN_WaitForNextRamp ((uint8_T)4U)
#define RS_DS_CM11_noSimulinkRS_period (0.001)
#include "MatSupp.h"

void CarMaker_Startup (void);
void CarMaker_FinishStartup (void);
void CarMaker_Cleanup (void);

/* Block signals (default storage) */
B_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_B;

/* Continuous states */
X_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_X;

/* Block states (default storage) */
DW_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_DW;

/* Previous zero-crossings (trigger) states */
PrevZCX_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_PrevZCX;

/* Real-time model */
static RT_MODEL_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_M_;
RT_MODEL_RS_DS_CM11_noSimulinkRS_T *const RS_DS_CM11_noSimulinkRS_M =
  &RS_DS_CM11_noSimulinkRS_M_;
real_T look1_binlcpw(real_T u0, const real_T bp0[], const real_T table[],
                     uint32_T maxIndex)
{
  real_T frac;
  real_T yL_0d0;
  uint32_T bpIdx;
  uint32_T iLeft;
  uint32_T iRght;

  /* Column-major Lookup 1-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Clip'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = 0.0;
  } else if (u0 < bp0[maxIndex]) {
    /* Binary Search */
    bpIdx = maxIndex >> 1U;
    iLeft = 0U;
    iRght = maxIndex;
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = 1.0;
  }

  /* Column-major Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  yL_0d0 = table[iLeft];
  return (table[iLeft + 1U] - yL_0d0) * frac + yL_0d0;
}

real_T look1_plinlcapw(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T prevIndex[], uint32_T maxIndex)
{
  real_T frac;
  real_T y;
  real_T yL_0d0;
  uint32_T bpIdx;

  /* Column-major Lookup 1-D
     Search method: 'linear'
     Use previous index: 'on'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Clip'
     Use last breakpoint for index at or above upper limit: 'on'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'linear'
     Extrapolation method: 'Clip'
     Use previous index: 'on'
     Use last breakpoint for index at or above upper limit: 'on'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    bpIdx = 0U;
    frac = 0.0;
  } else if (u0 < bp0[maxIndex]) {
    /* Linear Search */
    for (bpIdx = prevIndex[0U]; u0 < bp0[bpIdx]; bpIdx--) {
    }

    while (u0 >= bp0[bpIdx + 1U]) {
      bpIdx++;
    }

    frac = (u0 - bp0[bpIdx]) / (bp0[bpIdx + 1U] - bp0[bpIdx]);
  } else {
    bpIdx = maxIndex;
    frac = 0.0;
  }

  prevIndex[0U] = bpIdx;

  /* Column-major Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'on'
     Overflow mode: 'portable wrapping'
   */
  if (bpIdx == maxIndex) {
    y = table[bpIdx];
  } else {
    yL_0d0 = table[bpIdx];
    y = (table[bpIdx + 1U] - yL_0d0) * frac + yL_0d0;
  }

  return y;
}

/*
 * This function updates continuous states using the ODE1 fixed-step
 * solver algorithm
 */
static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  time_T tnew = rtsiGetSolverStopTime(si);
  time_T h = rtsiGetStepSize(si);
  real_T *x = rtsiGetContStates(si);
  ODE1_IntgData *id = (ODE1_IntgData *)rtsiGetSolverData(si);
  real_T *f0 = id->f[0];
  int_T i;
  int_T nXc = 2;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);
  rtsiSetdX(si, f0);
  RS_DS_CM11_noSimulinkRS_derivatives();
  rtsiSetT(si, tnew);
  for (i = 0; i < nXc; ++i) {
    x[i] += h * f0[i];
  }

  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

/* Model output function */
void RS_DS_CM11_noSimulinkRS_output(void)
{
  real_T u0;
  real_T u1;
  real_T u2;
  boolean_T out;
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    /* set solver stop time */
    if (!(RS_DS_CM11_noSimulinkRS_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
                            ((RS_DS_CM11_noSimulinkRS_M->Timing.clockTickH0 + 1)
        * RS_DS_CM11_noSimulinkRS_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
                            ((RS_DS_CM11_noSimulinkRS_M->Timing.clockTick0 + 1) *
        RS_DS_CM11_noSimulinkRS_M->Timing.stepSize0 +
        RS_DS_CM11_noSimulinkRS_M->Timing.clockTickH0 *
        RS_DS_CM11_noSimulinkRS_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    RS_DS_CM11_noSimulinkRS_M->Timing.t[0] = rtsiGetT
      (&RS_DS_CM11_noSimulinkRS_M->solverInfo);
  }

  /* Outputs for Atomic SubSystem: '<Root>/CarMaker' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    /* S-Function (CM_Sfun): '<S3>/IoIn' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[0];
      rinf->Func();
    }

    /* S-Function (CM_Sfun): '<S3>/Environment' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[1];
      double *p;
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[13] = *p++;
    }

    /* S-Function (CM_Sfun): '<S5>/DrivMan' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[2];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Environment_o2[7];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[15] = *p++;
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[16] = *p++;
    }

    /* S-Function (CM_Sfun): '<S8>/VehicleControl' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[3];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[15];
      *p++ = RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[16];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[15] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[16] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[17] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[18] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[19] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[20] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[21] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[22] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[23] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[24] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[25] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[26] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[27] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[28] = *p++;
    }

    /* Memory generated from: '<S8>/Memory' */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct =
      RS_DS_CM11_noSimulinkRS_DW.Memory_2_PreviousInput;

    /* Gain: '<S8>/Gain1' */
    RS_DS_CM11_noSimulinkRS_B.Gain1 = RS_DS_CM11_noSimulinkRS_P.Gain1_Gain_d *
      RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct;

    /* Memory generated from: '<S8>/Memory' */
    RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct =
      RS_DS_CM11_noSimulinkRS_DW.Memory_1_PreviousInput;

    /* Gain: '<S8>/Gain' */
    RS_DS_CM11_noSimulinkRS_B.Gain = RS_DS_CM11_noSimulinkRS_P.Gain_Gain_o *
      RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct;

    /* S-Function (read_dict): '<S8>/Read CM Dict3' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict3_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.CM_Time = e->GetFunc(e->Var);
    }

    /* Switch: '<S8>/Switch1' */
    if (RS_DS_CM11_noSimulinkRS_B.CM_Time >
        RS_DS_CM11_noSimulinkRS_P.Switch1_Threshold_e) {
      /* Switch: '<S8>/Switch1' incorporates:
       *  Constant: '<S8>/Driver Source'
       */
      RS_DS_CM11_noSimulinkRS_B.Switch1 =
        RS_DS_CM11_noSimulinkRS_P.DriverSource_Value;
    } else {
      /* Switch: '<S8>/Switch1' incorporates:
       *  Constant: '<S8>/Constant'
       */
      RS_DS_CM11_noSimulinkRS_B.Switch1 =
        RS_DS_CM11_noSimulinkRS_P.Constant_Value_b;
    }

    /* End of Switch: '<S8>/Switch1' */

    /* S-Function (read_dict): '<S1>/Read CM Dict3' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict3_PWORK_e.Entry;
      RS_DS_CM11_noSimulinkRS_B.CM_Time_l = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict7' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict7_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.SimCore_State = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict8' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict8_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.distanceTravel = e->GetFunc(e->Var);
    }

    /* Chart: '<S1>/RealSimHILCycle' incorporates:
     *  Constant: '<S1>/Constant'
     *  Constant: '<S1>/RampDownFlag'
     *  Constant: '<S1>/initialWaitTime'
     *  Constant: '<S1>/totalDistance (m)'
     *  Constant: '<S1>/totalTime (sec)'
     */
    if (RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 < MAX_uint32_T) {
      RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1++;
    }

    /* Gateway: CarMaker/RealSimHILCycle */
    /* During: CarMaker/RealSimHILCycle */
    if (RS_DS_CM11_noSimulinkRS_DW.is_active_c2_RS_DS_CM11_noSimulinkRS == 0U) {
      /* Entry: CarMaker/RealSimHILCycle */
      RS_DS_CM11_noSimulinkRS_DW.is_active_c2_RS_DS_CM11_noSimulinkRS = 1U;

      /* Entry Internal: CarMaker/RealSimHILCycle */
      /* Transition: '<S7>:2' */
      RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS =
        RS_DS_CM11_noSimulinkRS_IN_Idle;

      /* Entry 'Idle': '<S7>:1' */
      RS_DS_CM11_noSimulinkRS_B.CycleMode = 0.0;
      RS_DS_CM11_noSimulinkRS_B.SpeedCmd = 0.0;
    } else {
      switch (RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS) {
       case RS_DS_CM11_noSimulinkRS_IN_CycleOn:
        /* During 'CycleOn': '<S7>:17' */
        out = ((RS_DS_CM11_noSimulinkRS_B.distanceTravel >
                RS_DS_CM11_noSimulinkRS_P.totalDistancem_Value) ||
               (RS_DS_CM11_noSimulinkRS_B.CM_Time_l >
                RS_DS_CM11_noSimulinkRS_P.totalTimesec_Value) ||
               (RS_DS_CM11_noSimulinkRS_P.RampDownFlag_Value != 0.0) ||
               (RS_DS_CM11_noSimulinkRS_B.SimCore_State == 9.0));
        if (out) {
          /* Transition: '<S7>:7' */
          /* Exit Internal 'CycleOn': '<S7>:17' */
          RS_DS_CM11_noSimulinkRS_DW.is_CycleOn =
            RS_DS_CM11_noSimulinkRS_IN_NO_ACTIVE_CHILD;
          RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS =
            RS_DS_CM11_noSimulinkRS_IN_RampDown;

          /* Entry 'RampDown': '<S7>:4' */
          RS_DS_CM11_noSimulinkRS_B.CycleMode = 4.0;
          RS_DS_CM11_noSimulinkRS_B.SpeedCmd--;
        } else {
          switch (RS_DS_CM11_noSimulinkRS_DW.is_CycleOn) {
           case RS_DS_CM11_noSimulinkRS_IN_Driving:
            /* During 'Driving': '<S7>:5' */
            RS_DS_CM11_noSimulinkRS_B.CycleMode = 3.0;
            u0 = RS_DS_CM11_noSimulinkRS_P.RealSimPara.speedInit;
            if (u0 >= 0.0) {
              RS_DS_CM11_noSimulinkRS_B.SpeedCmd = u0;
            } else {
              RS_DS_CM11_noSimulinkRS_B.SpeedCmd = 0.0;
            }
            break;

           case RS_DS_CM11_noSimulinkRS_IN_InitialWait:
            /* During 'InitialWait': '<S7>:58' */
            if (RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 >= (uint32_T)ceil
                (RS_DS_CM11_noSimulinkRS_P.initialWaitTime_Value * 1000.0)) {
              /* Transition: '<S7>:40' */
              RS_DS_CM11_noSimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_noSimulinkRS_IN_RampUp;

              /* Entry 'RampUp': '<S7>:3' */
              RS_DS_CM11_noSimulinkRS_B.CycleMode = 2.0;
              RS_DS_CM11_noSimulinkRS_B.SpeedCmd += 0.1;
            } else {
              RS_DS_CM11_noSimulinkRS_B.CycleMode = 1.0;
            }
            break;

           case RS_DS_CM11_noSimulinkRS_IN_RampUp:
            /* During 'RampUp': '<S7>:3' */
            if (fabs(RS_DS_CM11_noSimulinkRS_B.SpeedCmd -
                     RS_DS_CM11_noSimulinkRS_P.RealSimPara.speedInit) < 0.5) {
              /* Transition: '<S7>:9' */
              RS_DS_CM11_noSimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_noSimulinkRS_IN_Driving;

              /* Entry 'Driving': '<S7>:5' */
              RS_DS_CM11_noSimulinkRS_B.CycleMode = 3.0;
              u0 = RS_DS_CM11_noSimulinkRS_P.RealSimPara.speedInit;
              if (u0 >= 0.0) {
                RS_DS_CM11_noSimulinkRS_B.SpeedCmd = u0;
              } else {
                RS_DS_CM11_noSimulinkRS_B.SpeedCmd = 0.0;
              }
            } else if (fabs(RS_DS_CM11_noSimulinkRS_B.SpeedCmd -
                            RS_DS_CM11_noSimulinkRS_P.RealSimPara.speedInit) >=
                       0.1) {
              /* Transition: '<S7>:55' */
              RS_DS_CM11_noSimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_noSimulinkRS_IN_WaitForNextRamp;
              RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 = 0U;
            } else {
              RS_DS_CM11_noSimulinkRS_B.CycleMode = 2.0;
              RS_DS_CM11_noSimulinkRS_B.SpeedCmd += 0.1;
            }
            break;

           default:
            /* During 'WaitForNextRamp': '<S7>:66' */
            if (RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 >= 100U) {
              /* Transition: '<S7>:56' */
              RS_DS_CM11_noSimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_noSimulinkRS_IN_RampUp;

              /* Entry 'RampUp': '<S7>:3' */
              RS_DS_CM11_noSimulinkRS_B.CycleMode = 2.0;
              RS_DS_CM11_noSimulinkRS_B.SpeedCmd += 0.1;
            }
            break;
          }
        }
        break;

       case RS_DS_CM11_noSimulinkRS_IN_Idle:
        /* During 'Idle': '<S7>:1' */
        out = ((RS_DS_CM11_noSimulinkRS_B.SimCore_State >= 6.0) &&
               (RS_DS_CM11_noSimulinkRS_B.distanceTravel <=
                RS_DS_CM11_noSimulinkRS_P.totalDistancem_Value) &&
               (RS_DS_CM11_noSimulinkRS_B.CM_Time_l <=
                RS_DS_CM11_noSimulinkRS_P.totalTimesec_Value) &&
               (!(RS_DS_CM11_noSimulinkRS_P.RampDownFlag_Value != 0.0)));
        if (out) {
          /* Transition: '<S7>:6' */
          RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS =
            RS_DS_CM11_noSimulinkRS_IN_CycleOn;

          /* Entry Internal 'CycleOn': '<S7>:17' */
          /* Transition: '<S7>:62' */
          RS_DS_CM11_noSimulinkRS_DW.is_CycleOn =
            RS_DS_CM11_noSimulinkRS_IN_InitialWait;
          RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 = 0U;

          /* Entry 'InitialWait': '<S7>:58' */
          RS_DS_CM11_noSimulinkRS_B.CycleMode = 1.0;
        } else {
          RS_DS_CM11_noSimulinkRS_B.CycleMode = 0.0;
          RS_DS_CM11_noSimulinkRS_B.SpeedCmd = 0.0;
        }
        break;

       case RS_DS_CM11_noSimulinkRS_IN_RampDown:
        /* During 'RampDown': '<S7>:4' */
        if (RS_DS_CM11_noSimulinkRS_B.SpeedCmd < 0.5) {
          /* Transition: '<S7>:8' */
          RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS =
            RS_DS_CM11_noSimulinkRS_IN_Idle;

          /* Entry 'Idle': '<S7>:1' */
          RS_DS_CM11_noSimulinkRS_B.CycleMode = 0.0;
          RS_DS_CM11_noSimulinkRS_B.SpeedCmd = 0.0;
        } else if (RS_DS_CM11_noSimulinkRS_B.SpeedCmd >= 0.5) {
          /* Transition: '<S7>:69' */
          RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS =
            RS_DS_CM11_noSimulinkRS_IN_WaitForNextRamp;
          RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 = 0U;
        } else {
          RS_DS_CM11_noSimulinkRS_B.CycleMode = 4.0;
          RS_DS_CM11_noSimulinkRS_B.SpeedCmd--;
        }
        break;

       default:
        /* During 'WaitForNextRamp': '<S7>:67' */
        if (RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 >= 100U) {
          /* Transition: '<S7>:68' */
          RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS =
            RS_DS_CM11_noSimulinkRS_IN_RampDown;

          /* Entry 'RampDown': '<S7>:4' */
          RS_DS_CM11_noSimulinkRS_B.CycleMode = 4.0;
          RS_DS_CM11_noSimulinkRS_B.SpeedCmd--;
        }
        break;
      }
    }

    /* End of Chart: '<S1>/RealSimHILCycle' */

    /* RelationalOperator: '<S8>/Equal' incorporates:
     *  Constant: '<S8>/Constant1'
     */
    RS_DS_CM11_noSimulinkRS_B.Equal = (RS_DS_CM11_noSimulinkRS_B.CycleMode ==
      RS_DS_CM11_noSimulinkRS_P.Constant1_Value);

    /* Switch: '<S8>/Switch2' */
    if (RS_DS_CM11_noSimulinkRS_B.Equal) {
      /* Switch: '<S8>/Switch2' */
      RS_DS_CM11_noSimulinkRS_B.driverSourceFinal =
        RS_DS_CM11_noSimulinkRS_B.Switch1;
    } else {
      /* Switch: '<S8>/Switch2' incorporates:
       *  Constant: '<S8>/Constant2'
       */
      RS_DS_CM11_noSimulinkRS_B.driverSourceFinal =
        RS_DS_CM11_noSimulinkRS_P.Constant2_Value;
    }

    /* End of Switch: '<S8>/Switch2' */

    /* Switch: '<S8>/Switch' */
    if (RS_DS_CM11_noSimulinkRS_B.driverSourceFinal >
        RS_DS_CM11_noSimulinkRS_P.Switch_Threshold) {
      /* Switch: '<S8>/Switch' */
      memcpy(&RS_DS_CM11_noSimulinkRS_B.Switch[0],
             &RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[0], 9U * sizeof(real_T));
      RS_DS_CM11_noSimulinkRS_B.Switch[9] = RS_DS_CM11_noSimulinkRS_B.Gain1;
      RS_DS_CM11_noSimulinkRS_B.Switch[10] =
        RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[10];
      RS_DS_CM11_noSimulinkRS_B.Switch[11] =
        RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[11];
      RS_DS_CM11_noSimulinkRS_B.Switch[12] = RS_DS_CM11_noSimulinkRS_B.Gain;
      memcpy(&RS_DS_CM11_noSimulinkRS_B.Switch[13],
             &RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[13], sizeof(real_T) <<
             4U);
    } else {
      /* Switch: '<S8>/Switch' */
      memcpy(&RS_DS_CM11_noSimulinkRS_B.Switch[0],
             &RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[0], 29U * sizeof
             (real_T));
    }

    /* End of Switch: '<S8>/Switch' */

    /* S-Function (CM_Sfun): '<S8>/VehicleControlUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[4];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[15];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[16];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[17];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[18];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[19];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[20];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[21];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[22];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[23];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[24];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[25];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[26];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[27];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Switch[28];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o2 = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o6[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o6[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[11] = *p++;
    }

    /* S-Function (CM_Sfun): '<S20>/Steering' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[5];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[4];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[15] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[16] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[17] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[18] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[19] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[20] = *p++;
    }

    /* S-Function (CM_Sfun): '<S18>/Kinematics' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[6];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Steering_o2[15];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[7] = *p++;
    }

    /* S-Function (CM_Sfun): '<S26>/Forces' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[7];
      double *p;
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[15] = *p++;
    }

    /* S-Function (CM_Sfun): '<S24>/Suspension Control Unit' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[8];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[7];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[15] = *p++;
    }

    /* S-Function (CM_Sfun): '<S24>/Suspension Force Elements' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[9];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[15];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[15] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[16] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[17] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[18] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[19] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[20] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[21] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[22] = *p++;
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[23] = *p++;
    }

    /* S-Function (CM_Sfun): '<S24>/Suspension Force Elements Update' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[10];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[15];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[16];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[17];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[18];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[19];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[20];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[21];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[22];
      *p++ = RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[23];
      rinf->Func();
    }

    /* S-Function (CM_Sfun): '<S19>/Kinetics' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[11];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o3[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Forces_o4[15];
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[3] = *p++;
    }

    /* S-Function (CM_Sfun): '<S21>/Trailer' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[12];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[3];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[15] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[16] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[17] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o9[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o9[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o9[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o9[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o9[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Trailer_o9[5] = *p++;
    }

    /* S-Function (CM_Sfun): '<S16>/CarAndTrailerUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[13];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o3[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[15];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[16];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o4[17];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o5[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o6[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o7[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o8[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o9[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o9[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o9[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o9[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o9[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Trailer_o9[5];
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      *p++ = 0.0;
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[8] = *p++;
    }

    /* S-Function (CM_Sfun): '<S10>/Brake' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[18];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o6[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o6[1];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[15] = *p++;
    }

    /* S-Function (CM_Sfun): '<S10>/BrakeUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[19];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.Brake_o2[15];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[15] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[16] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[17] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[18] = *p++;
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[19] = *p++;
    }

    /* S-Function (CM_Sfun): '<S13>/PowerTrain' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[20];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[15];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[16];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[17];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[18];
      *p++ = RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[19];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[11];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[0] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[1] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[2] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[3] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[4] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[5] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[6] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[7] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[8] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[9] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[10] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[11] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[12] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[13] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[14] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[15] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[16] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[17] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[18] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[19] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[20] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[21] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[22] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[23] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[24] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[25] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[26] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[27] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[28] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[29] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[30] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[31] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[32] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[33] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[34] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[35] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[36] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[37] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[38] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[39] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[40] = *p++;
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[41] = *p++;
    }

    /* S-Function (CM_Sfun): '<S13>/PowerTrainUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[21];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[0];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[1];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[2];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[3];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[4];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[5];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[6];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[7];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[8];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[9];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[10];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[11];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[12];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[13];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[14];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[15];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[16];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[17];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[18];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[19];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[20];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[21];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[22];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[23];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[24];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[25];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[26];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[27];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[28];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[29];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[30];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[31];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[32];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[33];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[34];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[35];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[36];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[37];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[38];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[39];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[40];
      *p++ = RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[41];
      rinf->Func();
    }

    /* S-Function (CM_Sfun): '<S4>/CM_User' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[22];
      rinf->Func();
    }

    /* S-Function (CM_Sfun): '<S4>/IoOut' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[23];
      rinf->Func();
    }

    /* Gain: '<S1>/Gain3' */
    RS_DS_CM11_noSimulinkRS_B.HIL_SpeedCmd_kph =
      RS_DS_CM11_noSimulinkRS_P.Gain3_Gain * RS_DS_CM11_noSimulinkRS_B.SpeedCmd;

    /* S-Function (read_dict): '<S6>/Read CM Dict' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.VehSpd = e->GetFunc(e->Var);
    }

    /* Gain: '<S6>/Gain1' */
    RS_DS_CM11_noSimulinkRS_B.VehSpdKph = RS_DS_CM11_noSimulinkRS_P.Gain1_Gain_o
      * RS_DS_CM11_noSimulinkRS_B.VehSpd;

    /* Gain: '<S6>/Gain3' */
    RS_DS_CM11_noSimulinkRS_B.VehSpdKph_h =
      RS_DS_CM11_noSimulinkRS_P.Gain3_Gain_n *
      RS_DS_CM11_noSimulinkRS_B.SpeedCmd;

    /* Outputs for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
    /* DataTypeConversion: '<S12>/Data Type Conversion11' */
    RS_DS_CM11_noSimulinkRS_B.Vehicle_Spd_kph =
      RS_DS_CM11_noSimulinkRS_B.VehSpdKph;

    /* Constant: '<S39>/Constant' */
    RS_DS_CM11_noSimulinkRS_B.Drive = RS_DS_CM11_noSimulinkRS_P.Constant_Value_n;

    /* Memory: '<S39>/Memory1' */
    RS_DS_CM11_noSimulinkRS_B.Memory1 =
      RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput;

    /* Memory: '<S39>/Memory2' */
    RS_DS_CM11_noSimulinkRS_B.Memory2 =
      RS_DS_CM11_noSimulinkRS_DW.Memory2_PreviousInput;

    /* Saturate: '<S39>/SpdSat' */
    u0 = RS_DS_CM11_noSimulinkRS_B.Memory1;
    u1 = RS_DS_CM11_noSimulinkRS_P.SpdSat_LowerSat;
    u2 = RS_DS_CM11_noSimulinkRS_P.SpdSat_UpperSat;
    if (u0 > u2) {
      /* Saturate: '<S39>/SpdSat' */
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr = u2;
    } else if (u0 < u1) {
      /* Saturate: '<S39>/SpdSat' */
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr = u1;
    } else {
      /* Saturate: '<S39>/SpdSat' */
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr = u0;
    }

    /* End of Saturate: '<S39>/SpdSat' */

    /* Memory: '<S40>/Memory1' */
    RS_DS_CM11_noSimulinkRS_B.Memory1_f =
      RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput_f;

    /* Memory: '<S40>/Memory' */
    RS_DS_CM11_noSimulinkRS_B.Memory =
      RS_DS_CM11_noSimulinkRS_DW.Memory_PreviousInput;

    /* Sum: '<S40>/Add' */
    RS_DS_CM11_noSimulinkRS_B.Add = RS_DS_CM11_noSimulinkRS_B.Memory1_f -
      RS_DS_CM11_noSimulinkRS_B.Memory;

    /* RelationalOperator: '<S41>/Compare' incorporates:
     *  Constant: '<S41>/Constant'
     */
    RS_DS_CM11_noSimulinkRS_B.Compare_p = (RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr <=
      RS_DS_CM11_noSimulinkRS_P.CompareToConstant1_const);

    /* RelationalOperator: '<S42>/Compare' incorporates:
     *  Constant: '<S42>/Constant'
     */
    RS_DS_CM11_noSimulinkRS_B.Compare_d = (RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr <=
      RS_DS_CM11_noSimulinkRS_P.CompareToConstant3_const);

    /* UnitDelay: '<S43>/Delay Input1' */
    RS_DS_CM11_noSimulinkRS_B.Uk1 =
      RS_DS_CM11_noSimulinkRS_DW.DelayInput1_DSTATE;

    /* RelationalOperator: '<S45>/Compare' incorporates:
     *  Constant: '<S45>/Constant'
     */
    RS_DS_CM11_noSimulinkRS_B.Compare_i = (RS_DS_CM11_noSimulinkRS_B.Add <
      RS_DS_CM11_noSimulinkRS_P.Constant_Value);

    /* RelationalOperator: '<S43>/FixPt Relational Operator' */
    RS_DS_CM11_noSimulinkRS_B.FixPtRelationalOperator = ((int32_T)
      RS_DS_CM11_noSimulinkRS_B.Compare_i > (int32_T)
      RS_DS_CM11_noSimulinkRS_B.Uk1);

    /* UnitDelay: '<S44>/Delay Input1' */
    RS_DS_CM11_noSimulinkRS_B.Uk1_p =
      RS_DS_CM11_noSimulinkRS_DW.DelayInput1_DSTATE_h;

    /* RelationalOperator: '<S46>/Compare' incorporates:
     *  Constant: '<S46>/Constant'
     */
    RS_DS_CM11_noSimulinkRS_B.Compare_f = (RS_DS_CM11_noSimulinkRS_B.Add >
      RS_DS_CM11_noSimulinkRS_P.Constant_Value_d);

    /* RelationalOperator: '<S44>/FixPt Relational Operator' */
    RS_DS_CM11_noSimulinkRS_B.FixPtRelationalOperator_b = ((int32_T)
      RS_DS_CM11_noSimulinkRS_B.Compare_f > (int32_T)
      RS_DS_CM11_noSimulinkRS_B.Uk1_p);

    /* Lookup_n-D: '<S40>/Feedforward' incorporates:
     *  Saturate: '<S39>/SpdSat'
     */
    RS_DS_CM11_noSimulinkRS_B.Feedforward = look1_binlcpw
      (RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr,
       RS_DS_CM11_noSimulinkRS_P.Feedforward_bp01Data,
       RS_DS_CM11_noSimulinkRS_P.Feedforward_tableData, 16U);

    /* Gain: '<S40>/FeedforwardGain' */
    RS_DS_CM11_noSimulinkRS_B.Driver_FdfwdTerm_pct =
      RS_DS_CM11_noSimulinkRS_P.FeedforwardGain_Gain *
      RS_DS_CM11_noSimulinkRS_B.Feedforward;

    /* Logic: '<S40>/Logical Operator' */
    RS_DS_CM11_noSimulinkRS_B.LogicalOperator =
      (RS_DS_CM11_noSimulinkRS_B.Compare_d ||
       RS_DS_CM11_noSimulinkRS_B.FixPtRelationalOperator_b ||
       RS_DS_CM11_noSimulinkRS_B.FixPtRelationalOperator);

    /* End of Outputs for SubSystem: '<S6>/Driver and Drive Cycle' */
  }

  /* Outputs for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
  /* Integrator: '<S40>/Limits [-50,50]' */
  /* Limited  Integrator  */
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    out = (((RS_DS_CM11_noSimulinkRS_PrevZCX.Limits5050_Reset_ZCE == 1) !=
            (int32_T)RS_DS_CM11_noSimulinkRS_B.LogicalOperator) &&
           (RS_DS_CM11_noSimulinkRS_PrevZCX.Limits5050_Reset_ZCE != 3));
    RS_DS_CM11_noSimulinkRS_PrevZCX.Limits5050_Reset_ZCE =
      RS_DS_CM11_noSimulinkRS_B.LogicalOperator;

    /* evaluate zero-crossings */
    if (out) {
      RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE =
        RS_DS_CM11_noSimulinkRS_P.Limits5050_IC;
    }
  }

  if (RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE >=
      RS_DS_CM11_noSimulinkRS_P.Limits5050_UpperSat) {
    RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE =
      RS_DS_CM11_noSimulinkRS_P.Limits5050_UpperSat;
  } else if (RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE <=
             RS_DS_CM11_noSimulinkRS_P.Limits5050_LowerSat) {
    RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE =
      RS_DS_CM11_noSimulinkRS_P.Limits5050_LowerSat;
  }

  /* Integrator: '<S40>/Limits [-50,50]' */
  RS_DS_CM11_noSimulinkRS_B.Driver_IntTerm_pct =
    RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE;

  /* TransferFcn: '<S40>/Lowpass Filter' */
  RS_DS_CM11_noSimulinkRS_B.VS_Flt = 0.0;
  RS_DS_CM11_noSimulinkRS_B.VS_Flt += RS_DS_CM11_noSimulinkRS_P.LowpassFilter_C *
    RS_DS_CM11_noSimulinkRS_X.LowpassFilter_CSTATE;

  /* Sum: '<S40>/Sum' */
  RS_DS_CM11_noSimulinkRS_B.SpdErr = RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr -
    RS_DS_CM11_noSimulinkRS_B.VS_Flt;

  /* Gain: '<S40>/Kp' */
  RS_DS_CM11_noSimulinkRS_B.Kp = RS_DS_CM11_noSimulinkRS_P.Kp_Gain *
    RS_DS_CM11_noSimulinkRS_B.SpdErr;
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    /* Lookup_n-D: '<S40>/Proportional Gain Scheduling' incorporates:
     *  Memory: '<S39>/Memory2'
     */
    RS_DS_CM11_noSimulinkRS_B.ProportionalGainScheduling = look1_plinlcapw
      (RS_DS_CM11_noSimulinkRS_B.Memory2,
       RS_DS_CM11_noSimulinkRS_P.ProportionalGainScheduling_bp01Data,
       RS_DS_CM11_noSimulinkRS_P.ProportionalGainScheduling_tableData,
       &RS_DS_CM11_noSimulinkRS_DW.m_bpIndex, 2U);
  }

  /* Product: '<S40>/Product1' */
  RS_DS_CM11_noSimulinkRS_B.Driver_PropTerm_pct = RS_DS_CM11_noSimulinkRS_B.Kp *
    RS_DS_CM11_noSimulinkRS_B.ProportionalGainScheduling;

  /* Sum: '<S40>/Sum1' */
  RS_DS_CM11_noSimulinkRS_B.Driver_TotCmdPreLim_pct =
    (RS_DS_CM11_noSimulinkRS_B.Driver_IntTerm_pct +
     RS_DS_CM11_noSimulinkRS_B.Driver_PropTerm_pct) +
    RS_DS_CM11_noSimulinkRS_B.Driver_FdfwdTerm_pct;

  /* Gain: '<S40>/Gain' */
  RS_DS_CM11_noSimulinkRS_B.Gain_m = RS_DS_CM11_noSimulinkRS_P.Gain_Gain *
    RS_DS_CM11_noSimulinkRS_B.Driver_TotCmdPreLim_pct;

  /* Gain: '<S40>/Ki' */
  RS_DS_CM11_noSimulinkRS_B.Ki = RS_DS_CM11_noSimulinkRS_P.Ki_Gain *
    RS_DS_CM11_noSimulinkRS_B.SpdErr;

  /* Switch: '<S40>/Switch2' */
  if (RS_DS_CM11_noSimulinkRS_B.Gain_m >
      RS_DS_CM11_noSimulinkRS_P.Switch2_Threshold) {
    /* Switch: '<S40>/Switch2' */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalPosPre_pct =
      RS_DS_CM11_noSimulinkRS_B.Gain_m;
  } else {
    /* Switch: '<S40>/Switch2' */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalPosPre_pct = 0.0;
  }

  /* End of Switch: '<S40>/Switch2' */

  /* RelationalOperator: '<S40>/Relational Operator' incorporates:
   *  Constant: '<S40>/Min threshold for Brake switch'
   */
  RS_DS_CM11_noSimulinkRS_B.BrakeSwitch_b =
    (RS_DS_CM11_noSimulinkRS_B.BrakePedalPosPre_pct >
     RS_DS_CM11_noSimulinkRS_P.MinthresholdforBrakeswitch_Value);

  /* Switch: '<S40>/Switch1' */
  if (RS_DS_CM11_noSimulinkRS_B.Driver_TotCmdPreLim_pct >
      RS_DS_CM11_noSimulinkRS_P.Switch1_Threshold) {
    /* Switch: '<S40>/Switch1' */
    RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPosPre_pct =
      RS_DS_CM11_noSimulinkRS_B.Driver_TotCmdPreLim_pct;
  } else {
    /* Switch: '<S40>/Switch1' */
    RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPosPre_pct = 0.0;
  }

  /* End of Switch: '<S40>/Switch1' */

  /* Switch: '<S40>/Switch3' incorporates:
   *  Switch: '<S40>/Switch4'
   */
  if (RS_DS_CM11_noSimulinkRS_B.Compare_p) {
    /* Switch: '<S40>/Switch3' incorporates:
     *  Constant: '<S40>/Constant'
     */
    RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPos_pct =
      RS_DS_CM11_noSimulinkRS_P.Constant_Value_p;

    /* Switch: '<S40>/Switch4' incorporates:
     *  Constant: '<S40>/standstill brake [0,100]'
     */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalPos_pct =
      RS_DS_CM11_noSimulinkRS_P.standstillbrake0100_Value;
  } else {
    /* Switch: '<S40>/Switch3' */
    RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPos_pct =
      RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPosPre_pct;

    /* Switch: '<S40>/Switch4' */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalPos_pct =
      RS_DS_CM11_noSimulinkRS_B.BrakePedalPosPre_pct;
  }

  /* End of Switch: '<S40>/Switch3' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    /* Gain: '<S39>/km to miles' */
    RS_DS_CM11_noSimulinkRS_B.RefSpdmph =
      RS_DS_CM11_noSimulinkRS_P.kmtomiles_Gain *
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr;

    /* Gain: '<S12>/Gain' */
    RS_DS_CM11_noSimulinkRS_B.Vehicle_Spd_mph =
      RS_DS_CM11_noSimulinkRS_P.Gain_Gain_b *
      RS_DS_CM11_noSimulinkRS_B.Vehicle_Spd_kph;

    /* RateLimiter: '<S12>/Rate Limiter' incorporates:
     *  Constant: '<S12>/ManSpd'
     */
    u0 = RS_DS_CM11_noSimulinkRS_P.ManSpd_Value -
      RS_DS_CM11_noSimulinkRS_DW.PrevY;
    if (u0 > RS_DS_CM11_noSimulinkRS_P.RateLimiter_RisingLim *
        RS_DS_CM11_noSimulinkRS_period) {
      /* RateLimiter: '<S12>/Rate Limiter' */
      RS_DS_CM11_noSimulinkRS_B.RateLimiter =
        RS_DS_CM11_noSimulinkRS_P.RateLimiter_RisingLim *
        RS_DS_CM11_noSimulinkRS_period + RS_DS_CM11_noSimulinkRS_DW.PrevY;
    } else if (u0 < RS_DS_CM11_noSimulinkRS_P.RateLimiter_FallingLim *
               RS_DS_CM11_noSimulinkRS_period) {
      /* RateLimiter: '<S12>/Rate Limiter' */
      RS_DS_CM11_noSimulinkRS_B.RateLimiter =
        RS_DS_CM11_noSimulinkRS_P.RateLimiter_FallingLim *
        RS_DS_CM11_noSimulinkRS_period + RS_DS_CM11_noSimulinkRS_DW.PrevY;
    } else {
      /* RateLimiter: '<S12>/Rate Limiter' */
      RS_DS_CM11_noSimulinkRS_B.RateLimiter =
        RS_DS_CM11_noSimulinkRS_P.ManSpd_Value;
    }

    RS_DS_CM11_noSimulinkRS_DW.PrevY = RS_DS_CM11_noSimulinkRS_B.RateLimiter;

    /* End of RateLimiter: '<S12>/Rate Limiter' */

    /* MultiPortSwitch: '<S12>/Multiport Switch1' incorporates:
     *  Constant: '<S12>/ManSpdSel'
     */
    switch ((int32_T)RS_DS_CM11_noSimulinkRS_P.ManSpdSel_Value) {
     case 1:
      /* MultiPortSwitch: '<S12>/Multiport Switch1' */
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr_e =
        RS_DS_CM11_noSimulinkRS_B.RateLimiter;
      break;

     case 2:
      /* MultiPortSwitch: '<S12>/Multiport Switch1' */
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr_e =
        RS_DS_CM11_noSimulinkRS_B.VehSpdKph;
      break;

     default:
      /* MultiPortSwitch: '<S12>/Multiport Switch1' */
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr_e =
        RS_DS_CM11_noSimulinkRS_B.VehSpdKph_h;
      break;
    }

    /* End of MultiPortSwitch: '<S12>/Multiport Switch1' */

    /* Gain: '<S12>/Gain1' */
    RS_DS_CM11_noSimulinkRS_B.Vehicle_SpdDmd_mph =
      RS_DS_CM11_noSimulinkRS_P.Gain1_Gain *
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr_e;

    /* Gain: '<S9>/Gain2' */
    RS_DS_CM11_noSimulinkRS_B.Gain2 = RS_DS_CM11_noSimulinkRS_P.Gain2_Gain *
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[10];
  }

  /* End of Outputs for SubSystem: '<S6>/Driver and Drive Cycle' */

  /* MultiPortSwitch: '<S9>/Multiport Switch1' incorporates:
   *  Constant: '<S9>/Driver Control Selection'
   */
  switch ((int32_T)RS_DS_CM11_noSimulinkRS_P.DriverControlSelection_Value) {
   case 1:
    /* MultiPortSwitch: '<S9>/Multiport Switch1' incorporates:
     *  Constant: '<S9>/APP'
     */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch1 =
      RS_DS_CM11_noSimulinkRS_P.APP_Value;
    break;

   case 2:
    /* MultiPortSwitch: '<S9>/Multiport Switch1' */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch1 =
      RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPos_pct;
    break;

   case 3:
    /* MultiPortSwitch: '<S9>/Multiport Switch1' */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch1 = RS_DS_CM11_noSimulinkRS_B.Gain2;
    break;

   default:
    /* MultiPortSwitch: '<S9>/Multiport Switch1' incorporates:
     *  Constant: '<S9>/Constant'
     */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch1 =
      RS_DS_CM11_noSimulinkRS_P.Constant_Value_e;
    break;
  }

  /* End of MultiPortSwitch: '<S9>/Multiport Switch1' */

  /* Saturate: '<S9>/AR_limit' */
  u0 = RS_DS_CM11_noSimulinkRS_B.MultiportSwitch1;
  u1 = RS_DS_CM11_noSimulinkRS_P.AR_limit_LowerSat;
  u2 = RS_DS_CM11_noSimulinkRS_P.AR_limit_UpperSat;
  if (u0 > u2) {
    /* Saturate: '<S9>/AR_limit' */
    RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct_h = u2;
  } else if (u0 < u1) {
    /* Saturate: '<S9>/AR_limit' */
    RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct_h = u1;
  } else {
    /* Saturate: '<S9>/AR_limit' */
    RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct_h = u0;
  }

  /* End of Saturate: '<S9>/AR_limit' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    /* Gain: '<S9>/Gain1' */
    RS_DS_CM11_noSimulinkRS_B.Gain1_j = RS_DS_CM11_noSimulinkRS_P.Gain1_Gain_c *
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[11];
  }

  /* MultiPortSwitch: '<S9>/Multiport Switch2' incorporates:
   *  Constant: '<S9>/Driver Control Selection'
   */
  switch ((int32_T)RS_DS_CM11_noSimulinkRS_P.DriverControlSelection_Value) {
   case 1:
    /* MultiPortSwitch: '<S9>/Multiport Switch2' incorporates:
     *  Constant: '<S9>/BR_command_0to100'
     */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch2 =
      RS_DS_CM11_noSimulinkRS_P.BR_command_0to100_Value;
    break;

   case 2:
    /* MultiPortSwitch: '<S9>/Multiport Switch2' */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch2 =
      RS_DS_CM11_noSimulinkRS_B.BrakePedalPos_pct;
    break;

   case 3:
    /* MultiPortSwitch: '<S9>/Multiport Switch2' */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch2 =
      RS_DS_CM11_noSimulinkRS_B.Gain1_j;
    break;

   default:
    /* MultiPortSwitch: '<S9>/Multiport Switch2' incorporates:
     *  Constant: '<S9>/Constant1'
     */
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch2 =
      RS_DS_CM11_noSimulinkRS_P.Constant1_Value_j;
    break;
  }

  /* End of MultiPortSwitch: '<S9>/Multiport Switch2' */

  /* Saturate: '<S9>/BR_limit' */
  u0 = RS_DS_CM11_noSimulinkRS_B.MultiportSwitch2;
  u1 = RS_DS_CM11_noSimulinkRS_P.BR_limit_LowerSat;
  u2 = RS_DS_CM11_noSimulinkRS_P.BR_limit_UpperSat;
  if (u0 > u2) {
    /* Saturate: '<S9>/BR_limit' */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct_j = u2;
  } else if (u0 < u1) {
    /* Saturate: '<S9>/BR_limit' */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct_j = u1;
  } else {
    /* Saturate: '<S9>/BR_limit' */
    RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct_j = u0;
  }

  /* End of Saturate: '<S9>/BR_limit' */

  /* RelationalOperator: '<S9>/Relational Operator' incorporates:
   *  Constant: '<S9>/Min threshold for Brake switch'
   */
  RS_DS_CM11_noSimulinkRS_B.BrakeSwitch =
    (RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct_j >
     RS_DS_CM11_noSimulinkRS_P.MinthresholdforBrakeswitch_Value_g);
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    /* Gain: '<S6>/Gain2' */
    RS_DS_CM11_noSimulinkRS_B.VehSpdMph = RS_DS_CM11_noSimulinkRS_P.Gain2_Gain_p
      * RS_DS_CM11_noSimulinkRS_B.VehSpd;

    /* RelationalOperator: '<S14>/Compare' incorporates:
     *  Constant: '<S14>/Constant'
     */
    RS_DS_CM11_noSimulinkRS_B.Compare =
      (RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o2 !=
       RS_DS_CM11_noSimulinkRS_P.VehicleSource_BuiltIn_const);

    /* Stop: '<S6>/Stop Simulation' */
    if (RS_DS_CM11_noSimulinkRS_B.Compare) {
      rtmSetStopRequested(RS_DS_CM11_noSimulinkRS_M, 1);
    }

    /* End of Stop: '<S6>/Stop Simulation' */

    /* S-Function (read_dict): '<S1>/Read CM Dict' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict_PWORK_c.Entry;
      RS_DS_CM11_noSimulinkRS_B.VehSpd_j = e->GetFunc(e->Var);
    }

    /* Gain: '<S1>/Gain1' */
    RS_DS_CM11_noSimulinkRS_B.VehSpdKph_p =
      RS_DS_CM11_noSimulinkRS_P.Gain1_Gain_dz *
      RS_DS_CM11_noSimulinkRS_B.VehSpd_j;

    /* Gain: '<S1>/Gain2' */
    RS_DS_CM11_noSimulinkRS_B.VehSpdMph_l =
      RS_DS_CM11_noSimulinkRS_P.Gain2_Gain_o *
      RS_DS_CM11_noSimulinkRS_B.VehSpd_j;

    /* S-Function (read_dict): '<S1>/Read CM Dict1' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict1_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.Trf_FOL_Targ_Dtct = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict2' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict2_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.Trf_FOL_dDist = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict4' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict4_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.Trf_FOL_dSpeed = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict5' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict5_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.Trf_FOL_ObjId = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict6' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_noSimulinkRS_DW.ReadCMDict6_PWORK.Entry;
      RS_DS_CM11_noSimulinkRS_B.Trf_FOL_State = e->GetFunc(e->Var);
    }
  }

  /* End of Outputs for SubSystem: '<Root>/CarMaker' */
}

/* Model update function */
void RS_DS_CM11_noSimulinkRS_update(void)
{
  /* Update for Atomic SubSystem: '<Root>/CarMaker' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    /* Update for Memory generated from: '<S8>/Memory' */
    RS_DS_CM11_noSimulinkRS_DW.Memory_2_PreviousInput =
      RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct_j;

    /* Update for Memory generated from: '<S8>/Memory' */
    RS_DS_CM11_noSimulinkRS_DW.Memory_1_PreviousInput =
      RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct_h;

    /* Update for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
    /* Update for Memory: '<S39>/Memory1' */
    RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput =
      RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr_e;

    /* Update for Memory: '<S39>/Memory2' */
    RS_DS_CM11_noSimulinkRS_DW.Memory2_PreviousInput =
      RS_DS_CM11_noSimulinkRS_B.Vehicle_Spd_kph;

    /* Update for Memory: '<S40>/Memory1' */
    RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput_f =
      RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPosPre_pct;

    /* Update for Memory: '<S40>/Memory' */
    RS_DS_CM11_noSimulinkRS_DW.Memory_PreviousInput =
      RS_DS_CM11_noSimulinkRS_B.BrakePedalPosPre_pct;

    /* Update for UnitDelay: '<S43>/Delay Input1' */
    RS_DS_CM11_noSimulinkRS_DW.DelayInput1_DSTATE =
      RS_DS_CM11_noSimulinkRS_B.Compare_i;

    /* Update for UnitDelay: '<S44>/Delay Input1' */
    RS_DS_CM11_noSimulinkRS_DW.DelayInput1_DSTATE_h =
      RS_DS_CM11_noSimulinkRS_B.Compare_f;

    /* End of Update for SubSystem: '<S6>/Driver and Drive Cycle' */
  }

  /* End of Update for SubSystem: '<Root>/CarMaker' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_noSimulinkRS_M)) {
    rt_ertODEUpdateContinuousStates(&RS_DS_CM11_noSimulinkRS_M->solverInfo);
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++RS_DS_CM11_noSimulinkRS_M->Timing.clockTick0)) {
    ++RS_DS_CM11_noSimulinkRS_M->Timing.clockTickH0;
  }

  RS_DS_CM11_noSimulinkRS_M->Timing.t[0] = rtsiGetSolverStopTime
    (&RS_DS_CM11_noSimulinkRS_M->solverInfo);

  {
    /* Update absolute timer for sample time: [0.001s, 0.0s] */
    /* The "clockTick1" counts the number of times the code of this task has
     * been executed. The resolution of this integer timer is 0.001, which is the step size
     * of the task. Size of "clockTick1" ensures timer will not overflow during the
     * application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick1 and the high bits
     * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
     */
    RS_DS_CM11_noSimulinkRS_M->Timing.clockTick1++;
    if (!RS_DS_CM11_noSimulinkRS_M->Timing.clockTick1) {
      RS_DS_CM11_noSimulinkRS_M->Timing.clockTickH1++;
    }
  }
}

/* Derivatives for root system: '<Root>' */
void RS_DS_CM11_noSimulinkRS_derivatives(void)
{
  XDot_RS_DS_CM11_noSimulinkRS_T *_rtXdot;
  boolean_T lsat;
  boolean_T usat;
  _rtXdot = ((XDot_RS_DS_CM11_noSimulinkRS_T *)
             RS_DS_CM11_noSimulinkRS_M->derivs);

  /* Derivatives for Atomic SubSystem: '<Root>/CarMaker' */
  /* Derivatives for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
  /* Derivatives for Integrator: '<S40>/Limits [-50,50]' */
  lsat = (RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE <=
          RS_DS_CM11_noSimulinkRS_P.Limits5050_LowerSat);
  usat = (RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE >=
          RS_DS_CM11_noSimulinkRS_P.Limits5050_UpperSat);
  if (((!lsat) && (!usat)) || (lsat && (RS_DS_CM11_noSimulinkRS_B.Ki > 0.0)) ||
      (usat && (RS_DS_CM11_noSimulinkRS_B.Ki < 0.0))) {
    _rtXdot->Limits5050_CSTATE = RS_DS_CM11_noSimulinkRS_B.Ki;
  } else {
    /* in saturation */
    _rtXdot->Limits5050_CSTATE = 0.0;
  }

  /* End of Derivatives for Integrator: '<S40>/Limits [-50,50]' */

  /* Derivatives for TransferFcn: '<S40>/Lowpass Filter' */
  _rtXdot->LowpassFilter_CSTATE = 0.0;
  _rtXdot->LowpassFilter_CSTATE += RS_DS_CM11_noSimulinkRS_P.LowpassFilter_A *
    RS_DS_CM11_noSimulinkRS_X.LowpassFilter_CSTATE;
  _rtXdot->LowpassFilter_CSTATE += RS_DS_CM11_noSimulinkRS_B.Memory2;

  /* End of Derivatives for SubSystem: '<S6>/Driver and Drive Cycle' */
  /* End of Derivatives for SubSystem: '<Root>/CarMaker' */
}

/* Model initialize function */
void RS_DS_CM11_noSimulinkRS_initialize(void)
{
  /* Registration code */

  /* initialize real-time model */
  (void) memset((void *)RS_DS_CM11_noSimulinkRS_M, 0,
                sizeof(RT_MODEL_RS_DS_CM11_noSimulinkRS_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
                          &RS_DS_CM11_noSimulinkRS_M->Timing.simTimeStep);
    rtsiSetTPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo, &rtmGetTPtr
                (RS_DS_CM11_noSimulinkRS_M));
    rtsiSetStepSizePtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
                       &RS_DS_CM11_noSimulinkRS_M->Timing.stepSize0);
    rtsiSetdXPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
                 &RS_DS_CM11_noSimulinkRS_M->derivs);
    rtsiSetContStatesPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo, (real_T **)
                         &RS_DS_CM11_noSimulinkRS_M->contStates);
    rtsiSetNumContStatesPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
      &RS_DS_CM11_noSimulinkRS_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
      &RS_DS_CM11_noSimulinkRS_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
      &RS_DS_CM11_noSimulinkRS_M->periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
      &RS_DS_CM11_noSimulinkRS_M->periodicContStateRanges);
    rtsiSetErrorStatusPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
                          (&rtmGetErrorStatus(RS_DS_CM11_noSimulinkRS_M)));
    rtsiSetRTModelPtr(&RS_DS_CM11_noSimulinkRS_M->solverInfo,
                      RS_DS_CM11_noSimulinkRS_M);
  }

  rtsiSetSimTimeStep(&RS_DS_CM11_noSimulinkRS_M->solverInfo, MAJOR_TIME_STEP);
  RS_DS_CM11_noSimulinkRS_M->intgData.f[0] = RS_DS_CM11_noSimulinkRS_M->odeF[0];
  RS_DS_CM11_noSimulinkRS_M->contStates = ((X_RS_DS_CM11_noSimulinkRS_T *)
    &RS_DS_CM11_noSimulinkRS_X);
  rtsiSetSolverData(&RS_DS_CM11_noSimulinkRS_M->solverInfo, (void *)
                    &RS_DS_CM11_noSimulinkRS_M->intgData);
  rtsiSetSolverName(&RS_DS_CM11_noSimulinkRS_M->solverInfo,"ode1");
  rtmSetTPtr(RS_DS_CM11_noSimulinkRS_M,
             &RS_DS_CM11_noSimulinkRS_M->Timing.tArray[0]);
  RS_DS_CM11_noSimulinkRS_M->Timing.stepSize0 = 0.001;

  /* block I/O */
  (void) memset(((void *) &RS_DS_CM11_noSimulinkRS_B), 0,
                sizeof(B_RS_DS_CM11_noSimulinkRS_T));

  {
    int32_T i;
    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_B.Environment_o2[i] = 0.0;
    }

    for (i = 0; i < 17; i++) {
      RS_DS_CM11_noSimulinkRS_B.DrivMan_o2[i] = 0.0;
    }

    for (i = 0; i < 29; i++) {
      RS_DS_CM11_noSimulinkRS_B.VehicleControl_o2[i] = 0.0;
    }

    for (i = 0; i < 29; i++) {
      RS_DS_CM11_noSimulinkRS_B.Switch[i] = 0.0;
    }

    for (i = 0; i < 5; i++) {
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o3[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o4[i] = 0.0;
    }

    for (i = 0; i < 11; i++) {
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o5[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o7[i] = 0.0;
    }

    for (i = 0; i < 21; i++) {
      RS_DS_CM11_noSimulinkRS_B.Steering_o2[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o2[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o3[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o4[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o5[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o6[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o7[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o8[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinematics_o9[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_B.Forces_o2[i] = 0.0;
    }

    for (i = 0; i < 13; i++) {
      RS_DS_CM11_noSimulinkRS_B.Forces_o3[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_noSimulinkRS_B.Forces_o4[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o2[i] = 0.0;
    }

    for (i = 0; i < 24; i++) {
      RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o2[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o2[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o3[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_noSimulinkRS_B.Kinetics_o4[i] = 0.0;
    }

    for (i = 0; i < 15; i++) {
      RS_DS_CM11_noSimulinkRS_B.Trailer_o3[i] = 0.0;
    }

    for (i = 0; i < 18; i++) {
      RS_DS_CM11_noSimulinkRS_B.Trailer_o4[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_B.Trailer_o5[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_B.Trailer_o6[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_B.Trailer_o7[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_B.Trailer_o8[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_noSimulinkRS_B.Trailer_o9[i] = 0.0;
    }

    for (i = 0; i < 9; i++) {
      RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o2[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_noSimulinkRS_B.Brake_o2[i] = 0.0;
    }

    for (i = 0; i < 20; i++) {
      RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o2[i] = 0.0;
    }

    for (i = 0; i < 42; i++) {
      RS_DS_CM11_noSimulinkRS_B.PowerTrain_o2[i] = 0.0;
    }

    RS_DS_CM11_noSimulinkRS_B.IoIn = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Environment_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.DrivMan_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehicleControl_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Gain1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Gain = 0.0;
    RS_DS_CM11_noSimulinkRS_B.CM_Time = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Switch1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.CM_Time_l = 0.0;
    RS_DS_CM11_noSimulinkRS_B.SimCore_State = 0.0;
    RS_DS_CM11_noSimulinkRS_B.distanceTravel = 0.0;
    RS_DS_CM11_noSimulinkRS_B.driverSourceFinal = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o2 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o6[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehicleControlUpd_o6[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Steering_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinematics_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Forces_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.SuspensionControlUnit_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.SuspensionForceElements_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.SuspensionForceElementsUpdate = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[2] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o5[3] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[2] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kinetics_o6[3] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trailer_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trailer_o2[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trailer_o2[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trailer_o2[2] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trailer_o2[3] = 0.0;
    RS_DS_CM11_noSimulinkRS_B.CarAndTrailerUpd_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Sync = 0.0;
    RS_DS_CM11_noSimulinkRS_B.BrakeUpd_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Sync_j = 0.0;
    RS_DS_CM11_noSimulinkRS_B.PowerTrainUpd = 0.0;
    RS_DS_CM11_noSimulinkRS_B.CM_User = 0.0;
    RS_DS_CM11_noSimulinkRS_B.IoOut = 0.0;
    RS_DS_CM11_noSimulinkRS_B.HIL_SpeedCmd_kph = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehSpd = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehSpdKph = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehSpdKph_h = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Gain2 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.AccelPedalCmd_pct_h = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Gain1_j = 0.0;
    RS_DS_CM11_noSimulinkRS_B.MultiportSwitch2 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.BrakePedalCmd_pct_j = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehSpdMph = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehSpd_j = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehSpdKph_p = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VehSpdMph_l = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trf_FOL_Targ_Dtct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trf_FOL_dDist = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trf_FOL_dSpeed = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trf_FOL_ObjId = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Trf_FOL_State = 0.0;
    RS_DS_CM11_noSimulinkRS_B.CycleMode = 0.0;
    RS_DS_CM11_noSimulinkRS_B.SpeedCmd = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Vehicle_Spd_kph = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Drive = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Memory1 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Memory2 = 0.0;
    RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Memory1_f = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Memory = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Add = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Feedforward = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Driver_FdfwdTerm_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Driver_IntTerm_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.VS_Flt = 0.0;
    RS_DS_CM11_noSimulinkRS_B.SpdErr = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Kp = 0.0;
    RS_DS_CM11_noSimulinkRS_B.ProportionalGainScheduling = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Driver_PropTerm_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Driver_TotCmdPreLim_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Gain_m = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Ki = 0.0;
    RS_DS_CM11_noSimulinkRS_B.BrakePedalPosPre_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPosPre_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.AcceleratorPedalPos_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.BrakePedalPos_pct = 0.0;
    RS_DS_CM11_noSimulinkRS_B.RefSpdmph = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Vehicle_Spd_mph = 0.0;
    RS_DS_CM11_noSimulinkRS_B.RateLimiter = 0.0;
    RS_DS_CM11_noSimulinkRS_B.RefSpdkmhr_e = 0.0;
    RS_DS_CM11_noSimulinkRS_B.Vehicle_SpdDmd_mph = 0.0;
  }

  /* states (continuous) */
  {
    (void) memset((void *)&RS_DS_CM11_noSimulinkRS_X, 0,
                  sizeof(X_RS_DS_CM11_noSimulinkRS_T));
  }

  /* states (dwork) */
  (void) memset((void *)&RS_DS_CM11_noSimulinkRS_DW, 0,
                sizeof(DW_RS_DS_CM11_noSimulinkRS_T));
  RS_DS_CM11_noSimulinkRS_DW.Memory_2_PreviousInput = 0.0;
  RS_DS_CM11_noSimulinkRS_DW.Memory_1_PreviousInput = 0.0;
  RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput = 0.0;
  RS_DS_CM11_noSimulinkRS_DW.Memory2_PreviousInput = 0.0;
  RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput_f = 0.0;
  RS_DS_CM11_noSimulinkRS_DW.Memory_PreviousInput = 0.0;
  RS_DS_CM11_noSimulinkRS_DW.PrevY = 0.0;

  {
    /* user code (registration function declaration) */
    /*Initialize global TRC pointers. */
    RS_DS_CM11_noSimulinkRS_rti_init_trc_pointers();
  }

  {
    /* user code (Start function Header) */
    CarMaker_Startup();

    /* Start for Atomic SubSystem: '<Root>/CarMaker' */

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict3' */
    {
      char **namevec = CM_Names2StrVec("Time", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict3_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict3' */
    {
      char **namevec = CM_Names2StrVec("Time", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict3_PWORK_e.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict7' */
    {
      char **namevec = CM_Names2StrVec("SC.State", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict7_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict8' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.Distance", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict8_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S6>/Read CM Dict' */
    {
      char **namevec = CM_Names2StrVec("Car.v", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict' */
    {
      char **namevec = CM_Names2StrVec("Car.v", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict_PWORK_c.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict1' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.Targ_Dtct", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict1_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict2' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dDist", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict2_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict4' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dSpeed", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict4_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict5' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.ObjId", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict5_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict6' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.State", NULL);
      RS_DS_CM11_noSimulinkRS_DW.ReadCMDict6_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* End of Start for SubSystem: '<Root>/CarMaker' */

    /* user code (Start function Trailer) */
    CarMaker_FinishStartup();
  }

  RS_DS_CM11_noSimulinkRS_PrevZCX.Limits5050_Reset_ZCE = UNINITIALIZED_ZCSIG;

  /* SystemInitialize for Atomic SubSystem: '<Root>/CarMaker' */
  /* InitializeConditions for Memory generated from: '<S8>/Memory' */
  RS_DS_CM11_noSimulinkRS_DW.Memory_2_PreviousInput =
    RS_DS_CM11_noSimulinkRS_P.Memory_2_InitialCondition;

  /* InitializeConditions for Memory generated from: '<S8>/Memory' */
  RS_DS_CM11_noSimulinkRS_DW.Memory_1_PreviousInput =
    RS_DS_CM11_noSimulinkRS_P.Memory_1_InitialCondition;

  /* SystemInitialize for Chart: '<S1>/RealSimHILCycle' */
  RS_DS_CM11_noSimulinkRS_DW.is_CycleOn =
    RS_DS_CM11_noSimulinkRS_IN_NO_ACTIVE_CHILD;
  RS_DS_CM11_noSimulinkRS_DW.temporalCounter_i1 = 0U;
  RS_DS_CM11_noSimulinkRS_DW.is_active_c2_RS_DS_CM11_noSimulinkRS = 0U;
  RS_DS_CM11_noSimulinkRS_DW.is_c2_RS_DS_CM11_noSimulinkRS =
    RS_DS_CM11_noSimulinkRS_IN_NO_ACTIVE_CHILD;
  RS_DS_CM11_noSimulinkRS_B.CycleMode = 0.0;
  RS_DS_CM11_noSimulinkRS_B.SpeedCmd = 0.0;

  /* SystemInitialize for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
  /* InitializeConditions for Memory: '<S39>/Memory1' */
  RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput =
    RS_DS_CM11_noSimulinkRS_P.Memory1_InitialCondition;

  /* InitializeConditions for Memory: '<S39>/Memory2' */
  RS_DS_CM11_noSimulinkRS_DW.Memory2_PreviousInput =
    RS_DS_CM11_noSimulinkRS_P.Memory2_InitialCondition;

  /* InitializeConditions for Memory: '<S40>/Memory1' */
  RS_DS_CM11_noSimulinkRS_DW.Memory1_PreviousInput_f =
    RS_DS_CM11_noSimulinkRS_P.Memory1_InitialCondition_m;

  /* InitializeConditions for Memory: '<S40>/Memory' */
  RS_DS_CM11_noSimulinkRS_DW.Memory_PreviousInput =
    RS_DS_CM11_noSimulinkRS_P.Memory_InitialCondition;

  /* InitializeConditions for UnitDelay: '<S43>/Delay Input1' */
  RS_DS_CM11_noSimulinkRS_DW.DelayInput1_DSTATE =
    RS_DS_CM11_noSimulinkRS_P.DetectFallNegative_vinit;

  /* InitializeConditions for UnitDelay: '<S44>/Delay Input1' */
  RS_DS_CM11_noSimulinkRS_DW.DelayInput1_DSTATE_h =
    RS_DS_CM11_noSimulinkRS_P.DetectRisePositive_vinit;

  /* InitializeConditions for Integrator: '<S40>/Limits [-50,50]' */
  RS_DS_CM11_noSimulinkRS_X.Limits5050_CSTATE =
    RS_DS_CM11_noSimulinkRS_P.Limits5050_IC;

  /* InitializeConditions for TransferFcn: '<S40>/Lowpass Filter' */
  RS_DS_CM11_noSimulinkRS_X.LowpassFilter_CSTATE = 0.0;

  /* InitializeConditions for RateLimiter: '<S12>/Rate Limiter' */
  RS_DS_CM11_noSimulinkRS_DW.PrevY = RS_DS_CM11_noSimulinkRS_P.RateLimiter_IC;

  /* End of SystemInitialize for SubSystem: '<S6>/Driver and Drive Cycle' */
  /* End of SystemInitialize for SubSystem: '<Root>/CarMaker' */
}

/* Model terminate function */
void RS_DS_CM11_noSimulinkRS_terminate(void)
{
  /* user code (Terminate function Trailer) */
  CarMaker_Cleanup();
}

#ifndef CM_APPNAME
#define CM_APPNAME                     ""
#endif

const char CM_AppName[] = CM_APPNAME;

#ifndef CM_CMDLINE
#define CM_CMDLINE                     ""
#endif

const char CM_CmdLine[] = CM_CMDLINE;

#ifndef CM_CMDENV
#define CM_CMDENV                      ""
#endif

const char CM_CmdEnv[] = CM_CMDENV;
void
  MdlTun_DeclQuants (void)
{
}

void
  MdlTun_SetParams (void)
{
}
