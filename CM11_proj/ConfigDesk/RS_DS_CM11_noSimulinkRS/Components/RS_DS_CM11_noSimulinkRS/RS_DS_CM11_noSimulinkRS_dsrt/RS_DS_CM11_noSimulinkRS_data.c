/*
 * RS_DS_CM11_noSimulinkRS_data.c
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

#include "RS_DS_CM11_noSimulinkRS.h"
#include "RS_DS_CM11_noSimulinkRS_private.h"

/* Block parameters (default storage) */
P_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_P = {
  /* Variable: RealSimPara
   * Referenced by: '<S1>/Constant'
   */
  {
    13.5,
    0.1,
    1.0
  },

  /* Mask Parameter: CompareToConstant1_const
   * Referenced by: '<S41>/Constant'
   */
  0.1,

  /* Mask Parameter: CompareToConstant3_const
   * Referenced by: '<S42>/Constant'
   */
  0.0,

  /* Mask Parameter: VehicleSource_BuiltIn_const
   * Referenced by: '<S14>/Constant'
   */
  1.0,

  /* Mask Parameter: DetectFallNegative_vinit
   * Referenced by: '<S43>/Delay Input1'
   */
  false,

  /* Mask Parameter: DetectRisePositive_vinit
   * Referenced by: '<S44>/Delay Input1'
   */
  false,

  /* Expression: 0
   * Referenced by: '<S45>/Constant'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S46>/Constant'
   */
  0.0,

  /* Expression: 1
   * Referenced by: '<S39>/Constant'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S39>/Memory1'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S39>/Memory2'
   */
  0.0,

  /* Expression: 190
   * Referenced by: '<S39>/SpdSat'
   */
  190.0,

  /* Expression: -5
   * Referenced by: '<S39>/SpdSat'
   */
  -5.0,

  /* Expression: 0
   * Referenced by: '<S40>/Memory1'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S40>/Memory'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S40>/Constant'
   */
  0.0,

  /* Expression: [0	3.5	12.5	16	20	21	22	24.5	26	29.5	32	33	34	36	37	38	39]
   * Referenced by: '<S40>/Feedforward'
   */
  { 0.0, 3.5, 12.5, 16.0, 20.0, 21.0, 22.0, 24.5, 26.0, 29.5, 32.0, 33.0, 34.0,
    36.0, 37.0, 38.0, 39.0 },

  /* Expression: [0 10    20    30    40    50    60    70    80    90   100   110   120   130   140   150   160]
   * Referenced by: '<S40>/Feedforward'
   */
  { 0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0,
    120.0, 130.0, 140.0, 150.0, 160.0 },

  /* Expression: 1
   * Referenced by: '<S40>/FeedforwardGain'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S40>/Limits [-50,50]'
   */
  0.0,

  /* Expression: 50
   * Referenced by: '<S40>/Limits [-50,50]'
   */
  50.0,

  /* Expression: -50
   * Referenced by: '<S40>/Limits [-50,50]'
   */
  -50.0,

  /* Computed Parameter: LowpassFilter_A
   * Referenced by: '<S40>/Lowpass Filter'
   */
  -50.0,

  /* Computed Parameter: LowpassFilter_C
   * Referenced by: '<S40>/Lowpass Filter'
   */
  50.0,

  /* Expression: 7
   * Referenced by: '<S40>/Kp'
   */
  7.0,

  /* Expression: [1 1 1]
   * Referenced by: '<S40>/Proportional Gain Scheduling'
   */
  { 1.0, 1.0, 1.0 },

  /* Expression: [ 0 30 70]
   * Referenced by: '<S40>/Proportional Gain Scheduling'
   */
  { 0.0, 30.0, 70.0 },

  /* Expression: -1
   * Referenced by: '<S40>/Gain'
   */
  -1.0,

  /* Expression: 1
   * Referenced by: '<S40>/Ki'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S40>/Min threshold for Brake switch'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S40>/Switch2'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S40>/Switch1'
   */
  0.0,

  /* Expression: 50
   * Referenced by: '<S40>/standstill brake [0,100]'
   */
  50.0,

  /* Expression: 1/1.609344
   * Referenced by: '<S39>/km to miles'
   */
  0.621371192237334,

  /* Expression: 1/1.609
   * Referenced by: '<S12>/Gain'
   */
  0.62150403977625857,

  /* Expression: 3
   * Referenced by: '<S12>/ManSpdSel'
   */
  3.0,

  /* Expression: 0
   * Referenced by: '<S12>/ManSpd'
   */
  0.0,

  /* Expression: 5
   * Referenced by: '<S12>/Rate Limiter'
   */
  5.0,

  /* Expression: -5
   * Referenced by: '<S12>/Rate Limiter'
   */
  -5.0,

  /* Expression: 0
   * Referenced by: '<S12>/Rate Limiter'
   */
  0.0,

  /* Expression: 1/1.609
   * Referenced by: '<S12>/Gain1'
   */
  0.62150403977625857,

  /* Expression: 0
   * Referenced by: '<S8>/Memory'
   */
  0.0,

  /* Expression: 0.01
   * Referenced by: '<S8>/Gain1'
   */
  0.01,

  /* Expression: 0
   * Referenced by: '<S8>/Memory'
   */
  0.0,

  /* Expression: 0.01
   * Referenced by: '<S8>/Gain'
   */
  0.01,

  /* Expression: 0
   * Referenced by: '<S8>/Driver Source'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S8>/Constant'
   */
  0.0,

  /* Expression: 0.5
   * Referenced by: '<S8>/Switch1'
   */
  0.5,

  /* Expression: 20
   * Referenced by: '<S1>/initialWaitTime'
   */
  20.0,

  /* Expression: 2000
   * Referenced by: '<S1>/totalDistance (m)'
   */
  2000.0,

  /* Expression: 1000
   * Referenced by: '<S1>/totalTime (sec)'
   */
  1000.0,

  /* Expression: 0
   * Referenced by: '<S1>/RampDownFlag'
   */
  0.0,

  /* Expression: 3
   * Referenced by: '<S8>/Constant1'
   */
  3.0,

  /* Expression: 1
   * Referenced by: '<S8>/Constant2'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S8>/Switch'
   */
  0.0,

  /* Expression: 3.6
   * Referenced by: '<S1>/Gain3'
   */
  3.6,

  /* Expression: 0
   * Referenced by: '<S9>/APP'
   */
  0.0,

  /* Expression: 2
   * Referenced by: '<S9>/Driver Control Selection'
   */
  2.0,

  /* Expression: 3.6
   * Referenced by: '<S6>/Gain1'
   */
  3.6,

  /* Expression: 3.6
   * Referenced by: '<S6>/Gain3'
   */
  3.6,

  /* Expression: 100
   * Referenced by: '<S9>/Gain2'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S9>/Constant'
   */
  0.0,

  /* Expression: 100
   * Referenced by: '<S9>/AR_limit'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S9>/AR_limit'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S9>/BR_command_0to100'
   */
  0.0,

  /* Expression: 100
   * Referenced by: '<S9>/Gain1'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S9>/Constant1'
   */
  0.0,

  /* Expression: 100
   * Referenced by: '<S9>/BR_limit'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S9>/BR_limit'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S9>/Min threshold for Brake switch'
   */
  0.0,

  /* Expression: 2.23694
   * Referenced by: '<S6>/Gain2'
   */
  2.23694,

  /* Expression: 3.6
   * Referenced by: '<S1>/Gain1'
   */
  3.6,

  /* Expression: 2.23694
   * Referenced by: '<S1>/Gain2'
   */
  2.23694
};
