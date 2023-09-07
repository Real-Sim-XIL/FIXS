/*
 * RS_DS_CM11_SimulinkRS_data.c
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

#include "RS_DS_CM11_SimulinkRS.h"
#include "RS_DS_CM11_SimulinkRS_private.h"

/* Block parameters (default storage) */
P_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_P = {
  /* Variable: RealSimPara
   * Referenced by:
   *   '<S1>/Constant'
   *   '<S7>/MATLAB System2'
   *   '<S10>/free speed'
   */
  {
    13.5,
    0.1,
    1.0
  },

  /* Variable: VehicleMessageFieldDefInputVec
   * Referenced by:
   *   '<S7>/MATLAB System'
   *   '<S7>/MATLAB System1'
   */
  { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 },

  /* Mask Parameter: CompareToConstant1_const
   * Referenced by: '<S44>/Constant'
   */
  0.1,

  /* Mask Parameter: CompareToConstant3_const
   * Referenced by: '<S45>/Constant'
   */
  0.0,

  /* Mask Parameter: VehicleSource_BuiltIn_const
   * Referenced by: '<S17>/Constant'
   */
  1.0,

  /* Mask Parameter: CompareToConstant_const
   * Referenced by: '<S63>/Constant'
   */
  20.0,

  /* Mask Parameter: CompareToConstant1_const_f
   * Referenced by: '<S64>/Constant'
   */
  3.0,

  /* Mask Parameter: CompareToConstant3_const_g
   * Referenced by: '<S65>/Constant'
   */
  0.0,

  /* Mask Parameter: DetectFallNegative_vinit
   * Referenced by: '<S46>/Delay Input1'
   */
  false,

  /* Mask Parameter: DetectRisePositive_vinit
   * Referenced by: '<S47>/Delay Input1'
   */
  false,

  /* Expression: 0
   * Referenced by: '<S48>/Constant'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S49>/Constant'
   */
  0.0,

  /* Expression: 1
   * Referenced by: '<S42>/Constant'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S42>/Memory1'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S42>/Memory2'
   */
  0.0,

  /* Expression: 190
   * Referenced by: '<S42>/SpdSat'
   */
  190.0,

  /* Expression: -5
   * Referenced by: '<S42>/SpdSat'
   */
  -5.0,

  /* Expression: 0
   * Referenced by: '<S43>/Memory1'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S43>/Memory'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S43>/Constant'
   */
  0.0,

  /* Expression: [0	3.5	12.5	16	20	21	22	24.5	26	29.5	32	33	34	36	37	38	39]
   * Referenced by: '<S43>/Feedforward'
   */
  { 0.0, 3.5, 12.5, 16.0, 20.0, 21.0, 22.0, 24.5, 26.0, 29.5, 32.0, 33.0, 34.0,
    36.0, 37.0, 38.0, 39.0 },

  /* Expression: [0 10    20    30    40    50    60    70    80    90   100   110   120   130   140   150   160]
   * Referenced by: '<S43>/Feedforward'
   */
  { 0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0,
    120.0, 130.0, 140.0, 150.0, 160.0 },

  /* Expression: 1
   * Referenced by: '<S43>/FeedforwardGain'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S43>/Limits [-50,50]'
   */
  0.0,

  /* Expression: 50
   * Referenced by: '<S43>/Limits [-50,50]'
   */
  50.0,

  /* Expression: -50
   * Referenced by: '<S43>/Limits [-50,50]'
   */
  -50.0,

  /* Computed Parameter: LowpassFilter_A
   * Referenced by: '<S43>/Lowpass Filter'
   */
  -50.0,

  /* Computed Parameter: LowpassFilter_C
   * Referenced by: '<S43>/Lowpass Filter'
   */
  50.0,

  /* Expression: 7
   * Referenced by: '<S43>/Kp'
   */
  7.0,

  /* Expression: [1 1 1]
   * Referenced by: '<S43>/Proportional Gain Scheduling'
   */
  { 1.0, 1.0, 1.0 },

  /* Expression: [ 0 30 70]
   * Referenced by: '<S43>/Proportional Gain Scheduling'
   */
  { 0.0, 30.0, 70.0 },

  /* Expression: -1
   * Referenced by: '<S43>/Gain'
   */
  -1.0,

  /* Expression: 1
   * Referenced by: '<S43>/Ki'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S43>/Min threshold for Brake switch'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S43>/Switch2'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S43>/Switch1'
   */
  0.0,

  /* Expression: 50
   * Referenced by: '<S43>/standstill brake [0,100]'
   */
  50.0,

  /* Expression: 1/1.609344
   * Referenced by: '<S42>/km to miles'
   */
  0.621371192237334,

  /* Expression: 1/1.609
   * Referenced by: '<S15>/Gain'
   */
  0.62150403977625857,

  /* Expression: 3
   * Referenced by: '<S15>/ManSpdSel'
   */
  3.0,

  /* Expression: 0
   * Referenced by: '<S15>/ManSpd'
   */
  0.0,

  /* Expression: 5
   * Referenced by: '<S15>/Rate Limiter'
   */
  5.0,

  /* Expression: -5
   * Referenced by: '<S15>/Rate Limiter'
   */
  -5.0,

  /* Expression: 0
   * Referenced by: '<S15>/Rate Limiter'
   */
  0.0,

  /* Expression: 1/1.609
   * Referenced by: '<S15>/Gain1'
   */
  0.62150403977625857,

  /* Expression: 30
   * Referenced by: '<S7>/MATLAB System2'
   */
  30.0,

  /* Expression: 0
   * Referenced by: '<S11>/Memory'
   */
  0.0,

  /* Expression: 0.01
   * Referenced by: '<S11>/Gain1'
   */
  0.01,

  /* Expression: 0
   * Referenced by: '<S11>/Memory'
   */
  0.0,

  /* Expression: 0.01
   * Referenced by: '<S11>/Gain'
   */
  0.01,

  /* Expression: 0
   * Referenced by: '<S11>/Driver Source'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S11>/Constant'
   */
  0.0,

  /* Expression: 0.5
   * Referenced by: '<S11>/Switch1'
   */
  0.5,

  /* Expression: 3
   * Referenced by: '<S7>/RS_actualSpeedSourceSelector'
   */
  3.0,

  /* Expression: 0
   * Referenced by: '<S7>/Memory'
   */
  0.0,

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
   * Referenced by: '<S11>/Constant1'
   */
  3.0,

  /* Expression: 1
   * Referenced by: '<S11>/Constant2'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S11>/Switch'
   */
  0.0,

  /* Expression: 3.6
   * Referenced by: '<S1>/Gain3'
   */
  3.6,

  /* Expression: 0
   * Referenced by: '<S12>/APP'
   */
  0.0,

  /* Expression: 2
   * Referenced by: '<S12>/Driver Control Selection'
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
   * Referenced by: '<S12>/Gain2'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S12>/Constant'
   */
  0.0,

  /* Expression: 100
   * Referenced by: '<S12>/AR_limit'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S12>/AR_limit'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S12>/BR_command_0to100'
   */
  0.0,

  /* Expression: 100
   * Referenced by: '<S12>/Gain1'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S12>/Constant1'
   */
  0.0,

  /* Expression: 100
   * Referenced by: '<S12>/BR_limit'
   */
  100.0,

  /* Expression: 0
   * Referenced by: '<S12>/BR_limit'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S12>/Min threshold for Brake switch'
   */
  0.0,

  /* Expression: 2.23694
   * Referenced by: '<S6>/Gain2'
   */
  2.23694,

  /* Expression: 1
   * Referenced by: '<S7>/Constant'
   */
  1.0,

  /* Expression: 0
   * Referenced by: '<S7>/RS_connectRequest'
   */
  0.0,

  /* Expression: 6
   * Referenced by: '<S7>/Switch'
   */
  6.0,

  /* Expression: 1
   * Referenced by: '<S7>/simState'
   */
  1.0,

  /* Expression: [255,0,0,255]
   * Referenced by: '<S8>/Constant'
   */
  { 255.0, 0.0, 0.0, 255.0 },

  /* Expression: 0
   * Referenced by: '<S61>/activeLaneChange'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S53>/Constant1'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S53>/switch for 0.1 send out'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S8>/Constant3'
   */
  0.0,

  /* Expression: 10
   * Referenced by: '<S10>/Constant1'
   */
  10.0,

  /* Expression: 4
   * Referenced by: '<S10>/headway (s)'
   */
  4.0,

  /* Expression: 0
   * Referenced by: '<S62>/Memory'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S62>/Memory1'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S10>/Switch'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S62>/Constant'
   */
  0.0,

  /* Expression: 0
   * Referenced by: '<S62>/Limits [-50,50]'
   */
  0.0,

  /* Expression: 50
   * Referenced by: '<S62>/Limits [-50,50]'
   */
  50.0,

  /* Expression: -50
   * Referenced by: '<S62>/Limits [-50,50]'
   */
  -50.0,

  /* Expression: 3
   * Referenced by: '<S62>/Kp'
   */
  3.0,

  /* Expression: 0
   * Referenced by: '<S62>/Limits [-50,50]1'
   */
  0.0,

  /* Expression: 50
   * Referenced by: '<S62>/Limits [-50,50]1'
   */
  50.0,

  /* Expression: -50
   * Referenced by: '<S62>/Limits [-50,50]1'
   */
  -50.0,

  /* Expression: 1
   * Referenced by: '<S62>/Kp_d'
   */
  1.0,

  /* Expression: max(10, RealSimPara.speedInit)
   * Referenced by: '<S62>/Saturation'
   */
  13.5,

  /* Expression: 0
   * Referenced by: '<S62>/Saturation'
   */
  0.0,

  /* Expression: 3
   * Referenced by: '<S62>/DrvSpeedCmdRateLimit'
   */
  3.0,

  /* Expression: -3
   * Referenced by: '<S62>/DrvSpeedCmdRateLimit'
   */
  -3.0,

  /* Expression: 0.3
   * Referenced by: '<S62>/Ki'
   */
  0.3,

  /* Expression: 0.1
   * Referenced by: '<S62>/Ki_d'
   */
  0.1,

  /* Expression: 0
   * Referenced by: '<S62>/Memory2'
   */
  0.0,

  /* Expression: 1
   * Referenced by: '<S62>/followDistRateLimit'
   */
  1.0,

  /* Expression: -1
   * Referenced by: '<S62>/followDistRateLimit'
   */
  -1.0,

  /* Expression: 0
   * Referenced by: '<S62>/followDistRateLimit'
   */
  0.0,

  /* Expression: 3.6
   * Referenced by: '<S1>/Gain1'
   */
  3.6,

  /* Expression: 2.23694
   * Referenced by: '<S1>/Gain2'
   */
  2.23694,

  /* Computed Parameter: ManualSwitch_CurrentSetting
   * Referenced by: '<S1>/Manual Switch'
   */
  1U
};
