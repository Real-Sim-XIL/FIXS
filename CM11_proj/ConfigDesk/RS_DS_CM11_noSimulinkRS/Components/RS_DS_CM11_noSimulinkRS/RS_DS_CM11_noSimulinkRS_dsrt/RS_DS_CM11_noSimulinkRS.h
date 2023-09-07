/*
 * RS_DS_CM11_noSimulinkRS.h
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

#ifndef RTW_HEADER_RS_DS_CM11_noSimulinkRS_h_
#define RTW_HEADER_RS_DS_CM11_noSimulinkRS_h_
#include <math.h>
#include <string.h>
#ifndef RS_DS_CM11_noSimulinkRS_COMMON_INCLUDES_
#define RS_DS_CM11_noSimulinkRS_COMMON_INCLUDES_
#include <Global.h>
#include <CarMaker4SL.h>
#include <TextUtils.h>
#include <DataDict.h>
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                            /* RS_DS_CM11_noSimulinkRS_COMMON_INCLUDES_ */

#include "RS_DS_CM11_noSimulinkRS_types.h"

/* Shared type includes */
#include "multiword_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetContStateDisabled
#define rtmGetContStateDisabled(rtm)   ((rtm)->contStateDisabled)
#endif

#ifndef rtmSetContStateDisabled
#define rtmSetContStateDisabled(rtm, val) ((rtm)->contStateDisabled = (val))
#endif

#ifndef rtmGetContStates
#define rtmGetContStates(rtm)          ((rtm)->contStates)
#endif

#ifndef rtmSetContStates
#define rtmSetContStates(rtm, val)     ((rtm)->contStates = (val))
#endif

#ifndef rtmGetContTimeOutputInconsistentWithStateAtMajorStepFlag
#define rtmGetContTimeOutputInconsistentWithStateAtMajorStepFlag(rtm) ((rtm)->CTOutputIncnstWithState)
#endif

#ifndef rtmSetContTimeOutputInconsistentWithStateAtMajorStepFlag
#define rtmSetContTimeOutputInconsistentWithStateAtMajorStepFlag(rtm, val) ((rtm)->CTOutputIncnstWithState = (val))
#endif

#ifndef rtmGetDerivCacheNeedsReset
#define rtmGetDerivCacheNeedsReset(rtm) ((rtm)->derivCacheNeedsReset)
#endif

#ifndef rtmSetDerivCacheNeedsReset
#define rtmSetDerivCacheNeedsReset(rtm, val) ((rtm)->derivCacheNeedsReset = (val))
#endif

#ifndef rtmGetIntgData
#define rtmGetIntgData(rtm)            ((rtm)->intgData)
#endif

#ifndef rtmSetIntgData
#define rtmSetIntgData(rtm, val)       ((rtm)->intgData = (val))
#endif

#ifndef rtmGetOdeF
#define rtmGetOdeF(rtm)                ((rtm)->odeF)
#endif

#ifndef rtmSetOdeF
#define rtmSetOdeF(rtm, val)           ((rtm)->odeF = (val))
#endif

#ifndef rtmGetPeriodicContStateIndices
#define rtmGetPeriodicContStateIndices(rtm) ((rtm)->periodicContStateIndices)
#endif

#ifndef rtmSetPeriodicContStateIndices
#define rtmSetPeriodicContStateIndices(rtm, val) ((rtm)->periodicContStateIndices = (val))
#endif

#ifndef rtmGetPeriodicContStateRanges
#define rtmGetPeriodicContStateRanges(rtm) ((rtm)->periodicContStateRanges)
#endif

#ifndef rtmSetPeriodicContStateRanges
#define rtmSetPeriodicContStateRanges(rtm, val) ((rtm)->periodicContStateRanges = (val))
#endif

#ifndef rtmGetZCCacheNeedsReset
#define rtmGetZCCacheNeedsReset(rtm)   ((rtm)->zCCacheNeedsReset)
#endif

#ifndef rtmSetZCCacheNeedsReset
#define rtmSetZCCacheNeedsReset(rtm, val) ((rtm)->zCCacheNeedsReset = (val))
#endif

#ifndef rtmGetdX
#define rtmGetdX(rtm)                  ((rtm)->derivs)
#endif

#ifndef rtmSetdX
#define rtmSetdX(rtm, val)             ((rtm)->derivs = (val))
#endif

#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
#define rtmGetStopRequested(rtm)       ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
#define rtmSetStopRequested(rtm, val)  ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
#define rtmGetStopRequestedPtr(rtm)    (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
#define rtmGetT(rtm)                   (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTPtr
#define rtmGetTPtr(rtm)                ((rtm)->Timing.t)
#endif

/* Block signals (default storage) */
typedef struct {
  real_T IoIn;                         /* '<S3>/IoIn' */
  real_T Environment_o1;               /* '<S3>/Environment' */
  real_T Environment_o2[14];           /* '<S3>/Environment' */
  real_T DrivMan_o1;                   /* '<S5>/DrivMan' */
  real_T DrivMan_o2[17];               /* '<S5>/DrivMan' */
  real_T VehicleControl_o1;            /* '<S8>/VehicleControl' */
  real_T VehicleControl_o2[29];        /* '<S8>/VehicleControl' */
  real_T BrakePedalCmd_pct;            /* '<S8>/Memory' */
  real_T Gain1;                        /* '<S8>/Gain1' */
  real_T AccelPedalCmd_pct;            /* '<S8>/Memory' */
  real_T Gain;                         /* '<S8>/Gain' */
  real_T CM_Time;                      /* '<S8>/Read CM Dict3' */
  real_T Switch1;                      /* '<S8>/Switch1' */
  real_T CM_Time_l;                    /* '<S1>/Read CM Dict3' */
  real_T SimCore_State;                /* '<S1>/Read CM Dict7' */
  real_T distanceTravel;               /* '<S1>/Read CM Dict8' */
  real_T driverSourceFinal;            /* '<S8>/Switch2' */
  real_T Switch[29];                   /* '<S8>/Switch' */
  real_T VehicleControlUpd_o1;         /* '<S8>/VehicleControlUpd' */
  real_T VehicleControlUpd_o2;         /* '<S8>/VehicleControlUpd' */
  real_T VehicleControlUpd_o3[5];      /* '<S8>/VehicleControlUpd' */
  real_T VehicleControlUpd_o4[12];     /* '<S8>/VehicleControlUpd' */
  real_T VehicleControlUpd_o5[11];     /* '<S8>/VehicleControlUpd' */
  real_T VehicleControlUpd_o6[2];      /* '<S8>/VehicleControlUpd' */
  real_T VehicleControlUpd_o7[12];     /* '<S8>/VehicleControlUpd' */
  real_T Steering_o1;                  /* '<S20>/Steering' */
  real_T Steering_o2[21];              /* '<S20>/Steering' */
  real_T Kinematics_o1;                /* '<S18>/Kinematics' */
  real_T Kinematics_o2[7];             /* '<S18>/Kinematics' */
  real_T Kinematics_o3[7];             /* '<S18>/Kinematics' */
  real_T Kinematics_o4[7];             /* '<S18>/Kinematics' */
  real_T Kinematics_o5[7];             /* '<S18>/Kinematics' */
  real_T Kinematics_o6[8];             /* '<S18>/Kinematics' */
  real_T Kinematics_o7[8];             /* '<S18>/Kinematics' */
  real_T Kinematics_o8[8];             /* '<S18>/Kinematics' */
  real_T Kinematics_o9[8];             /* '<S18>/Kinematics' */
  real_T Forces_o1;                    /* '<S26>/Forces' */
  real_T Forces_o2[12];                /* '<S26>/Forces' */
  real_T Forces_o3[13];                /* '<S26>/Forces' */
  real_T Forces_o4[16];                /* '<S26>/Forces' */
  real_T SuspensionControlUnit_o1;     /* '<S24>/Suspension Control Unit' */
  real_T SuspensionControlUnit_o2[16]; /* '<S24>/Suspension Control Unit' */
  real_T SuspensionForceElements_o1;   /* '<S24>/Suspension Force Elements' */
  real_T SuspensionForceElements_o2[24];/* '<S24>/Suspension Force Elements' */
  real_T SuspensionForceElementsUpdate;
                                  /* '<S24>/Suspension Force Elements Update' */
  real_T Kinetics_o1;                  /* '<S19>/Kinetics' */
  real_T Kinetics_o2[12];              /* '<S19>/Kinetics' */
  real_T Kinetics_o3[6];               /* '<S19>/Kinetics' */
  real_T Kinetics_o4[6];               /* '<S19>/Kinetics' */
  real_T Kinetics_o5[4];               /* '<S19>/Kinetics' */
  real_T Kinetics_o6[4];               /* '<S19>/Kinetics' */
  real_T Trailer_o1;                   /* '<S21>/Trailer' */
  real_T Trailer_o2[4];                /* '<S21>/Trailer' */
  real_T Trailer_o3[15];               /* '<S21>/Trailer' */
  real_T Trailer_o4[18];               /* '<S21>/Trailer' */
  real_T Trailer_o5[14];               /* '<S21>/Trailer' */
  real_T Trailer_o6[14];               /* '<S21>/Trailer' */
  real_T Trailer_o7[14];               /* '<S21>/Trailer' */
  real_T Trailer_o8[14];               /* '<S21>/Trailer' */
  real_T Trailer_o9[6];                /* '<S21>/Trailer' */
  real_T CarAndTrailerUpd_o1;          /* '<S16>/CarAndTrailerUpd' */
  real_T CarAndTrailerUpd_o2[9];       /* '<S16>/CarAndTrailerUpd' */
  real_T Sync;                         /* '<S10>/Brake' */
  real_T Brake_o2[16];                 /* '<S10>/Brake' */
  real_T BrakeUpd_o1;                  /* '<S10>/BrakeUpd' */
  real_T BrakeUpd_o2[20];              /* '<S10>/BrakeUpd' */
  real_T Sync_j;                       /* '<S13>/PowerTrain' */
  real_T PowerTrain_o2[42];            /* '<S13>/PowerTrain' */
  real_T PowerTrainUpd;                /* '<S13>/PowerTrainUpd' */
  real_T CM_User;                      /* '<S4>/CM_User' */
  real_T IoOut;                        /* '<S4>/IoOut' */
  real_T HIL_SpeedCmd_kph;             /* '<S1>/Gain3' */
  real_T VehSpd;                       /* '<S6>/Read CM Dict' */
  real_T VehSpdKph;                    /* '<S6>/Gain1' */
  real_T VehSpdKph_h;                  /* '<S6>/Gain3' */
  real_T Gain2;                        /* '<S9>/Gain2' */
  real_T MultiportSwitch1;             /* '<S9>/Multiport Switch1' */
  real_T AccelPedalCmd_pct_h;          /* '<S9>/AR_limit' */
  real_T Gain1_j;                      /* '<S9>/Gain1' */
  real_T MultiportSwitch2;             /* '<S9>/Multiport Switch2' */
  real_T BrakePedalCmd_pct_j;          /* '<S9>/BR_limit' */
  real_T VehSpdMph;                    /* '<S6>/Gain2' */
  real_T VehSpd_j;                     /* '<S1>/Read CM Dict' */
  real_T VehSpdKph_p;                  /* '<S1>/Gain1' */
  real_T VehSpdMph_l;                  /* '<S1>/Gain2' */
  real_T Trf_FOL_Targ_Dtct;            /* '<S1>/Read CM Dict1' */
  real_T Trf_FOL_dDist;                /* '<S1>/Read CM Dict2' */
  real_T Trf_FOL_dSpeed;               /* '<S1>/Read CM Dict4' */
  real_T Trf_FOL_ObjId;                /* '<S1>/Read CM Dict5' */
  real_T Trf_FOL_State;                /* '<S1>/Read CM Dict6' */
  real_T CycleMode;                    /* '<S1>/RealSimHILCycle' */
  real_T SpeedCmd;                     /* '<S1>/RealSimHILCycle' */
  real_T Vehicle_Spd_kph;              /* '<S12>/Data Type Conversion11' */
  real_T Drive;                        /* '<S39>/Constant' */
  real_T Memory1;                      /* '<S39>/Memory1' */
  real_T Memory2;                      /* '<S39>/Memory2' */
  real_T RefSpdkmhr;                   /* '<S39>/SpdSat' */
  real_T Memory1_f;                    /* '<S40>/Memory1' */
  real_T Memory;                       /* '<S40>/Memory' */
  real_T Add;                          /* '<S40>/Add' */
  real_T Feedforward;                  /* '<S40>/Feedforward' */
  real_T Driver_FdfwdTerm_pct;         /* '<S40>/FeedforwardGain' */
  real_T Driver_IntTerm_pct;           /* '<S40>/Limits [-50,50]' */
  real_T VS_Flt;                       /* '<S40>/Lowpass Filter' */
  real_T SpdErr;                       /* '<S40>/Sum' */
  real_T Kp;                           /* '<S40>/Kp' */
  real_T ProportionalGainScheduling;  /* '<S40>/Proportional Gain Scheduling' */
  real_T Driver_PropTerm_pct;          /* '<S40>/Product1' */
  real_T Driver_TotCmdPreLim_pct;      /* '<S40>/Sum1' */
  real_T Gain_m;                       /* '<S40>/Gain' */
  real_T Ki;                           /* '<S40>/Ki' */
  real_T BrakePedalPosPre_pct;         /* '<S40>/Switch2' */
  real_T AcceleratorPedalPosPre_pct;   /* '<S40>/Switch1' */
  real_T AcceleratorPedalPos_pct;      /* '<S40>/Switch3' */
  real_T BrakePedalPos_pct;            /* '<S40>/Switch4' */
  real_T RefSpdmph;                    /* '<S39>/km to miles' */
  real_T Vehicle_Spd_mph;              /* '<S12>/Gain' */
  real_T RateLimiter;                  /* '<S12>/Rate Limiter' */
  real_T RefSpdkmhr_e;                 /* '<S12>/Multiport Switch1' */
  real_T Vehicle_SpdDmd_mph;           /* '<S12>/Gain1' */
  boolean_T Equal;                     /* '<S8>/Equal' */
  boolean_T BrakeSwitch;               /* '<S9>/Relational Operator' */
  boolean_T Compare;                   /* '<S14>/Compare' */
  boolean_T Compare_p;                 /* '<S41>/Compare' */
  boolean_T Compare_d;                 /* '<S42>/Compare' */
  boolean_T Uk1;                       /* '<S43>/Delay Input1' */
  boolean_T Compare_i;                 /* '<S45>/Compare' */
  boolean_T FixPtRelationalOperator;   /* '<S43>/FixPt Relational Operator' */
  boolean_T Uk1_p;                     /* '<S44>/Delay Input1' */
  boolean_T Compare_f;                 /* '<S46>/Compare' */
  boolean_T FixPtRelationalOperator_b; /* '<S44>/FixPt Relational Operator' */
  boolean_T LogicalOperator;           /* '<S40>/Logical Operator' */
  boolean_T BrakeSwitch_b;             /* '<S40>/Relational Operator' */
} B_RS_DS_CM11_noSimulinkRS_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T Memory_2_PreviousInput;       /* '<S8>/Memory' */
  real_T Memory_1_PreviousInput;       /* '<S8>/Memory' */
  real_T Memory1_PreviousInput;        /* '<S39>/Memory1' */
  real_T Memory2_PreviousInput;        /* '<S39>/Memory2' */
  real_T Memory1_PreviousInput_f;      /* '<S40>/Memory1' */
  real_T Memory_PreviousInput;         /* '<S40>/Memory' */
  real_T PrevY;                        /* '<S12>/Rate Limiter' */
  struct {
    void *Entry;
  } ReadCMDict3_PWORK;                 /* '<S8>/Read CM Dict3' */

  struct {
    void *Entry;
  } ReadCMDict3_PWORK_e;               /* '<S1>/Read CM Dict3' */

  struct {
    void *Entry;
  } ReadCMDict7_PWORK;                 /* '<S1>/Read CM Dict7' */

  struct {
    void *Entry;
  } ReadCMDict8_PWORK;                 /* '<S1>/Read CM Dict8' */

  struct {
    void *Entry;
  } ReadCMDict_PWORK;                  /* '<S6>/Read CM Dict' */

  struct {
    void *Entry;
  } ReadCMDict_PWORK_c;                /* '<S1>/Read CM Dict' */

  struct {
    void *Entry;
  } ReadCMDict1_PWORK;                 /* '<S1>/Read CM Dict1' */

  struct {
    void *Entry;
  } ReadCMDict2_PWORK;                 /* '<S1>/Read CM Dict2' */

  struct {
    void *Entry;
  } ReadCMDict4_PWORK;                 /* '<S1>/Read CM Dict4' */

  struct {
    void *Entry;
  } ReadCMDict5_PWORK;                 /* '<S1>/Read CM Dict5' */

  struct {
    void *Entry;
  } ReadCMDict6_PWORK;                 /* '<S1>/Read CM Dict6' */

  uint32_T temporalCounter_i1;         /* '<S1>/RealSimHILCycle' */
  uint32_T m_bpIndex;                 /* '<S40>/Proportional Gain Scheduling' */
  boolean_T DelayInput1_DSTATE;        /* '<S43>/Delay Input1' */
  boolean_T DelayInput1_DSTATE_h;      /* '<S44>/Delay Input1' */
  uint8_T is_active_c2_RS_DS_CM11_noSimulinkRS;/* '<S1>/RealSimHILCycle' */
  uint8_T is_c2_RS_DS_CM11_noSimulinkRS;/* '<S1>/RealSimHILCycle' */
  uint8_T is_CycleOn;                  /* '<S1>/RealSimHILCycle' */
} DW_RS_DS_CM11_noSimulinkRS_T;

/* Continuous states (default storage) */
typedef struct {
  real_T Limits5050_CSTATE;            /* '<S40>/Limits [-50,50]' */
  real_T LowpassFilter_CSTATE;         /* '<S40>/Lowpass Filter' */
} X_RS_DS_CM11_noSimulinkRS_T;

/* State derivatives (default storage) */
typedef struct {
  real_T Limits5050_CSTATE;            /* '<S40>/Limits [-50,50]' */
  real_T LowpassFilter_CSTATE;         /* '<S40>/Lowpass Filter' */
} XDot_RS_DS_CM11_noSimulinkRS_T;

/* State disabled  */
typedef struct {
  boolean_T Limits5050_CSTATE;         /* '<S40>/Limits [-50,50]' */
  boolean_T LowpassFilter_CSTATE;      /* '<S40>/Lowpass Filter' */
} XDis_RS_DS_CM11_noSimulinkRS_T;

/* Zero-crossing (trigger) state */
typedef struct {
  ZCSigState Limits5050_Reset_ZCE;     /* '<S40>/Limits [-50,50]' */
} PrevZCX_RS_DS_CM11_noSimulinkRS_T;

#ifndef ODE1_INTG
#define ODE1_INTG

/* ODE1 Integration Data */
typedef struct {
  real_T *f[1];                        /* derivatives */
} ODE1_IntgData;

#endif

/* Parameters (default storage) */
struct P_RS_DS_CM11_noSimulinkRS_T_ {
  struct_GphLW54WCcJjc0lEaqz83B RealSimPara;/* Variable: RealSimPara
                                             * Referenced by: '<S1>/Constant'
                                             */
  real_T CompareToConstant1_const;   /* Mask Parameter: CompareToConstant1_const
                                      * Referenced by: '<S41>/Constant'
                                      */
  real_T CompareToConstant3_const;   /* Mask Parameter: CompareToConstant3_const
                                      * Referenced by: '<S42>/Constant'
                                      */
  real_T VehicleSource_BuiltIn_const;
                                  /* Mask Parameter: VehicleSource_BuiltIn_const
                                   * Referenced by: '<S14>/Constant'
                                   */
  boolean_T DetectFallNegative_vinit;/* Mask Parameter: DetectFallNegative_vinit
                                      * Referenced by: '<S43>/Delay Input1'
                                      */
  boolean_T DetectRisePositive_vinit;/* Mask Parameter: DetectRisePositive_vinit
                                      * Referenced by: '<S44>/Delay Input1'
                                      */
  real_T Constant_Value;               /* Expression: 0
                                        * Referenced by: '<S45>/Constant'
                                        */
  real_T Constant_Value_d;             /* Expression: 0
                                        * Referenced by: '<S46>/Constant'
                                        */
  real_T Constant_Value_n;             /* Expression: 1
                                        * Referenced by: '<S39>/Constant'
                                        */
  real_T Memory1_InitialCondition;     /* Expression: 0
                                        * Referenced by: '<S39>/Memory1'
                                        */
  real_T Memory2_InitialCondition;     /* Expression: 0
                                        * Referenced by: '<S39>/Memory2'
                                        */
  real_T SpdSat_UpperSat;              /* Expression: 190
                                        * Referenced by: '<S39>/SpdSat'
                                        */
  real_T SpdSat_LowerSat;              /* Expression: -5
                                        * Referenced by: '<S39>/SpdSat'
                                        */
  real_T Memory1_InitialCondition_m;   /* Expression: 0
                                        * Referenced by: '<S40>/Memory1'
                                        */
  real_T Memory_InitialCondition;      /* Expression: 0
                                        * Referenced by: '<S40>/Memory'
                                        */
  real_T Constant_Value_p;             /* Expression: 0
                                        * Referenced by: '<S40>/Constant'
                                        */
  real_T Feedforward_tableData[17];
       /* Expression: [0	3.5	12.5	16	20	21	22	24.5	26	29.5	32	33	34	36	37	38	39]
        * Referenced by: '<S40>/Feedforward'
        */
  real_T Feedforward_bp01Data[17];
  /* Expression: [0 10    20    30    40    50    60    70    80    90   100   110   120   130   140   150   160]
   * Referenced by: '<S40>/Feedforward'
   */
  real_T FeedforwardGain_Gain;         /* Expression: 1
                                        * Referenced by: '<S40>/FeedforwardGain'
                                        */
  real_T Limits5050_IC;                /* Expression: 0
                                        * Referenced by: '<S40>/Limits [-50,50]'
                                        */
  real_T Limits5050_UpperSat;          /* Expression: 50
                                        * Referenced by: '<S40>/Limits [-50,50]'
                                        */
  real_T Limits5050_LowerSat;          /* Expression: -50
                                        * Referenced by: '<S40>/Limits [-50,50]'
                                        */
  real_T LowpassFilter_A;              /* Computed Parameter: LowpassFilter_A
                                        * Referenced by: '<S40>/Lowpass Filter'
                                        */
  real_T LowpassFilter_C;              /* Computed Parameter: LowpassFilter_C
                                        * Referenced by: '<S40>/Lowpass Filter'
                                        */
  real_T Kp_Gain;                      /* Expression: 7
                                        * Referenced by: '<S40>/Kp'
                                        */
  real_T ProportionalGainScheduling_tableData[3];/* Expression: [1 1 1]
                                                  * Referenced by: '<S40>/Proportional Gain Scheduling'
                                                  */
  real_T ProportionalGainScheduling_bp01Data[3];/* Expression: [ 0 30 70]
                                                 * Referenced by: '<S40>/Proportional Gain Scheduling'
                                                 */
  real_T Gain_Gain;                    /* Expression: -1
                                        * Referenced by: '<S40>/Gain'
                                        */
  real_T Ki_Gain;                      /* Expression: 1
                                        * Referenced by: '<S40>/Ki'
                                        */
  real_T MinthresholdforBrakeswitch_Value;/* Expression: 0
                                           * Referenced by: '<S40>/Min threshold for Brake switch'
                                           */
  real_T Switch2_Threshold;            /* Expression: 0
                                        * Referenced by: '<S40>/Switch2'
                                        */
  real_T Switch1_Threshold;            /* Expression: 0
                                        * Referenced by: '<S40>/Switch1'
                                        */
  real_T standstillbrake0100_Value;    /* Expression: 50
                                        * Referenced by: '<S40>/standstill brake [0,100]'
                                        */
  real_T kmtomiles_Gain;               /* Expression: 1/1.609344
                                        * Referenced by: '<S39>/km to miles'
                                        */
  real_T Gain_Gain_b;                  /* Expression: 1/1.609
                                        * Referenced by: '<S12>/Gain'
                                        */
  real_T ManSpdSel_Value;              /* Expression: 3
                                        * Referenced by: '<S12>/ManSpdSel'
                                        */
  real_T ManSpd_Value;                 /* Expression: 0
                                        * Referenced by: '<S12>/ManSpd'
                                        */
  real_T RateLimiter_RisingLim;        /* Expression: 5
                                        * Referenced by: '<S12>/Rate Limiter'
                                        */
  real_T RateLimiter_FallingLim;       /* Expression: -5
                                        * Referenced by: '<S12>/Rate Limiter'
                                        */
  real_T RateLimiter_IC;               /* Expression: 0
                                        * Referenced by: '<S12>/Rate Limiter'
                                        */
  real_T Gain1_Gain;                   /* Expression: 1/1.609
                                        * Referenced by: '<S12>/Gain1'
                                        */
  real_T Memory_2_InitialCondition;    /* Expression: 0
                                        * Referenced by: '<S8>/Memory'
                                        */
  real_T Gain1_Gain_d;                 /* Expression: 0.01
                                        * Referenced by: '<S8>/Gain1'
                                        */
  real_T Memory_1_InitialCondition;    /* Expression: 0
                                        * Referenced by: '<S8>/Memory'
                                        */
  real_T Gain_Gain_o;                  /* Expression: 0.01
                                        * Referenced by: '<S8>/Gain'
                                        */
  real_T DriverSource_Value;           /* Expression: 0
                                        * Referenced by: '<S8>/Driver Source'
                                        */
  real_T Constant_Value_b;             /* Expression: 0
                                        * Referenced by: '<S8>/Constant'
                                        */
  real_T Switch1_Threshold_e;          /* Expression: 0.5
                                        * Referenced by: '<S8>/Switch1'
                                        */
  real_T initialWaitTime_Value;        /* Expression: 20
                                        * Referenced by: '<S1>/initialWaitTime'
                                        */
  real_T totalDistancem_Value;         /* Expression: 2000
                                        * Referenced by: '<S1>/totalDistance (m)'
                                        */
  real_T totalTimesec_Value;           /* Expression: 1000
                                        * Referenced by: '<S1>/totalTime (sec)'
                                        */
  real_T RampDownFlag_Value;           /* Expression: 0
                                        * Referenced by: '<S1>/RampDownFlag'
                                        */
  real_T Constant1_Value;              /* Expression: 3
                                        * Referenced by: '<S8>/Constant1'
                                        */
  real_T Constant2_Value;              /* Expression: 1
                                        * Referenced by: '<S8>/Constant2'
                                        */
  real_T Switch_Threshold;             /* Expression: 0
                                        * Referenced by: '<S8>/Switch'
                                        */
  real_T Gain3_Gain;                   /* Expression: 3.6
                                        * Referenced by: '<S1>/Gain3'
                                        */
  real_T APP_Value;                    /* Expression: 0
                                        * Referenced by: '<S9>/APP'
                                        */
  real_T DriverControlSelection_Value; /* Expression: 2
                                        * Referenced by: '<S9>/Driver Control Selection'
                                        */
  real_T Gain1_Gain_o;                 /* Expression: 3.6
                                        * Referenced by: '<S6>/Gain1'
                                        */
  real_T Gain3_Gain_n;                 /* Expression: 3.6
                                        * Referenced by: '<S6>/Gain3'
                                        */
  real_T Gain2_Gain;                   /* Expression: 100
                                        * Referenced by: '<S9>/Gain2'
                                        */
  real_T Constant_Value_e;             /* Expression: 0
                                        * Referenced by: '<S9>/Constant'
                                        */
  real_T AR_limit_UpperSat;            /* Expression: 100
                                        * Referenced by: '<S9>/AR_limit'
                                        */
  real_T AR_limit_LowerSat;            /* Expression: 0
                                        * Referenced by: '<S9>/AR_limit'
                                        */
  real_T BR_command_0to100_Value;      /* Expression: 0
                                        * Referenced by: '<S9>/BR_command_0to100'
                                        */
  real_T Gain1_Gain_c;                 /* Expression: 100
                                        * Referenced by: '<S9>/Gain1'
                                        */
  real_T Constant1_Value_j;            /* Expression: 0
                                        * Referenced by: '<S9>/Constant1'
                                        */
  real_T BR_limit_UpperSat;            /* Expression: 100
                                        * Referenced by: '<S9>/BR_limit'
                                        */
  real_T BR_limit_LowerSat;            /* Expression: 0
                                        * Referenced by: '<S9>/BR_limit'
                                        */
  real_T MinthresholdforBrakeswitch_Value_g;/* Expression: 0
                                             * Referenced by: '<S9>/Min threshold for Brake switch'
                                             */
  real_T Gain2_Gain_p;                 /* Expression: 2.23694
                                        * Referenced by: '<S6>/Gain2'
                                        */
  real_T Gain1_Gain_dz;                /* Expression: 3.6
                                        * Referenced by: '<S1>/Gain1'
                                        */
  real_T Gain2_Gain_o;                 /* Expression: 2.23694
                                        * Referenced by: '<S1>/Gain2'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_RS_DS_CM11_noSimulinkRS_T {
  const char_T *errorStatus;
  RTWSolverInfo solverInfo;
  X_RS_DS_CM11_noSimulinkRS_T *contStates;
  int_T *periodicContStateIndices;
  real_T *periodicContStateRanges;
  real_T *derivs;
  boolean_T *contStateDisabled;
  boolean_T zCCacheNeedsReset;
  boolean_T derivCacheNeedsReset;
  boolean_T CTOutputIncnstWithState;
  real_T odeF[1][2];
  ODE1_IntgData intgData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numSampTimes;
  } Sizes;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Block parameters (default storage) */
extern P_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_P;

/* Block signals (default storage) */
extern B_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_B;

/* Continuous states (default storage) */
extern X_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_X;

/* Block states (default storage) */
extern DW_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_DW;

/* Zero-crossing (trigger) state */
extern PrevZCX_RS_DS_CM11_noSimulinkRS_T RS_DS_CM11_noSimulinkRS_PrevZCX;

/* Model entry point functions */
extern void RS_DS_CM11_noSimulinkRS_initialize(void);
extern void RS_DS_CM11_noSimulinkRS_output(void);
extern void RS_DS_CM11_noSimulinkRS_update(void);
extern void RS_DS_CM11_noSimulinkRS_terminate(void);

/* Real-time Model object */
extern RT_MODEL_RS_DS_CM11_noSimulinkRS_T *const RS_DS_CM11_noSimulinkRS_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'RS_DS_CM11_noSimulinkRS'
 * '<S1>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker'
 * '<S2>'   : 'RS_DS_CM11_noSimulinkRS/Open CarMaker GUI'
 * '<S3>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker/CM_FIRST'
 * '<S4>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker/CM_LAST'
 * '<S5>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker/DrivMan'
 * '<S6>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle'
 * '<S7>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker/RealSimHILCycle'
 * '<S8>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker/VehicleControl'
 * '<S9>'   : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Actuation and Outputs'
 * '<S10>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Brake'
 * '<S11>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer'
 * '<S12>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle'
 * '<S13>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/PowerTrain'
 * '<S14>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/VehicleSource_BuiltIn'
 * '<S15>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Brake/CreateBus Vhcl.Wheel.PT'
 * '<S16>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/CarAndTrailerUpd'
 * '<S17>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces'
 * '<S18>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Kinematics'
 * '<S19>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Kinetics'
 * '<S20>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Steering'
 * '<S21>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer'
 * '<S22>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/CarAndTrailerUpd/CreateBus Brake.IF.In'
 * '<S23>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/External Suspension Forces'
 * '<S24>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Suspension Forces'
 * '<S25>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires'
 * '<S26>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Vehicle Forces'
 * '<S27>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireFL'
 * '<S28>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireFR'
 * '<S29>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireRL'
 * '<S30>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireRR'
 * '<S31>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Car.Hitch.FrcTrq'
 * '<S32>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Motion'
 * '<S33>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.PoI'
 * '<S34>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Steering'
 * '<S35>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.FL'
 * '<S36>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.FR'
 * '<S37>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.RL'
 * '<S38>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.RR'
 * '<S39>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control'
 * '<S40>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller'
 * '<S41>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Compare To Constant1'
 * '<S42>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Compare To Constant3'
 * '<S43>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Fall Negative'
 * '<S44>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Rise Positive'
 * '<S45>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Fall Negative/Negative'
 * '<S46>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Rise Positive/Positive'
 * '<S47>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/VehicleControl/CreateBus Vhcl.Misc'
 * '<S48>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/VehicleControl/CreateBus Vhcl.Wheel.In'
 * '<S49>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/VehicleControl/CreateBus VhclCtrl'
 * '<S50>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/VehicleControl/CreateBus VhclCtrl.PT'
 * '<S51>'  : 'RS_DS_CM11_noSimulinkRS/CarMaker/VehicleControl/CreateBus VhclCtrl.Steering'
 */
#endif                               /* RTW_HEADER_RS_DS_CM11_noSimulinkRS_h_ */
