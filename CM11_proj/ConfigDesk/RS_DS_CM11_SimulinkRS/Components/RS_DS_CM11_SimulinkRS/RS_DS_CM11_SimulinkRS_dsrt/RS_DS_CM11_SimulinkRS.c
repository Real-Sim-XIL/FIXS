/*
 * RS_DS_CM11_SimulinkRS.c
 *
 * Code generation for model "RS_DS_CM11_SimulinkRS".
 *
 * Model version              : 5.8
 * Simulink Coder version : 9.6 (R2021b) 14-May-2021
 * C source code generated on : Fri May 19 17:14:26 2023
 *
 * Target selection: dsrt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "RS_DS_CM11_SimulinkRS_dsrtvdf.h"
#include "RS_DS_CM11_SimulinkRS.h"
#include "RS_DS_CM11_SimulinkRS_private.h"

/* Named constants for Chart: '<S1>/RealSimHILCycle' */
#define RS_DS_CM11_SimulinkRS_IN_CycleOn ((uint8_T)1U)
#define RS_DS_CM11_SimulinkRS_IN_Driving ((uint8_T)1U)
#define RS_DS_CM11_SimulinkRS_IN_Idle  ((uint8_T)2U)
#define RS_DS_CM11_SimulinkRS_IN_InitialWait ((uint8_T)2U)
#define RS_DS_CM11_SimulinkRS_IN_NO_ACTIVE_CHILD ((uint8_T)0U)
#define RS_DS_CM11_SimulinkRS_IN_RampDown ((uint8_T)3U)
#define RS_DS_CM11_SimulinkRS_IN_RampUp ((uint8_T)3U)
#define RS_DS_CM11_SimulinkRS_IN_WaitForNextRamp ((uint8_T)4U)
#define RS_DS_CM11_SimulinkRS_period   (0.001)
#include "MatSupp.h"

void CarMaker_Startup (void);
void CarMaker_FinishStartup (void);
void CarMaker_Cleanup (void);
const VehDataBus RS_DS_CM11_SimulinkRS_rtZVehDataBus = {
  {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }
  ,                                    /* id */
  0.0,                                 /* idLength */

  {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }
  ,                                    /* type */
  0.0,                                 /* typeLength */

  {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }
  ,                                    /* vehicleClass */
  0.0,                                 /* vehicleClassLength */
  0.0,                                 /* speed */
  0.0,                                 /* acceleration */
  0.0,                                 /* positionX */
  0.0,                                 /* positionY */
  0.0,                                 /* positionZ */
  0.0,                                 /* heading */
  0.0,                                 /* color */

  {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }
  ,                                    /* linkId */
  0.0,                                 /* linkIdLength */
  0.0,                                 /* laneId */
  0.0,                                 /* distanceTravel */
  0.0,                                 /* speedDesired */
  0.0,                                 /* accelerationDesired */
  0.0,                                 /* hasPrecedingVehicle */

  {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }
  ,                                    /* precedingVehicleId */
  0.0,                                 /* precedingVehicleIdLength */
  0.0,                                 /* precedingVehicleDistance */
  0.0,                                 /* precedingVehicleSpeed */

  {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }
  ,                                    /* signalLightId */
  0.0,                                 /* signalLightIdLength */
  0.0,                                 /* signalLightHeadId */
  0.0,                                 /* signalLightDistance */
  0.0,                                 /* signalLightColor */
  0.0,                                 /* speedLimit */
  0.0,                                 /* speedLimitNext */
  0.0,                                 /* speedLimitChangeDistance */

  {
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U }
  ,                                    /* linkIdNext */
  0.0,                                 /* linkIdNextLength */
  0.0,                                 /* grade */
  0.0                                  /* activeLaneChange */
};                                     /* VehDataBus ground */

/* Block signals (default storage) */
B_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_B;

/* Continuous states */
X_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_X;

/* Block states (default storage) */
DW_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_DW;

/* Previous zero-crossings (trigger) states */
PrevZCX_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_PrevZCX;

/* Real-time model */
static RT_MODEL_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_M_;
RT_MODEL_RS_DS_CM11_SimulinkRS_T *const RS_DS_CM11_SimulinkRS_M =
  &RS_DS_CM11_SimulinkRS_M_;

/* Forward declaration for local functions */
static void RS_DS_CM11_SimulinkRS_RealSimDepack_setupImpl
  (RealSimDepack_RS_DS_CM11_SimulinkRS_T *obj);
static void RS_DS_CM11_SimulinkRS_RealSimInterpSpeed_setupImpl
  (RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T *obj);
static void RS_DS_CM11_SimulinkRS_RealSimPack_setupImpl
  (RealSimPack_RS_DS_CM11_SimulinkRS_T *obj);
static void RS_DS_CM11_SimulinkRS_RealSimDepack_stepImpl
  (RealSimDepack_RS_DS_CM11_SimulinkRS_T *obj, const uint8_T ByteData[1024],
   uint32_T nByte, real_T connectionState, real_T *simState, real_T *t, real_T
   *nVeh, VehDataBus *b_VehDataBus, real_T *isVehicleInNetwork);
static void RS_DS_CM11_SimulinkRS_copysign(char_T s1_data[], int32_T *idx, const
  char_T s_data[], int32_T *k, int32_T n, boolean_T *foundsign, boolean_T
  *success);
static boolean_T RS_DS_CM11_SimulinkRS_isUnitImag(const char_T s_data[], int32_T
  k, int32_T n);
static void RS_DS_CM11_SimulinkRS_readNonFinite(const char_T s_data[], int32_T
  *k, int32_T n, boolean_T *b_finite, real_T *fv);
static boolean_T RS_DS_CM11_SimulinkRS_copydigits(char_T s1_data[], int32_T *idx,
  const char_T s_data[], int32_T *k, int32_T n, boolean_T allowpoint);
static boolean_T RS_DS_CM11_SimulinkRS_copyexponent(char_T s1_data[], int32_T
  *idx, const char_T s_data[], int32_T *k, int32_T n);
static void RS_DS_CM11_SimulinkRS_readfloat(char_T s1_data[], int32_T *idx,
  const char_T s_data[], int32_T *k, int32_T n, boolean_T *isimag, boolean_T
  *b_finite, real_T *nfv, boolean_T *foundsign, boolean_T *success);
static real_T RS_DS_CM11_SimulinkRS_RealSimMsgClass_packString_ilb(uint8_T
  ByteData[200], real_T iByte, const uint8_T VehData_precedingVehicleId[50],
  real_T VehData_precedingVehicleIdLength);
static void RS_DS_CM11_SimulinkRS_RealSimPack_stepImpl
  (RealSimPack_RS_DS_CM11_SimulinkRS_T *obj, real_T simState, real_T t, const
   VehDataBus *VehDataBus_0, uint8_T ByteData[200], real_T *nMsgSize);
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
  int_T nXc = 4;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);
  rtsiSetdX(si, f0);
  RS_DS_CM11_SimulinkRS_derivatives();
  rtsiSetT(si, tnew);
  for (i = 0; i < nXc; ++i) {
    x[i] += h * f0[i];
  }

  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

static void RS_DS_CM11_SimulinkRS_RealSimDepack_setupImpl
  (RealSimDepack_RS_DS_CM11_SimulinkRS_T *obj)
{
  RealSimDepack_RS_DS_CM11_SimulinkRS_T *obj_0;

  /*         %% Common functions */
  /*  Perform one-time calculations, such as computing constants */
  obj_0 = obj;

  /*  convert VehicleMessageFieldDefInputVec to VehicleMessageFieldDef */
  obj_0->VehicleMessageFieldDef.id = obj_0->VehicleMessageFieldDefInputVec[0];
  obj_0->VehicleMessageFieldDef.type = obj_0->VehicleMessageFieldDefInputVec[1];
  obj_0->VehicleMessageFieldDef.vehicleClass =
    obj_0->VehicleMessageFieldDefInputVec[2];
  obj_0->VehicleMessageFieldDef.speed = obj_0->VehicleMessageFieldDefInputVec[3];
  obj_0->VehicleMessageFieldDef.acceleration =
    obj_0->VehicleMessageFieldDefInputVec[4];
  obj_0->VehicleMessageFieldDef.positionX =
    obj_0->VehicleMessageFieldDefInputVec[5];
  obj_0->VehicleMessageFieldDef.positionY =
    obj_0->VehicleMessageFieldDefInputVec[6];
  obj_0->VehicleMessageFieldDef.positionZ =
    obj_0->VehicleMessageFieldDefInputVec[7];
  obj_0->VehicleMessageFieldDef.heading = obj_0->VehicleMessageFieldDefInputVec
    [8];
  obj_0->VehicleMessageFieldDef.color = obj_0->VehicleMessageFieldDefInputVec[9];
  obj_0->VehicleMessageFieldDef.linkId = obj_0->VehicleMessageFieldDefInputVec
    [10];
  obj_0->VehicleMessageFieldDef.laneId = obj_0->VehicleMessageFieldDefInputVec
    [11];
  obj_0->VehicleMessageFieldDef.distanceTravel =
    obj_0->VehicleMessageFieldDefInputVec[12];
  obj_0->VehicleMessageFieldDef.speedDesired =
    obj_0->VehicleMessageFieldDefInputVec[13];
  obj_0->VehicleMessageFieldDef.accelerationDesired =
    obj_0->VehicleMessageFieldDefInputVec[14];
  obj_0->VehicleMessageFieldDef.hasPrecedingVehicle =
    obj_0->VehicleMessageFieldDefInputVec[15];
  obj_0->VehicleMessageFieldDef.precedingVehicleId =
    obj_0->VehicleMessageFieldDefInputVec[16];
  obj_0->VehicleMessageFieldDef.precedingVehicleDistance =
    obj_0->VehicleMessageFieldDefInputVec[17];
  obj_0->VehicleMessageFieldDef.precedingVehicleSpeed =
    obj_0->VehicleMessageFieldDefInputVec[18];
  obj_0->VehicleMessageFieldDef.signalLightId =
    obj_0->VehicleMessageFieldDefInputVec[19];
  obj_0->VehicleMessageFieldDef.signalLightHeadId =
    obj_0->VehicleMessageFieldDefInputVec[20];
  obj_0->VehicleMessageFieldDef.signalLightDistance =
    obj_0->VehicleMessageFieldDefInputVec[21];
  obj_0->VehicleMessageFieldDef.signalLightColor =
    obj_0->VehicleMessageFieldDefInputVec[22];
  obj_0->VehicleMessageFieldDef.speedLimit =
    obj_0->VehicleMessageFieldDefInputVec[23];
  obj_0->VehicleMessageFieldDef.speedLimitNext =
    obj_0->VehicleMessageFieldDefInputVec[24];
  obj_0->VehicleMessageFieldDef.speedLimitChangeDistance =
    obj_0->VehicleMessageFieldDefInputVec[25];
  obj_0->VehicleMessageFieldDef.linkIdNext =
    obj_0->VehicleMessageFieldDefInputVec[26];
  obj_0->VehicleMessageFieldDef.grade = obj_0->VehicleMessageFieldDefInputVec[27];
  obj_0->VehicleMessageFieldDef.activeLaneChange =
    obj_0->VehicleMessageFieldDefInputVec[28];
  obj->simStatePrevious = 0.0;
  obj->tPrevious = 0.0;
  obj->VehicleDataPrevious = obj->VehicleDataEmpty;
  obj->isVehicleInNetworkPrevious = 0.0;
}

static void RS_DS_CM11_SimulinkRS_RealSimInterpSpeed_setupImpl
  (RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T *obj)
{
  /*         %% Common functions */
  /*  Perform one-time calculations, such as computing constants */
  obj->timePrevious = 0.0;
  obj->speedPrevious = 0.0;
  obj->timeNext = 0.0;
  obj->speedNext = 0.0;
  obj->connectionState = 0.0;
  obj->timeSimulator = 0.0;
  obj->simulatorStartTime = 0.0;
  obj->timeStepSimulator = 0.0;
  obj->timeStepSimulatorPrevious = 0.0;
  obj->timeReceivePrevious = 0.0;
  obj->simState = 0.0;
  obj->RealSimDelay = 0.0;
  obj->speedLookAheadPrevious = 0.0;
  obj->accelerationDesiredPrevious = 0.0;
  obj->speedInterpolationMode = 0.0;
}

static void RS_DS_CM11_SimulinkRS_RealSimPack_setupImpl
  (RealSimPack_RS_DS_CM11_SimulinkRS_T *obj)
{
  RealSimPack_RS_DS_CM11_SimulinkRS_T *obj_0;

  /*         %% Common functions */
  /*  Perform one-time calculations, such as computing constants */
  /*  convert vector message field definition to struct  */
  obj_0 = obj;

  /*  convert VehicleMessageFieldDefInputVec to VehicleMessageFieldDef */
  obj_0->VehicleMessageFieldDef.id = obj_0->VehicleMessageFieldDefInputVec[0];
  obj_0->VehicleMessageFieldDef.type = obj_0->VehicleMessageFieldDefInputVec[1];
  obj_0->VehicleMessageFieldDef.vehicleClass =
    obj_0->VehicleMessageFieldDefInputVec[2];
  obj_0->VehicleMessageFieldDef.speed = obj_0->VehicleMessageFieldDefInputVec[3];
  obj_0->VehicleMessageFieldDef.acceleration =
    obj_0->VehicleMessageFieldDefInputVec[4];
  obj_0->VehicleMessageFieldDef.positionX =
    obj_0->VehicleMessageFieldDefInputVec[5];
  obj_0->VehicleMessageFieldDef.positionY =
    obj_0->VehicleMessageFieldDefInputVec[6];
  obj_0->VehicleMessageFieldDef.positionZ =
    obj_0->VehicleMessageFieldDefInputVec[7];
  obj_0->VehicleMessageFieldDef.heading = obj_0->VehicleMessageFieldDefInputVec
    [8];
  obj_0->VehicleMessageFieldDef.color = obj_0->VehicleMessageFieldDefInputVec[9];
  obj_0->VehicleMessageFieldDef.linkId = obj_0->VehicleMessageFieldDefInputVec
    [10];
  obj_0->VehicleMessageFieldDef.laneId = obj_0->VehicleMessageFieldDefInputVec
    [11];
  obj_0->VehicleMessageFieldDef.distanceTravel =
    obj_0->VehicleMessageFieldDefInputVec[12];
  obj_0->VehicleMessageFieldDef.speedDesired =
    obj_0->VehicleMessageFieldDefInputVec[13];
  obj_0->VehicleMessageFieldDef.accelerationDesired =
    obj_0->VehicleMessageFieldDefInputVec[14];
  obj_0->VehicleMessageFieldDef.hasPrecedingVehicle =
    obj_0->VehicleMessageFieldDefInputVec[15];
  obj_0->VehicleMessageFieldDef.precedingVehicleId =
    obj_0->VehicleMessageFieldDefInputVec[16];
  obj_0->VehicleMessageFieldDef.precedingVehicleDistance =
    obj_0->VehicleMessageFieldDefInputVec[17];
  obj_0->VehicleMessageFieldDef.precedingVehicleSpeed =
    obj_0->VehicleMessageFieldDefInputVec[18];
  obj_0->VehicleMessageFieldDef.signalLightId =
    obj_0->VehicleMessageFieldDefInputVec[19];
  obj_0->VehicleMessageFieldDef.signalLightHeadId =
    obj_0->VehicleMessageFieldDefInputVec[20];
  obj_0->VehicleMessageFieldDef.signalLightDistance =
    obj_0->VehicleMessageFieldDefInputVec[21];
  obj_0->VehicleMessageFieldDef.signalLightColor =
    obj_0->VehicleMessageFieldDefInputVec[22];
  obj_0->VehicleMessageFieldDef.speedLimit =
    obj_0->VehicleMessageFieldDefInputVec[23];
  obj_0->VehicleMessageFieldDef.speedLimitNext =
    obj_0->VehicleMessageFieldDefInputVec[24];
  obj_0->VehicleMessageFieldDef.speedLimitChangeDistance =
    obj_0->VehicleMessageFieldDefInputVec[25];
  obj_0->VehicleMessageFieldDef.linkIdNext =
    obj_0->VehicleMessageFieldDefInputVec[26];
  obj_0->VehicleMessageFieldDef.grade = obj_0->VehicleMessageFieldDefInputVec[27];
  obj_0->VehicleMessageFieldDef.activeLaneChange =
    obj_0->VehicleMessageFieldDefInputVec[28];
}

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

static void RS_DS_CM11_SimulinkRS_RealSimDepack_stepImpl
  (RealSimDepack_RS_DS_CM11_SimulinkRS_T *obj, const uint8_T ByteData[1024],
   uint32_T nByte, real_T connectionState, real_T *simState, real_T *t, real_T
   *nVeh, VehDataBus *b_VehDataBus, real_T *isVehicleInNetwork)
{
  RealSimDepack_RS_DS_CM11_SimulinkRS_T *obj_0;
  real_T iByte;
  real_T iMsgSizeRecv;
  real_T iMsgTypeRecv;
  real_T msgProcessed;
  int32_T d;
  int32_T exitg1;
  int32_T i;
  int32_T strLen;
  real32_T y_0;
  uint32_T totalMsgSize;
  uint32_T y_1;
  uint16_T y_2;
  int8_T y_3;
  uint8_T ByteData_data[1024];
  uint8_T x_0[4];
  uint8_T x_1[2];
  uint8_T x;
  uint8_T y;
  boolean_T f;

  /*  INPUTS */
  /*    ByteData: bytes vector received from TrafficSimualtor */
  /*    nByte: size of bytes received */
  /*    connectionState:  */
  /*  */
  /*  OUTPUTS */
  /*    simState: simulation state. NOT actively used */
  /*    t: simulation time of TrafficSimulator */
  /*    nVeh: number of vehicle data received. */
  /*    VehDataBus: a vehicle data bus signal */
  /*    isVehicleInNetwork: indicator whether ego is in network */
  /*  %             try */
  /*  defualt values for outputs */
  *simState = obj->simStatePrevious;
  *t = obj->tPrevious;
  *nVeh = 0.0;

  /*  init values for other variables */
  totalMsgSize = 0U;

  /*  this is the total message size received from Real-Sim */
  msgProcessed = 0.0;

  /*  to record how many messages have been processed */
  *b_VehDataBus = obj->VehicleDataPrevious;

  /* --------------------------- */
  /*  read received buffer */
  /* --------------------------- */
  /*  need to check size, before connection, there could be no */
  /*  message at all              */
  if (nByte >= obj->msgHeaderSize) {
    x = ByteData[0];
    memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
    *simState = y;
    x_0[0] = ByteData[1];
    x_0[1] = ByteData[2];
    x_0[2] = ByteData[3];
    x_0[3] = ByteData[4];
    memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof(real32_T)));
    *t = y_0;
    x_0[0] = ByteData[5];
    x_0[1] = ByteData[6];
    x_0[2] = ByteData[7];
    x_0[3] = ByteData[8];
    memcpy((void *)&y_1, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof(uint32_T)));
    totalMsgSize = y_1;
    msgProcessed = obj->msgHeaderSize;
    obj->simStatePrevious = *simState;
    obj->tPrevious = *t;
  }

  /*  read actual message fields */
  do {
    exitg1 = 0;
    iMsgSizeRecv = msgProcessed;
    iMsgSizeRecv = rt_roundd_snf(iMsgSizeRecv);
    iMsgTypeRecv = totalMsgSize;
    if (iMsgSizeRecv < iMsgTypeRecv) {
      x_1[0] = ByteData[(int32_T)(msgProcessed + 1.0) - 1];
      x_1[1] = ByteData[(int32_T)(msgProcessed + 2.0) - 1];
      memcpy((void *)&y_2, (void *)&x_1[0], (uint32_T)((size_t)1 * sizeof
              (uint16_T)));
      iMsgSizeRecv = y_2;
      x = ByteData[(int32_T)(msgProcessed + 3.0) - 1];
      memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
      iMsgTypeRecv = y;

      /*  !!! for now, Simulink only supports vehicle data type */
      if (obj->enableDebug != 0.0) {
        obj->vehMsgIdentifer = iMsgTypeRecv;
      } else {
        obj->vehMsgIdentifer = 1.0;
      }

      if (iMsgTypeRecv == obj->vehMsgIdentifer) {
        iMsgTypeRecv = (msgProcessed + obj->msgEachHeaderSize) + 1.0;
        iByte = msgProcessed + iMsgSizeRecv;
        f = (iMsgTypeRecv > iByte);
        if (f) {
          i = 0;
          d = -1;
        } else {
          i = (int32_T)iMsgTypeRecv - 1;
          d = (int32_T)iByte - 1;
        }

        obj_0 = obj;
        d -= i;
        for (strLen = 0; strLen <= d; strLen++) {
          ByteData_data[strLen] = ByteData[i + strLen];
        }

        iByte = 1.0;
        if (obj_0->VehicleMessageFieldDef.id != 0.0) {
          x = ByteData_data[0];
          memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
          strLen = y;

          /*  initialize with size 50  */
          for (i = 0; i < 50; i++) {
            b_VehDataBus->id[i] = 0U;
          }

          iMsgTypeRecv = strLen;
          d = (int32_T)iMsgTypeRecv - 1;
          for (i = 0; i <= d; i++) {
            iMsgTypeRecv = (real_T)i + 1.0;

            /*                  str = [str, ByteData(iByte)]; */
            b_VehDataBus->id[(int32_T)iMsgTypeRecv - 1] = ByteData_data[i + 1];
          }

          iByte = ((real_T)d + 1.0) + 2.0;
          b_VehDataBus->idLength = strLen;
        }

        if (obj_0->VehicleMessageFieldDef.type != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
          strLen = y;
          iByte++;

          /*  initialize with size 50  */
          for (i = 0; i < 50; i++) {
            b_VehDataBus->type[i] = 0U;
          }

          iMsgTypeRecv = strLen;
          d = (int32_T)iMsgTypeRecv - 1;
          for (i = 0; i <= d; i++) {
            iMsgTypeRecv = (real_T)i + 1.0;

            /*                  str = [str, ByteData(iByte)]; */
            b_VehDataBus->type[(int32_T)iMsgTypeRecv - 1] = ByteData_data
              [((int32_T)iByte + i) - 1];
          }

          iByte += (real_T)d + 1.0;
          b_VehDataBus->typeLength = strLen;
        }

        if (obj_0->VehicleMessageFieldDef.vehicleClass != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
          strLen = y;
          iByte++;

          /*  initialize with size 50  */
          for (i = 0; i < 50; i++) {
            b_VehDataBus->vehicleClass[i] = 0U;
          }

          iMsgTypeRecv = strLen;
          d = (int32_T)iMsgTypeRecv - 1;
          for (i = 0; i <= d; i++) {
            iMsgTypeRecv = (real_T)i + 1.0;

            /*                  str = [str, ByteData(iByte)]; */
            b_VehDataBus->vehicleClass[(int32_T)iMsgTypeRecv - 1] =
              ByteData_data[((int32_T)iByte + i) - 1];
          }

          iByte += (real_T)d + 1.0;
          b_VehDataBus->vehicleClassLength = strLen;
        }

        if (obj_0->VehicleMessageFieldDef.speed != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->speed = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.acceleration != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->acceleration = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.positionX != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->positionX = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.positionY != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->positionY = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.positionZ != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->positionZ = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.heading != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->heading = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.color != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_1, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (uint32_T)));
          b_VehDataBus->color = y_1;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.linkId != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
          strLen = y;
          iByte++;

          /*  initialize with size 50  */
          for (i = 0; i < 50; i++) {
            b_VehDataBus->linkId[i] = 0U;
          }

          iMsgTypeRecv = strLen;
          d = (int32_T)iMsgTypeRecv - 1;
          for (i = 0; i <= d; i++) {
            iMsgTypeRecv = (real_T)i + 1.0;

            /*                  str = [str, ByteData(iByte)]; */
            b_VehDataBus->linkId[(int32_T)iMsgTypeRecv - 1] = ByteData_data
              [((int32_T)iByte + i) - 1];
          }

          iByte += (real_T)d + 1.0;
          b_VehDataBus->linkIdLength = strLen;
        }

        if (obj_0->VehicleMessageFieldDef.laneId != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&i, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (int32_T)));
          b_VehDataBus->laneId = i;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.distanceTravel != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->distanceTravel = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.speedDesired != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->speedDesired = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.accelerationDesired != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->accelerationDesired = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.hasPrecedingVehicle != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y_3, (void *)&x, (uint32_T)((size_t)1 * sizeof(int8_T)));
          b_VehDataBus->hasPrecedingVehicle = y_3;
          iByte++;
        }

        if (obj_0->VehicleMessageFieldDef.precedingVehicleId != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
          strLen = y;
          iByte++;

          /*  initialize with size 50  */
          for (i = 0; i < 50; i++) {
            b_VehDataBus->precedingVehicleId[i] = 0U;
          }

          iMsgTypeRecv = strLen;
          d = (int32_T)iMsgTypeRecv - 1;
          for (i = 0; i <= d; i++) {
            iMsgTypeRecv = (real_T)i + 1.0;

            /*                  str = [str, ByteData(iByte)]; */
            b_VehDataBus->precedingVehicleId[(int32_T)iMsgTypeRecv - 1] =
              ByteData_data[((int32_T)iByte + i) - 1];
          }

          iByte += (real_T)d + 1.0;
          b_VehDataBus->precedingVehicleIdLength = strLen;
        }

        if (obj_0->VehicleMessageFieldDef.precedingVehicleDistance != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->precedingVehicleDistance = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.precedingVehicleSpeed != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->precedingVehicleSpeed = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.signalLightId != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
          strLen = y;
          iByte++;

          /*  initialize with size 50  */
          for (i = 0; i < 50; i++) {
            b_VehDataBus->signalLightId[i] = 0U;
          }

          iMsgTypeRecv = strLen;
          d = (int32_T)iMsgTypeRecv - 1;
          for (i = 0; i <= d; i++) {
            iMsgTypeRecv = (real_T)i + 1.0;

            /*                  str = [str, ByteData(iByte)]; */
            b_VehDataBus->signalLightId[(int32_T)iMsgTypeRecv - 1] =
              ByteData_data[((int32_T)iByte + i) - 1];
          }

          iByte += (real_T)d + 1.0;
          b_VehDataBus->signalLightIdLength = strLen;
        }

        if (obj_0->VehicleMessageFieldDef.signalLightHeadId != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&i, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (int32_T)));
          b_VehDataBus->signalLightHeadId = i;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.signalLightDistance != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->signalLightDistance = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.signalLightColor != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y_3, (void *)&x, (uint32_T)((size_t)1 * sizeof(int8_T)));
          b_VehDataBus->signalLightColor = y_3;
          iByte++;
        }

        if (obj_0->VehicleMessageFieldDef.speedLimit != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->speedLimit = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.speedLimitNext != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->speedLimitNext = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.speedLimitChangeDistance != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->speedLimitChangeDistance = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.linkIdNext != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
          strLen = y;
          iByte++;

          /*  initialize with size 50  */
          for (i = 0; i < 50; i++) {
            b_VehDataBus->linkIdNext[i] = 0U;
          }

          iMsgTypeRecv = strLen;
          d = (int32_T)iMsgTypeRecv - 1;
          for (i = 0; i <= d; i++) {
            iMsgTypeRecv = (real_T)i + 1.0;

            /*                  str = [str, ByteData(iByte)]; */
            b_VehDataBus->linkIdNext[(int32_T)iMsgTypeRecv - 1] = ByteData_data
              [((int32_T)iByte + i) - 1];
          }

          iByte += (real_T)d + 1.0;

          /* str(1:strLen); */
          b_VehDataBus->linkIdNextLength = strLen;
        }

        if (obj_0->VehicleMessageFieldDef.grade != 0.0) {
          x_0[0] = ByteData_data[(int32_T)iByte - 1];
          x_0[1] = ByteData_data[(int32_T)iByte];
          x_0[2] = ByteData_data[(int32_T)iByte + 1];
          x_0[3] = ByteData_data[(int32_T)iByte + 2];
          memcpy((void *)&y_0, (void *)&x_0[0], (uint32_T)((size_t)1 * sizeof
                  (real32_T)));
          b_VehDataBus->grade = y_0;
          iByte += 4.0;
        }

        if (obj_0->VehicleMessageFieldDef.activeLaneChange != 0.0) {
          x = ByteData_data[(int32_T)iByte - 1];
          memcpy((void *)&y_3, (void *)&x, (uint32_T)((size_t)1 * sizeof(int8_T)));
          b_VehDataBus->activeLaneChange = y_3;
        }

        (*nVeh)++;
        obj->VehicleDataPrevious = *b_VehDataBus;
        obj->isVehicleInNetworkPrevious = 1.0;
      } else {
        /*  not supported yet!!! */
      }

      msgProcessed += iMsgSizeRecv;
    } else {
      exitg1 = 1;
    }
  } while (exitg1 == 0);

  msgProcessed = connectionState - 3.0;
  msgProcessed = fabs(msgProcessed);
  if (msgProcessed > 1.0E-5) {
    /*  if not connected, then set to 0 */
    obj->simStatePrevious = 0.0;
    obj->tPrevious = 0.0;
    obj->VehicleDataPrevious = obj->VehicleDataEmpty;
    obj->isVehicleInNetworkPrevious = 0.0;
  } else if (totalMsgSize == 9U) {
    obj->isVehicleInNetworkPrevious = 0.0;
  }

  *isVehicleInNetwork = obj->isVehicleInNetworkPrevious;

  /*  %             catch */
  /*  %                 printf('ERROR: RealSimDepack'); */
  /*  %                 return */
  /*  %             end */
}

/* Function for MATLAB Function: '<S7>/MATLAB Function' */
static void RS_DS_CM11_SimulinkRS_copysign(char_T s1_data[], int32_T *idx, const
  char_T s_data[], int32_T *k, int32_T n, boolean_T *foundsign, boolean_T
  *success)
{
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    if (s_data[*k - 1] == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (s_data[*k - 1] == ',') {
      (*k)++;
    } else if (s_data[*k - 1] == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(uint8_T)s_data[*k - 1] & 127]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1_data[*idx - 2] == '-')) {
      s1_data[*idx - 2] = ' ';
    } else {
      s1_data[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S7>/MATLAB Function' */
static boolean_T RS_DS_CM11_SimulinkRS_isUnitImag(const char_T s_data[], int32_T
  k, int32_T n)
{
  int32_T b_k;
  char_T c_idx_0;
  char_T c_idx_1;
  char_T c_idx_2;
  boolean_T p;
  p = false;
  if (k <= n) {
    if (s_data[k - 1] == 'j') {
      p = true;
    } else if (s_data[k - 1] == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        c_idx_0 = '\x00';
        while ((b_k <= n) && (s_data[b_k - 1] == ',')) {
          b_k++;
        }

        if (b_k <= n) {
          c_idx_0 = s_data[b_k - 1];
        }

        b_k++;
        c_idx_1 = '\x00';
        while ((b_k <= n) && (s_data[b_k - 1] == ',')) {
          b_k++;
        }

        if (b_k <= n) {
          c_idx_1 = s_data[b_k - 1];
        }

        b_k++;
        c_idx_2 = '\x00';
        while ((b_k <= n) && (s_data[b_k - 1] == ',')) {
          b_k++;
        }

        if (b_k <= n) {
          c_idx_2 = s_data[b_k - 1];
        }

        if ((((c_idx_0 == 'I') || (c_idx_0 == 'i')) && ((c_idx_1 == 'N') ||
              (c_idx_1 == 'n')) && ((c_idx_2 == 'F') || (c_idx_2 == 'f'))) ||
            (((c_idx_0 == 'N') || (c_idx_0 == 'n')) && ((c_idx_1 == 'A') ||
              (c_idx_1 == 'a')) && ((c_idx_2 == 'N') || (c_idx_2 == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S7>/MATLAB Function' */
static void RS_DS_CM11_SimulinkRS_readNonFinite(const char_T s_data[], int32_T
  *k, int32_T n, boolean_T *b_finite, real_T *fv)
{
  int32_T ksaved;
  char_T c_idx_0;
  char_T c_idx_1;
  char_T c_idx_2;
  ksaved = *k;
  c_idx_0 = '\x00';
  while ((*k <= n) && (s_data[*k - 1] == ',')) {
    (*k)++;
  }

  if (*k <= n) {
    c_idx_0 = s_data[*k - 1];
  }

  (*k)++;
  c_idx_1 = '\x00';
  while ((*k <= n) && (s_data[*k - 1] == ',')) {
    (*k)++;
  }

  if (*k <= n) {
    c_idx_1 = s_data[*k - 1];
  }

  (*k)++;
  c_idx_2 = '\x00';
  while ((*k <= n) && (s_data[*k - 1] == ',')) {
    (*k)++;
  }

  if (*k <= n) {
    c_idx_2 = s_data[*k - 1];
  }

  (*k)++;
  if (((c_idx_0 == 'I') || (c_idx_0 == 'i')) && ((c_idx_1 == 'N') || (c_idx_1 ==
        'n')) && ((c_idx_2 == 'F') || (c_idx_2 == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c_idx_0 == 'N') || (c_idx_0 == 'n')) && ((c_idx_1 == 'A') ||
              (c_idx_1 == 'a')) && ((c_idx_2 == 'N') || (c_idx_2 == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S7>/MATLAB Function' */
static boolean_T RS_DS_CM11_SimulinkRS_copydigits(char_T s1_data[], int32_T *idx,
  const char_T s_data[], int32_T *k, int32_T n, boolean_T allowpoint)
{
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    if ((s_data[*k - 1] >= '0') && (s_data[*k - 1] <= '9')) {
      s1_data[*idx - 1] = s_data[*k - 1];
      (*idx)++;
      (*k)++;
    } else if (s_data[*k - 1] == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1_data[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (s_data[*k - 1] == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S7>/MATLAB Function' */
static boolean_T RS_DS_CM11_SimulinkRS_copyexponent(char_T s1_data[], int32_T
  *idx, const char_T s_data[], int32_T *k, int32_T n)
{
  int32_T b_k;
  int32_T kexp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if ((*k <= n) && ((s_data[*k - 1] == 'E') || (s_data[*k - 1] == 'e'))) {
    s1_data[*idx - 1] = 'e';
    (*idx)++;
    (*k)++;
    while ((*k <= n) && (s_data[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      if (s_data[*k - 1] == '-') {
        s1_data[*idx - 1] = '-';
        (*idx)++;
        (*k)++;
      } else if (s_data[*k - 1] == '+') {
        (*k)++;
      }
    }

    kexp = *k;
    b_k = *k;
    b_success = RS_DS_CM11_SimulinkRS_copydigits(s1_data, idx, s_data, &b_k, n,
      false);
    *k = b_k;
    if ((!b_success) || (b_k <= kexp)) {
      success = false;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S7>/MATLAB Function' */
static void RS_DS_CM11_SimulinkRS_readfloat(char_T s1_data[], int32_T *idx,
  const char_T s_data[], int32_T *k, int32_T n, boolean_T *isimag, boolean_T
  *b_finite, real_T *nfv, boolean_T *foundsign, boolean_T *success)
{
  int32_T b_idx;
  int32_T b_k;
  boolean_T a__3;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  RS_DS_CM11_SimulinkRS_copysign(s1_data, &b_idx, s_data, &b_k, n, foundsign,
    success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (RS_DS_CM11_SimulinkRS_isUnitImag(s_data, b_k, n)) {
      *isimag = true;
      *k = b_k + 1;
      while ((*k <= n) && (b[(uint8_T)s_data[*k - 1] & 127] || (s_data[*k - 1] ==
               '\x00') || (s_data[*k - 1] == ','))) {
        (*k)++;
      }

      if ((*k <= n) && (s_data[*k - 1] == '*')) {
        b_k = *k + 1;
        RS_DS_CM11_SimulinkRS_copysign(s1_data, &b_idx, s_data, &b_k, n, &a__3,
          success);
        *idx = b_idx;
        if (*success) {
          if (RS_DS_CM11_SimulinkRS_isUnitImag(s_data, b_k, n)) {
            *success = false;
          } else {
            RS_DS_CM11_SimulinkRS_readNonFinite(s_data, &b_k, n, b_finite, nfv);
            if (*b_finite) {
              *success = RS_DS_CM11_SimulinkRS_copydigits(s1_data, &b_idx,
                s_data, &b_k, n, true);
              *idx = b_idx;
              if (*success) {
                *success = RS_DS_CM11_SimulinkRS_copyexponent(s1_data, idx,
                  s_data, &b_k, n);
              }
            } else if ((b_idx >= 2) && (s1_data[b_idx - 2] == '-')) {
              *idx = b_idx - 1;
              s1_data[b_idx - 2] = ' ';
              *nfv = -*nfv;
            }

            while ((b_k <= n) && (b[(uint8_T)s_data[b_k - 1] & 127] ||
                                  (s_data[b_k - 1] == '\x00') || (s_data[b_k - 1]
                     == ','))) {
              b_k++;
            }

            if ((b_k <= n) && (s_data[b_k - 1] == '*')) {
              b_k++;
              while ((b_k <= n) && (b[(uint8_T)s_data[b_k - 1] & 127] ||
                                    (s_data[b_k - 1] == '\x00') || (s_data[b_k -
                       1] == ','))) {
                b_k++;
              }
            }

            if ((b_k <= n) && ((s_data[b_k - 1] == 'i') || (s_data[b_k - 1] ==
                  'j'))) {
              b_k++;
            }
          }

          while ((b_k <= n) && (b[(uint8_T)s_data[b_k - 1] & 127] || (s_data[b_k
                   - 1] == '\x00') || (s_data[b_k - 1] == ','))) {
            b_k++;
          }
        }

        *k = b_k;
      } else {
        s1_data[b_idx - 1] = '1';
        *idx = b_idx + 1;
      }
    } else {
      RS_DS_CM11_SimulinkRS_readNonFinite(s_data, &b_k, n, b_finite, nfv);
      *k = b_k;
      if (*b_finite) {
        *success = RS_DS_CM11_SimulinkRS_copydigits(s1_data, &b_idx, s_data,
          &b_k, n, true);
        *idx = b_idx;
        *k = b_k;
        if (*success) {
          *success = RS_DS_CM11_SimulinkRS_copyexponent(s1_data, idx, s_data, k,
            n);
        }
      } else if ((b_idx >= 2) && (s1_data[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1_data[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      while ((*k <= n) && (b[(uint8_T)s_data[*k - 1] & 127] || (s_data[*k - 1] ==
               '\x00') || (s_data[*k - 1] == ','))) {
        (*k)++;
      }

      if ((*k <= n) && (s_data[*k - 1] == '*')) {
        (*k)++;
        while ((*k <= n) && (b[(uint8_T)s_data[*k - 1] & 127] || (s_data[*k - 1]
                 == '\x00') || (s_data[*k - 1] == ','))) {
          (*k)++;
        }
      }

      if ((*k <= n) && ((s_data[*k - 1] == 'i') || (s_data[*k - 1] == 'j'))) {
        (*k)++;
        *isimag = true;
      }
    }

    while ((*k <= n) && (b[(uint8_T)s_data[*k - 1] & 127] || (s_data[*k - 1] ==
             '\x00') || (s_data[*k - 1] == ','))) {
      (*k)++;
    }
  }
}

static real_T RS_DS_CM11_SimulinkRS_RealSimMsgClass_packString_ilb(uint8_T
  ByteData[200], real_T iByte, const uint8_T VehData_precedingVehicleId[50],
  real_T VehData_precedingVehicleIdLength)
{
  real_T b_iByte;
  real_T c;
  int32_T b;
  int32_T e;
  int32_T f_size_idx_1;
  uint8_T f_data[50];
  uint8_T tempUint8;
  uint8_T x;
  boolean_T h;
  b_iByte = iByte;

  /*  parser string length */
  c = rt_roundd_snf(VehData_precedingVehicleIdLength);
  if (c < 256.0) {
    if (c >= 0.0) {
      x = (uint8_T)c;
    } else {
      x = 0U;
    }
  } else {
    x = MAX_uint8_T;
  }

  memcpy((void *)&tempUint8, (void *)&x, (uint32_T)((size_t)1 * sizeof(uint8_T)));
  ByteData[(int32_T)b_iByte - 1] = tempUint8;
  b_iByte++;

  /*  parser string */
  h = (1.0 > VehData_precedingVehicleIdLength);
  if (h) {
    b = -1;
  } else {
    b = (int32_T)VehData_precedingVehicleIdLength - 1;
  }

  c = (b_iByte + VehData_precedingVehicleIdLength) - 1.0;
  h = (b_iByte > c);
  if (h) {
    e = 0;
  } else {
    e = (int32_T)b_iByte - 1;
  }

  f_size_idx_1 = b + 1;
  if (0 <= b) {
    memcpy(&f_data[0], &VehData_precedingVehicleId[0], (b + 1) * sizeof(uint8_T));
  }

  b = f_size_idx_1;
  for (f_size_idx_1 = 0; f_size_idx_1 < b; f_size_idx_1++) {
    ByteData[e + f_size_idx_1] = f_data[f_size_idx_1];
  }

  b_iByte += VehData_precedingVehicleIdLength;
  return b_iByte;
}

static void RS_DS_CM11_SimulinkRS_RealSimPack_stepImpl
  (RealSimPack_RS_DS_CM11_SimulinkRS_T *obj, real_T simState, real_T t, const
   VehDataBus *VehDataBus_0, uint8_T ByteData[200], real_T *nMsgSize)
{
  RealSimPack_RS_DS_CM11_SimulinkRS_T *obj_0;
  real_T VehData_acceleration;
  real_T VehData_accelerationDesired;
  real_T VehData_activeLaneChange;
  real_T VehData_color;
  real_T VehData_distanceTravel;
  real_T VehData_grade;
  real_T VehData_hasPrecedingVehicle;
  real_T VehData_heading;
  real_T VehData_laneId;
  real_T VehData_linkIdNextLength;
  real_T VehData_positionX;
  real_T VehData_positionY;
  real_T VehData_positionZ;
  real_T VehData_precedingVehicleDistance;
  real_T VehData_precedingVehicleIdLength;
  real_T VehData_precedingVehicleSpeed;
  real_T VehData_signalLightColor;
  real_T VehData_signalLightDistance;
  real_T VehData_signalLightHeadId;
  real_T VehData_signalLightIdLength;
  real_T VehData_speed;
  real_T VehData_speedDesired;
  real_T VehData_speedLimit;
  real_T VehData_speedLimitChangeDistance;
  real_T VehData_speedLimitNext;
  real_T c;
  real_T nVehMsgSize;
  real_T strLen;
  real_T strLen_0;
  real_T strLen_1;
  real_T strLen_2;
  int32_T b;
  int32_T e;
  int32_T i;
  real32_T x;
  uint32_T x_0;
  uint16_T x_1;
  int8_T x_2;
  uint8_T VehData_id[50];
  uint8_T f_data[50];
  uint8_T tempFloat[4];
  uint8_T tempUint16[2];
  uint8_T tempInt8;
  uint8_T tempUint8;
  boolean_T h;

  /*  INPUTS */
  /*    simState: always set to 1. NOT actively used. */
  /*    t: simulation time */
  /*    speedDesired: desired speed in m/s */
  /*    accelerationDesired: desired acceleration in m/s^2 */
  /*  */
  /*    NOTE: if select both speedDesired and accelerationDesired */
  /*    in config.yaml, then only accelerationDesired will be used */
  /*    to control vehicles in TrafficSimulator. if only one of */
  /*    speedDesired and accelerationDesired selected in */
  /*    config.yaml, then only the one selected will be used for */
  /*    control, the other input can be leave as 0. */
  /*  */
  /*  OUTPUTS */
  /*    ByteData: bytes vector that will be sent to TrafficSimualtor */
  /*    nMsgSize: size of bytes to be sent */
  /*  %             try */
  /*  initialize ByteData */
  /*  parser ByteData */
  obj_0 = obj;
  strLen = VehDataBus_0->idLength;
  strLen_0 = VehDataBus_0->typeLength;
  strLen_1 = VehDataBus_0->vehicleClassLength;
  VehData_speed = VehDataBus_0->speed;
  VehData_acceleration = VehDataBus_0->acceleration;
  VehData_positionX = VehDataBus_0->positionX;
  VehData_positionY = VehDataBus_0->positionY;
  VehData_positionZ = VehDataBus_0->positionZ;
  VehData_heading = VehDataBus_0->heading;
  VehData_color = VehDataBus_0->color;
  strLen_2 = VehDataBus_0->linkIdLength;
  VehData_laneId = VehDataBus_0->laneId;
  VehData_distanceTravel = VehDataBus_0->distanceTravel;
  VehData_speedDesired = VehDataBus_0->speedDesired;
  VehData_accelerationDesired = VehDataBus_0->accelerationDesired;
  VehData_hasPrecedingVehicle = VehDataBus_0->hasPrecedingVehicle;
  VehData_precedingVehicleIdLength = VehDataBus_0->precedingVehicleIdLength;
  VehData_precedingVehicleDistance = VehDataBus_0->precedingVehicleDistance;
  VehData_precedingVehicleSpeed = VehDataBus_0->precedingVehicleSpeed;
  VehData_signalLightIdLength = VehDataBus_0->signalLightIdLength;
  VehData_signalLightHeadId = VehDataBus_0->signalLightHeadId;
  VehData_signalLightDistance = VehDataBus_0->signalLightDistance;
  VehData_signalLightColor = VehDataBus_0->signalLightColor;
  VehData_speedLimit = VehDataBus_0->speedLimit;
  VehData_speedLimitNext = VehDataBus_0->speedLimitNext;
  VehData_speedLimitChangeDistance = VehDataBus_0->speedLimitChangeDistance;
  VehData_linkIdNextLength = VehDataBus_0->linkIdNextLength;
  VehData_grade = VehDataBus_0->grade;
  VehData_activeLaneChange = VehDataBus_0->activeLaneChange;
  memset(&ByteData[0], 0, 200U * sizeof(uint8_T));
  *nMsgSize = (((((((((((((((((((((((((((((strLen + 1.0) *
    obj_0->VehicleMessageFieldDef.id + (obj_0->msgHeaderSize +
    obj_0->msgEachHeaderSize)) + (strLen_0 + 1.0) *
    obj_0->VehicleMessageFieldDef.type) + (strLen_1 + 1.0) *
    obj_0->VehicleMessageFieldDef.vehicleClass) +
    obj_0->VehicleMessageFieldDef.speed * 4.0) +
    obj_0->VehicleMessageFieldDef.acceleration * 4.0) +
    obj_0->VehicleMessageFieldDef.positionX * 4.0) +
    obj_0->VehicleMessageFieldDef.positionY * 4.0) +
    obj_0->VehicleMessageFieldDef.positionZ * 4.0) +
    obj_0->VehicleMessageFieldDef.heading * 4.0) +
    obj_0->VehicleMessageFieldDef.color * 4.0) + (strLen_2 + 1.0) *
    obj_0->VehicleMessageFieldDef.linkId) + obj_0->VehicleMessageFieldDef.laneId
    * 4.0) + obj_0->VehicleMessageFieldDef.distanceTravel * 4.0) +
    obj_0->VehicleMessageFieldDef.speedDesired * 4.0) +
    obj_0->VehicleMessageFieldDef.accelerationDesired * 4.0) +
    obj_0->VehicleMessageFieldDef.hasPrecedingVehicle) +
    (VehData_precedingVehicleIdLength + 1.0) *
    obj_0->VehicleMessageFieldDef.precedingVehicleId) +
    obj_0->VehicleMessageFieldDef.precedingVehicleDistance * 4.0) +
                        obj_0->VehicleMessageFieldDef.precedingVehicleSpeed *
                        4.0) + (VehData_signalLightIdLength + 1.0) *
                       obj_0->VehicleMessageFieldDef.signalLightId) +
                      obj_0->VehicleMessageFieldDef.signalLightHeadId * 4.0) +
                     obj_0->VehicleMessageFieldDef.signalLightDistance * 4.0) +
                    obj_0->VehicleMessageFieldDef.signalLightColor) +
                   obj_0->VehicleMessageFieldDef.speedLimit * 4.0) +
                  obj_0->VehicleMessageFieldDef.speedLimitNext * 4.0) +
                 obj_0->VehicleMessageFieldDef.speedLimitChangeDistance * 4.0) +
                (VehData_linkIdNextLength + 1.0) *
                obj_0->VehicleMessageFieldDef.linkIdNext) +
               obj_0->VehicleMessageFieldDef.grade * 4.0) +
    obj_0->VehicleMessageFieldDef.activeLaneChange;
  *nMsgSize = rt_roundd_snf(*nMsgSize);

  /*  % speed */
  /*  % acceleration */
  /*  % positionX */
  /*  % positionY */
  /*  % positionZ */
  /*  % heading */
  /*  % color */
  /*  % linkId */
  /*  % laneId */
  /*  % distanceTravel */
  /*  % speedDesired */
  /*  % accelerationDesired */
  /*  % hasPrecedingVehicle */
  /*  % precedingVehicleId */
  /*  % precedingVehicleDistance */
  /*  % precedingVehicleSpeed */
  /*  % signalLightId */
  /*  % signalLightHeadId */
  /*  % signalLightDistance */
  /*  % signalLightColor */
  /*  % speedLimit */
  /*  % speedLimitNext */
  /*  % speedLimitChangeDistance */
  /*  % linkIdNext */
  /*  % grade */
  /*  % activeLaneChange */
  nVehMsgSize = *nMsgSize - obj_0->msgHeaderSize;
  nVehMsgSize = rt_roundd_snf(nVehMsgSize);
  c = rt_roundd_snf(simState);
  if (c < 256.0) {
    if (c >= 0.0) {
      tempInt8 = (uint8_T)c;
    } else {
      tempInt8 = 0U;
    }
  } else {
    tempInt8 = MAX_uint8_T;
  }

  memcpy((void *)&tempUint8, (void *)&tempInt8, (uint32_T)((size_t)1 * sizeof
          (uint8_T)));
  ByteData[0] = tempUint8;
  x = (real32_T)t;
  memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
          (uint8_T)));
  ByteData[1] = tempFloat[0];
  ByteData[2] = tempFloat[1];
  ByteData[3] = tempFloat[2];
  ByteData[4] = tempFloat[3];
  if (*nMsgSize < 4.294967296E+9) {
    if (*nMsgSize >= 0.0) {
      x_0 = (uint32_T)*nMsgSize;
    } else {
      x_0 = 0U;
    }
  } else {
    x_0 = MAX_uint32_T;
  }

  memcpy((void *)&tempFloat[0], (void *)&x_0, (uint32_T)((size_t)4 * sizeof
          (uint8_T)));
  ByteData[5] = tempFloat[0];
  ByteData[6] = tempFloat[1];
  ByteData[7] = tempFloat[2];
  ByteData[8] = tempFloat[3];

  /*  message length */
  if (nVehMsgSize < 65536.0) {
    if (nVehMsgSize >= 0.0) {
      x_1 = (uint16_T)nVehMsgSize;
    } else {
      x_1 = 0U;
    }
  } else {
    x_1 = MAX_uint16_T;
  }

  memcpy((void *)&tempUint16[0], (void *)&x_1, (uint32_T)((size_t)2 * sizeof
          (uint8_T)));
  ByteData[9] = tempUint16[0];
  ByteData[10] = tempUint16[1];

  /*  message type */
  tempInt8 = 1U;
  memcpy((void *)&tempUint8, (void *)&tempInt8, (uint32_T)((size_t)1 * sizeof
          (uint8_T)));
  ByteData[11] = tempUint8;
  nVehMsgSize = 13.0;
  if (obj_0->VehicleMessageFieldDef.id != 0.0) {
    for (i = 0; i < 50; i++) {
      VehData_id[i] = VehDataBus_0->id[i];
    }

    /*  parser string length */
    c = rt_roundd_snf(strLen);
    if (c < 256.0) {
      if (c >= 0.0) {
        tempInt8 = (uint8_T)c;
      } else {
        tempInt8 = 0U;
      }
    } else {
      tempInt8 = MAX_uint8_T;
    }

    memcpy((void *)&tempUint8, (void *)&tempInt8, (uint32_T)((size_t)1 * sizeof
            (uint8_T)));
    ByteData[12] = tempUint8;

    /*  parser string */
    h = (1.0 > strLen);
    if (h) {
      b = -1;
    } else {
      b = (int32_T)strLen - 1;
    }

    c = (strLen + 14.0) - 1.0;
    h = (14.0 > c);
    if (h) {
      e = 0;
    } else {
      e = 13;
    }

    i = b + 1;
    if (0 <= b) {
      memcpy(&f_data[0], &VehData_id[0], (b + 1) * sizeof(uint8_T));
    }

    b = i;
    for (i = 0; i < b; i++) {
      ByteData[e + i] = f_data[i];
    }

    nVehMsgSize = strLen + 14.0;
  }

  if (obj_0->VehicleMessageFieldDef.type != 0.0) {
    for (i = 0; i < 50; i++) {
      VehData_id[i] = VehDataBus_0->type[i];
    }

    /*  parser string length */
    c = rt_roundd_snf(strLen_0);
    if (c < 256.0) {
      if (c >= 0.0) {
        tempInt8 = (uint8_T)c;
      } else {
        tempInt8 = 0U;
      }
    } else {
      tempInt8 = MAX_uint8_T;
    }

    memcpy((void *)&tempUint8, (void *)&tempInt8, (uint32_T)((size_t)1 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempUint8;
    nVehMsgSize++;

    /*  parser string */
    h = (1.0 > strLen_0);
    if (h) {
      b = -1;
    } else {
      b = (int32_T)strLen_0 - 1;
    }

    c = (nVehMsgSize + strLen_0) - 1.0;
    h = (nVehMsgSize > c);
    if (h) {
      e = 0;
    } else {
      e = (int32_T)nVehMsgSize - 1;
    }

    i = b + 1;
    if (0 <= b) {
      memcpy(&f_data[0], &VehData_id[0], (b + 1) * sizeof(uint8_T));
    }

    b = i;
    for (i = 0; i < b; i++) {
      ByteData[e + i] = f_data[i];
    }

    nVehMsgSize += strLen_0;
  }

  if (obj_0->VehicleMessageFieldDef.vehicleClass != 0.0) {
    for (i = 0; i < 50; i++) {
      VehData_id[i] = VehDataBus_0->vehicleClass[i];
    }

    /*  parser string length */
    c = rt_roundd_snf(strLen_1);
    if (c < 256.0) {
      if (c >= 0.0) {
        tempInt8 = (uint8_T)c;
      } else {
        tempInt8 = 0U;
      }
    } else {
      tempInt8 = MAX_uint8_T;
    }

    memcpy((void *)&tempUint8, (void *)&tempInt8, (uint32_T)((size_t)1 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempUint8;
    nVehMsgSize++;

    /*  parser string */
    h = (1.0 > strLen_1);
    if (h) {
      b = -1;
    } else {
      b = (int32_T)strLen_1 - 1;
    }

    c = (nVehMsgSize + strLen_1) - 1.0;
    h = (nVehMsgSize > c);
    if (h) {
      e = 0;
    } else {
      e = (int32_T)nVehMsgSize - 1;
    }

    i = b + 1;
    if (0 <= b) {
      memcpy(&f_data[0], &VehData_id[0], (b + 1) * sizeof(uint8_T));
    }

    b = i;
    for (i = 0; i < b; i++) {
      ByteData[e + i] = f_data[i];
    }

    nVehMsgSize += strLen_1;
  }

  if (obj_0->VehicleMessageFieldDef.speed != 0.0) {
    /*  speed */
    x = (real32_T)VehData_speed;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.acceleration != 0.0) {
    /*  acceleration */
    x = (real32_T)VehData_acceleration;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.positionX != 0.0) {
    /*  position X */
    x = (real32_T)VehData_positionX;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.positionY != 0.0) {
    /*  position Y */
    x = (real32_T)VehData_positionY;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.positionZ != 0.0) {
    /*  position Z */
    x = (real32_T)VehData_positionZ;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.heading != 0.0) {
    /*  heading */
    x = (real32_T)VehData_heading;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.color != 0.0) {
    /*  color */
    c = rt_roundd_snf(VehData_color);
    if (c < 4.294967296E+9) {
      if (c >= 0.0) {
        x_0 = (uint32_T)c;
      } else {
        x_0 = 0U;
      }
    } else {
      x_0 = MAX_uint32_T;
    }

    memcpy((void *)&tempFloat[0], (void *)&x_0, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.linkId != 0.0) {
    for (i = 0; i < 50; i++) {
      VehData_id[i] = VehDataBus_0->linkId[i];
    }

    /*  parser string length */
    c = rt_roundd_snf(strLen_2);
    if (c < 256.0) {
      if (c >= 0.0) {
        tempInt8 = (uint8_T)c;
      } else {
        tempInt8 = 0U;
      }
    } else {
      tempInt8 = MAX_uint8_T;
    }

    memcpy((void *)&tempUint8, (void *)&tempInt8, (uint32_T)((size_t)1 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempUint8;
    nVehMsgSize++;

    /*  parser string */
    h = (1.0 > strLen_2);
    if (h) {
      b = -1;
    } else {
      b = (int32_T)strLen_2 - 1;
    }

    c = (nVehMsgSize + strLen_2) - 1.0;
    h = (nVehMsgSize > c);
    if (h) {
      e = 0;
    } else {
      e = (int32_T)nVehMsgSize - 1;
    }

    i = b + 1;
    if (0 <= b) {
      memcpy(&f_data[0], &VehData_id[0], (b + 1) * sizeof(uint8_T));
    }

    b = i;
    for (i = 0; i < b; i++) {
      ByteData[e + i] = f_data[i];
    }

    nVehMsgSize += strLen_2;
  }

  if (obj_0->VehicleMessageFieldDef.laneId != 0.0) {
    /*  lane id */
    c = rt_roundd_snf(VehData_laneId);
    if (c < 2.147483648E+9) {
      if (c >= -2.147483648E+9) {
        b = (int32_T)c;
      } else {
        b = MIN_int32_T;
      }
    } else {
      b = MAX_int32_T;
    }

    memcpy((void *)&tempFloat[0], (void *)&b, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.distanceTravel != 0.0) {
    /*  distance travel */
    x = (real32_T)VehData_distanceTravel;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.speedDesired != 0.0) {
    /*  speedDesired */
    x = (real32_T)VehData_speedDesired;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.accelerationDesired != 0.0) {
    /*  accelerationDesired */
    x = (real32_T)VehData_accelerationDesired;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.hasPrecedingVehicle != 0.0) {
    /*  hasPrecedingVehicle */
    c = rt_roundd_snf(VehData_hasPrecedingVehicle);
    if (c < 128.0) {
      if (c >= -128.0) {
        x_2 = (int8_T)c;
      } else {
        x_2 = MIN_int8_T;
      }
    } else {
      x_2 = MAX_int8_T;
    }

    memcpy((void *)&tempInt8, (void *)&x_2, (uint32_T)((size_t)1 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempInt8;
    nVehMsgSize++;
  }

  if (obj_0->VehicleMessageFieldDef.precedingVehicleId != 0.0) {
    VehData_speed = RS_DS_CM11_SimulinkRS_RealSimMsgClass_packString_ilb
      (ByteData, nVehMsgSize, VehDataBus_0->precedingVehicleId,
       VehData_precedingVehicleIdLength);
    nVehMsgSize = VehData_speed;
  }

  if (obj_0->VehicleMessageFieldDef.precedingVehicleDistance != 0.0) {
    /*  precedingVehicleDistance */
    x = (real32_T)VehData_precedingVehicleDistance;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.precedingVehicleSpeed != 0.0) {
    /*  precedingVehicleSpeed */
    x = (real32_T)VehData_precedingVehicleSpeed;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.signalLightId != 0.0) {
    VehData_precedingVehicleIdLength =
      RS_DS_CM11_SimulinkRS_RealSimMsgClass_packString_ilb(ByteData, nVehMsgSize,
      VehDataBus_0->signalLightId, VehData_signalLightIdLength);
    nVehMsgSize = VehData_precedingVehicleIdLength;
  }

  if (obj_0->VehicleMessageFieldDef.signalLightHeadId != 0.0) {
    /*  signalLightHeadId */
    c = rt_roundd_snf(VehData_signalLightHeadId);
    if (c < 2.147483648E+9) {
      if (c >= -2.147483648E+9) {
        b = (int32_T)c;
      } else {
        b = MIN_int32_T;
      }
    } else {
      b = MAX_int32_T;
    }

    memcpy((void *)&tempFloat[0], (void *)&b, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.signalLightDistance != 0.0) {
    /*  signalLightDistance */
    x = (real32_T)VehData_signalLightDistance;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.signalLightColor != 0.0) {
    /*  signalLightColor */
    c = rt_roundd_snf(VehData_signalLightColor);
    if (c < 128.0) {
      if (c >= -128.0) {
        x_2 = (int8_T)c;
      } else {
        x_2 = MIN_int8_T;
      }
    } else {
      x_2 = MAX_int8_T;
    }

    memcpy((void *)&tempInt8, (void *)&x_2, (uint32_T)((size_t)1 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempInt8;
    nVehMsgSize++;
  }

  if (obj_0->VehicleMessageFieldDef.speedLimit != 0.0) {
    /*  speedLimit */
    x = (real32_T)VehData_speedLimit;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.speedLimitNext != 0.0) {
    /*  speedLimitNext */
    x = (real32_T)VehData_speedLimitNext;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.speedLimitChangeDistance != 0.0) {
    /*  speedLimitChangeDistance */
    x = (real32_T)VehData_speedLimitChangeDistance;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.linkIdNext != 0.0) {
    VehData_signalLightIdLength =
      RS_DS_CM11_SimulinkRS_RealSimMsgClass_packString_ilb(ByteData, nVehMsgSize,
      VehDataBus_0->linkIdNext, VehData_linkIdNextLength);
    nVehMsgSize = VehData_signalLightIdLength;
  }

  if (obj_0->VehicleMessageFieldDef.grade != 0.0) {
    /*  grade */
    x = (real32_T)VehData_grade;
    memcpy((void *)&tempFloat[0], (void *)&x, (uint32_T)((size_t)4 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempFloat[0];
    ByteData[(int32_T)(nVehMsgSize + 1.0) - 1] = tempFloat[1];
    ByteData[(int32_T)(nVehMsgSize + 2.0) - 1] = tempFloat[2];
    ByteData[(int32_T)(nVehMsgSize + 3.0) - 1] = tempFloat[3];
    nVehMsgSize += 4.0;
  }

  if (obj_0->VehicleMessageFieldDef.activeLaneChange != 0.0) {
    /*  activeLaneChange */
    c = rt_roundd_snf(VehData_activeLaneChange);
    if (c < 128.0) {
      if (c >= -128.0) {
        x_2 = (int8_T)c;
      } else {
        x_2 = MIN_int8_T;
      }
    } else {
      x_2 = MAX_int8_T;
    }

    memcpy((void *)&tempInt8, (void *)&x_2, (uint32_T)((size_t)1 * sizeof
            (uint8_T)));
    ByteData[(int32_T)nVehMsgSize - 1] = tempInt8;
  }

  /*  %             catch */
  /*  %                 printf('ERROR: RealSimPack'); */
  /*  %                 return */
  /*  %             end */
}

/* Model output function */
void RS_DS_CM11_SimulinkRS_output(void)
{
  RealSimDepack_RS_DS_CM11_SimulinkRS_T *obj;
  RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T *obj_0;
  RealSimPack_RS_DS_CM11_SimulinkRS_T *obj_1;
  VehDataBus b_varargout_4;
  VehDataBus expl_temp;
  real_T varargin_1[2];
  real_T ZERO;
  real_T b_NAN;
  real_T b_scanned1;
  real_T b_varargout_4_accelerationDesired;
  real_T b_varargout_4_activeLaneChange;
  real_T b_varargout_4_color;
  real_T b_varargout_4_distanceTravel;
  real_T b_varargout_4_grade;
  real_T b_varargout_4_hasPrecedingVehicle;
  real_T b_varargout_4_heading;
  real_T b_varargout_4_idLength;
  real_T b_varargout_4_laneId;
  real_T b_varargout_4_linkIdLength;
  real_T b_varargout_4_linkIdNextLength;
  real_T b_varargout_4_precedingVehicleDistance;
  real_T b_varargout_4_precedingVehicleIdLength;
  real_T b_varargout_4_precedingVehicleSpeed;
  real_T b_varargout_4_signalLightColor;
  real_T b_varargout_4_signalLightDistance;
  real_T b_varargout_4_signalLightHeadId;
  real_T b_varargout_4_signalLightIdLength;
  real_T b_varargout_4_speedDesired;
  real_T b_varargout_4_speedLimit;
  real_T b_varargout_4_speedLimitChangeDistance;
  real_T b_varargout_4_speedLimitNext;
  real_T hasNewData;
  real_T k;
  real_T maxx;
  real_T onemr;
  real_T scanned1;
  real_T scanned2;
  real_T xnp1;
  real_T xtmp;
  int32_T ONE;
  int32_T low_i;
  int32_T low_ip1;
  int32_T nyrows;
  int32_T xend;
  uint32_T u1;
  char_T s1_data[52];
  char_T strVec[50];
  char_T strVec_data[50];
  int8_T tmp;
  uint8_T b_varargout_1[200];
  boolean_T c_success;
  boolean_T foundsign;
  boolean_T p;
  boolean_T p_0;
  boolean_T rEQ0;
  boolean_T success;
  static const boolean_T d[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  const real_T *VehicleMessageFieldDefInputVec;
  real_T *varargin_1_0;
  int32_T exitg2;
  uint8_T *u0;
  boolean_T exitg1;
  boolean_T guard1 = false;
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* set solver stop time */
    if (!(RS_DS_CM11_SimulinkRS_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&RS_DS_CM11_SimulinkRS_M->solverInfo,
                            ((RS_DS_CM11_SimulinkRS_M->Timing.clockTickH0 + 1) *
        RS_DS_CM11_SimulinkRS_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&RS_DS_CM11_SimulinkRS_M->solverInfo,
                            ((RS_DS_CM11_SimulinkRS_M->Timing.clockTick0 + 1) *
        RS_DS_CM11_SimulinkRS_M->Timing.stepSize0 +
        RS_DS_CM11_SimulinkRS_M->Timing.clockTickH0 *
        RS_DS_CM11_SimulinkRS_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    RS_DS_CM11_SimulinkRS_M->Timing.t[0] = rtsiGetT
      (&RS_DS_CM11_SimulinkRS_M->solverInfo);
  }

  /* Outputs for Atomic SubSystem: '<Root>/CarMaker' */

  /* user code (Output function Body) */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Function module access point of system <Root>/CarMaker    */
    ap_entry_RS_DS_CM11_SimulinkRS_SIDS1_TID1();
  }

  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
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
      RS_DS_CM11_SimulinkRS_B.Environment_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Environment_o2[13] = *p++;
    }

    /* S-Function (CM_Sfun): '<S5>/DrivMan' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[2];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Environment_o2[7];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[15] = *p++;
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[16] = *p++;
    }

    /* S-Function (CM_Sfun): '<S11>/VehicleControl' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[3];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[15];
      *p++ = RS_DS_CM11_SimulinkRS_B.DrivMan_o2[16];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[15] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[16] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[17] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[18] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[19] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[20] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[21] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[22] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[23] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[24] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[25] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[26] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[27] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[28] = *p++;
    }

    /* Memory generated from: '<S11>/Memory' */
    RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct =
      RS_DS_CM11_SimulinkRS_DW.Memory_2_PreviousInput;

    /* Gain: '<S11>/Gain1' */
    RS_DS_CM11_SimulinkRS_B.Gain1 = RS_DS_CM11_SimulinkRS_P.Gain1_Gain_p *
      RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct;

    /* Memory generated from: '<S11>/Memory' */
    RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct =
      RS_DS_CM11_SimulinkRS_DW.Memory_1_PreviousInput;

    /* Gain: '<S11>/Gain' */
    RS_DS_CM11_SimulinkRS_B.Gain = RS_DS_CM11_SimulinkRS_P.Gain_Gain_p *
      RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct;

    /* S-Function (read_dict): '<S11>/Read CM Dict3' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict3_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.CM_Time = e->GetFunc(e->Var);
    }

    /* Switch: '<S11>/Switch1' */
    if (RS_DS_CM11_SimulinkRS_B.CM_Time >
        RS_DS_CM11_SimulinkRS_P.Switch1_Threshold_i) {
      /* Switch: '<S11>/Switch1' incorporates:
       *  Constant: '<S11>/Driver Source'
       */
      RS_DS_CM11_SimulinkRS_B.Switch1 =
        RS_DS_CM11_SimulinkRS_P.DriverSource_Value;
    } else {
      /* Switch: '<S11>/Switch1' incorporates:
       *  Constant: '<S11>/Constant'
       */
      RS_DS_CM11_SimulinkRS_B.Switch1 =
        RS_DS_CM11_SimulinkRS_P.Constant_Value_pu;
    }

    /* End of Switch: '<S11>/Switch1' */

    /* S-Function (read_dict): '<S1>/Read CM Dict3' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict3_PWORK_k.Entry;
      RS_DS_CM11_SimulinkRS_B.CM_Time_b = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict7' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict7_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.SimCore_State = e->GetFunc(e->Var);
    }
  }

  /* Clock: '<S7>/Clock' */
  RS_DS_CM11_SimulinkRS_B.Clock = RS_DS_CM11_SimulinkRS_M->Timing.t[0];
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* S-Function (dsmpb_datainport): '<S55>/Data Inport S-Fcn' */

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)] non-bus port 1 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P1_S1
      (&RS_DS_CM11_SimulinkRS_B.DataVector[0]);

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)] non-bus port 2 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P2_S1
      (&RS_DS_CM11_SimulinkRS_B.Status);

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)] non-bus port 3 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P3_S1
      (&RS_DS_CM11_SimulinkRS_B.ReceivedBytes);

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)] non-bus port 4 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport1_P4_S1
      (&RS_DS_CM11_SimulinkRS_B.AvailableBytes);

    /* S-Function (dsmpb_dataoutport): '<S56>/Data Outport S-Fcn' */

    /* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_Out [TCP (1)] non-bus port 1 */
    ap_write_RS_DS_CM11_SimulinkRS_DataOutport1_P1_S1
      (&RS_DS_CM11_SimulinkRS_B.AvailableBytes);

    /* S-Function (dsmpb_datainport): '<S57>/Data Inport S-Fcn' */

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Status//Control_In [TCP (1)] non-bus port 1 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport2_P1_S1
      (&RS_DS_CM11_SimulinkRS_B.ConnectionState_f);

    /* DataTypeConversion: '<S52>/Data Type Conversion4' */
    RS_DS_CM11_SimulinkRS_B.ConnectionState =
      RS_DS_CM11_SimulinkRS_B.ConnectionState_f;

    /* DataTypeConversion: '<S52>/Data Type Conversion1' */
    memcpy(&RS_DS_CM11_SimulinkRS_B.DataTypeConversion1[0],
           &RS_DS_CM11_SimulinkRS_B.DataVector[0], sizeof(uint8_T) << 10U);

    /* MATLABSystem: '<S7>/MATLAB System' incorporates:
     *  MATLABSystem: '<S7>/MATLAB System1'
     */
    VehicleMessageFieldDefInputVec =
      &RS_DS_CM11_SimulinkRS_P.VehicleMessageFieldDefInputVec[0];
    u0 = &RS_DS_CM11_SimulinkRS_B.DataTypeConversion1[0];
    u1 = RS_DS_CM11_SimulinkRS_B.ReceivedBytes;
    hasNewData = RS_DS_CM11_SimulinkRS_B.ConnectionState;
    varargin_1_0 = &RS_DS_CM11_SimulinkRS_DW.obj.VehicleMessageFieldDefInputVec
      [0];
    rEQ0 = false;
    p = true;
    low_i = 0;
    exitg1 = false;
    while ((!exitg1) && (low_i < 29)) {
      k = (real_T)low_i + 1.0;
      scanned1 = varargin_1_0[(int32_T)k - 1];
      k = VehicleMessageFieldDefInputVec[(int32_T)k - 1];
      p_0 = (scanned1 == k);
      if (!p_0) {
        p = false;
        exitg1 = true;
      } else {
        low_i++;
      }
    }

    if (p) {
      rEQ0 = true;
    }

    if (!rEQ0) {
      memcpy(&RS_DS_CM11_SimulinkRS_DW.obj.VehicleMessageFieldDefInputVec[0],
             &VehicleMessageFieldDefInputVec[0], 29U * sizeof(real_T));
    }

    obj = &RS_DS_CM11_SimulinkRS_DW.obj;
    RS_DS_CM11_SimulinkRS_RealSimDepack_stepImpl(obj, u0, u1, hasNewData,
      &b_scanned1, &k, &scanned2, &b_varargout_4, &scanned1);
    b_varargout_4_idLength = b_varargout_4.idLength;
    ZERO = b_varargout_4.typeLength;
    onemr = b_varargout_4.vehicleClassLength;
    xtmp = b_varargout_4.speed;
    hasNewData = b_varargout_4.acceleration;
    xnp1 = b_varargout_4.positionX;
    maxx = b_varargout_4.positionY;
    b_NAN = b_varargout_4.positionZ;
    b_varargout_4_heading = b_varargout_4.heading;
    b_varargout_4_color = b_varargout_4.color;
    b_varargout_4_linkIdLength = b_varargout_4.linkIdLength;
    b_varargout_4_laneId = b_varargout_4.laneId;
    b_varargout_4_distanceTravel = b_varargout_4.distanceTravel;
    b_varargout_4_speedDesired = b_varargout_4.speedDesired;
    b_varargout_4_accelerationDesired = b_varargout_4.accelerationDesired;
    b_varargout_4_hasPrecedingVehicle = b_varargout_4.hasPrecedingVehicle;
    b_varargout_4_precedingVehicleIdLength =
      b_varargout_4.precedingVehicleIdLength;
    b_varargout_4_precedingVehicleDistance =
      b_varargout_4.precedingVehicleDistance;
    b_varargout_4_precedingVehicleSpeed = b_varargout_4.precedingVehicleSpeed;
    b_varargout_4_signalLightIdLength = b_varargout_4.signalLightIdLength;
    b_varargout_4_signalLightHeadId = b_varargout_4.signalLightHeadId;
    b_varargout_4_signalLightDistance = b_varargout_4.signalLightDistance;
    b_varargout_4_signalLightColor = b_varargout_4.signalLightColor;
    b_varargout_4_speedLimit = b_varargout_4.speedLimit;
    b_varargout_4_speedLimitNext = b_varargout_4.speedLimitNext;
    b_varargout_4_speedLimitChangeDistance =
      b_varargout_4.speedLimitChangeDistance;
    b_varargout_4_linkIdNextLength = b_varargout_4.linkIdNextLength;
    b_varargout_4_grade = b_varargout_4.grade;
    b_varargout_4_activeLaneChange = b_varargout_4.activeLaneChange;

    /* MATLABSystem: '<S7>/MATLAB System' */
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o1 = b_scanned1;

    /* MATLABSystem: '<S7>/MATLAB System' */
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o2 = k;

    /* MATLABSystem: '<S7>/MATLAB System' */
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o3 = scanned2;

    /* MATLABSystem: '<S7>/MATLAB System' */
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.idLength = b_varargout_4_idLength;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.typeLength = ZERO;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.vehicleClassLength = onemr;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speed = xtmp;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.acceleration = hasNewData;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.positionX = xnp1;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.positionY = maxx;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.positionZ = b_NAN;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.heading = b_varargout_4_heading;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.color = b_varargout_4_color;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdLength =
      b_varargout_4_linkIdLength;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.laneId = b_varargout_4_laneId;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.distanceTravel =
      b_varargout_4_distanceTravel;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedDesired =
      b_varargout_4_speedDesired;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.accelerationDesired =
      b_varargout_4_accelerationDesired;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.hasPrecedingVehicle =
      b_varargout_4_hasPrecedingVehicle;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleIdLength =
      b_varargout_4_precedingVehicleIdLength;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleDistance =
      b_varargout_4_precedingVehicleDistance;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleSpeed =
      b_varargout_4_precedingVehicleSpeed;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightIdLength =
      b_varargout_4_signalLightIdLength;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightHeadId =
      b_varargout_4_signalLightHeadId;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightDistance =
      b_varargout_4_signalLightDistance;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightColor =
      b_varargout_4_signalLightColor;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimit =
      b_varargout_4_speedLimit;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimitNext =
      b_varargout_4_speedLimitNext;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimitChangeDistance =
      b_varargout_4_speedLimitChangeDistance;
    for (low_i = 0; low_i < 50; low_i++) {
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.id[low_i] = b_varargout_4.id[low_i];
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.type[low_i] =
        b_varargout_4.type[low_i];
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.vehicleClass[low_i] =
        b_varargout_4.vehicleClass[low_i];
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkId[low_i] =
        b_varargout_4.linkId[low_i];
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleId[low_i] =
        b_varargout_4.precedingVehicleId[low_i];
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightId[low_i] =
        b_varargout_4.signalLightId[low_i];
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdNext[low_i] =
        b_varargout_4.linkIdNext[low_i];
    }

    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdNextLength =
      b_varargout_4_linkIdNextLength;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.grade = b_varargout_4_grade;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.activeLaneChange =
      b_varargout_4_activeLaneChange;

    /* End of MATLABSystem: '<S7>/MATLAB System' */

    /* MATLABSystem: '<S7>/MATLAB System' */
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o5 = scanned1;

    /* SignalConversion generated from: '<S7>/Bus Selector' */
    RS_DS_CM11_SimulinkRS_B.speedDesired =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedDesired;

    /* Memory: '<S7>/Memory' */
    RS_DS_CM11_SimulinkRS_B.Memory =
      RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput;

    /* S-Function (read_dict): '<S8>/Read CM Dict2' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict2_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict2 = e->GetFunc(e->Var);
    }

    /* MultiPortSwitch: '<S7>/Multiport Switch' incorporates:
     *  Constant: '<S7>/RS_actualSpeedSourceSelector'
     */
    switch ((int32_T)RS_DS_CM11_SimulinkRS_P.RS_actualSpeedSourceSelector_Value)
    {
     case 1:
      /* MultiPortSwitch: '<S7>/Multiport Switch' */
      RS_DS_CM11_SimulinkRS_B.speedActual = RS_DS_CM11_SimulinkRS_B.speedDesired;
      break;

     case 2:
      /* MultiPortSwitch: '<S7>/Multiport Switch' */
      RS_DS_CM11_SimulinkRS_B.speedActual = RS_DS_CM11_SimulinkRS_B.Memory;
      break;

     default:
      /* MultiPortSwitch: '<S7>/Multiport Switch' */
      RS_DS_CM11_SimulinkRS_B.speedActual = RS_DS_CM11_SimulinkRS_B.ReadCMDict2;
      break;
    }

    /* End of MultiPortSwitch: '<S7>/Multiport Switch' */
  }

  /* MATLABSystem: '<S7>/MATLAB System2' */
  scanned1 = RS_DS_CM11_SimulinkRS_P.RealSimPara.speedInit;
  onemr = RS_DS_CM11_SimulinkRS_P.RealSimPara.tLookahead;
  xtmp = RS_DS_CM11_SimulinkRS_P.MATLABSystem2_speedUpperBound;
  scanned2 = RS_DS_CM11_SimulinkRS_B.Clock;
  b_scanned1 = RS_DS_CM11_SimulinkRS_B.ConnectionState;
  hasNewData = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o1;
  k = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o2;
  maxx = RS_DS_CM11_SimulinkRS_B.speedDesired;
  ZERO = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o5;
  b_varargout_4_idLength = RS_DS_CM11_SimulinkRS_B.speedActual;
  if (RS_DS_CM11_SimulinkRS_DW.obj_p.initialSpeed != scanned1) {
    RS_DS_CM11_SimulinkRS_DW.obj_p.initialSpeed = scanned1;
  }

  if (RS_DS_CM11_SimulinkRS_DW.obj_p.speedLookAheadHorizon != onemr) {
    RS_DS_CM11_SimulinkRS_DW.obj_p.speedLookAheadHorizon = onemr;
  }

  if (RS_DS_CM11_SimulinkRS_DW.obj_p.speedUpperBound != xtmp) {
    RS_DS_CM11_SimulinkRS_DW.obj_p.speedUpperBound = xtmp;
  }

  obj_0 = &RS_DS_CM11_SimulinkRS_DW.obj_p;

  /*  INPUTS */
  /*    ByteData: bytes vector received from TrafficSimualtor */
  /*    nByte: size of bytes received */
  /*    initSpeed: initial speed command sent to controller m/s */
  /*    speedPrev: previous speed command received from TrafficSimulator */
  /*  */
  /*  OUTPUTS */
  /*    simState: simulation state. NOT actively used */
  /*    t: simulation time of TrafficSimulator */
  /*    nVeh: number of vehicle data received. */
  /*    speed: speed of ego vehicle in TrafficSimulator m/s */
  onemr = 0.0;
  scanned1 = 0.0;

  /*  if just connected to server at current step */
  /*              if abs(connectionState-3) < 1e-5 && abs(obj.connectionState-3) > 1e-5  */
  xtmp = hasNewData - 1.0;
  xnp1 = fabs(xtmp);
  guard1 = false;
  if (xnp1 < 1.0E-5) {
    xtmp = obj_0->simState;
    xnp1 = fabs(xtmp);
    if (xnp1 < 1.0E-5) {
      xtmp = obj_0->connectionState - 3.0;
      xnp1 = fabs(xtmp);
      if (xnp1 < 1.0E-5) {
        /*  set connectionState and start time */
        obj_0->simulatorStartTime = scanned2;
        obj_0->simState = hasNewData;

        /*                  temp = 1; */
        /*  if just disconnected to server at current step */
      } else {
        guard1 = true;
      }
    } else {
      guard1 = true;
    }
  } else {
    guard1 = true;
  }

  if (guard1) {
    xtmp = obj_0->connectionState - 3.0;
    xnp1 = fabs(xtmp);
    if (xnp1 < 1.0E-5) {
      xtmp = b_scanned1 - 3.0;
      xnp1 = fabs(xtmp);
      if (xnp1 > 1.0E-5) {
        /*  resetting */
        obj_0->connectionState = 0.0;
        obj_0->timeSimulator = 0.0;
        obj_0->simulatorStartTime = 0.0;
        obj_0->timeStepSimulator = 0.0;
        obj_0->timeStepSimulatorPrevious = 0.0;
        obj_0->simState = 0.0;
        obj_0->RealSimDelay = 0.0;
      }
    }
  }

  /*              temp = obj.connectionState; */
  obj_0->connectionState = b_scanned1;

  /*  get absolute time in Simulator, increase every Simulink step */
  if (obj_0->simState != 0.0) {
    xtmp = obj_0->connectionState - 3.0;
    xnp1 = fabs(xtmp);
    if (xnp1 < 1.0E-5) {
      obj_0->timeSimulator = scanned2 - obj_0->simulatorStartTime;
    } else {
      obj_0->timeSimulator = -1.0;
    }
  } else {
    obj_0->timeSimulator = -1.0;
  }

  /*  get time step in Simulator, change every VISSIM/SUMO step */
  /*  as byproduct, get triggered time every 0.1 second */
  guard1 = false;
  if (obj_0->simState != 0.0) {
    xtmp = obj_0->connectionState - 3.0;
    xnp1 = fabs(xtmp);
    if (xnp1 < 1.0E-5) {
      if (obj_0->timeSimulator >= (obj_0->timeStepSimulatorPrevious + 0.1) -
          1.0E-5) {
        guard1 = true;
      } else {
        xtmp = obj_0->simulatorStartTime - scanned2;
        xnp1 = fabs(xtmp);
        if (xnp1 < 1.0E-5) {
          guard1 = true;
        } else {
          obj_0->timeStepSimulator = obj_0->timeStepSimulatorPrevious;
        }
      }
    } else {
      obj_0->timeStepSimulator = obj_0->timeStepSimulatorPrevious;
    }
  } else {
    obj_0->timeStepSimulator = obj_0->timeStepSimulatorPrevious;
  }

  if (guard1) {
    obj_0->timeStepSimulator = obj_0->timeSimulator;
    obj_0->timeStepSimulatorPrevious = obj_0->timeStepSimulator;
    scanned1 = 1.0;
  }

  scanned2 = obj_0->timeSimulator;
  b_scanned1 = obj_0->timeStepSimulator;

  /*  check if has new data from traffic simulator */
  if (k >= 0.2) {
    xtmp = k - obj_0->timeReceivePrevious;
    xnp1 = fabs(xtmp);
    if (xnp1 > 1.0E-5) {
      hasNewData = 1.0;
    } else {
      hasNewData = 0.0;
    }
  } else {
    hasNewData = 0.0;
  }

  /*  speed handling */
  if ((obj_0->simState != 0.0) && (obj_0->connectionState != 0.0) && (ZERO !=
       0.0)) {
    /* %% interpolation */
    if (obj_0->speedInterpolationMode == 0.0) {
      onemr = maxx;
    } else if (obj_0->speedInterpolationMode == 1.0) {
      /*  update interpolation point */
      if (hasNewData == 1.0) {
        obj_0->timePrevious = obj_0->timeNext;
        obj_0->timeNext = obj_0->timeStepSimulator + 0.1;
        obj_0->speedPrevious = obj_0->speedNext;
        obj_0->speedNext = maxx;
      }

      /*  doing interpolation */
      varargin_1[0] = obj_0->timePrevious;
      varargin_1[1] = obj_0->timeNext;
      b_NAN = obj_0->speedPrevious;
      b_varargout_4_heading = obj_0->speedNext;
      onemr = obj_0->timeSimulator;
      xnp1 = (rtNaN);
      low_i = 1;
      do {
        exitg2 = 0;
        if (low_i - 1 < 2) {
          nyrows = low_i - 1;
          xtmp = varargin_1[nyrows];
          rEQ0 = rtIsNaN(xtmp);
          if (rEQ0) {
            exitg2 = 1;
          } else {
            low_i++;
          }
        } else {
          if (varargin_1[1] < varargin_1[0]) {
            xtmp = varargin_1[0];
            varargin_1[0] = varargin_1[1];
            varargin_1[1] = xtmp;
            xtmp = b_NAN;
            b_NAN = b_varargout_4_heading;
            b_varargout_4_heading = xtmp;
          }

          xtmp = varargin_1[0];
          maxx = varargin_1[1];
          rEQ0 = rtIsNaN(onemr);
          if ((!rEQ0) && (!(onemr > maxx)) && (!(onemr < xtmp))) {
            xtmp = varargin_1[0];
            xnp1 = varargin_1[1];
            maxx = (onemr - xtmp) / (xnp1 - xtmp);
            onemr = 1.0 - maxx;
            if (maxx == 0.0) {
              xtmp = b_NAN;
              xnp1 = xtmp;
            } else if (maxx == 1.0) {
              xnp1 = b_varargout_4_heading;
            } else {
              xtmp = b_NAN;
              xnp1 = b_varargout_4_heading;
              if (xtmp == xnp1) {
                xnp1 = xtmp;
              } else {
                xnp1 = onemr * xtmp + maxx * xnp1;
              }
            }
          }

          exitg2 = 1;
        }
      } while (exitg2 == 0);

      if (obj_0->timeSimulator > obj_0->timeNext) {
        onemr = obj_0->speedNext;
      } else {
        onemr = xnp1;
      }
    }
  } else {
    /*  use initial speed */
    onemr = obj_0->initialSpeed;
  }

  /*  calculate a lookahead speed accounting for dynamics if */
  /*  traffic simulator is determining the desired speed           */
  if (ZERO != 0.0) {
    /*  should update lookahead speed whenever receive new data from traffic simulator */
    if (hasNewData == 1.0) {
      /*  speedInterp is from SUMO, speedActual */
      /*  due to tcp/ip delay, the remaining time could be less */
      /*  than 0.1 seconds */
      xtmp = obj_0->timeSimulator;
      rEQ0 = rtIsNaN(xtmp);
      if (rEQ0) {
        maxx = (rtNaN);
      } else {
        rEQ0 = rtIsInf(xtmp);
        if (rEQ0) {
          maxx = (rtNaN);
        } else if (xtmp == 0.0) {
          maxx = 0.0;
        } else {
          maxx = fmod(xtmp, 0.1);
          rEQ0 = (maxx == 0.0);
          if (!rEQ0) {
            b_NAN = fabs(xtmp / 0.1);
            rEQ0 = !(fabs(b_NAN - floor(b_NAN + 0.5)) > 2.2204460492503131E-16 *
                     b_NAN);
          }

          if (rEQ0) {
            maxx = 0.0;
          } else if (xtmp < 0.0) {
            maxx += 0.1;
          }
        }
      }

      ZERO = 0.1 - maxx;
      xtmp = -5.0;
      onemr = (onemr - b_varargout_4_idLength) / ZERO;
      if ((!(-5.0 >= onemr)) && (!rtIsNaN(onemr))) {
        xtmp = onemr;
      }

      if (xtmp <= 5.0) {
        ZERO = xtmp;
      } else {
        ZERO = 5.0;
      }

      xtmp = 0.0;
      onemr = obj_0->speedLookAheadHorizon * ZERO + b_varargout_4_idLength;
      if ((!(0.0 >= onemr)) && (!rtIsNaN(onemr))) {
        xtmp = onemr;
      }

      onemr = obj_0->speedUpperBound;
      if ((xtmp <= onemr) || rtIsNaN(onemr)) {
        onemr = xtmp;
      }

      obj_0->accelerationDesiredPrevious = ZERO;
      obj_0->speedLookAheadPrevious = onemr;
    } else {
      ZERO = obj_0->accelerationDesiredPrevious;
      onemr = obj_0->speedLookAheadPrevious;
    }
  } else {
    ZERO = 0.0;
  }

  /*  calc delay */
  if (k >= 0.2) {
    xtmp = k - obj_0->timeReceivePrevious;
    xnp1 = fabs(xtmp);
    if (xnp1 > 1.0E-5) {
      obj_0->RealSimDelay = (obj_0->timeSimulator + 0.2) - k;
      obj_0->timeReceivePrevious = k;
    }
  }

  k = obj_0->RealSimDelay;

  /* MATLABSystem: '<S7>/MATLAB System2' */
  RS_DS_CM11_SimulinkRS_B.MATLABSystem2_o1 = onemr;

  /* MATLABSystem: '<S7>/MATLAB System2' */
  RS_DS_CM11_SimulinkRS_B.accelerationDesired = ZERO;

  /* MATLABSystem: '<S7>/MATLAB System2' */
  RS_DS_CM11_SimulinkRS_B.timeSimulator = scanned2;

  /* MATLABSystem: '<S7>/MATLAB System2' */
  RS_DS_CM11_SimulinkRS_B.timeStepSimulator = b_scanned1;

  /* MATLABSystem: '<S7>/MATLAB System2' */
  RS_DS_CM11_SimulinkRS_B.timeStepTrigger = scanned1;

  /* MATLABSystem: '<S7>/MATLAB System2' */
  RS_DS_CM11_SimulinkRS_B.MATLABSystem2_o6 = k;

  /* ManualSwitch: '<S1>/Manual Switch' */
  if (RS_DS_CM11_SimulinkRS_P.ManualSwitch_CurrentSetting == 1) {
    /* ManualSwitch: '<S1>/Manual Switch' incorporates:
     *  Constant: '<S1>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.ManualSwitch =
      RS_DS_CM11_SimulinkRS_P.RealSimPara.speedInit;
  } else {
    /* ManualSwitch: '<S1>/Manual Switch' */
    RS_DS_CM11_SimulinkRS_B.ManualSwitch =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem2_o1;
  }

  /* End of ManualSwitch: '<S1>/Manual Switch' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* S-Function (read_dict): '<S1>/Read CM Dict8' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict8_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.distanceTravel = e->GetFunc(e->Var);
    }

    /* Chart: '<S1>/RealSimHILCycle' incorporates:
     *  Constant: '<S1>/Constant'
     *  Constant: '<S1>/RampDownFlag'
     *  Constant: '<S1>/initialWaitTime'
     *  Constant: '<S1>/totalDistance (m)'
     *  Constant: '<S1>/totalTime (sec)'
     */
    if (RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 < MAX_uint32_T) {
      RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1++;
    }

    /* Gateway: CarMaker/RealSimHILCycle */
    /* During: CarMaker/RealSimHILCycle */
    if (RS_DS_CM11_SimulinkRS_DW.is_active_c2_RS_DS_CM11_SimulinkRS == 0U) {
      /* Entry: CarMaker/RealSimHILCycle */
      RS_DS_CM11_SimulinkRS_DW.is_active_c2_RS_DS_CM11_SimulinkRS = 1U;

      /* Entry Internal: CarMaker/RealSimHILCycle */
      /* Transition: '<S9>:2' */
      RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS =
        RS_DS_CM11_SimulinkRS_IN_Idle;

      /* Entry 'Idle': '<S9>:1' */
      RS_DS_CM11_SimulinkRS_B.CycleMode = 0.0;
      RS_DS_CM11_SimulinkRS_B.SpeedCmd = 0.0;
    } else {
      switch (RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS) {
       case RS_DS_CM11_SimulinkRS_IN_CycleOn:
        /* During 'CycleOn': '<S9>:17' */
        rEQ0 = ((RS_DS_CM11_SimulinkRS_B.distanceTravel >
                 RS_DS_CM11_SimulinkRS_P.totalDistancem_Value) ||
                (RS_DS_CM11_SimulinkRS_B.CM_Time_b >
                 RS_DS_CM11_SimulinkRS_P.totalTimesec_Value) ||
                (RS_DS_CM11_SimulinkRS_P.RampDownFlag_Value != 0.0) ||
                (RS_DS_CM11_SimulinkRS_B.SimCore_State == 9.0));
        if (rEQ0) {
          /* Transition: '<S9>:7' */
          /* Exit Internal 'CycleOn': '<S9>:17' */
          RS_DS_CM11_SimulinkRS_DW.is_CycleOn =
            RS_DS_CM11_SimulinkRS_IN_NO_ACTIVE_CHILD;
          RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS =
            RS_DS_CM11_SimulinkRS_IN_RampDown;

          /* Entry 'RampDown': '<S9>:4' */
          RS_DS_CM11_SimulinkRS_B.CycleMode = 4.0;
          RS_DS_CM11_SimulinkRS_B.SpeedCmd--;
        } else {
          switch (RS_DS_CM11_SimulinkRS_DW.is_CycleOn) {
           case RS_DS_CM11_SimulinkRS_IN_Driving:
            /* During 'Driving': '<S9>:5' */
            RS_DS_CM11_SimulinkRS_B.CycleMode = 3.0;
            scanned1 = RS_DS_CM11_SimulinkRS_B.ManualSwitch;
            if (scanned1 >= 0.0) {
              RS_DS_CM11_SimulinkRS_B.SpeedCmd = scanned1;
            } else {
              RS_DS_CM11_SimulinkRS_B.SpeedCmd = 0.0;
            }
            break;

           case RS_DS_CM11_SimulinkRS_IN_InitialWait:
            /* During 'InitialWait': '<S9>:58' */
            if (RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 >= (uint32_T)ceil
                (RS_DS_CM11_SimulinkRS_P.initialWaitTime_Value * 1000.0)) {
              /* Transition: '<S9>:40' */
              RS_DS_CM11_SimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_SimulinkRS_IN_RampUp;

              /* Entry 'RampUp': '<S9>:3' */
              RS_DS_CM11_SimulinkRS_B.CycleMode = 2.0;
              RS_DS_CM11_SimulinkRS_B.SpeedCmd += 0.1;
            } else {
              RS_DS_CM11_SimulinkRS_B.CycleMode = 1.0;
            }
            break;

           case RS_DS_CM11_SimulinkRS_IN_RampUp:
            /* During 'RampUp': '<S9>:3' */
            if (fabs(RS_DS_CM11_SimulinkRS_B.SpeedCmd -
                     RS_DS_CM11_SimulinkRS_P.RealSimPara.speedInit) < 0.5) {
              /* Transition: '<S9>:9' */
              RS_DS_CM11_SimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_SimulinkRS_IN_Driving;

              /* Entry 'Driving': '<S9>:5' */
              RS_DS_CM11_SimulinkRS_B.CycleMode = 3.0;
              scanned1 = RS_DS_CM11_SimulinkRS_B.ManualSwitch;
              if (scanned1 >= 0.0) {
                RS_DS_CM11_SimulinkRS_B.SpeedCmd = scanned1;
              } else {
                RS_DS_CM11_SimulinkRS_B.SpeedCmd = 0.0;
              }
            } else if (fabs(RS_DS_CM11_SimulinkRS_B.SpeedCmd -
                            RS_DS_CM11_SimulinkRS_P.RealSimPara.speedInit) >=
                       0.1) {
              /* Transition: '<S9>:55' */
              RS_DS_CM11_SimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_SimulinkRS_IN_WaitForNextRamp;
              RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 = 0U;
            } else {
              RS_DS_CM11_SimulinkRS_B.CycleMode = 2.0;
              RS_DS_CM11_SimulinkRS_B.SpeedCmd += 0.1;
            }
            break;

           default:
            /* During 'WaitForNextRamp': '<S9>:66' */
            if (RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 >= 100U) {
              /* Transition: '<S9>:56' */
              RS_DS_CM11_SimulinkRS_DW.is_CycleOn =
                RS_DS_CM11_SimulinkRS_IN_RampUp;

              /* Entry 'RampUp': '<S9>:3' */
              RS_DS_CM11_SimulinkRS_B.CycleMode = 2.0;
              RS_DS_CM11_SimulinkRS_B.SpeedCmd += 0.1;
            }
            break;
          }
        }
        break;

       case RS_DS_CM11_SimulinkRS_IN_Idle:
        /* During 'Idle': '<S9>:1' */
        rEQ0 = ((RS_DS_CM11_SimulinkRS_B.SimCore_State >= 6.0) &&
                (RS_DS_CM11_SimulinkRS_B.distanceTravel <=
                 RS_DS_CM11_SimulinkRS_P.totalDistancem_Value) &&
                (RS_DS_CM11_SimulinkRS_B.CM_Time_b <=
                 RS_DS_CM11_SimulinkRS_P.totalTimesec_Value) &&
                (!(RS_DS_CM11_SimulinkRS_P.RampDownFlag_Value != 0.0)));
        if (rEQ0) {
          /* Transition: '<S9>:6' */
          RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS =
            RS_DS_CM11_SimulinkRS_IN_CycleOn;

          /* Entry Internal 'CycleOn': '<S9>:17' */
          /* Transition: '<S9>:62' */
          RS_DS_CM11_SimulinkRS_DW.is_CycleOn =
            RS_DS_CM11_SimulinkRS_IN_InitialWait;
          RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 = 0U;

          /* Entry 'InitialWait': '<S9>:58' */
          RS_DS_CM11_SimulinkRS_B.CycleMode = 1.0;
        } else {
          RS_DS_CM11_SimulinkRS_B.CycleMode = 0.0;
          RS_DS_CM11_SimulinkRS_B.SpeedCmd = 0.0;
        }
        break;

       case RS_DS_CM11_SimulinkRS_IN_RampDown:
        /* During 'RampDown': '<S9>:4' */
        if (RS_DS_CM11_SimulinkRS_B.SpeedCmd < 0.5) {
          /* Transition: '<S9>:8' */
          RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS =
            RS_DS_CM11_SimulinkRS_IN_Idle;

          /* Entry 'Idle': '<S9>:1' */
          RS_DS_CM11_SimulinkRS_B.CycleMode = 0.0;
          RS_DS_CM11_SimulinkRS_B.SpeedCmd = 0.0;
        } else if (RS_DS_CM11_SimulinkRS_B.SpeedCmd >= 0.5) {
          /* Transition: '<S9>:69' */
          RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS =
            RS_DS_CM11_SimulinkRS_IN_WaitForNextRamp;
          RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 = 0U;
        } else {
          RS_DS_CM11_SimulinkRS_B.CycleMode = 4.0;
          RS_DS_CM11_SimulinkRS_B.SpeedCmd--;
        }
        break;

       default:
        /* During 'WaitForNextRamp': '<S9>:67' */
        if (RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 >= 100U) {
          /* Transition: '<S9>:68' */
          RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS =
            RS_DS_CM11_SimulinkRS_IN_RampDown;

          /* Entry 'RampDown': '<S9>:4' */
          RS_DS_CM11_SimulinkRS_B.CycleMode = 4.0;
          RS_DS_CM11_SimulinkRS_B.SpeedCmd--;
        }
        break;
      }
    }

    /* End of Chart: '<S1>/RealSimHILCycle' */

    /* RelationalOperator: '<S11>/Equal' incorporates:
     *  Constant: '<S11>/Constant1'
     */
    RS_DS_CM11_SimulinkRS_B.Equal = (RS_DS_CM11_SimulinkRS_B.CycleMode ==
      RS_DS_CM11_SimulinkRS_P.Constant1_Value);

    /* Switch: '<S11>/Switch2' */
    if (RS_DS_CM11_SimulinkRS_B.Equal) {
      /* Switch: '<S11>/Switch2' */
      RS_DS_CM11_SimulinkRS_B.driverSourceFinal =
        RS_DS_CM11_SimulinkRS_B.Switch1;
    } else {
      /* Switch: '<S11>/Switch2' incorporates:
       *  Constant: '<S11>/Constant2'
       */
      RS_DS_CM11_SimulinkRS_B.driverSourceFinal =
        RS_DS_CM11_SimulinkRS_P.Constant2_Value;
    }

    /* End of Switch: '<S11>/Switch2' */

    /* Switch: '<S11>/Switch' */
    if (RS_DS_CM11_SimulinkRS_B.driverSourceFinal >
        RS_DS_CM11_SimulinkRS_P.Switch_Threshold) {
      /* Switch: '<S11>/Switch' */
      memcpy(&RS_DS_CM11_SimulinkRS_B.Switch[0],
             &RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[0], 9U * sizeof(real_T));
      RS_DS_CM11_SimulinkRS_B.Switch[9] = RS_DS_CM11_SimulinkRS_B.Gain1;
      RS_DS_CM11_SimulinkRS_B.Switch[10] =
        RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[10];
      RS_DS_CM11_SimulinkRS_B.Switch[11] =
        RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[11];
      RS_DS_CM11_SimulinkRS_B.Switch[12] = RS_DS_CM11_SimulinkRS_B.Gain;
      memcpy(&RS_DS_CM11_SimulinkRS_B.Switch[13],
             &RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[13], sizeof(real_T) <<
             4U);
    } else {
      /* Switch: '<S11>/Switch' */
      memcpy(&RS_DS_CM11_SimulinkRS_B.Switch[0],
             &RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[0], 29U * sizeof(real_T));
    }

    /* End of Switch: '<S11>/Switch' */

    /* S-Function (CM_Sfun): '<S11>/VehicleControlUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[4];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[15];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[16];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[17];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[18];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[19];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[20];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[21];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[22];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[23];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[24];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[25];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[26];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[27];
      *p++ = RS_DS_CM11_SimulinkRS_B.Switch[28];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o2 = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o6[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o6[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[11] = *p++;
    }

    /* S-Function (CM_Sfun): '<S23>/Steering' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[5];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[4];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[15] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[16] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[17] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[18] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[19] = *p++;
      RS_DS_CM11_SimulinkRS_B.Steering_o2[20] = *p++;
    }

    /* S-Function (CM_Sfun): '<S21>/Kinematics' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[6];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.Steering_o2[15];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[7] = *p++;
    }

    /* S-Function (CM_Sfun): '<S29>/Forces' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[7];
      double *p;
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o3[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.Forces_o4[15] = *p++;
    }

    /* S-Function (CM_Sfun): '<S27>/Suspension Control Unit' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[8];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[7];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[15] = *p++;
    }

    /* S-Function (CM_Sfun): '<S27>/Suspension Force Elements' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[9];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o6[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o7[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o8[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinematics_o9[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[15];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[15] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[16] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[17] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[18] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[19] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[20] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[21] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[22] = *p++;
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[23] = *p++;
    }

    /* S-Function (CM_Sfun): '<S27>/Suspension Force Elements Update' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[10];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[15];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[16];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[17];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[18];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[19];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[20];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[21];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[22];
      *p++ = RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[23];
      rinf->Func();
    }

    /* S-Function (CM_Sfun): '<S22>/Kinetics' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[11];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o3[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.Forces_o4[15];
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
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o3[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o3[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o3[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o3[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o3[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o3[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o4[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o4[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o4[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o4[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o4[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o4[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o5[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o5[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o5[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o5[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o6[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o6[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o6[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Kinetics_o6[3] = *p++;
    }

    /* S-Function (CM_Sfun): '<S24>/Trailer' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[12];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o3[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o3[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o3[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o3[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o3[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o3[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o4[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o4[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o4[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o4[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o4[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o4[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o5[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o5[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o5[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o5[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o6[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o6[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o6[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Kinetics_o6[3];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.Trailer_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[15] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[16] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[17] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o9[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o9[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o9[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o9[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o9[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Trailer_o9[5] = *p++;
    }

    /* S-Function (CM_Sfun): '<S19>/CarAndTrailerUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[13];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o3[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[15];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[16];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o4[17];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o5[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o6[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o7[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o8[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o9[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o9[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o9[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o9[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o9[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Trailer_o9[5];
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
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[8] = *p++;
    }

    /* S-Function (CM_Sfun): '<S13>/Brake' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[18];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o6[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o6[1];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.Brake_o2[15] = *p++;
    }

    /* S-Function (CM_Sfun): '<S13>/BrakeUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[19];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.Brake_o2[15];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[15] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[16] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[17] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[18] = *p++;
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[19] = *p++;
    }

    /* S-Function (CM_Sfun): '<S16>/PowerTrain' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[20];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[15];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[16];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[17];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[18];
      *p++ = RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[19];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[11];
      rinf->Func();

      /* Copy back outputs from CarMaker */
      p = rinf->Outputs;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[0] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[1] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[2] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[3] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[4] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[5] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[6] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[7] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[8] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[9] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[10] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[11] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[12] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[13] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[14] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[15] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[16] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[17] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[18] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[19] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[20] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[21] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[22] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[23] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[24] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[25] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[26] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[27] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[28] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[29] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[30] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[31] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[32] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[33] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[34] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[35] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[36] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[37] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[38] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[39] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[40] = *p++;
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[41] = *p++;
    }

    /* S-Function (CM_Sfun): '<S16>/PowerTrainUpd' */
    {
      const tRoleInfo *rinf = &CM_RoleInfos.v[21];
      double *p;

      /* Copy inputs to CarMaker */
      p = rinf->Inputs;
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[0];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[1];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[2];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[3];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[4];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[5];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[6];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[7];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[8];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[9];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[10];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[11];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[12];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[13];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[14];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[15];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[16];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[17];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[18];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[19];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[20];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[21];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[22];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[23];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[24];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[25];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[26];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[27];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[28];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[29];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[30];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[31];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[32];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[33];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[34];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[35];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[36];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[37];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[38];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[39];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[40];
      *p++ = RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[41];
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
    RS_DS_CM11_SimulinkRS_B.HIL_SpeedCmd_kph =
      RS_DS_CM11_SimulinkRS_P.Gain3_Gain * RS_DS_CM11_SimulinkRS_B.SpeedCmd;

    /* S-Function (read_dict): '<S6>/Read CM Dict' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.VehSpd = e->GetFunc(e->Var);
    }

    /* Gain: '<S6>/Gain1' */
    RS_DS_CM11_SimulinkRS_B.VehSpdKph = RS_DS_CM11_SimulinkRS_P.Gain1_Gain_c *
      RS_DS_CM11_SimulinkRS_B.VehSpd;

    /* Gain: '<S6>/Gain3' */
    RS_DS_CM11_SimulinkRS_B.VehSpdKph_p = RS_DS_CM11_SimulinkRS_P.Gain3_Gain_m *
      RS_DS_CM11_SimulinkRS_B.SpeedCmd;

    /* Outputs for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
    /* DataTypeConversion: '<S15>/Data Type Conversion11' */
    RS_DS_CM11_SimulinkRS_B.Vehicle_Spd_kph = RS_DS_CM11_SimulinkRS_B.VehSpdKph;

    /* Constant: '<S42>/Constant' */
    RS_DS_CM11_SimulinkRS_B.Drive = RS_DS_CM11_SimulinkRS_P.Constant_Value_p;

    /* Memory: '<S42>/Memory1' */
    RS_DS_CM11_SimulinkRS_B.Memory1_b =
      RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_e;

    /* Memory: '<S42>/Memory2' */
    RS_DS_CM11_SimulinkRS_B.Memory2_o =
      RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput_l;

    /* Saturate: '<S42>/SpdSat' */
    scanned1 = RS_DS_CM11_SimulinkRS_B.Memory1_b;
    scanned2 = RS_DS_CM11_SimulinkRS_P.SpdSat_LowerSat;
    hasNewData = RS_DS_CM11_SimulinkRS_P.SpdSat_UpperSat;
    if (scanned1 > hasNewData) {
      /* Saturate: '<S42>/SpdSat' */
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr = hasNewData;
    } else if (scanned1 < scanned2) {
      /* Saturate: '<S42>/SpdSat' */
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr = scanned2;
    } else {
      /* Saturate: '<S42>/SpdSat' */
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr = scanned1;
    }

    /* End of Saturate: '<S42>/SpdSat' */

    /* Memory: '<S43>/Memory1' */
    RS_DS_CM11_SimulinkRS_B.Memory1_e =
      RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_h;

    /* Memory: '<S43>/Memory' */
    RS_DS_CM11_SimulinkRS_B.Memory_m =
      RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_o;

    /* Sum: '<S43>/Add' */
    RS_DS_CM11_SimulinkRS_B.Add_i = RS_DS_CM11_SimulinkRS_B.Memory1_e -
      RS_DS_CM11_SimulinkRS_B.Memory_m;

    /* RelationalOperator: '<S44>/Compare' incorporates:
     *  Constant: '<S44>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare_e = (RS_DS_CM11_SimulinkRS_B.RefSpdkmhr <=
      RS_DS_CM11_SimulinkRS_P.CompareToConstant1_const);

    /* RelationalOperator: '<S45>/Compare' incorporates:
     *  Constant: '<S45>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare_eb = (RS_DS_CM11_SimulinkRS_B.RefSpdkmhr <=
      RS_DS_CM11_SimulinkRS_P.CompareToConstant3_const);

    /* UnitDelay: '<S46>/Delay Input1' */
    RS_DS_CM11_SimulinkRS_B.Uk1 = RS_DS_CM11_SimulinkRS_DW.DelayInput1_DSTATE;

    /* RelationalOperator: '<S48>/Compare' incorporates:
     *  Constant: '<S48>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare_px = (RS_DS_CM11_SimulinkRS_B.Add_i <
      RS_DS_CM11_SimulinkRS_P.Constant_Value);

    /* RelationalOperator: '<S46>/FixPt Relational Operator' */
    RS_DS_CM11_SimulinkRS_B.FixPtRelationalOperator = ((int32_T)
      RS_DS_CM11_SimulinkRS_B.Compare_px > (int32_T)RS_DS_CM11_SimulinkRS_B.Uk1);

    /* UnitDelay: '<S47>/Delay Input1' */
    RS_DS_CM11_SimulinkRS_B.Uk1_l =
      RS_DS_CM11_SimulinkRS_DW.DelayInput1_DSTATE_l;

    /* RelationalOperator: '<S49>/Compare' incorporates:
     *  Constant: '<S49>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare_c = (RS_DS_CM11_SimulinkRS_B.Add_i >
      RS_DS_CM11_SimulinkRS_P.Constant_Value_c);

    /* RelationalOperator: '<S47>/FixPt Relational Operator' */
    RS_DS_CM11_SimulinkRS_B.FixPtRelationalOperator_a = ((int32_T)
      RS_DS_CM11_SimulinkRS_B.Compare_c > (int32_T)RS_DS_CM11_SimulinkRS_B.Uk1_l);

    /* Lookup_n-D: '<S43>/Feedforward' incorporates:
     *  Saturate: '<S42>/SpdSat'
     */
    RS_DS_CM11_SimulinkRS_B.Feedforward = look1_binlcpw
      (RS_DS_CM11_SimulinkRS_B.RefSpdkmhr,
       RS_DS_CM11_SimulinkRS_P.Feedforward_bp01Data,
       RS_DS_CM11_SimulinkRS_P.Feedforward_tableData, 16U);

    /* Gain: '<S43>/FeedforwardGain' */
    RS_DS_CM11_SimulinkRS_B.Driver_FdfwdTerm_pct =
      RS_DS_CM11_SimulinkRS_P.FeedforwardGain_Gain *
      RS_DS_CM11_SimulinkRS_B.Feedforward;

    /* Logic: '<S43>/Logical Operator' */
    RS_DS_CM11_SimulinkRS_B.LogicalOperator =
      (RS_DS_CM11_SimulinkRS_B.Compare_eb ||
       RS_DS_CM11_SimulinkRS_B.FixPtRelationalOperator_a ||
       RS_DS_CM11_SimulinkRS_B.FixPtRelationalOperator);

    /* End of Outputs for SubSystem: '<S6>/Driver and Drive Cycle' */
  }

  /* Outputs for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
  /* Integrator: '<S43>/Limits [-50,50]' */
  /* Limited  Integrator  */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    rEQ0 = (((RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE_b == 1) !=
             (int32_T)RS_DS_CM11_SimulinkRS_B.LogicalOperator) &&
            (RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE_b != 3));
    RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE_b =
      RS_DS_CM11_SimulinkRS_B.LogicalOperator;

    /* evaluate zero-crossings */
    if (rEQ0) {
      RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l =
        RS_DS_CM11_SimulinkRS_P.Limits5050_IC;
    }
  }

  if (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l >=
      RS_DS_CM11_SimulinkRS_P.Limits5050_UpperSat) {
    RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l =
      RS_DS_CM11_SimulinkRS_P.Limits5050_UpperSat;
  } else if (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l <=
             RS_DS_CM11_SimulinkRS_P.Limits5050_LowerSat) {
    RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l =
      RS_DS_CM11_SimulinkRS_P.Limits5050_LowerSat;
  }

  /* Integrator: '<S43>/Limits [-50,50]' */
  RS_DS_CM11_SimulinkRS_B.Driver_IntTerm_pct_b =
    RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l;

  /* TransferFcn: '<S43>/Lowpass Filter' */
  RS_DS_CM11_SimulinkRS_B.VS_Flt = 0.0;
  RS_DS_CM11_SimulinkRS_B.VS_Flt += RS_DS_CM11_SimulinkRS_P.LowpassFilter_C *
    RS_DS_CM11_SimulinkRS_X.LowpassFilter_CSTATE;

  /* Sum: '<S43>/Sum' */
  RS_DS_CM11_SimulinkRS_B.SpdErr_j = RS_DS_CM11_SimulinkRS_B.RefSpdkmhr -
    RS_DS_CM11_SimulinkRS_B.VS_Flt;

  /* Gain: '<S43>/Kp' */
  RS_DS_CM11_SimulinkRS_B.Kp = RS_DS_CM11_SimulinkRS_P.Kp_Gain *
    RS_DS_CM11_SimulinkRS_B.SpdErr_j;
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Lookup_n-D: '<S43>/Proportional Gain Scheduling' incorporates:
     *  Memory: '<S42>/Memory2'
     */
    RS_DS_CM11_SimulinkRS_B.ProportionalGainScheduling = look1_plinlcapw
      (RS_DS_CM11_SimulinkRS_B.Memory2_o,
       RS_DS_CM11_SimulinkRS_P.ProportionalGainScheduling_bp01Data,
       RS_DS_CM11_SimulinkRS_P.ProportionalGainScheduling_tableData,
       &RS_DS_CM11_SimulinkRS_DW.m_bpIndex, 2U);
  }

  /* Product: '<S43>/Product1' */
  RS_DS_CM11_SimulinkRS_B.Driver_PropTerm_pct_i = RS_DS_CM11_SimulinkRS_B.Kp *
    RS_DS_CM11_SimulinkRS_B.ProportionalGainScheduling;

  /* Sum: '<S43>/Sum1' */
  RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct_l =
    (RS_DS_CM11_SimulinkRS_B.Driver_IntTerm_pct_b +
     RS_DS_CM11_SimulinkRS_B.Driver_PropTerm_pct_i) +
    RS_DS_CM11_SimulinkRS_B.Driver_FdfwdTerm_pct;

  /* Gain: '<S43>/Gain' */
  RS_DS_CM11_SimulinkRS_B.Gain_d = RS_DS_CM11_SimulinkRS_P.Gain_Gain *
    RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct_l;

  /* Gain: '<S43>/Ki' */
  RS_DS_CM11_SimulinkRS_B.Ki_g = RS_DS_CM11_SimulinkRS_P.Ki_Gain *
    RS_DS_CM11_SimulinkRS_B.SpdErr_j;

  /* Switch: '<S43>/Switch2' */
  if (RS_DS_CM11_SimulinkRS_B.Gain_d > RS_DS_CM11_SimulinkRS_P.Switch2_Threshold)
  {
    /* Switch: '<S43>/Switch2' */
    RS_DS_CM11_SimulinkRS_B.BrakePedalPosPre_pct =
      RS_DS_CM11_SimulinkRS_B.Gain_d;
  } else {
    /* Switch: '<S43>/Switch2' */
    RS_DS_CM11_SimulinkRS_B.BrakePedalPosPre_pct = 0.0;
  }

  /* End of Switch: '<S43>/Switch2' */

  /* RelationalOperator: '<S43>/Relational Operator' incorporates:
   *  Constant: '<S43>/Min threshold for Brake switch'
   */
  RS_DS_CM11_SimulinkRS_B.BrakeSwitch_m =
    (RS_DS_CM11_SimulinkRS_B.BrakePedalPosPre_pct >
     RS_DS_CM11_SimulinkRS_P.MinthresholdforBrakeswitch_Value);

  /* Switch: '<S43>/Switch1' */
  if (RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct_l >
      RS_DS_CM11_SimulinkRS_P.Switch1_Threshold) {
    /* Switch: '<S43>/Switch1' */
    RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPosPre_pct =
      RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct_l;
  } else {
    /* Switch: '<S43>/Switch1' */
    RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPosPre_pct = 0.0;
  }

  /* End of Switch: '<S43>/Switch1' */

  /* Switch: '<S43>/Switch3' incorporates:
   *  Switch: '<S43>/Switch4'
   */
  if (RS_DS_CM11_SimulinkRS_B.Compare_e) {
    /* Switch: '<S43>/Switch3' incorporates:
     *  Constant: '<S43>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPos_pct =
      RS_DS_CM11_SimulinkRS_P.Constant_Value_b;

    /* Switch: '<S43>/Switch4' incorporates:
     *  Constant: '<S43>/standstill brake [0,100]'
     */
    RS_DS_CM11_SimulinkRS_B.BrakePedalPos_pct =
      RS_DS_CM11_SimulinkRS_P.standstillbrake0100_Value;
  } else {
    /* Switch: '<S43>/Switch3' */
    RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPos_pct =
      RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPosPre_pct;

    /* Switch: '<S43>/Switch4' */
    RS_DS_CM11_SimulinkRS_B.BrakePedalPos_pct =
      RS_DS_CM11_SimulinkRS_B.BrakePedalPosPre_pct;
  }

  /* End of Switch: '<S43>/Switch3' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Gain: '<S42>/km to miles' */
    RS_DS_CM11_SimulinkRS_B.RefSpdmph = RS_DS_CM11_SimulinkRS_P.kmtomiles_Gain *
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr;

    /* Gain: '<S15>/Gain' */
    RS_DS_CM11_SimulinkRS_B.Vehicle_Spd_mph =
      RS_DS_CM11_SimulinkRS_P.Gain_Gain_h *
      RS_DS_CM11_SimulinkRS_B.Vehicle_Spd_kph;

    /* RateLimiter: '<S15>/Rate Limiter' incorporates:
     *  Constant: '<S15>/ManSpd'
     */
    scanned1 = RS_DS_CM11_SimulinkRS_P.ManSpd_Value -
      RS_DS_CM11_SimulinkRS_DW.PrevY_a;
    if (scanned1 > RS_DS_CM11_SimulinkRS_P.RateLimiter_RisingLim *
        RS_DS_CM11_SimulinkRS_period) {
      /* RateLimiter: '<S15>/Rate Limiter' */
      RS_DS_CM11_SimulinkRS_B.RateLimiter =
        RS_DS_CM11_SimulinkRS_P.RateLimiter_RisingLim *
        RS_DS_CM11_SimulinkRS_period + RS_DS_CM11_SimulinkRS_DW.PrevY_a;
    } else if (scanned1 < RS_DS_CM11_SimulinkRS_P.RateLimiter_FallingLim *
               RS_DS_CM11_SimulinkRS_period) {
      /* RateLimiter: '<S15>/Rate Limiter' */
      RS_DS_CM11_SimulinkRS_B.RateLimiter =
        RS_DS_CM11_SimulinkRS_P.RateLimiter_FallingLim *
        RS_DS_CM11_SimulinkRS_period + RS_DS_CM11_SimulinkRS_DW.PrevY_a;
    } else {
      /* RateLimiter: '<S15>/Rate Limiter' */
      RS_DS_CM11_SimulinkRS_B.RateLimiter = RS_DS_CM11_SimulinkRS_P.ManSpd_Value;
    }

    RS_DS_CM11_SimulinkRS_DW.PrevY_a = RS_DS_CM11_SimulinkRS_B.RateLimiter;

    /* End of RateLimiter: '<S15>/Rate Limiter' */

    /* MultiPortSwitch: '<S15>/Multiport Switch1' incorporates:
     *  Constant: '<S15>/ManSpdSel'
     */
    switch ((int32_T)RS_DS_CM11_SimulinkRS_P.ManSpdSel_Value) {
     case 1:
      /* MultiPortSwitch: '<S15>/Multiport Switch1' */
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr_n = RS_DS_CM11_SimulinkRS_B.RateLimiter;
      break;

     case 2:
      /* MultiPortSwitch: '<S15>/Multiport Switch1' */
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr_n = RS_DS_CM11_SimulinkRS_B.VehSpdKph;
      break;

     default:
      /* MultiPortSwitch: '<S15>/Multiport Switch1' */
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr_n = RS_DS_CM11_SimulinkRS_B.VehSpdKph_p;
      break;
    }

    /* End of MultiPortSwitch: '<S15>/Multiport Switch1' */

    /* Gain: '<S15>/Gain1' */
    RS_DS_CM11_SimulinkRS_B.Vehicle_SpdDmd_mph =
      RS_DS_CM11_SimulinkRS_P.Gain1_Gain * RS_DS_CM11_SimulinkRS_B.RefSpdkmhr_n;

    /* Gain: '<S12>/Gain2' */
    RS_DS_CM11_SimulinkRS_B.Gain2 = RS_DS_CM11_SimulinkRS_P.Gain2_Gain *
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[10];
  }

  /* End of Outputs for SubSystem: '<S6>/Driver and Drive Cycle' */

  /* MultiPortSwitch: '<S12>/Multiport Switch1' incorporates:
   *  Constant: '<S12>/Driver Control Selection'
   */
  switch ((int32_T)RS_DS_CM11_SimulinkRS_P.DriverControlSelection_Value) {
   case 1:
    /* MultiPortSwitch: '<S12>/Multiport Switch1' incorporates:
     *  Constant: '<S12>/APP'
     */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch1 = RS_DS_CM11_SimulinkRS_P.APP_Value;
    break;

   case 2:
    /* MultiPortSwitch: '<S12>/Multiport Switch1' */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch1 =
      RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPos_pct;
    break;

   case 3:
    /* MultiPortSwitch: '<S12>/Multiport Switch1' */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch1 = RS_DS_CM11_SimulinkRS_B.Gain2;
    break;

   default:
    /* MultiPortSwitch: '<S12>/Multiport Switch1' incorporates:
     *  Constant: '<S12>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch1 =
      RS_DS_CM11_SimulinkRS_P.Constant_Value_k;
    break;
  }

  /* End of MultiPortSwitch: '<S12>/Multiport Switch1' */

  /* Saturate: '<S12>/AR_limit' */
  scanned1 = RS_DS_CM11_SimulinkRS_B.MultiportSwitch1;
  scanned2 = RS_DS_CM11_SimulinkRS_P.AR_limit_LowerSat;
  hasNewData = RS_DS_CM11_SimulinkRS_P.AR_limit_UpperSat;
  if (scanned1 > hasNewData) {
    /* Saturate: '<S12>/AR_limit' */
    RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct_g = hasNewData;
  } else if (scanned1 < scanned2) {
    /* Saturate: '<S12>/AR_limit' */
    RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct_g = scanned2;
  } else {
    /* Saturate: '<S12>/AR_limit' */
    RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct_g = scanned1;
  }

  /* End of Saturate: '<S12>/AR_limit' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Gain: '<S12>/Gain1' */
    RS_DS_CM11_SimulinkRS_B.Gain1_p = RS_DS_CM11_SimulinkRS_P.Gain1_Gain_h *
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[11];
  }

  /* MultiPortSwitch: '<S12>/Multiport Switch2' incorporates:
   *  Constant: '<S12>/Driver Control Selection'
   */
  switch ((int32_T)RS_DS_CM11_SimulinkRS_P.DriverControlSelection_Value) {
   case 1:
    /* MultiPortSwitch: '<S12>/Multiport Switch2' incorporates:
     *  Constant: '<S12>/BR_command_0to100'
     */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch2 =
      RS_DS_CM11_SimulinkRS_P.BR_command_0to100_Value;
    break;

   case 2:
    /* MultiPortSwitch: '<S12>/Multiport Switch2' */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch2 =
      RS_DS_CM11_SimulinkRS_B.BrakePedalPos_pct;
    break;

   case 3:
    /* MultiPortSwitch: '<S12>/Multiport Switch2' */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch2 = RS_DS_CM11_SimulinkRS_B.Gain1_p;
    break;

   default:
    /* MultiPortSwitch: '<S12>/Multiport Switch2' incorporates:
     *  Constant: '<S12>/Constant1'
     */
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch2 =
      RS_DS_CM11_SimulinkRS_P.Constant1_Value_o;
    break;
  }

  /* End of MultiPortSwitch: '<S12>/Multiport Switch2' */

  /* Saturate: '<S12>/BR_limit' */
  scanned1 = RS_DS_CM11_SimulinkRS_B.MultiportSwitch2;
  scanned2 = RS_DS_CM11_SimulinkRS_P.BR_limit_LowerSat;
  hasNewData = RS_DS_CM11_SimulinkRS_P.BR_limit_UpperSat;
  if (scanned1 > hasNewData) {
    /* Saturate: '<S12>/BR_limit' */
    RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct_a = hasNewData;
  } else if (scanned1 < scanned2) {
    /* Saturate: '<S12>/BR_limit' */
    RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct_a = scanned2;
  } else {
    /* Saturate: '<S12>/BR_limit' */
    RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct_a = scanned1;
  }

  /* End of Saturate: '<S12>/BR_limit' */

  /* RelationalOperator: '<S12>/Relational Operator' incorporates:
   *  Constant: '<S12>/Min threshold for Brake switch'
   */
  RS_DS_CM11_SimulinkRS_B.BrakeSwitch =
    (RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct_a >
     RS_DS_CM11_SimulinkRS_P.MinthresholdforBrakeswitch_Value_c);
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Gain: '<S6>/Gain2' */
    RS_DS_CM11_SimulinkRS_B.VehSpdMph = RS_DS_CM11_SimulinkRS_P.Gain2_Gain_b *
      RS_DS_CM11_SimulinkRS_B.VehSpd;

    /* RelationalOperator: '<S17>/Compare' incorporates:
     *  Constant: '<S17>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare =
      (RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o2 !=
       RS_DS_CM11_SimulinkRS_P.VehicleSource_BuiltIn_const);

    /* Stop: '<S6>/Stop Simulation' */
    if (RS_DS_CM11_SimulinkRS_B.Compare) {
      rtmSetStopRequested(RS_DS_CM11_SimulinkRS_M, 1);
    }

    /* End of Stop: '<S6>/Stop Simulation' */

    /* S-Function (read_dict): '<S7>/Read CM Dict7' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict7_PWORK_m.Entry;
      RS_DS_CM11_SimulinkRS_B.SimCore_State_b = e->GetFunc(e->Var);
    }

    /* Switch: '<S7>/Switch' */
    if (RS_DS_CM11_SimulinkRS_B.SimCore_State_b >=
        RS_DS_CM11_SimulinkRS_P.Switch_Threshold_i) {
      /* Switch: '<S7>/Switch' incorporates:
       *  Constant: '<S7>/Constant'
       */
      RS_DS_CM11_SimulinkRS_B.connectRequest =
        RS_DS_CM11_SimulinkRS_P.Constant_Value_a;
    } else {
      /* Switch: '<S7>/Switch' incorporates:
       *  Constant: '<S7>/RS_connectRequest'
       */
      RS_DS_CM11_SimulinkRS_B.connectRequest =
        RS_DS_CM11_SimulinkRS_P.RS_connectRequest_Value;
    }

    /* End of Switch: '<S7>/Switch' */

    /* DataTypeConversion: '<S7>/Data Type Conversion5' */
    RS_DS_CM11_SimulinkRS_B.DataTypeConversion5 =
      (RS_DS_CM11_SimulinkRS_B.connectRequest != 0.0);

    /* S-Function (dsmpb_dataoutport): '<S51>/Data Outport S-Fcn' */

    /* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/Status//Control_Out [TCP (1)] non-bus port 1 */
    ap_write_RS_DS_CM11_SimulinkRS_DataOutport2_P1_S1
      (&RS_DS_CM11_SimulinkRS_B.DataTypeConversion5);

    /* MATLAB Function: '<S7>/MATLAB Function' */
    /* MATLAB Function 'CarMaker/RealSim Core/MATLAB Function': '<S50>:1' */
    /* '<S50>:1:3' */
    for (low_i = 0; low_i < 50; low_i++) {
      strVec[low_i] = ' ';
    }

    /* '<S50>:1:4' */
    for (nyrows = 0; nyrows < (int32_T)
         RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdLength; nyrows++) {
      /* '<S50>:1:4' */
      /* '<S50>:1:5' */
      strVec[nyrows] = (int8_T)
        RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkId[nyrows];
    }

    if (RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdLength > 0.0) {
      /* '<S50>:1:7' */
      if (1.0 > RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdLength) {
        low_i = -1;
      } else {
        low_i = (int32_T)RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdLength -
          1;
      }

      ONE = low_i + 1;

      /* '<S50>:1:8' */
      hasNewData = (rtNaN);
      if ((int8_T)ONE >= 1) {
        xend = 0;
        nyrows = 1;
        while ((nyrows <= (int8_T)ONE) && (d[(uint8_T)strVec[nyrows - 1] & 127] ||
                (strVec[nyrows - 1] == '\x00'))) {
          nyrows++;
        }

        tmp = (int8_T)ONE;
        low_i = (int8_T)(tmp + 2) - 1;
        if (0 <= low_i) {
          memset(&s1_data[0], 0, (low_i + 1) * sizeof(char_T));
        }

        low_ip1 = 1;
        low_i = nyrows;
        if (0 <= ONE - 1) {
          memcpy(&strVec_data[0], &strVec[0], ONE * sizeof(char_T));
        }

        RS_DS_CM11_SimulinkRS_readfloat(s1_data, &low_ip1, strVec_data, &low_i,
          (int32_T)(int8_T)ONE, &rEQ0, &p, &scanned1, &p_0, &success);
        if (p) {
          xend = 1;
        }

        if (success && (low_i <= (int8_T)ONE)) {
          s1_data[low_ip1 - 1] = ' ';
          low_ip1++;
          if (0 <= ONE - 1) {
            memcpy(&strVec_data[0], &strVec[0], ONE * sizeof(char_T));
          }

          RS_DS_CM11_SimulinkRS_readfloat(s1_data, &low_ip1, strVec_data, &low_i,
            (int32_T)(int8_T)ONE, &p_0, &success, &scanned2, &foundsign,
            &c_success);
          if (success) {
            xend++;
          }

          if (c_success && (low_i > (int8_T)ONE) && ((rEQ0 != p_0) && foundsign))
          {
            success = true;
          } else {
            success = false;
          }
        } else {
          scanned2 = 0.0;
        }

        if (success) {
          s1_data[low_ip1 - 1] = '\x00';
          if (xend == 2) {
            nyrows = sscanf(&s1_data[0], "%lf %lf", &scanned1, &scanned2);
            if (nyrows != 2) {
              scanned1 = (rtNaN);
              scanned2 = (rtNaN);
            }
          } else if (xend == 1) {
            nyrows = sscanf(&s1_data[0], "%lf", &b_scanned1);
            if (nyrows != 1) {
              b_scanned1 = (rtNaN);
            }

            if (p) {
              scanned1 = b_scanned1;
            } else {
              scanned2 = b_scanned1;
            }
          }

          if (rEQ0) {
            hasNewData = scanned2;
          } else {
            hasNewData = scanned1;
          }
        }
      }

      RS_DS_CM11_SimulinkRS_B.intVal = hasNewData;
    } else {
      /* '<S50>:1:10' */
      RS_DS_CM11_SimulinkRS_B.intVal = -1.0;
    }

    /* End of MATLAB Function: '<S7>/MATLAB Function' */
    /* SignalConversion generated from: '<S7>/Bus Selector2' */
    RS_DS_CM11_SimulinkRS_B.heading =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.heading;
    for (low_i = 0; low_i < 50; low_i++) {
      /* SignalConversion generated from: '<S7>/Bus Selector2' */
      RS_DS_CM11_SimulinkRS_B.signalLightId[low_i] =
        RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightId[low_i];
    }

    /* SignalConversion generated from: '<S7>/Bus Selector2' */
    RS_DS_CM11_SimulinkRS_B.signalLightHeadId =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightHeadId;

    /* SignalConversion generated from: '<S7>/Bus Selector2' */
    RS_DS_CM11_SimulinkRS_B.signalLightDistance =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightDistance;

    /* SignalConversion generated from: '<S7>/Bus Selector2' */
    RS_DS_CM11_SimulinkRS_B.signalLightColor =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightColor;

    /* SignalConversion generated from: '<S7>/Bus Selector2' */
    RS_DS_CM11_SimulinkRS_B.speedLimit =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimit;

    /* SignalConversion generated from: '<S7>/Bus Selector2' */
    RS_DS_CM11_SimulinkRS_B.speedLimitNext =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimitNext;

    /* SignalConversion generated from: '<S7>/Bus Selector2' */
    RS_DS_CM11_SimulinkRS_B.speedLimitChangeDistance =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimitChangeDistance;

    /* S-Function (read_dict): '<S8>/Read CM Dict1' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict1_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict1 = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S8>/Read CM Dict6' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict6_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict6 = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S8>/Read CM Dict7' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict7_PWORK_f.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict7 = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S8>/Read CM Dict' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict_PWORK_a.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S8>/Read CM Dict4' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict4_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict4 = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S8>/Read CM Dict5' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict5_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict5 = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S8>/Read CM Dict3' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict3_PWORK_i.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict3 = e->GetFunc(e->Var);
    }

    /* MATLAB Function: '<S8>/MATLAB Function' incorporates:
     *  Constant: '<S8>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.positionZ = RS_DS_CM11_SimulinkRS_B.ReadCMDict5;
    RS_DS_CM11_SimulinkRS_B.speedDesired_i = RS_DS_CM11_SimulinkRS_B.ReadCMDict2;

    /* MATLAB Function 'CarMaker/RealSim Translate CM Signal to Bus as Feedback to SUMO//VISSIM/MATLAB Function': '<S60>:1' */
    /* '<S60>:1:16' */
    RS_DS_CM11_SimulinkRS_B.positionX = (RS_DS_CM11_SimulinkRS_B.ReadCMDict1 -
      RS_DS_CM11_SimulinkRS_B.ReadCMDict) + RS_DS_CM11_SimulinkRS_B.ReadCMDict1;

    /* '<S60>:1:17' */
    RS_DS_CM11_SimulinkRS_B.positionY = (RS_DS_CM11_SimulinkRS_B.ReadCMDict6 -
      RS_DS_CM11_SimulinkRS_B.ReadCMDict4) + RS_DS_CM11_SimulinkRS_B.ReadCMDict6;

    /* '<S60>:1:18' */
    if ((RS_DS_CM11_SimulinkRS_B.ReadCMDict3 >= -3.1415926535897931) &&
        (RS_DS_CM11_SimulinkRS_B.ReadCMDict3 <= 1.5707963267948966)) {
      /* '<S60>:1:21' */
      /* '<S60>:1:22' */
      RS_DS_CM11_SimulinkRS_B.heading_b = (-RS_DS_CM11_SimulinkRS_B.ReadCMDict3
        + 1.5707963267948966) * 180.0 / 3.1415926535897931;
    } else {
      /* '<S60>:1:24' */
      RS_DS_CM11_SimulinkRS_B.heading_b = (-RS_DS_CM11_SimulinkRS_B.ReadCMDict3
        + 7.8539816339744828) * 180.0 / 3.1415926535897931;
    }

    /* '<S60>:1:28' */
    scanned1 = RS_DS_CM11_SimulinkRS_P.Constant_Value_c0[0] * 1.6777216E+7;
    scanned2 = RS_DS_CM11_SimulinkRS_P.Constant_Value_c0[1] * 65536.0;
    hasNewData = RS_DS_CM11_SimulinkRS_P.Constant_Value_c0[2] * 256.0;
    k = RS_DS_CM11_SimulinkRS_P.Constant_Value_c0[3];
    RS_DS_CM11_SimulinkRS_B.color = scanned1;
    RS_DS_CM11_SimulinkRS_B.color += scanned2;
    RS_DS_CM11_SimulinkRS_B.color += hasNewData;
    RS_DS_CM11_SimulinkRS_B.color += k;

    /* End of MATLAB Function: '<S8>/MATLAB Function' */

    /* BusCreator generated from: '<S7>/MATLAB System1' incorporates:
     *  Constant: '<S61>/activeLaneChange'
     */
    /* '<S60>:1:31' */
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.idLength
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.idLength;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.typeLength
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.typeLength;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.vehicleClassLength
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.vehicleClassLength;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speed
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speed;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.acceleration
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.acceleration;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.positionX
      = RS_DS_CM11_SimulinkRS_B.positionX;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.positionY
      = RS_DS_CM11_SimulinkRS_B.positionY;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.positionZ
      = RS_DS_CM11_SimulinkRS_B.positionZ;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.heading
      = RS_DS_CM11_SimulinkRS_B.heading_b;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.color
      = RS_DS_CM11_SimulinkRS_B.color;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkIdLength
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdLength;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.laneId
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.laneId;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.distanceTravel
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.distanceTravel;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedDesired
      = RS_DS_CM11_SimulinkRS_B.speedDesired_i;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.accelerationDesired
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.accelerationDesired;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.hasPrecedingVehicle
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.hasPrecedingVehicle;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleIdLength
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleIdLength;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleDistance
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleDistance;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleSpeed
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleSpeed;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightIdLength
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightIdLength;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightHeadId
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightHeadId;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightDistance
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightDistance;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightColor
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightColor;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedLimit
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimit;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedLimitNext
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimitNext;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedLimitChangeDistance
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.speedLimitChangeDistance;
    for (low_i = 0; low_i < 50; low_i++) {
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.id
        [low_i] = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.id[low_i];
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.type
        [low_i] = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.type[low_i];
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.vehicleClass
        [low_i] = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.vehicleClass[low_i];
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkId
        [low_i] = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkId[low_i];
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleId
        [low_i] =
        RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.precedingVehicleId[low_i];
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightId
        [low_i] = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.signalLightId[low_i];
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkIdNext
        [low_i] = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdNext[low_i];
    }

    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkIdNextLength
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.linkIdNextLength;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.grade
      = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4.grade;
    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.activeLaneChange
      = RS_DS_CM11_SimulinkRS_P.activeLaneChange_Value;

    /* End of BusCreator generated from: '<S7>/MATLAB System1' */

    /* MATLABSystem: '<S7>/MATLAB System1' incorporates:
     *  Constant: '<S7>/simState'
     *  MATLABSystem: '<S7>/MATLAB System'
     */
    VehicleMessageFieldDefInputVec =
      &RS_DS_CM11_SimulinkRS_P.VehicleMessageFieldDefInputVec[0];
    scanned2 = RS_DS_CM11_SimulinkRS_P.simState_Value;
    b_scanned1 = RS_DS_CM11_SimulinkRS_B.MATLABSystem_o2;
    b_varargout_4_idLength =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.idLength;
    ZERO =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.typeLength;
    onemr =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.vehicleClassLength;
    xtmp =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speed;
    hasNewData =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.acceleration;
    xnp1 =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.positionX;
    maxx =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.positionY;
    b_NAN =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.positionZ;
    b_varargout_4_heading =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.heading;
    b_varargout_4_color =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.color;
    b_varargout_4_linkIdLength =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkIdLength;
    b_varargout_4_laneId =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.laneId;
    b_varargout_4_distanceTravel =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.distanceTravel;
    b_varargout_4_speedDesired =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedDesired;
    b_varargout_4_accelerationDesired =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.accelerationDesired;
    b_varargout_4_hasPrecedingVehicle =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.hasPrecedingVehicle;
    b_varargout_4_precedingVehicleIdLength =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleIdLength;
    b_varargout_4_precedingVehicleDistance =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleDistance;
    b_varargout_4_precedingVehicleSpeed =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleSpeed;
    b_varargout_4_signalLightIdLength =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightIdLength;
    b_varargout_4_signalLightHeadId =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightHeadId;
    b_varargout_4_signalLightDistance =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightDistance;
    b_varargout_4_signalLightColor =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightColor;
    b_varargout_4_speedLimit =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedLimit;
    b_varargout_4_speedLimitNext =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedLimitNext;
    b_varargout_4_speedLimitChangeDistance =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.speedLimitChangeDistance;
    for (low_i = 0; low_i < 50; low_i++) {
      expl_temp.id[low_i] =
        RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.id
        [low_i];
      expl_temp.type[low_i] =
        RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.type
        [low_i];
      expl_temp.vehicleClass[low_i] =
        RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.vehicleClass
        [low_i];
      expl_temp.linkId[low_i] =
        RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkId
        [low_i];
      expl_temp.precedingVehicleId[low_i] =
        RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.precedingVehicleId
        [low_i];
      expl_temp.signalLightId[low_i] =
        RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.signalLightId
        [low_i];
      expl_temp.linkIdNext[low_i] =
        RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkIdNext
        [low_i];
    }

    b_varargout_4_linkIdNextLength =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.linkIdNextLength;
    b_varargout_4_grade =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.grade;
    b_varargout_4_activeLaneChange =
      RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1.activeLaneChange;
    varargin_1_0 =
      &RS_DS_CM11_SimulinkRS_DW.obj_l.VehicleMessageFieldDefInputVec[0];
    rEQ0 = false;
    p = true;
    low_i = 0;
    exitg1 = false;
    while ((!exitg1) && (low_i < 29)) {
      k = (real_T)low_i + 1.0;
      scanned1 = varargin_1_0[(int32_T)k - 1];
      k = VehicleMessageFieldDefInputVec[(int32_T)k - 1];
      p_0 = (scanned1 == k);
      if (!p_0) {
        p = false;
        exitg1 = true;
      } else {
        low_i++;
      }
    }

    if (p) {
      rEQ0 = true;
    }

    if (!rEQ0) {
      memcpy(&RS_DS_CM11_SimulinkRS_DW.obj_l.VehicleMessageFieldDefInputVec[0],
             &VehicleMessageFieldDefInputVec[0], 29U * sizeof(real_T));
    }

    obj_1 = &RS_DS_CM11_SimulinkRS_DW.obj_l;
    expl_temp.activeLaneChange = b_varargout_4_activeLaneChange;
    expl_temp.grade = b_varargout_4_grade;
    expl_temp.linkIdNextLength = b_varargout_4_linkIdNextLength;
    expl_temp.speedLimitChangeDistance = b_varargout_4_speedLimitChangeDistance;
    expl_temp.speedLimitNext = b_varargout_4_speedLimitNext;
    expl_temp.speedLimit = b_varargout_4_speedLimit;
    expl_temp.signalLightColor = b_varargout_4_signalLightColor;
    expl_temp.signalLightDistance = b_varargout_4_signalLightDistance;
    expl_temp.signalLightHeadId = b_varargout_4_signalLightHeadId;
    expl_temp.signalLightIdLength = b_varargout_4_signalLightIdLength;
    expl_temp.precedingVehicleSpeed = b_varargout_4_precedingVehicleSpeed;
    expl_temp.precedingVehicleDistance = b_varargout_4_precedingVehicleDistance;
    expl_temp.precedingVehicleIdLength = b_varargout_4_precedingVehicleIdLength;
    expl_temp.hasPrecedingVehicle = b_varargout_4_hasPrecedingVehicle;
    expl_temp.accelerationDesired = b_varargout_4_accelerationDesired;
    expl_temp.speedDesired = b_varargout_4_speedDesired;
    expl_temp.distanceTravel = b_varargout_4_distanceTravel;
    expl_temp.laneId = b_varargout_4_laneId;
    expl_temp.linkIdLength = b_varargout_4_linkIdLength;
    expl_temp.color = b_varargout_4_color;
    expl_temp.heading = b_varargout_4_heading;
    expl_temp.positionZ = b_NAN;
    expl_temp.positionY = maxx;
    expl_temp.positionX = xnp1;
    expl_temp.acceleration = hasNewData;
    expl_temp.speed = xtmp;
    expl_temp.vehicleClassLength = onemr;
    expl_temp.typeLength = ZERO;
    expl_temp.idLength = b_varargout_4_idLength;
    RS_DS_CM11_SimulinkRS_RealSimPack_stepImpl(obj_1, scanned2, b_scanned1,
      &expl_temp, b_varargout_1, &k);

    /* MATLABSystem: '<S7>/MATLAB System1' */
    memcpy(&RS_DS_CM11_SimulinkRS_B.ByteSend[0], &b_varargout_1[0], 200U *
           sizeof(uint8_T));

    /* MATLABSystem: '<S7>/MATLAB System1' */
    RS_DS_CM11_SimulinkRS_B.nMsgSend = k;

    /* DataTypeConversion: '<S53>/Data Type Conversion4' */
    scanned1 = floor(RS_DS_CM11_SimulinkRS_B.nMsgSend);
    if (rtIsNaN(scanned1) || rtIsInf(scanned1)) {
      scanned1 = 0.0;
    } else {
      scanned1 = fmod(scanned1, 4.294967296E+9);
    }

    /* DataTypeConversion: '<S53>/Data Type Conversion4' */
    RS_DS_CM11_SimulinkRS_B.DataTypeConversion4 = scanned1 < 0.0 ? (uint32_T)
      -(int32_T)(uint32_T)-scanned1 : (uint32_T)scanned1;
  }

  /* Switch: '<S53>/switch for 0.1 send out' incorporates:
   *  Constant: '<S53>/Constant1'
   */
  if (RS_DS_CM11_SimulinkRS_B.timeStepTrigger >
      RS_DS_CM11_SimulinkRS_P.switchfor01sendout_Threshold) {
    /* Switch: '<S53>/switch for 0.1 send out' */
    RS_DS_CM11_SimulinkRS_B.sendByteSize_RealSimPack =
      RS_DS_CM11_SimulinkRS_B.DataTypeConversion4;
  } else {
    scanned1 = floor(RS_DS_CM11_SimulinkRS_P.Constant1_Value_h);
    if (rtIsNaN(scanned1) || rtIsInf(scanned1)) {
      scanned1 = 0.0;
    } else {
      scanned1 = fmod(scanned1, 4.294967296E+9);
    }

    /* Switch: '<S53>/switch for 0.1 send out' incorporates:
     *  Constant: '<S53>/Constant1'
     */
    RS_DS_CM11_SimulinkRS_B.sendByteSize_RealSimPack = scanned1 < 0.0 ?
      (uint32_T)-(int32_T)(uint32_T)-scanned1 : (uint32_T)scanned1;
  }

  /* End of Switch: '<S53>/switch for 0.1 send out' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* S-Function (dsmpb_dataoutport): '<S59>/Data Outport S-Fcn' */

    /* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP Out/Transmit_Out [TCP (1)] non-bus port 1 */
    ap_write_RS_DS_CM11_SimulinkRS_DataOutport3_P1_S1
      (&RS_DS_CM11_SimulinkRS_B.ByteSend[0]);

    /* Write access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP Out/Transmit_Out [TCP (1)] non-bus port 2 */
    ap_write_RS_DS_CM11_SimulinkRS_DataOutport3_P2_S1
      (&RS_DS_CM11_SimulinkRS_B.sendByteSize_RealSimPack);

    /* S-Function (dsmpb_datainport): '<S54>/Data Inport S-Fcn' */

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Ethernet Setup (1) non-bus port 1 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport3_P1_S1
      (&RS_DS_CM11_SimulinkRS_B.LinkSpeed);

    /* S-Function (dsmpb_datainport): '<S58>/Data Inport S-Fcn' */

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Transmit_In [TCP (1)] non-bus port 1 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport4_P1_S1
      (&RS_DS_CM11_SimulinkRS_B.Status_l);

    /* Read access point of block RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Transmit_In [TCP (1)] non-bus port 2 */
    ap_read_RS_DS_CM11_SimulinkRS_DataInport4_P2_S1
      (&RS_DS_CM11_SimulinkRS_B.SentBytes);

    /* Constant: '<S8>/Constant3' */
    RS_DS_CM11_SimulinkRS_B.Constant3 = RS_DS_CM11_SimulinkRS_P.Constant3_Value;

    /* S-Function (read_dict): '<S10>/Read CM Dict11' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict11_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict11 = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S10>/Read CM Dict9' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict9_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_dSpeed = e->GetFunc(e->Var);
    }

    /* Sum: '<S10>/Add2' */
    RS_DS_CM11_SimulinkRS_B.Trf_Prec_Speed =
      RS_DS_CM11_SimulinkRS_B.ReadCMDict11 +
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_dSpeed;

    /* Product: '<S10>/Divide' incorporates:
     *  Constant: '<S10>/headway (s)'
     */
    RS_DS_CM11_SimulinkRS_B.Divide = RS_DS_CM11_SimulinkRS_B.Trf_Prec_Speed *
      RS_DS_CM11_SimulinkRS_P.headways_Value;

    /* Sum: '<S10>/Add3' incorporates:
     *  Constant: '<S10>/Constant1'
     */
    RS_DS_CM11_SimulinkRS_B.followingDistanceDesired =
      RS_DS_CM11_SimulinkRS_P.Constant1_Value_e + RS_DS_CM11_SimulinkRS_B.Divide;

    /* Memory: '<S62>/Memory' */
    RS_DS_CM11_SimulinkRS_B.Memory_g =
      RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_d;

    /* Sum: '<S62>/Add' */
    RS_DS_CM11_SimulinkRS_B.Add = RS_DS_CM11_SimulinkRS_B.CycleMode -
      RS_DS_CM11_SimulinkRS_B.Memory_g;

    /* S-Function (read_dict): '<S62>/Read CM Dict6' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict6_PWORK_i.Entry;
      RS_DS_CM11_SimulinkRS_B.ReadCMDict6_n = e->GetFunc(e->Var);
    }

    /* Memory: '<S62>/Memory1' */
    RS_DS_CM11_SimulinkRS_B.Memory1 =
      RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput;

    /* Sum: '<S62>/Add1' */
    RS_DS_CM11_SimulinkRS_B.Add1 = RS_DS_CM11_SimulinkRS_B.ReadCMDict6_n -
      RS_DS_CM11_SimulinkRS_B.Memory1;

    /* RelationalOperator: '<S63>/Compare' incorporates:
     *  Constant: '<S63>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare_f =
      (RS_DS_CM11_SimulinkRS_B.followingDistanceDesired <=
       RS_DS_CM11_SimulinkRS_P.CompareToConstant_const);

    /* S-Function (read_dict): '<S10>/Read CM Dict10' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict10_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_Targ_Dtct = e->GetFunc(e->Var);
    }

    /* Switch: '<S10>/Switch' */
    if (RS_DS_CM11_SimulinkRS_B.Trf_FOL_Targ_Dtct >
        RS_DS_CM11_SimulinkRS_P.Switch_Threshold_d) {
      /* Switch: '<S10>/Switch' */
      RS_DS_CM11_SimulinkRS_B.RefSpd = RS_DS_CM11_SimulinkRS_B.Trf_Prec_Speed;
    } else {
      /* Switch: '<S10>/Switch' incorporates:
       *  Constant: '<S10>/free speed'
       */
      RS_DS_CM11_SimulinkRS_B.RefSpd =
        RS_DS_CM11_SimulinkRS_P.RealSimPara.speedInit;
    }

    /* End of Switch: '<S10>/Switch' */

    /* RelationalOperator: '<S64>/Compare' incorporates:
     *  Constant: '<S64>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare_p = (RS_DS_CM11_SimulinkRS_B.RefSpd <=
      RS_DS_CM11_SimulinkRS_P.CompareToConstant1_const_f);

    /* RelationalOperator: '<S65>/Compare' incorporates:
     *  Constant: '<S65>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.Compare_m = (RS_DS_CM11_SimulinkRS_B.RefSpd <=
      RS_DS_CM11_SimulinkRS_P.CompareToConstant3_const_g);

    /* Logic: '<S62>/Logical Operator' */
    RS_DS_CM11_SimulinkRS_B.iTermReset = (RS_DS_CM11_SimulinkRS_B.Compare_m ||
      (RS_DS_CM11_SimulinkRS_B.Add != 0.0) || (RS_DS_CM11_SimulinkRS_B.Add1 !=
      0.0));
  }

  /* Integrator: '<S62>/Limits [-50,50]' */
  /* Limited  Integrator  */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    rEQ0 = (((RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE == 1) !=
             (int32_T)RS_DS_CM11_SimulinkRS_B.iTermReset) &&
            (RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE != 3));
    RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE =
      RS_DS_CM11_SimulinkRS_B.iTermReset;

    /* evaluate zero-crossings */
    if (rEQ0) {
      RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE =
        RS_DS_CM11_SimulinkRS_P.Limits5050_IC_a;
    }
  }

  if (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE >=
      RS_DS_CM11_SimulinkRS_P.Limits5050_UpperSat_n) {
    RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE =
      RS_DS_CM11_SimulinkRS_P.Limits5050_UpperSat_n;
  } else if (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE <=
             RS_DS_CM11_SimulinkRS_P.Limits5050_LowerSat_i) {
    RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE =
      RS_DS_CM11_SimulinkRS_P.Limits5050_LowerSat_i;
  }

  /* Integrator: '<S62>/Limits [-50,50]' */
  RS_DS_CM11_SimulinkRS_B.Driver_IntTerm_pct =
    RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE;
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Sum: '<S62>/Sum' */
    RS_DS_CM11_SimulinkRS_B.SpdErr = RS_DS_CM11_SimulinkRS_B.RefSpd -
      RS_DS_CM11_SimulinkRS_B.ReadCMDict11;

    /* Gain: '<S62>/Kp' */
    RS_DS_CM11_SimulinkRS_B.Driver_PropTerm_pct =
      RS_DS_CM11_SimulinkRS_P.Kp_Gain_o * RS_DS_CM11_SimulinkRS_B.SpdErr;
  }

  /* Integrator: '<S62>/Limits [-50,50]1' */
  /* Limited  Integrator  */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    rEQ0 = (((RS_DS_CM11_SimulinkRS_PrevZCX.Limits50501_Reset_ZCE == 1) !=
             (int32_T)RS_DS_CM11_SimulinkRS_B.iTermReset) &&
            (RS_DS_CM11_SimulinkRS_PrevZCX.Limits50501_Reset_ZCE != 3));
    RS_DS_CM11_SimulinkRS_PrevZCX.Limits50501_Reset_ZCE =
      RS_DS_CM11_SimulinkRS_B.iTermReset;

    /* evaluate zero-crossings */
    if (rEQ0) {
      RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE =
        RS_DS_CM11_SimulinkRS_P.Limits50501_IC;
    }
  }

  if (RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE >=
      RS_DS_CM11_SimulinkRS_P.Limits50501_UpperSat) {
    RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE =
      RS_DS_CM11_SimulinkRS_P.Limits50501_UpperSat;
  } else if (RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE <=
             RS_DS_CM11_SimulinkRS_P.Limits50501_LowerSat) {
    RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE =
      RS_DS_CM11_SimulinkRS_P.Limits50501_LowerSat;
  }

  /* Integrator: '<S62>/Limits [-50,50]1' */
  RS_DS_CM11_SimulinkRS_B.iTermDist = RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE;
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* S-Function (read_dict): '<S10>/Read CM Dict12' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict12_PWORK.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_dDist = e->GetFunc(e->Var);
    }

    /* Sum: '<S62>/Sum3' */
    RS_DS_CM11_SimulinkRS_B.distErr = RS_DS_CM11_SimulinkRS_B.Trf_FOL_dDist -
      RS_DS_CM11_SimulinkRS_B.followingDistanceDesired;

    /* Gain: '<S62>/Kp_d' */
    RS_DS_CM11_SimulinkRS_B.pTermDist = RS_DS_CM11_SimulinkRS_P.Kp_d_Gain *
      RS_DS_CM11_SimulinkRS_B.distErr;

    /* Logic: '<S62>/Logical Operator1' */
    RS_DS_CM11_SimulinkRS_B.LogicalOperator1 =
      ((RS_DS_CM11_SimulinkRS_B.ReadCMDict6_n != 0.0) &&
       RS_DS_CM11_SimulinkRS_B.Compare_f && RS_DS_CM11_SimulinkRS_B.Compare_p);
  }

  /* Sum: '<S62>/Sum2' */
  RS_DS_CM11_SimulinkRS_B.Sum2 = RS_DS_CM11_SimulinkRS_B.iTermDist +
    RS_DS_CM11_SimulinkRS_B.pTermDist;

  /* Switch: '<S62>/Switch' */
  if (RS_DS_CM11_SimulinkRS_B.LogicalOperator1) {
    /* Switch: '<S62>/Switch' */
    RS_DS_CM11_SimulinkRS_B.distCmd = RS_DS_CM11_SimulinkRS_B.Sum2;
  } else {
    /* Switch: '<S62>/Switch' incorporates:
     *  Constant: '<S62>/Constant'
     */
    RS_DS_CM11_SimulinkRS_B.distCmd = RS_DS_CM11_SimulinkRS_P.Constant_Value_kr;
  }

  /* End of Switch: '<S62>/Switch' */

  /* Sum: '<S62>/Sum1' */
  RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct =
    ((RS_DS_CM11_SimulinkRS_B.Driver_IntTerm_pct +
      RS_DS_CM11_SimulinkRS_B.Driver_PropTerm_pct) +
     RS_DS_CM11_SimulinkRS_B.ReadCMDict11) + RS_DS_CM11_SimulinkRS_B.distCmd;

  /* Saturate: '<S62>/Saturation' */
  scanned1 = RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct;
  scanned2 = RS_DS_CM11_SimulinkRS_P.Saturation_LowerSat;
  hasNewData = RS_DS_CM11_SimulinkRS_P.Saturation_UpperSat;
  if (scanned1 > hasNewData) {
    /* Saturate: '<S62>/Saturation' */
    RS_DS_CM11_SimulinkRS_B.Saturation = hasNewData;
  } else if (scanned1 < scanned2) {
    /* Saturate: '<S62>/Saturation' */
    RS_DS_CM11_SimulinkRS_B.Saturation = scanned2;
  } else {
    /* Saturate: '<S62>/Saturation' */
    RS_DS_CM11_SimulinkRS_B.Saturation = scanned1;
  }

  /* End of Saturate: '<S62>/Saturation' */

  /* RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
  if (RS_DS_CM11_SimulinkRS_DW.LastMajorTime == (rtInf)) {
    /* RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
    RS_DS_CM11_SimulinkRS_B.DrvSpeedCmdRateLimit =
      RS_DS_CM11_SimulinkRS_B.Saturation;
  } else {
    scanned2 = RS_DS_CM11_SimulinkRS_M->Timing.t[0] -
      RS_DS_CM11_SimulinkRS_DW.LastMajorTime;
    b_scanned1 = scanned2 *
      RS_DS_CM11_SimulinkRS_P.DrvSpeedCmdRateLimit_RisingLim;
    scanned1 = RS_DS_CM11_SimulinkRS_B.Saturation -
      RS_DS_CM11_SimulinkRS_DW.PrevY;
    if (scanned1 > b_scanned1) {
      /* RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
      RS_DS_CM11_SimulinkRS_B.DrvSpeedCmdRateLimit =
        RS_DS_CM11_SimulinkRS_DW.PrevY + b_scanned1;
    } else {
      scanned2 *= RS_DS_CM11_SimulinkRS_P.DrvSpeedCmdRateLimit_FallingLim;
      if (scanned1 < scanned2) {
        /* RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
        RS_DS_CM11_SimulinkRS_B.DrvSpeedCmdRateLimit =
          RS_DS_CM11_SimulinkRS_DW.PrevY + scanned2;
      } else {
        /* RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
        RS_DS_CM11_SimulinkRS_B.DrvSpeedCmdRateLimit =
          RS_DS_CM11_SimulinkRS_B.Saturation;
      }
    }
  }

  /* End of RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Gain: '<S62>/Ki' */
    RS_DS_CM11_SimulinkRS_B.Ki = RS_DS_CM11_SimulinkRS_P.Ki_Gain_o *
      RS_DS_CM11_SimulinkRS_B.SpdErr;

    /* Gain: '<S62>/Ki_d' */
    RS_DS_CM11_SimulinkRS_B.Ki_d = RS_DS_CM11_SimulinkRS_P.Ki_d_Gain *
      RS_DS_CM11_SimulinkRS_B.distErr;

    /* Memory: '<S62>/Memory2' */
    RS_DS_CM11_SimulinkRS_B.Memory2 =
      RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput;

    /* RateLimiter: '<S62>/followDistRateLimit' */
    scanned1 = 0.0 - RS_DS_CM11_SimulinkRS_DW.PrevY_m;
    if (scanned1 > RS_DS_CM11_SimulinkRS_P.followDistRateLimit_RisingLim *
        RS_DS_CM11_SimulinkRS_period) {
      /* RateLimiter: '<S62>/followDistRateLimit' */
      RS_DS_CM11_SimulinkRS_B.followDistRateLimit =
        RS_DS_CM11_SimulinkRS_P.followDistRateLimit_RisingLim *
        RS_DS_CM11_SimulinkRS_period + RS_DS_CM11_SimulinkRS_DW.PrevY_m;
    } else if (scanned1 < RS_DS_CM11_SimulinkRS_P.followDistRateLimit_FallingLim
               * RS_DS_CM11_SimulinkRS_period) {
      /* RateLimiter: '<S62>/followDistRateLimit' */
      RS_DS_CM11_SimulinkRS_B.followDistRateLimit =
        RS_DS_CM11_SimulinkRS_P.followDistRateLimit_FallingLim *
        RS_DS_CM11_SimulinkRS_period + RS_DS_CM11_SimulinkRS_DW.PrevY_m;
    } else {
      /* RateLimiter: '<S62>/followDistRateLimit' */
      RS_DS_CM11_SimulinkRS_B.followDistRateLimit = 0.0;
    }

    RS_DS_CM11_SimulinkRS_DW.PrevY_m =
      RS_DS_CM11_SimulinkRS_B.followDistRateLimit;

    /* End of RateLimiter: '<S62>/followDistRateLimit' */

    /* S-Function (read_dict): '<S1>/Read CM Dict' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict_PWORK_c.Entry;
      RS_DS_CM11_SimulinkRS_B.VehSpd_b = e->GetFunc(e->Var);
    }

    /* Gain: '<S1>/Gain1' */
    RS_DS_CM11_SimulinkRS_B.VehSpdKph_p4 = RS_DS_CM11_SimulinkRS_P.Gain1_Gain_d *
      RS_DS_CM11_SimulinkRS_B.VehSpd_b;

    /* Gain: '<S1>/Gain2' */
    RS_DS_CM11_SimulinkRS_B.VehSpdMph_g = RS_DS_CM11_SimulinkRS_P.Gain2_Gain_e *
      RS_DS_CM11_SimulinkRS_B.VehSpd_b;

    /* S-Function (read_dict): '<S1>/Read CM Dict1' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict1_PWORK_j.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_Targ_Dtct_l = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict2' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict2_PWORK_d.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_dDist_j = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict4' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict4_PWORK_b.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_dSpeed_i = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict5' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict5_PWORK_k.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_ObjId = e->GetFunc(e->Var);
    }

    /* S-Function (read_dict): '<S1>/Read CM Dict6' */
    {
      tDDictEntry *e;
      e = (tDDictEntry *)RS_DS_CM11_SimulinkRS_DW.ReadCMDict6_PWORK_j.Entry;
      RS_DS_CM11_SimulinkRS_B.Trf_FOL_State = e->GetFunc(e->Var);
    }
  }

  /* user code (Output function Trailer) */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Function module access point of system <Root>/CarMaker    */
    ap_exit_RS_DS_CM11_SimulinkRS_SIDS1_TID1();
  }

  /* End of Outputs for SubSystem: '<Root>/CarMaker' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
  }
}

/* Model update function */
void RS_DS_CM11_SimulinkRS_update(void)
{
  /* Update for Atomic SubSystem: '<Root>/CarMaker' */
  /* Update for RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
  RS_DS_CM11_SimulinkRS_DW.PrevY = RS_DS_CM11_SimulinkRS_B.DrvSpeedCmdRateLimit;
  RS_DS_CM11_SimulinkRS_DW.LastMajorTime = RS_DS_CM11_SimulinkRS_M->Timing.t[0];
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    /* Update for Memory generated from: '<S11>/Memory' */
    RS_DS_CM11_SimulinkRS_DW.Memory_2_PreviousInput =
      RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct_a;

    /* Update for Memory generated from: '<S11>/Memory' */
    RS_DS_CM11_SimulinkRS_DW.Memory_1_PreviousInput =
      RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct_g;

    /* Update for Memory: '<S7>/Memory' */
    RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput =
      RS_DS_CM11_SimulinkRS_B.MATLABSystem2_o1;

    /* Update for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
    /* Update for Memory: '<S42>/Memory1' */
    RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_e =
      RS_DS_CM11_SimulinkRS_B.RefSpdkmhr_n;

    /* Update for Memory: '<S42>/Memory2' */
    RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput_l =
      RS_DS_CM11_SimulinkRS_B.Vehicle_Spd_kph;

    /* Update for Memory: '<S43>/Memory1' */
    RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_h =
      RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPosPre_pct;

    /* Update for Memory: '<S43>/Memory' */
    RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_o =
      RS_DS_CM11_SimulinkRS_B.BrakePedalPosPre_pct;

    /* Update for UnitDelay: '<S46>/Delay Input1' */
    RS_DS_CM11_SimulinkRS_DW.DelayInput1_DSTATE =
      RS_DS_CM11_SimulinkRS_B.Compare_px;

    /* Update for UnitDelay: '<S47>/Delay Input1' */
    RS_DS_CM11_SimulinkRS_DW.DelayInput1_DSTATE_l =
      RS_DS_CM11_SimulinkRS_B.Compare_c;

    /* End of Update for SubSystem: '<S6>/Driver and Drive Cycle' */

    /* Update for Memory: '<S62>/Memory' */
    RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_d =
      RS_DS_CM11_SimulinkRS_B.CycleMode;

    /* Update for Memory: '<S62>/Memory1' */
    RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput =
      RS_DS_CM11_SimulinkRS_B.ReadCMDict6_n;

    /* Update for Memory: '<S62>/Memory2' */
    RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput =
      RS_DS_CM11_SimulinkRS_B.DrvSpeedCmdRateLimit;
  }

  /* End of Update for SubSystem: '<Root>/CarMaker' */
  if (rtmIsMajorTimeStep(RS_DS_CM11_SimulinkRS_M)) {
    rt_ertODEUpdateContinuousStates(&RS_DS_CM11_SimulinkRS_M->solverInfo);
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
  if (!(++RS_DS_CM11_SimulinkRS_M->Timing.clockTick0)) {
    ++RS_DS_CM11_SimulinkRS_M->Timing.clockTickH0;
  }

  RS_DS_CM11_SimulinkRS_M->Timing.t[0] = rtsiGetSolverStopTime
    (&RS_DS_CM11_SimulinkRS_M->solverInfo);

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
    RS_DS_CM11_SimulinkRS_M->Timing.clockTick1++;
    if (!RS_DS_CM11_SimulinkRS_M->Timing.clockTick1) {
      RS_DS_CM11_SimulinkRS_M->Timing.clockTickH1++;
    }
  }
}

/* Derivatives for root system: '<Root>' */
void RS_DS_CM11_SimulinkRS_derivatives(void)
{
  XDot_RS_DS_CM11_SimulinkRS_T *_rtXdot;
  boolean_T lsat;
  boolean_T usat;
  _rtXdot = ((XDot_RS_DS_CM11_SimulinkRS_T *) RS_DS_CM11_SimulinkRS_M->derivs);

  /* Derivatives for Atomic SubSystem: '<Root>/CarMaker' */
  /* Derivatives for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
  /* Derivatives for Integrator: '<S43>/Limits [-50,50]' */
  lsat = (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l <=
          RS_DS_CM11_SimulinkRS_P.Limits5050_LowerSat);
  usat = (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l >=
          RS_DS_CM11_SimulinkRS_P.Limits5050_UpperSat);
  if (((!lsat) && (!usat)) || (lsat && (RS_DS_CM11_SimulinkRS_B.Ki_g > 0.0)) ||
      (usat && (RS_DS_CM11_SimulinkRS_B.Ki_g < 0.0))) {
    _rtXdot->Limits5050_CSTATE_l = RS_DS_CM11_SimulinkRS_B.Ki_g;
  } else {
    /* in saturation */
    _rtXdot->Limits5050_CSTATE_l = 0.0;
  }

  /* End of Derivatives for Integrator: '<S43>/Limits [-50,50]' */

  /* Derivatives for TransferFcn: '<S43>/Lowpass Filter' */
  _rtXdot->LowpassFilter_CSTATE = 0.0;
  _rtXdot->LowpassFilter_CSTATE += RS_DS_CM11_SimulinkRS_P.LowpassFilter_A *
    RS_DS_CM11_SimulinkRS_X.LowpassFilter_CSTATE;
  _rtXdot->LowpassFilter_CSTATE += RS_DS_CM11_SimulinkRS_B.Memory2_o;

  /* End of Derivatives for SubSystem: '<S6>/Driver and Drive Cycle' */

  /* Derivatives for Integrator: '<S62>/Limits [-50,50]' */
  lsat = (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE <=
          RS_DS_CM11_SimulinkRS_P.Limits5050_LowerSat_i);
  usat = (RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE >=
          RS_DS_CM11_SimulinkRS_P.Limits5050_UpperSat_n);
  if (((!lsat) && (!usat)) || (lsat && (RS_DS_CM11_SimulinkRS_B.Ki > 0.0)) ||
      (usat && (RS_DS_CM11_SimulinkRS_B.Ki < 0.0))) {
    _rtXdot->Limits5050_CSTATE = RS_DS_CM11_SimulinkRS_B.Ki;
  } else {
    /* in saturation */
    _rtXdot->Limits5050_CSTATE = 0.0;
  }

  /* End of Derivatives for Integrator: '<S62>/Limits [-50,50]' */

  /* Derivatives for Integrator: '<S62>/Limits [-50,50]1' */
  lsat = (RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE <=
          RS_DS_CM11_SimulinkRS_P.Limits50501_LowerSat);
  usat = (RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE >=
          RS_DS_CM11_SimulinkRS_P.Limits50501_UpperSat);
  if (((!lsat) && (!usat)) || (lsat && (RS_DS_CM11_SimulinkRS_B.Ki_d > 0.0)) ||
      (usat && (RS_DS_CM11_SimulinkRS_B.Ki_d < 0.0))) {
    _rtXdot->Limits50501_CSTATE = RS_DS_CM11_SimulinkRS_B.Ki_d;
  } else {
    /* in saturation */
    _rtXdot->Limits50501_CSTATE = 0.0;
  }

  /* End of Derivatives for Integrator: '<S62>/Limits [-50,50]1' */
  /* End of Derivatives for SubSystem: '<Root>/CarMaker' */
}

/* Model initialize function */
void RS_DS_CM11_SimulinkRS_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)RS_DS_CM11_SimulinkRS_M, 0,
                sizeof(RT_MODEL_RS_DS_CM11_SimulinkRS_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
                          &RS_DS_CM11_SimulinkRS_M->Timing.simTimeStep);
    rtsiSetTPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo, &rtmGetTPtr
                (RS_DS_CM11_SimulinkRS_M));
    rtsiSetStepSizePtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
                       &RS_DS_CM11_SimulinkRS_M->Timing.stepSize0);
    rtsiSetdXPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
                 &RS_DS_CM11_SimulinkRS_M->derivs);
    rtsiSetContStatesPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo, (real_T **)
                         &RS_DS_CM11_SimulinkRS_M->contStates);
    rtsiSetNumContStatesPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
      &RS_DS_CM11_SimulinkRS_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
      &RS_DS_CM11_SimulinkRS_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
      &RS_DS_CM11_SimulinkRS_M->periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
      &RS_DS_CM11_SimulinkRS_M->periodicContStateRanges);
    rtsiSetErrorStatusPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
                          (&rtmGetErrorStatus(RS_DS_CM11_SimulinkRS_M)));
    rtsiSetRTModelPtr(&RS_DS_CM11_SimulinkRS_M->solverInfo,
                      RS_DS_CM11_SimulinkRS_M);
  }

  rtsiSetSimTimeStep(&RS_DS_CM11_SimulinkRS_M->solverInfo, MAJOR_TIME_STEP);
  RS_DS_CM11_SimulinkRS_M->intgData.f[0] = RS_DS_CM11_SimulinkRS_M->odeF[0];
  RS_DS_CM11_SimulinkRS_M->contStates = ((X_RS_DS_CM11_SimulinkRS_T *)
    &RS_DS_CM11_SimulinkRS_X);
  rtsiSetSolverData(&RS_DS_CM11_SimulinkRS_M->solverInfo, (void *)
                    &RS_DS_CM11_SimulinkRS_M->intgData);
  rtsiSetSolverName(&RS_DS_CM11_SimulinkRS_M->solverInfo,"ode1");
  rtmSetTPtr(RS_DS_CM11_SimulinkRS_M, &RS_DS_CM11_SimulinkRS_M->Timing.tArray[0]);
  RS_DS_CM11_SimulinkRS_M->Timing.stepSize0 = 0.001;

  /* block I/O */
  (void) memset(((void *) &RS_DS_CM11_SimulinkRS_B), 0,
                sizeof(B_RS_DS_CM11_SimulinkRS_T));

  {
    int32_T i;
    for (i = 0; i < 14; i++) {
      RS_DS_CM11_SimulinkRS_B.Environment_o2[i] = 0.0;
    }

    for (i = 0; i < 17; i++) {
      RS_DS_CM11_SimulinkRS_B.DrivMan_o2[i] = 0.0;
    }

    for (i = 0; i < 29; i++) {
      RS_DS_CM11_SimulinkRS_B.VehicleControl_o2[i] = 0.0;
    }

    for (i = 0; i < 29; i++) {
      RS_DS_CM11_SimulinkRS_B.Switch[i] = 0.0;
    }

    for (i = 0; i < 5; i++) {
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o3[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o4[i] = 0.0;
    }

    for (i = 0; i < 11; i++) {
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o5[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o7[i] = 0.0;
    }

    for (i = 0; i < 21; i++) {
      RS_DS_CM11_SimulinkRS_B.Steering_o2[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o2[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o3[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o4[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o5[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o6[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o7[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o8[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinematics_o9[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_SimulinkRS_B.Forces_o2[i] = 0.0;
    }

    for (i = 0; i < 13; i++) {
      RS_DS_CM11_SimulinkRS_B.Forces_o3[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_SimulinkRS_B.Forces_o4[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o2[i] = 0.0;
    }

    for (i = 0; i < 24; i++) {
      RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o2[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinetics_o2[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinetics_o3[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_SimulinkRS_B.Kinetics_o4[i] = 0.0;
    }

    for (i = 0; i < 15; i++) {
      RS_DS_CM11_SimulinkRS_B.Trailer_o3[i] = 0.0;
    }

    for (i = 0; i < 18; i++) {
      RS_DS_CM11_SimulinkRS_B.Trailer_o4[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_SimulinkRS_B.Trailer_o5[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_SimulinkRS_B.Trailer_o6[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_SimulinkRS_B.Trailer_o7[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_SimulinkRS_B.Trailer_o8[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_SimulinkRS_B.Trailer_o9[i] = 0.0;
    }

    for (i = 0; i < 9; i++) {
      RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o2[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_SimulinkRS_B.Brake_o2[i] = 0.0;
    }

    for (i = 0; i < 20; i++) {
      RS_DS_CM11_SimulinkRS_B.BrakeUpd_o2[i] = 0.0;
    }

    for (i = 0; i < 42; i++) {
      RS_DS_CM11_SimulinkRS_B.PowerTrain_o2[i] = 0.0;
    }

    RS_DS_CM11_SimulinkRS_B.BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1
      = RS_DS_CM11_SimulinkRS_rtZVehDataBus;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o4 =
      RS_DS_CM11_SimulinkRS_rtZVehDataBus;
    RS_DS_CM11_SimulinkRS_B.IoIn = 0.0;
    RS_DS_CM11_SimulinkRS_B.Environment_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.DrivMan_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehicleControl_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.Gain1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.Gain = 0.0;
    RS_DS_CM11_SimulinkRS_B.CM_Time = 0.0;
    RS_DS_CM11_SimulinkRS_B.Switch1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.CM_Time_b = 0.0;
    RS_DS_CM11_SimulinkRS_B.SimCore_State = 0.0;
    RS_DS_CM11_SimulinkRS_B.Clock = 0.0;
    RS_DS_CM11_SimulinkRS_B.ConnectionState = 0.0;
    RS_DS_CM11_SimulinkRS_B.speedDesired = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict2 = 0.0;
    RS_DS_CM11_SimulinkRS_B.speedActual = 0.0;
    RS_DS_CM11_SimulinkRS_B.ManualSwitch = 0.0;
    RS_DS_CM11_SimulinkRS_B.distanceTravel = 0.0;
    RS_DS_CM11_SimulinkRS_B.driverSourceFinal = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o2 = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o6[0] = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehicleControlUpd_o6[1] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Steering_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinematics_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Forces_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.SuspensionControlUnit_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.SuspensionForceElements_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.SuspensionForceElementsUpdate = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o5[0] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o5[1] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o5[2] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o5[3] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o6[0] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o6[1] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o6[2] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kinetics_o6[3] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trailer_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trailer_o2[0] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trailer_o2[1] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trailer_o2[2] = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trailer_o2[3] = 0.0;
    RS_DS_CM11_SimulinkRS_B.CarAndTrailerUpd_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Sync = 0.0;
    RS_DS_CM11_SimulinkRS_B.BrakeUpd_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Sync_c = 0.0;
    RS_DS_CM11_SimulinkRS_B.PowerTrainUpd = 0.0;
    RS_DS_CM11_SimulinkRS_B.CM_User = 0.0;
    RS_DS_CM11_SimulinkRS_B.IoOut = 0.0;
    RS_DS_CM11_SimulinkRS_B.HIL_SpeedCmd_kph = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehSpd = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehSpdKph = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehSpdKph_p = 0.0;
    RS_DS_CM11_SimulinkRS_B.Gain2 = 0.0;
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.AccelPedalCmd_pct_g = 0.0;
    RS_DS_CM11_SimulinkRS_B.Gain1_p = 0.0;
    RS_DS_CM11_SimulinkRS_B.MultiportSwitch2 = 0.0;
    RS_DS_CM11_SimulinkRS_B.BrakePedalCmd_pct_a = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehSpdMph = 0.0;
    RS_DS_CM11_SimulinkRS_B.SimCore_State_b = 0.0;
    RS_DS_CM11_SimulinkRS_B.connectRequest = 0.0;
    RS_DS_CM11_SimulinkRS_B.heading = 0.0;
    RS_DS_CM11_SimulinkRS_B.signalLightHeadId = 0.0;
    RS_DS_CM11_SimulinkRS_B.signalLightDistance = 0.0;
    RS_DS_CM11_SimulinkRS_B.signalLightColor = 0.0;
    RS_DS_CM11_SimulinkRS_B.speedLimit = 0.0;
    RS_DS_CM11_SimulinkRS_B.speedLimitNext = 0.0;
    RS_DS_CM11_SimulinkRS_B.speedLimitChangeDistance = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict6 = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict7 = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict4 = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict5 = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict3 = 0.0;
    RS_DS_CM11_SimulinkRS_B.LinkSpeed = 0.0;
    RS_DS_CM11_SimulinkRS_B.Constant3 = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict11 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_dSpeed = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_Prec_Speed = 0.0;
    RS_DS_CM11_SimulinkRS_B.Divide = 0.0;
    RS_DS_CM11_SimulinkRS_B.followingDistanceDesired = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory_g = 0.0;
    RS_DS_CM11_SimulinkRS_B.Add = 0.0;
    RS_DS_CM11_SimulinkRS_B.ReadCMDict6_n = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Add1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_Targ_Dtct = 0.0;
    RS_DS_CM11_SimulinkRS_B.RefSpd = 0.0;
    RS_DS_CM11_SimulinkRS_B.Driver_IntTerm_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.SpdErr = 0.0;
    RS_DS_CM11_SimulinkRS_B.Driver_PropTerm_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.iTermDist = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_dDist = 0.0;
    RS_DS_CM11_SimulinkRS_B.distErr = 0.0;
    RS_DS_CM11_SimulinkRS_B.pTermDist = 0.0;
    RS_DS_CM11_SimulinkRS_B.Sum2 = 0.0;
    RS_DS_CM11_SimulinkRS_B.distCmd = 0.0;
    RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.Saturation = 0.0;
    RS_DS_CM11_SimulinkRS_B.DrvSpeedCmdRateLimit = 0.0;
    RS_DS_CM11_SimulinkRS_B.Ki = 0.0;
    RS_DS_CM11_SimulinkRS_B.Ki_d = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory2 = 0.0;
    RS_DS_CM11_SimulinkRS_B.followDistRateLimit = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehSpd_b = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehSpdKph_p4 = 0.0;
    RS_DS_CM11_SimulinkRS_B.VehSpdMph_g = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_Targ_Dtct_l = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_dDist_j = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_dSpeed_i = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_ObjId = 0.0;
    RS_DS_CM11_SimulinkRS_B.Trf_FOL_State = 0.0;
    RS_DS_CM11_SimulinkRS_B.CycleMode = 0.0;
    RS_DS_CM11_SimulinkRS_B.SpeedCmd = 0.0;
    RS_DS_CM11_SimulinkRS_B.positionX = 0.0;
    RS_DS_CM11_SimulinkRS_B.positionY = 0.0;
    RS_DS_CM11_SimulinkRS_B.positionZ = 0.0;
    RS_DS_CM11_SimulinkRS_B.heading_b = 0.0;
    RS_DS_CM11_SimulinkRS_B.color = 0.0;
    RS_DS_CM11_SimulinkRS_B.speedDesired_i = 0.0;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem2_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.accelerationDesired = 0.0;
    RS_DS_CM11_SimulinkRS_B.timeSimulator = 0.0;
    RS_DS_CM11_SimulinkRS_B.timeStepSimulator = 0.0;
    RS_DS_CM11_SimulinkRS_B.timeStepTrigger = 0.0;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem2_o6 = 0.0;
    RS_DS_CM11_SimulinkRS_B.nMsgSend = 0.0;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o1 = 0.0;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o2 = 0.0;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o3 = 0.0;
    RS_DS_CM11_SimulinkRS_B.MATLABSystem_o5 = 0.0;
    RS_DS_CM11_SimulinkRS_B.intVal = 0.0;
    RS_DS_CM11_SimulinkRS_B.Vehicle_Spd_kph = 0.0;
    RS_DS_CM11_SimulinkRS_B.Drive = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory1_b = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory2_o = 0.0;
    RS_DS_CM11_SimulinkRS_B.RefSpdkmhr = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory1_e = 0.0;
    RS_DS_CM11_SimulinkRS_B.Memory_m = 0.0;
    RS_DS_CM11_SimulinkRS_B.Add_i = 0.0;
    RS_DS_CM11_SimulinkRS_B.Feedforward = 0.0;
    RS_DS_CM11_SimulinkRS_B.Driver_FdfwdTerm_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.Driver_IntTerm_pct_b = 0.0;
    RS_DS_CM11_SimulinkRS_B.VS_Flt = 0.0;
    RS_DS_CM11_SimulinkRS_B.SpdErr_j = 0.0;
    RS_DS_CM11_SimulinkRS_B.Kp = 0.0;
    RS_DS_CM11_SimulinkRS_B.ProportionalGainScheduling = 0.0;
    RS_DS_CM11_SimulinkRS_B.Driver_PropTerm_pct_i = 0.0;
    RS_DS_CM11_SimulinkRS_B.Driver_TotCmdPreLim_pct_l = 0.0;
    RS_DS_CM11_SimulinkRS_B.Gain_d = 0.0;
    RS_DS_CM11_SimulinkRS_B.Ki_g = 0.0;
    RS_DS_CM11_SimulinkRS_B.BrakePedalPosPre_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPosPre_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.AcceleratorPedalPos_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.BrakePedalPos_pct = 0.0;
    RS_DS_CM11_SimulinkRS_B.RefSpdmph = 0.0;
    RS_DS_CM11_SimulinkRS_B.Vehicle_Spd_mph = 0.0;
    RS_DS_CM11_SimulinkRS_B.RateLimiter = 0.0;
    RS_DS_CM11_SimulinkRS_B.RefSpdkmhr_n = 0.0;
    RS_DS_CM11_SimulinkRS_B.Vehicle_SpdDmd_mph = 0.0;
  }

  /* states (continuous) */
  {
    (void) memset((void *)&RS_DS_CM11_SimulinkRS_X, 0,
                  sizeof(X_RS_DS_CM11_SimulinkRS_T));
  }

  /* states (dwork) */
  (void) memset((void *)&RS_DS_CM11_SimulinkRS_DW, 0,
                sizeof(DW_RS_DS_CM11_SimulinkRS_T));
  RS_DS_CM11_SimulinkRS_DW.Memory_2_PreviousInput = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory_1_PreviousInput = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_d = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput = 0.0;
  RS_DS_CM11_SimulinkRS_DW.PrevY = 0.0;
  RS_DS_CM11_SimulinkRS_DW.LastMajorTime = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput = 0.0;
  RS_DS_CM11_SimulinkRS_DW.PrevY_m = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_e = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput_l = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_h = 0.0;
  RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_o = 0.0;
  RS_DS_CM11_SimulinkRS_DW.PrevY_a = 0.0;

  {
    /* user code (registration function declaration) */
    /*Initialize global TRC pointers. */
    RS_DS_CM11_SimulinkRS_rti_init_trc_pointers();
  }

  {
    RealSimDepack_RS_DS_CM11_SimulinkRS_T *b_obj;
    RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T *b_obj_0;
    RealSimPack_RS_DS_CM11_SimulinkRS_T *b_obj_1;
    real_T initialSpeed;
    real_T speedLookAheadHorizon;
    real_T speedUpperBound;
    static const VehDataBus tmp = { { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U },                  /* id */
      0.0,                             /* idLength */

      { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U },/* type */
      0.0,                             /* typeLength */

      { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U },/* vehicleClass */
      0.0,                             /* vehicleClassLength */
      0.0,                             /* speed */
      0.0,                             /* acceleration */
      0.0,                             /* positionX */
      0.0,                             /* positionY */
      0.0,                             /* positionZ */
      0.0,                             /* heading */
      0.0,                             /* color */

      { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U },/* linkId */
      0.0,                             /* linkIdLength */
      0.0,                             /* laneId */
      0.0,                             /* distanceTravel */
      0.0,                             /* speedDesired */
      0.0,                             /* accelerationDesired */
      0.0,                             /* hasPrecedingVehicle */

      { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U },/* precedingVehicleId */
      0.0,                             /* precedingVehicleIdLength */
      0.0,                             /* precedingVehicleDistance */
      0.0,                             /* precedingVehicleSpeed */

      { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U },/* signalLightId */
      0.0,                             /* signalLightIdLength */
      0.0,                             /* signalLightHeadId */
      0.0,                             /* signalLightDistance */
      0.0,                             /* signalLightColor */
      0.0,                             /* speedLimit */
      0.0,                             /* speedLimitNext */
      0.0,                             /* speedLimitChangeDistance */

      { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U,
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U },/* linkIdNext */
      0.0,                             /* linkIdNextLength */
      0.0,                             /* grade */
      0.0                              /* activeLaneChange */
    };

    const real_T *VehicleMessageFieldDefInputVec;

    /* user code (Start function Header) */
    CarMaker_Startup();

    /* Start for Atomic SubSystem: '<Root>/CarMaker' */
    /* Start for S-Function (read_dict): '<S11>/Read CM Dict3' */
    {
      char **namevec = CM_Names2StrVec("Time", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict3_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict3' */
    {
      char **namevec = CM_Names2StrVec("Time", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict3_PWORK_k.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict7' */
    {
      char **namevec = CM_Names2StrVec("SC.State", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict7_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (dsmpb_datainport): '<S55>/Data Inport S-Fcn' */

    /* the initial values are all 0, therefore, we use memset */
    memset(&RS_DS_CM11_SimulinkRS_B.DataVector[0], 0, 1024U*sizeof(uint8_T));
    RS_DS_CM11_SimulinkRS_B.Status = 0;
    RS_DS_CM11_SimulinkRS_B.ReceivedBytes = 0;
    RS_DS_CM11_SimulinkRS_B.AvailableBytes = 0;

    /* Start for S-Function (dsmpb_datainport): '<S57>/Data Inport S-Fcn' */
    RS_DS_CM11_SimulinkRS_B.ConnectionState_f = 0;

    /* Start for MATLABSystem: '<S7>/MATLAB System' incorporates:
     *  MATLABSystem: '<S7>/MATLAB System1'
     */
    VehicleMessageFieldDefInputVec =
      &RS_DS_CM11_SimulinkRS_P.VehicleMessageFieldDefInputVec[0];
    b_obj = &RS_DS_CM11_SimulinkRS_DW.obj;

    /*  Constructor */
    b_obj->isInitialized = 0;

    /*  Support name-value pair arguments when constructing object */
    /*              setProperties(obj,nargin,varargin{:}) */
    /*              coder.allowpcode('plain') */
    b_obj->msgHeaderSize = 9.0;
    b_obj->msgEachHeaderSize = 3.0;
    b_obj->enableDebug = 0.0;
    b_obj->VehicleDataEmpty = tmp;

    /*  Constructor */
    /*  Support name-value pair arguments when constructing object */
    /*              setProperties(obj,nargin,varargin{:}) */
    /*              coder.allowpcode('plain') */
    RS_DS_CM11_SimulinkRS_DW.objisempty_n = true;
    memcpy(&RS_DS_CM11_SimulinkRS_DW.obj.VehicleMessageFieldDefInputVec[0],
           &VehicleMessageFieldDefInputVec[0], 29U * sizeof(real_T));
    b_obj = &RS_DS_CM11_SimulinkRS_DW.obj;
    b_obj->isInitialized = 1;

    /*  Validate inputs to the step method at initialization */
    RS_DS_CM11_SimulinkRS_RealSimDepack_setupImpl(b_obj);

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict2' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.v", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict2_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for MATLABSystem: '<S7>/MATLAB System2' */
    initialSpeed = RS_DS_CM11_SimulinkRS_P.RealSimPara.speedInit;
    speedLookAheadHorizon = RS_DS_CM11_SimulinkRS_P.RealSimPara.tLookahead;
    speedUpperBound = RS_DS_CM11_SimulinkRS_P.MATLABSystem2_speedUpperBound;
    b_obj_0 = &RS_DS_CM11_SimulinkRS_DW.obj_p;

    /*  Constructor */
    b_obj_0->isInitialized = 0;

    /*  Support name-value pair arguments when constructing object */
    /*              setProperties(obj,nargin,varargin{:}) */
    /*              coder.allowpcode('plain') */
    RS_DS_CM11_SimulinkRS_DW.objisempty = true;
    RS_DS_CM11_SimulinkRS_DW.obj_p.initialSpeed = initialSpeed;
    RS_DS_CM11_SimulinkRS_DW.obj_p.speedLookAheadHorizon = speedLookAheadHorizon;
    RS_DS_CM11_SimulinkRS_DW.obj_p.speedUpperBound = speedUpperBound;
    b_obj_0 = &RS_DS_CM11_SimulinkRS_DW.obj_p;
    b_obj_0->isInitialized = 1;

    /*  Validate inputs to the step method at initialization */
    RS_DS_CM11_SimulinkRS_RealSimInterpSpeed_setupImpl(b_obj_0);

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict8' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.Distance", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict8_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S6>/Read CM Dict' */
    {
      char **namevec = CM_Names2StrVec("Car.v", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S7>/Read CM Dict7' */
    {
      char **namevec = CM_Names2StrVec("SC.State", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict7_PWORK_m.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict1' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.PoI.x", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict1_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict6' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.PoI.y", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict6_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict7' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.PoI.z", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict7_PWORK_f.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.Fr1.x", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict_PWORK_a.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict4' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.Fr1.y", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict4_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict5' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.Fr1.z", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict5_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S8>/Read CM Dict3' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.Yaw", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict3_PWORK_i.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for MATLABSystem: '<S7>/MATLAB System1' */
    b_obj_1 = &RS_DS_CM11_SimulinkRS_DW.obj_l;

    /*  Constructor */
    b_obj_1->isInitialized = 0;

    /*  Support name-value pair arguments when constructing object */
    /*              setProperties(obj,nargin,varargin{:}) */
    /*              coder.allowpcode('plain') */
    b_obj_1->msgHeaderSize = 9.0;
    b_obj_1->msgEachHeaderSize = 3.0;

    /*  Constructor */
    /*  Support name-value pair arguments when constructing object */
    /*              setProperties(obj,nargin,varargin{:}) */
    /*              coder.allowpcode('plain') */
    RS_DS_CM11_SimulinkRS_DW.objisempty_h = true;
    memcpy(&RS_DS_CM11_SimulinkRS_DW.obj_l.VehicleMessageFieldDefInputVec[0],
           &VehicleMessageFieldDefInputVec[0], 29U * sizeof(real_T));
    b_obj_1 = &RS_DS_CM11_SimulinkRS_DW.obj_l;
    b_obj_1->isInitialized = 1;

    /*  Validate inputs to the step method at initialization */
    RS_DS_CM11_SimulinkRS_RealSimPack_setupImpl(b_obj_1);

    /* Start for S-Function (dsmpb_datainport): '<S54>/Data Inport S-Fcn' */
    RS_DS_CM11_SimulinkRS_B.LinkSpeed = 0;

    /* Start for S-Function (dsmpb_datainport): '<S58>/Data Inport S-Fcn' */
    RS_DS_CM11_SimulinkRS_B.Status_l = 0;
    RS_DS_CM11_SimulinkRS_B.SentBytes = 0;

    /* Start for S-Function (read_dict): '<S10>/Read CM Dict11' */
    {
      char **namevec = CM_Names2StrVec("Car.v", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict11_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S10>/Read CM Dict9' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dSpeed", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict9_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S62>/Read CM Dict6' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.Targ_Dtct", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict6_PWORK_i.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S10>/Read CM Dict10' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.Targ_Dtct", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict10_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S10>/Read CM Dict12' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dDist", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict12_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict' */
    {
      char **namevec = CM_Names2StrVec("Car.v", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict_PWORK_c.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict1' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.Targ_Dtct", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict1_PWORK_j.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict2' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dDist", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict2_PWORK_d.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict4' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dSpeed", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict4_PWORK_b.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict5' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.ObjId", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict5_PWORK_k.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict6' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.State", NULL);
      RS_DS_CM11_SimulinkRS_DW.ReadCMDict6_PWORK_j.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* End of Start for SubSystem: '<Root>/CarMaker' */

    /* user code (Start function Trailer) */
    CarMaker_FinishStartup();
  }

  RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE_b = UNINITIALIZED_ZCSIG;
  RS_DS_CM11_SimulinkRS_PrevZCX.Limits5050_Reset_ZCE = UNINITIALIZED_ZCSIG;
  RS_DS_CM11_SimulinkRS_PrevZCX.Limits50501_Reset_ZCE = UNINITIALIZED_ZCSIG;

  /* SystemInitialize for Atomic SubSystem: '<Root>/CarMaker' */
  /* InitializeConditions for Memory generated from: '<S11>/Memory' */
  RS_DS_CM11_SimulinkRS_DW.Memory_2_PreviousInput =
    RS_DS_CM11_SimulinkRS_P.Memory_2_InitialCondition;

  /* InitializeConditions for Memory generated from: '<S11>/Memory' */
  RS_DS_CM11_SimulinkRS_DW.Memory_1_PreviousInput =
    RS_DS_CM11_SimulinkRS_P.Memory_1_InitialCondition;

  /* InitializeConditions for Memory: '<S7>/Memory' */
  RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput =
    RS_DS_CM11_SimulinkRS_P.Memory_InitialCondition_d;

  /* InitializeConditions for Memory: '<S62>/Memory' */
  RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_d =
    RS_DS_CM11_SimulinkRS_P.Memory_InitialCondition_f;

  /* InitializeConditions for Memory: '<S62>/Memory1' */
  RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput =
    RS_DS_CM11_SimulinkRS_P.Memory1_InitialCondition_n;

  /* InitializeConditions for Integrator: '<S62>/Limits [-50,50]' */
  RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE =
    RS_DS_CM11_SimulinkRS_P.Limits5050_IC_a;

  /* InitializeConditions for Integrator: '<S62>/Limits [-50,50]1' */
  RS_DS_CM11_SimulinkRS_X.Limits50501_CSTATE =
    RS_DS_CM11_SimulinkRS_P.Limits50501_IC;

  /* InitializeConditions for RateLimiter: '<S62>/DrvSpeedCmdRateLimit' */
  RS_DS_CM11_SimulinkRS_DW.LastMajorTime = (rtInf);

  /* InitializeConditions for Memory: '<S62>/Memory2' */
  RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput =
    RS_DS_CM11_SimulinkRS_P.Memory2_InitialCondition_j;

  /* InitializeConditions for RateLimiter: '<S62>/followDistRateLimit' */
  RS_DS_CM11_SimulinkRS_DW.PrevY_m =
    RS_DS_CM11_SimulinkRS_P.followDistRateLimit_IC;

  /* SystemInitialize for Chart: '<S1>/RealSimHILCycle' */
  RS_DS_CM11_SimulinkRS_DW.is_CycleOn = RS_DS_CM11_SimulinkRS_IN_NO_ACTIVE_CHILD;
  RS_DS_CM11_SimulinkRS_DW.temporalCounter_i1 = 0U;
  RS_DS_CM11_SimulinkRS_DW.is_active_c2_RS_DS_CM11_SimulinkRS = 0U;
  RS_DS_CM11_SimulinkRS_DW.is_c2_RS_DS_CM11_SimulinkRS =
    RS_DS_CM11_SimulinkRS_IN_NO_ACTIVE_CHILD;
  RS_DS_CM11_SimulinkRS_B.CycleMode = 0.0;
  RS_DS_CM11_SimulinkRS_B.SpeedCmd = 0.0;

  /* SystemInitialize for Atomic SubSystem: '<S6>/Driver and Drive Cycle' */
  /* InitializeConditions for Memory: '<S42>/Memory1' */
  RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_e =
    RS_DS_CM11_SimulinkRS_P.Memory1_InitialCondition;

  /* InitializeConditions for Memory: '<S42>/Memory2' */
  RS_DS_CM11_SimulinkRS_DW.Memory2_PreviousInput_l =
    RS_DS_CM11_SimulinkRS_P.Memory2_InitialCondition;

  /* InitializeConditions for Memory: '<S43>/Memory1' */
  RS_DS_CM11_SimulinkRS_DW.Memory1_PreviousInput_h =
    RS_DS_CM11_SimulinkRS_P.Memory1_InitialCondition_h;

  /* InitializeConditions for Memory: '<S43>/Memory' */
  RS_DS_CM11_SimulinkRS_DW.Memory_PreviousInput_o =
    RS_DS_CM11_SimulinkRS_P.Memory_InitialCondition;

  /* InitializeConditions for UnitDelay: '<S46>/Delay Input1' */
  RS_DS_CM11_SimulinkRS_DW.DelayInput1_DSTATE =
    RS_DS_CM11_SimulinkRS_P.DetectFallNegative_vinit;

  /* InitializeConditions for UnitDelay: '<S47>/Delay Input1' */
  RS_DS_CM11_SimulinkRS_DW.DelayInput1_DSTATE_l =
    RS_DS_CM11_SimulinkRS_P.DetectRisePositive_vinit;

  /* InitializeConditions for Integrator: '<S43>/Limits [-50,50]' */
  RS_DS_CM11_SimulinkRS_X.Limits5050_CSTATE_l =
    RS_DS_CM11_SimulinkRS_P.Limits5050_IC;

  /* InitializeConditions for TransferFcn: '<S43>/Lowpass Filter' */
  RS_DS_CM11_SimulinkRS_X.LowpassFilter_CSTATE = 0.0;

  /* InitializeConditions for RateLimiter: '<S15>/Rate Limiter' */
  RS_DS_CM11_SimulinkRS_DW.PrevY_a = RS_DS_CM11_SimulinkRS_P.RateLimiter_IC;

  /* End of SystemInitialize for SubSystem: '<S6>/Driver and Drive Cycle' */
  /* End of SystemInitialize for SubSystem: '<Root>/CarMaker' */
  /*  Initialize / reset discrete-state properties */
}

/* Model terminate function */
void RS_DS_CM11_SimulinkRS_terminate(void)
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
