/*
 * RS_DS_CM11_SimulinkRS.h
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

#ifndef RTW_HEADER_RS_DS_CM11_SimulinkRS_h_
#define RTW_HEADER_RS_DS_CM11_SimulinkRS_h_
#include <math.h>
#include <string.h>
#ifndef RS_DS_CM11_SimulinkRS_COMMON_INCLUDES_
#define RS_DS_CM11_SimulinkRS_COMMON_INCLUDES_
#include <stdio.h>
#include <Global.h>
#include <CarMaker4SL.h>
#include <TextUtils.h>
#include <DataDict.h>
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "RS_DS_CM11_SimulinkRS_types.h"
#include "RS_DS_CM11_SimulinkRS_dsmpbap.h"
#endif                              /* RS_DS_CM11_SimulinkRS_COMMON_INCLUDES_ */

#include "RS_DS_CM11_SimulinkRS_types.h"

/* Shared type includes */
#include "multiword_types.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"
#include "rtGetNaN.h"

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
  VehDataBus BusConversion_InsertedFor_MATLABSystem1_at_inport_2_BusCreator1;
  VehDataBus MATLABSystem_o4;          /* '<S7>/MATLAB System' */
  real_T IoIn;                         /* '<S3>/IoIn' */
  real_T Environment_o1;               /* '<S3>/Environment' */
  real_T Environment_o2[14];           /* '<S3>/Environment' */
  real_T DrivMan_o1;                   /* '<S5>/DrivMan' */
  real_T DrivMan_o2[17];               /* '<S5>/DrivMan' */
  real_T VehicleControl_o1;            /* '<S11>/VehicleControl' */
  real_T VehicleControl_o2[29];        /* '<S11>/VehicleControl' */
  real_T BrakePedalCmd_pct;            /* '<S11>/Memory' */
  real_T Gain1;                        /* '<S11>/Gain1' */
  real_T AccelPedalCmd_pct;            /* '<S11>/Memory' */
  real_T Gain;                         /* '<S11>/Gain' */
  real_T CM_Time;                      /* '<S11>/Read CM Dict3' */
  real_T Switch1;                      /* '<S11>/Switch1' */
  real_T CM_Time_b;                    /* '<S1>/Read CM Dict3' */
  real_T SimCore_State;                /* '<S1>/Read CM Dict7' */
  real_T Clock;                        /* '<S7>/Clock' */
  real_T ConnectionState;              /* '<S52>/Data Type Conversion4' */
  real_T speedDesired;                 /* '<S7>/Bus Selector' */
  real_T Memory;                       /* '<S7>/Memory' */
  real_T ReadCMDict2;                  /* '<S8>/Read CM Dict2' */
  real_T speedActual;                  /* '<S7>/Multiport Switch' */
  real_T ManualSwitch;                 /* '<S1>/Manual Switch' */
  real_T distanceTravel;               /* '<S1>/Read CM Dict8' */
  real_T driverSourceFinal;            /* '<S11>/Switch2' */
  real_T Switch[29];                   /* '<S11>/Switch' */
  real_T VehicleControlUpd_o1;         /* '<S11>/VehicleControlUpd' */
  real_T VehicleControlUpd_o2;         /* '<S11>/VehicleControlUpd' */
  real_T VehicleControlUpd_o3[5];      /* '<S11>/VehicleControlUpd' */
  real_T VehicleControlUpd_o4[12];     /* '<S11>/VehicleControlUpd' */
  real_T VehicleControlUpd_o5[11];     /* '<S11>/VehicleControlUpd' */
  real_T VehicleControlUpd_o6[2];      /* '<S11>/VehicleControlUpd' */
  real_T VehicleControlUpd_o7[12];     /* '<S11>/VehicleControlUpd' */
  real_T Steering_o1;                  /* '<S23>/Steering' */
  real_T Steering_o2[21];              /* '<S23>/Steering' */
  real_T Kinematics_o1;                /* '<S21>/Kinematics' */
  real_T Kinematics_o2[7];             /* '<S21>/Kinematics' */
  real_T Kinematics_o3[7];             /* '<S21>/Kinematics' */
  real_T Kinematics_o4[7];             /* '<S21>/Kinematics' */
  real_T Kinematics_o5[7];             /* '<S21>/Kinematics' */
  real_T Kinematics_o6[8];             /* '<S21>/Kinematics' */
  real_T Kinematics_o7[8];             /* '<S21>/Kinematics' */
  real_T Kinematics_o8[8];             /* '<S21>/Kinematics' */
  real_T Kinematics_o9[8];             /* '<S21>/Kinematics' */
  real_T Forces_o1;                    /* '<S29>/Forces' */
  real_T Forces_o2[12];                /* '<S29>/Forces' */
  real_T Forces_o3[13];                /* '<S29>/Forces' */
  real_T Forces_o4[16];                /* '<S29>/Forces' */
  real_T SuspensionControlUnit_o1;     /* '<S27>/Suspension Control Unit' */
  real_T SuspensionControlUnit_o2[16]; /* '<S27>/Suspension Control Unit' */
  real_T SuspensionForceElements_o1;   /* '<S27>/Suspension Force Elements' */
  real_T SuspensionForceElements_o2[24];/* '<S27>/Suspension Force Elements' */
  real_T SuspensionForceElementsUpdate;
                                  /* '<S27>/Suspension Force Elements Update' */
  real_T Kinetics_o1;                  /* '<S22>/Kinetics' */
  real_T Kinetics_o2[12];              /* '<S22>/Kinetics' */
  real_T Kinetics_o3[6];               /* '<S22>/Kinetics' */
  real_T Kinetics_o4[6];               /* '<S22>/Kinetics' */
  real_T Kinetics_o5[4];               /* '<S22>/Kinetics' */
  real_T Kinetics_o6[4];               /* '<S22>/Kinetics' */
  real_T Trailer_o1;                   /* '<S24>/Trailer' */
  real_T Trailer_o2[4];                /* '<S24>/Trailer' */
  real_T Trailer_o3[15];               /* '<S24>/Trailer' */
  real_T Trailer_o4[18];               /* '<S24>/Trailer' */
  real_T Trailer_o5[14];               /* '<S24>/Trailer' */
  real_T Trailer_o6[14];               /* '<S24>/Trailer' */
  real_T Trailer_o7[14];               /* '<S24>/Trailer' */
  real_T Trailer_o8[14];               /* '<S24>/Trailer' */
  real_T Trailer_o9[6];                /* '<S24>/Trailer' */
  real_T CarAndTrailerUpd_o1;          /* '<S19>/CarAndTrailerUpd' */
  real_T CarAndTrailerUpd_o2[9];       /* '<S19>/CarAndTrailerUpd' */
  real_T Sync;                         /* '<S13>/Brake' */
  real_T Brake_o2[16];                 /* '<S13>/Brake' */
  real_T BrakeUpd_o1;                  /* '<S13>/BrakeUpd' */
  real_T BrakeUpd_o2[20];              /* '<S13>/BrakeUpd' */
  real_T Sync_c;                       /* '<S16>/PowerTrain' */
  real_T PowerTrain_o2[42];            /* '<S16>/PowerTrain' */
  real_T PowerTrainUpd;                /* '<S16>/PowerTrainUpd' */
  real_T CM_User;                      /* '<S4>/CM_User' */
  real_T IoOut;                        /* '<S4>/IoOut' */
  real_T HIL_SpeedCmd_kph;             /* '<S1>/Gain3' */
  real_T VehSpd;                       /* '<S6>/Read CM Dict' */
  real_T VehSpdKph;                    /* '<S6>/Gain1' */
  real_T VehSpdKph_p;                  /* '<S6>/Gain3' */
  real_T Gain2;                        /* '<S12>/Gain2' */
  real_T MultiportSwitch1;             /* '<S12>/Multiport Switch1' */
  real_T AccelPedalCmd_pct_g;          /* '<S12>/AR_limit' */
  real_T Gain1_p;                      /* '<S12>/Gain1' */
  real_T MultiportSwitch2;             /* '<S12>/Multiport Switch2' */
  real_T BrakePedalCmd_pct_a;          /* '<S12>/BR_limit' */
  real_T VehSpdMph;                    /* '<S6>/Gain2' */
  real_T SimCore_State_b;              /* '<S7>/Read CM Dict7' */
  real_T connectRequest;               /* '<S7>/Switch' */
  real_T heading;                      /* '<S7>/Bus Selector2' */
  real_T signalLightHeadId;            /* '<S7>/Bus Selector2' */
  real_T signalLightDistance;          /* '<S7>/Bus Selector2' */
  real_T signalLightColor;             /* '<S7>/Bus Selector2' */
  real_T speedLimit;                   /* '<S7>/Bus Selector2' */
  real_T speedLimitNext;               /* '<S7>/Bus Selector2' */
  real_T speedLimitChangeDistance;     /* '<S7>/Bus Selector2' */
  real_T ReadCMDict1;                  /* '<S8>/Read CM Dict1' */
  real_T ReadCMDict6;                  /* '<S8>/Read CM Dict6' */
  real_T ReadCMDict7;                  /* '<S8>/Read CM Dict7' */
  real_T ReadCMDict;                   /* '<S8>/Read CM Dict' */
  real_T ReadCMDict4;                  /* '<S8>/Read CM Dict4' */
  real_T ReadCMDict5;                  /* '<S8>/Read CM Dict5' */
  real_T ReadCMDict3;                  /* '<S8>/Read CM Dict3' */
  real_T LinkSpeed;                    /* '<S54>/Data Inport S-Fcn' */
  real_T Constant3;                    /* '<S8>/Constant3' */
  real_T ReadCMDict11;                 /* '<S10>/Read CM Dict11' */
  real_T Trf_FOL_dSpeed;               /* '<S10>/Read CM Dict9' */
  real_T Trf_Prec_Speed;               /* '<S10>/Add2' */
  real_T Divide;                       /* '<S10>/Divide' */
  real_T followingDistanceDesired;     /* '<S10>/Add3' */
  real_T Memory_g;                     /* '<S62>/Memory' */
  real_T Add;                          /* '<S62>/Add' */
  real_T ReadCMDict6_n;                /* '<S62>/Read CM Dict6' */
  real_T Memory1;                      /* '<S62>/Memory1' */
  real_T Add1;                         /* '<S62>/Add1' */
  real_T Trf_FOL_Targ_Dtct;            /* '<S10>/Read CM Dict10' */
  real_T RefSpd;                       /* '<S10>/Switch' */
  real_T Driver_IntTerm_pct;           /* '<S62>/Limits [-50,50]' */
  real_T SpdErr;                       /* '<S62>/Sum' */
  real_T Driver_PropTerm_pct;          /* '<S62>/Kp' */
  real_T iTermDist;                    /* '<S62>/Limits [-50,50]1' */
  real_T Trf_FOL_dDist;                /* '<S10>/Read CM Dict12' */
  real_T distErr;                      /* '<S62>/Sum3' */
  real_T pTermDist;                    /* '<S62>/Kp_d' */
  real_T Sum2;                         /* '<S62>/Sum2' */
  real_T distCmd;                      /* '<S62>/Switch' */
  real_T Driver_TotCmdPreLim_pct;      /* '<S62>/Sum1' */
  real_T Saturation;                   /* '<S62>/Saturation' */
  real_T DrvSpeedCmdRateLimit;         /* '<S62>/DrvSpeedCmdRateLimit' */
  real_T Ki;                           /* '<S62>/Ki' */
  real_T Ki_d;                         /* '<S62>/Ki_d' */
  real_T Memory2;                      /* '<S62>/Memory2' */
  real_T followDistRateLimit;          /* '<S62>/followDistRateLimit' */
  real_T VehSpd_b;                     /* '<S1>/Read CM Dict' */
  real_T VehSpdKph_p4;                 /* '<S1>/Gain1' */
  real_T VehSpdMph_g;                  /* '<S1>/Gain2' */
  real_T Trf_FOL_Targ_Dtct_l;          /* '<S1>/Read CM Dict1' */
  real_T Trf_FOL_dDist_j;              /* '<S1>/Read CM Dict2' */
  real_T Trf_FOL_dSpeed_i;             /* '<S1>/Read CM Dict4' */
  real_T Trf_FOL_ObjId;                /* '<S1>/Read CM Dict5' */
  real_T Trf_FOL_State;                /* '<S1>/Read CM Dict6' */
  real_T CycleMode;                    /* '<S1>/RealSimHILCycle' */
  real_T SpeedCmd;                     /* '<S1>/RealSimHILCycle' */
  real_T positionX;                    /* '<S8>/MATLAB Function' */
  real_T positionY;                    /* '<S8>/MATLAB Function' */
  real_T positionZ;                    /* '<S8>/MATLAB Function' */
  real_T heading_b;                    /* '<S8>/MATLAB Function' */
  real_T color;                        /* '<S8>/MATLAB Function' */
  real_T speedDesired_i;               /* '<S8>/MATLAB Function' */
  real_T MATLABSystem2_o1;             /* '<S7>/MATLAB System2' */
  real_T accelerationDesired;          /* '<S7>/MATLAB System2' */
  real_T timeSimulator;                /* '<S7>/MATLAB System2' */
  real_T timeStepSimulator;            /* '<S7>/MATLAB System2' */
  real_T timeStepTrigger;              /* '<S7>/MATLAB System2' */
  real_T MATLABSystem2_o6;             /* '<S7>/MATLAB System2' */
  real_T nMsgSend;                     /* '<S7>/MATLAB System1' */
  real_T MATLABSystem_o1;              /* '<S7>/MATLAB System' */
  real_T MATLABSystem_o2;              /* '<S7>/MATLAB System' */
  real_T MATLABSystem_o3;              /* '<S7>/MATLAB System' */
  real_T MATLABSystem_o5;              /* '<S7>/MATLAB System' */
  real_T intVal;                       /* '<S7>/MATLAB Function' */
  real_T Vehicle_Spd_kph;              /* '<S15>/Data Type Conversion11' */
  real_T Drive;                        /* '<S42>/Constant' */
  real_T Memory1_b;                    /* '<S42>/Memory1' */
  real_T Memory2_o;                    /* '<S42>/Memory2' */
  real_T RefSpdkmhr;                   /* '<S42>/SpdSat' */
  real_T Memory1_e;                    /* '<S43>/Memory1' */
  real_T Memory_m;                     /* '<S43>/Memory' */
  real_T Add_i;                        /* '<S43>/Add' */
  real_T Feedforward;                  /* '<S43>/Feedforward' */
  real_T Driver_FdfwdTerm_pct;         /* '<S43>/FeedforwardGain' */
  real_T Driver_IntTerm_pct_b;         /* '<S43>/Limits [-50,50]' */
  real_T VS_Flt;                       /* '<S43>/Lowpass Filter' */
  real_T SpdErr_j;                     /* '<S43>/Sum' */
  real_T Kp;                           /* '<S43>/Kp' */
  real_T ProportionalGainScheduling;  /* '<S43>/Proportional Gain Scheduling' */
  real_T Driver_PropTerm_pct_i;        /* '<S43>/Product1' */
  real_T Driver_TotCmdPreLim_pct_l;    /* '<S43>/Sum1' */
  real_T Gain_d;                       /* '<S43>/Gain' */
  real_T Ki_g;                         /* '<S43>/Ki' */
  real_T BrakePedalPosPre_pct;         /* '<S43>/Switch2' */
  real_T AcceleratorPedalPosPre_pct;   /* '<S43>/Switch1' */
  real_T AcceleratorPedalPos_pct;      /* '<S43>/Switch3' */
  real_T BrakePedalPos_pct;            /* '<S43>/Switch4' */
  real_T RefSpdmph;                    /* '<S42>/km to miles' */
  real_T Vehicle_Spd_mph;              /* '<S15>/Gain' */
  real_T RateLimiter;                  /* '<S15>/Rate Limiter' */
  real_T RefSpdkmhr_n;                 /* '<S15>/Multiport Switch1' */
  real_T Vehicle_SpdDmd_mph;           /* '<S15>/Gain1' */
  uint32_T ReceivedBytes;              /* '<S55>/Data Inport S-Fcn' */
  uint32_T AvailableBytes;             /* '<S55>/Data Inport S-Fcn' */
  uint32_T DataTypeConversion4;        /* '<S53>/Data Type Conversion4' */
  uint32_T sendByteSize_RealSimPack;   /* '<S53>/switch for 0.1 send out' */
  uint32_T SentBytes;                  /* '<S58>/Data Inport S-Fcn' */
  int32_T ConnectionState_f;           /* '<S57>/Data Inport S-Fcn' */
  uint8_T DataVector[1024];            /* '<S55>/Data Inport S-Fcn' */
  uint8_T DataTypeConversion1[1024];   /* '<S52>/Data Type Conversion1' */
  uint8_T signalLightId[50];           /* '<S7>/Bus Selector2' */
  uint8_T ByteSend[200];               /* '<S7>/MATLAB System1' */
  boolean_T Status;                    /* '<S55>/Data Inport S-Fcn' */
  boolean_T Equal;                     /* '<S11>/Equal' */
  boolean_T BrakeSwitch;               /* '<S12>/Relational Operator' */
  boolean_T Compare;                   /* '<S17>/Compare' */
  boolean_T DataTypeConversion5;       /* '<S7>/Data Type Conversion5' */
  boolean_T Status_l;                  /* '<S58>/Data Inport S-Fcn' */
  boolean_T Compare_f;                 /* '<S63>/Compare' */
  boolean_T Compare_p;                 /* '<S64>/Compare' */
  boolean_T Compare_m;                 /* '<S65>/Compare' */
  boolean_T iTermReset;                /* '<S62>/Logical Operator' */
  boolean_T LogicalOperator1;          /* '<S62>/Logical Operator1' */
  boolean_T Compare_e;                 /* '<S44>/Compare' */
  boolean_T Compare_eb;                /* '<S45>/Compare' */
  boolean_T Uk1;                       /* '<S46>/Delay Input1' */
  boolean_T Compare_px;                /* '<S48>/Compare' */
  boolean_T FixPtRelationalOperator;   /* '<S46>/FixPt Relational Operator' */
  boolean_T Uk1_l;                     /* '<S47>/Delay Input1' */
  boolean_T Compare_c;                 /* '<S49>/Compare' */
  boolean_T FixPtRelationalOperator_a; /* '<S47>/FixPt Relational Operator' */
  boolean_T LogicalOperator;           /* '<S43>/Logical Operator' */
  boolean_T BrakeSwitch_m;             /* '<S43>/Relational Operator' */
} B_RS_DS_CM11_SimulinkRS_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  RealSimDepack_RS_DS_CM11_SimulinkRS_T obj;/* '<S7>/MATLAB System' */
  RealSimPack_RS_DS_CM11_SimulinkRS_T obj_l;/* '<S7>/MATLAB System1' */
  RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T obj_p;/* '<S7>/MATLAB System2' */
  real_T Memory_2_PreviousInput;       /* '<S11>/Memory' */
  real_T Memory_1_PreviousInput;       /* '<S11>/Memory' */
  real_T Memory_PreviousInput;         /* '<S7>/Memory' */
  real_T Memory_PreviousInput_d;       /* '<S62>/Memory' */
  real_T Memory1_PreviousInput;        /* '<S62>/Memory1' */
  real_T PrevY;                        /* '<S62>/DrvSpeedCmdRateLimit' */
  real_T LastMajorTime;                /* '<S62>/DrvSpeedCmdRateLimit' */
  real_T Memory2_PreviousInput;        /* '<S62>/Memory2' */
  real_T PrevY_m;                      /* '<S62>/followDistRateLimit' */
  real_T Memory1_PreviousInput_e;      /* '<S42>/Memory1' */
  real_T Memory2_PreviousInput_l;      /* '<S42>/Memory2' */
  real_T Memory1_PreviousInput_h;      /* '<S43>/Memory1' */
  real_T Memory_PreviousInput_o;       /* '<S43>/Memory' */
  real_T PrevY_a;                      /* '<S15>/Rate Limiter' */
  struct {
    void *Entry;
  } ReadCMDict3_PWORK;                 /* '<S11>/Read CM Dict3' */

  struct {
    void *Entry;
  } ReadCMDict3_PWORK_k;               /* '<S1>/Read CM Dict3' */

  struct {
    void *Entry;
  } ReadCMDict7_PWORK;                 /* '<S1>/Read CM Dict7' */

  void *DataInportSFcn_PWORK[4];       /* '<S55>/Data Inport S-Fcn' */
  void *DataInportSFcn_PWORK_e;        /* '<S57>/Data Inport S-Fcn' */
  struct {
    void *Entry;
  } ReadCMDict2_PWORK;                 /* '<S8>/Read CM Dict2' */

  struct {
    void *Entry;
  } ReadCMDict8_PWORK;                 /* '<S1>/Read CM Dict8' */

  struct {
    void *Entry;
  } ReadCMDict_PWORK;                  /* '<S6>/Read CM Dict' */

  struct {
    void *Entry;
  } ReadCMDict7_PWORK_m;               /* '<S7>/Read CM Dict7' */

  struct {
    void *Entry;
  } ReadCMDict1_PWORK;                 /* '<S8>/Read CM Dict1' */

  struct {
    void *Entry;
  } ReadCMDict6_PWORK;                 /* '<S8>/Read CM Dict6' */

  struct {
    void *Entry;
  } ReadCMDict7_PWORK_f;               /* '<S8>/Read CM Dict7' */

  struct {
    void *Entry;
  } ReadCMDict_PWORK_a;                /* '<S8>/Read CM Dict' */

  struct {
    void *Entry;
  } ReadCMDict4_PWORK;                 /* '<S8>/Read CM Dict4' */

  struct {
    void *Entry;
  } ReadCMDict5_PWORK;                 /* '<S8>/Read CM Dict5' */

  struct {
    void *Entry;
  } ReadCMDict3_PWORK_i;               /* '<S8>/Read CM Dict3' */

  void *DataInportSFcn_PWORK_p;        /* '<S54>/Data Inport S-Fcn' */
  void *DataInportSFcn_PWORK_pi[2];    /* '<S58>/Data Inport S-Fcn' */
  struct {
    void *Entry;
  } ReadCMDict11_PWORK;                /* '<S10>/Read CM Dict11' */

  struct {
    void *Entry;
  } ReadCMDict9_PWORK;                 /* '<S10>/Read CM Dict9' */

  struct {
    void *Entry;
  } ReadCMDict6_PWORK_i;               /* '<S62>/Read CM Dict6' */

  struct {
    void *Entry;
  } ReadCMDict10_PWORK;                /* '<S10>/Read CM Dict10' */

  struct {
    void *Entry;
  } ReadCMDict12_PWORK;                /* '<S10>/Read CM Dict12' */

  struct {
    void *Entry;
  } ReadCMDict_PWORK_c;                /* '<S1>/Read CM Dict' */

  struct {
    void *Entry;
  } ReadCMDict1_PWORK_j;               /* '<S1>/Read CM Dict1' */

  struct {
    void *Entry;
  } ReadCMDict2_PWORK_d;               /* '<S1>/Read CM Dict2' */

  struct {
    void *Entry;
  } ReadCMDict4_PWORK_b;               /* '<S1>/Read CM Dict4' */

  struct {
    void *Entry;
  } ReadCMDict5_PWORK_k;               /* '<S1>/Read CM Dict5' */

  struct {
    void *Entry;
  } ReadCMDict6_PWORK_j;               /* '<S1>/Read CM Dict6' */

  uint32_T temporalCounter_i1;         /* '<S1>/RealSimHILCycle' */
  uint32_T m_bpIndex;                 /* '<S43>/Proportional Gain Scheduling' */
  boolean_T DelayInput1_DSTATE;        /* '<S46>/Delay Input1' */
  boolean_T DelayInput1_DSTATE_l;      /* '<S47>/Delay Input1' */
  uint8_T is_active_c2_RS_DS_CM11_SimulinkRS;/* '<S1>/RealSimHILCycle' */
  uint8_T is_c2_RS_DS_CM11_SimulinkRS; /* '<S1>/RealSimHILCycle' */
  uint8_T is_CycleOn;                  /* '<S1>/RealSimHILCycle' */
  boolean_T objisempty;                /* '<S7>/MATLAB System2' */
  boolean_T objisempty_h;              /* '<S7>/MATLAB System1' */
  boolean_T objisempty_n;              /* '<S7>/MATLAB System' */
} DW_RS_DS_CM11_SimulinkRS_T;

/* Continuous states (default storage) */
typedef struct {
  real_T Limits5050_CSTATE;            /* '<S62>/Limits [-50,50]' */
  real_T Limits50501_CSTATE;           /* '<S62>/Limits [-50,50]1' */
  real_T Limits5050_CSTATE_l;          /* '<S43>/Limits [-50,50]' */
  real_T LowpassFilter_CSTATE;         /* '<S43>/Lowpass Filter' */
} X_RS_DS_CM11_SimulinkRS_T;

/* State derivatives (default storage) */
typedef struct {
  real_T Limits5050_CSTATE;            /* '<S62>/Limits [-50,50]' */
  real_T Limits50501_CSTATE;           /* '<S62>/Limits [-50,50]1' */
  real_T Limits5050_CSTATE_l;          /* '<S43>/Limits [-50,50]' */
  real_T LowpassFilter_CSTATE;         /* '<S43>/Lowpass Filter' */
} XDot_RS_DS_CM11_SimulinkRS_T;

/* State disabled  */
typedef struct {
  boolean_T Limits5050_CSTATE;         /* '<S62>/Limits [-50,50]' */
  boolean_T Limits50501_CSTATE;        /* '<S62>/Limits [-50,50]1' */
  boolean_T Limits5050_CSTATE_l;       /* '<S43>/Limits [-50,50]' */
  boolean_T LowpassFilter_CSTATE;      /* '<S43>/Lowpass Filter' */
} XDis_RS_DS_CM11_SimulinkRS_T;

/* Zero-crossing (trigger) state */
typedef struct {
  ZCSigState Limits5050_Reset_ZCE;     /* '<S62>/Limits [-50,50]' */
  ZCSigState Limits50501_Reset_ZCE;    /* '<S62>/Limits [-50,50]1' */
  ZCSigState Limits5050_Reset_ZCE_b;   /* '<S43>/Limits [-50,50]' */
} PrevZCX_RS_DS_CM11_SimulinkRS_T;

#ifndef ODE1_INTG
#define ODE1_INTG

/* ODE1 Integration Data */
typedef struct {
  real_T *f[1];                        /* derivatives */
} ODE1_IntgData;

#endif

/* Parameters (default storage) */
struct P_RS_DS_CM11_SimulinkRS_T_ {
  struct_GphLW54WCcJjc0lEaqz83B RealSimPara;/* Variable: RealSimPara
                                             * Referenced by:
                                             *   '<S1>/Constant'
                                             *   '<S7>/MATLAB System2'
                                             *   '<S10>/free speed'
                                             */
  real_T VehicleMessageFieldDefInputVec[29];
                                     /* Variable: VehicleMessageFieldDefInputVec
                                      * Referenced by:
                                      *   '<S7>/MATLAB System'
                                      *   '<S7>/MATLAB System1'
                                      */
  real_T CompareToConstant1_const;   /* Mask Parameter: CompareToConstant1_const
                                      * Referenced by: '<S44>/Constant'
                                      */
  real_T CompareToConstant3_const;   /* Mask Parameter: CompareToConstant3_const
                                      * Referenced by: '<S45>/Constant'
                                      */
  real_T VehicleSource_BuiltIn_const;
                                  /* Mask Parameter: VehicleSource_BuiltIn_const
                                   * Referenced by: '<S17>/Constant'
                                   */
  real_T CompareToConstant_const;     /* Mask Parameter: CompareToConstant_const
                                       * Referenced by: '<S63>/Constant'
                                       */
  real_T CompareToConstant1_const_f;
                                   /* Mask Parameter: CompareToConstant1_const_f
                                    * Referenced by: '<S64>/Constant'
                                    */
  real_T CompareToConstant3_const_g;
                                   /* Mask Parameter: CompareToConstant3_const_g
                                    * Referenced by: '<S65>/Constant'
                                    */
  boolean_T DetectFallNegative_vinit;/* Mask Parameter: DetectFallNegative_vinit
                                      * Referenced by: '<S46>/Delay Input1'
                                      */
  boolean_T DetectRisePositive_vinit;/* Mask Parameter: DetectRisePositive_vinit
                                      * Referenced by: '<S47>/Delay Input1'
                                      */
  real_T Constant_Value;               /* Expression: 0
                                        * Referenced by: '<S48>/Constant'
                                        */
  real_T Constant_Value_c;             /* Expression: 0
                                        * Referenced by: '<S49>/Constant'
                                        */
  real_T Constant_Value_p;             /* Expression: 1
                                        * Referenced by: '<S42>/Constant'
                                        */
  real_T Memory1_InitialCondition;     /* Expression: 0
                                        * Referenced by: '<S42>/Memory1'
                                        */
  real_T Memory2_InitialCondition;     /* Expression: 0
                                        * Referenced by: '<S42>/Memory2'
                                        */
  real_T SpdSat_UpperSat;              /* Expression: 190
                                        * Referenced by: '<S42>/SpdSat'
                                        */
  real_T SpdSat_LowerSat;              /* Expression: -5
                                        * Referenced by: '<S42>/SpdSat'
                                        */
  real_T Memory1_InitialCondition_h;   /* Expression: 0
                                        * Referenced by: '<S43>/Memory1'
                                        */
  real_T Memory_InitialCondition;      /* Expression: 0
                                        * Referenced by: '<S43>/Memory'
                                        */
  real_T Constant_Value_b;             /* Expression: 0
                                        * Referenced by: '<S43>/Constant'
                                        */
  real_T Feedforward_tableData[17];
       /* Expression: [0	3.5	12.5	16	20	21	22	24.5	26	29.5	32	33	34	36	37	38	39]
        * Referenced by: '<S43>/Feedforward'
        */
  real_T Feedforward_bp01Data[17];
  /* Expression: [0 10    20    30    40    50    60    70    80    90   100   110   120   130   140   150   160]
   * Referenced by: '<S43>/Feedforward'
   */
  real_T FeedforwardGain_Gain;         /* Expression: 1
                                        * Referenced by: '<S43>/FeedforwardGain'
                                        */
  real_T Limits5050_IC;                /* Expression: 0
                                        * Referenced by: '<S43>/Limits [-50,50]'
                                        */
  real_T Limits5050_UpperSat;          /* Expression: 50
                                        * Referenced by: '<S43>/Limits [-50,50]'
                                        */
  real_T Limits5050_LowerSat;          /* Expression: -50
                                        * Referenced by: '<S43>/Limits [-50,50]'
                                        */
  real_T LowpassFilter_A;              /* Computed Parameter: LowpassFilter_A
                                        * Referenced by: '<S43>/Lowpass Filter'
                                        */
  real_T LowpassFilter_C;              /* Computed Parameter: LowpassFilter_C
                                        * Referenced by: '<S43>/Lowpass Filter'
                                        */
  real_T Kp_Gain;                      /* Expression: 7
                                        * Referenced by: '<S43>/Kp'
                                        */
  real_T ProportionalGainScheduling_tableData[3];/* Expression: [1 1 1]
                                                  * Referenced by: '<S43>/Proportional Gain Scheduling'
                                                  */
  real_T ProportionalGainScheduling_bp01Data[3];/* Expression: [ 0 30 70]
                                                 * Referenced by: '<S43>/Proportional Gain Scheduling'
                                                 */
  real_T Gain_Gain;                    /* Expression: -1
                                        * Referenced by: '<S43>/Gain'
                                        */
  real_T Ki_Gain;                      /* Expression: 1
                                        * Referenced by: '<S43>/Ki'
                                        */
  real_T MinthresholdforBrakeswitch_Value;/* Expression: 0
                                           * Referenced by: '<S43>/Min threshold for Brake switch'
                                           */
  real_T Switch2_Threshold;            /* Expression: 0
                                        * Referenced by: '<S43>/Switch2'
                                        */
  real_T Switch1_Threshold;            /* Expression: 0
                                        * Referenced by: '<S43>/Switch1'
                                        */
  real_T standstillbrake0100_Value;    /* Expression: 50
                                        * Referenced by: '<S43>/standstill brake [0,100]'
                                        */
  real_T kmtomiles_Gain;               /* Expression: 1/1.609344
                                        * Referenced by: '<S42>/km to miles'
                                        */
  real_T Gain_Gain_h;                  /* Expression: 1/1.609
                                        * Referenced by: '<S15>/Gain'
                                        */
  real_T ManSpdSel_Value;              /* Expression: 3
                                        * Referenced by: '<S15>/ManSpdSel'
                                        */
  real_T ManSpd_Value;                 /* Expression: 0
                                        * Referenced by: '<S15>/ManSpd'
                                        */
  real_T RateLimiter_RisingLim;        /* Expression: 5
                                        * Referenced by: '<S15>/Rate Limiter'
                                        */
  real_T RateLimiter_FallingLim;       /* Expression: -5
                                        * Referenced by: '<S15>/Rate Limiter'
                                        */
  real_T RateLimiter_IC;               /* Expression: 0
                                        * Referenced by: '<S15>/Rate Limiter'
                                        */
  real_T Gain1_Gain;                   /* Expression: 1/1.609
                                        * Referenced by: '<S15>/Gain1'
                                        */
  real_T MATLABSystem2_speedUpperBound;/* Expression: 30
                                        * Referenced by: '<S7>/MATLAB System2'
                                        */
  real_T Memory_2_InitialCondition;    /* Expression: 0
                                        * Referenced by: '<S11>/Memory'
                                        */
  real_T Gain1_Gain_p;                 /* Expression: 0.01
                                        * Referenced by: '<S11>/Gain1'
                                        */
  real_T Memory_1_InitialCondition;    /* Expression: 0
                                        * Referenced by: '<S11>/Memory'
                                        */
  real_T Gain_Gain_p;                  /* Expression: 0.01
                                        * Referenced by: '<S11>/Gain'
                                        */
  real_T DriverSource_Value;           /* Expression: 0
                                        * Referenced by: '<S11>/Driver Source'
                                        */
  real_T Constant_Value_pu;            /* Expression: 0
                                        * Referenced by: '<S11>/Constant'
                                        */
  real_T Switch1_Threshold_i;          /* Expression: 0.5
                                        * Referenced by: '<S11>/Switch1'
                                        */
  real_T RS_actualSpeedSourceSelector_Value;/* Expression: 3
                                             * Referenced by: '<S7>/RS_actualSpeedSourceSelector'
                                             */
  real_T Memory_InitialCondition_d;    /* Expression: 0
                                        * Referenced by: '<S7>/Memory'
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
                                        * Referenced by: '<S11>/Constant1'
                                        */
  real_T Constant2_Value;              /* Expression: 1
                                        * Referenced by: '<S11>/Constant2'
                                        */
  real_T Switch_Threshold;             /* Expression: 0
                                        * Referenced by: '<S11>/Switch'
                                        */
  real_T Gain3_Gain;                   /* Expression: 3.6
                                        * Referenced by: '<S1>/Gain3'
                                        */
  real_T APP_Value;                    /* Expression: 0
                                        * Referenced by: '<S12>/APP'
                                        */
  real_T DriverControlSelection_Value; /* Expression: 2
                                        * Referenced by: '<S12>/Driver Control Selection'
                                        */
  real_T Gain1_Gain_c;                 /* Expression: 3.6
                                        * Referenced by: '<S6>/Gain1'
                                        */
  real_T Gain3_Gain_m;                 /* Expression: 3.6
                                        * Referenced by: '<S6>/Gain3'
                                        */
  real_T Gain2_Gain;                   /* Expression: 100
                                        * Referenced by: '<S12>/Gain2'
                                        */
  real_T Constant_Value_k;             /* Expression: 0
                                        * Referenced by: '<S12>/Constant'
                                        */
  real_T AR_limit_UpperSat;            /* Expression: 100
                                        * Referenced by: '<S12>/AR_limit'
                                        */
  real_T AR_limit_LowerSat;            /* Expression: 0
                                        * Referenced by: '<S12>/AR_limit'
                                        */
  real_T BR_command_0to100_Value;      /* Expression: 0
                                        * Referenced by: '<S12>/BR_command_0to100'
                                        */
  real_T Gain1_Gain_h;                 /* Expression: 100
                                        * Referenced by: '<S12>/Gain1'
                                        */
  real_T Constant1_Value_o;            /* Expression: 0
                                        * Referenced by: '<S12>/Constant1'
                                        */
  real_T BR_limit_UpperSat;            /* Expression: 100
                                        * Referenced by: '<S12>/BR_limit'
                                        */
  real_T BR_limit_LowerSat;            /* Expression: 0
                                        * Referenced by: '<S12>/BR_limit'
                                        */
  real_T MinthresholdforBrakeswitch_Value_c;/* Expression: 0
                                             * Referenced by: '<S12>/Min threshold for Brake switch'
                                             */
  real_T Gain2_Gain_b;                 /* Expression: 2.23694
                                        * Referenced by: '<S6>/Gain2'
                                        */
  real_T Constant_Value_a;             /* Expression: 1
                                        * Referenced by: '<S7>/Constant'
                                        */
  real_T RS_connectRequest_Value;      /* Expression: 0
                                        * Referenced by: '<S7>/RS_connectRequest'
                                        */
  real_T Switch_Threshold_i;           /* Expression: 6
                                        * Referenced by: '<S7>/Switch'
                                        */
  real_T simState_Value;               /* Expression: 1
                                        * Referenced by: '<S7>/simState'
                                        */
  real_T Constant_Value_c0[4];         /* Expression: [255,0,0,255]
                                        * Referenced by: '<S8>/Constant'
                                        */
  real_T activeLaneChange_Value;       /* Expression: 0
                                        * Referenced by: '<S61>/activeLaneChange'
                                        */
  real_T Constant1_Value_h;            /* Expression: 0
                                        * Referenced by: '<S53>/Constant1'
                                        */
  real_T switchfor01sendout_Threshold; /* Expression: 0
                                        * Referenced by: '<S53>/switch for 0.1 send out'
                                        */
  real_T Constant3_Value;              /* Expression: 0
                                        * Referenced by: '<S8>/Constant3'
                                        */
  real_T Constant1_Value_e;            /* Expression: 10
                                        * Referenced by: '<S10>/Constant1'
                                        */
  real_T headways_Value;               /* Expression: 4
                                        * Referenced by: '<S10>/headway (s)'
                                        */
  real_T Memory_InitialCondition_f;    /* Expression: 0
                                        * Referenced by: '<S62>/Memory'
                                        */
  real_T Memory1_InitialCondition_n;   /* Expression: 0
                                        * Referenced by: '<S62>/Memory1'
                                        */
  real_T Switch_Threshold_d;           /* Expression: 0
                                        * Referenced by: '<S10>/Switch'
                                        */
  real_T Constant_Value_kr;            /* Expression: 0
                                        * Referenced by: '<S62>/Constant'
                                        */
  real_T Limits5050_IC_a;              /* Expression: 0
                                        * Referenced by: '<S62>/Limits [-50,50]'
                                        */
  real_T Limits5050_UpperSat_n;        /* Expression: 50
                                        * Referenced by: '<S62>/Limits [-50,50]'
                                        */
  real_T Limits5050_LowerSat_i;        /* Expression: -50
                                        * Referenced by: '<S62>/Limits [-50,50]'
                                        */
  real_T Kp_Gain_o;                    /* Expression: 3
                                        * Referenced by: '<S62>/Kp'
                                        */
  real_T Limits50501_IC;               /* Expression: 0
                                        * Referenced by: '<S62>/Limits [-50,50]1'
                                        */
  real_T Limits50501_UpperSat;         /* Expression: 50
                                        * Referenced by: '<S62>/Limits [-50,50]1'
                                        */
  real_T Limits50501_LowerSat;         /* Expression: -50
                                        * Referenced by: '<S62>/Limits [-50,50]1'
                                        */
  real_T Kp_d_Gain;                    /* Expression: 1
                                        * Referenced by: '<S62>/Kp_d'
                                        */
  real_T Saturation_UpperSat;      /* Expression: max(10, RealSimPara.speedInit)
                                    * Referenced by: '<S62>/Saturation'
                                    */
  real_T Saturation_LowerSat;          /* Expression: 0
                                        * Referenced by: '<S62>/Saturation'
                                        */
  real_T DrvSpeedCmdRateLimit_RisingLim;/* Expression: 3
                                         * Referenced by: '<S62>/DrvSpeedCmdRateLimit'
                                         */
  real_T DrvSpeedCmdRateLimit_FallingLim;/* Expression: -3
                                          * Referenced by: '<S62>/DrvSpeedCmdRateLimit'
                                          */
  real_T Ki_Gain_o;                    /* Expression: 0.3
                                        * Referenced by: '<S62>/Ki'
                                        */
  real_T Ki_d_Gain;                    /* Expression: 0.1
                                        * Referenced by: '<S62>/Ki_d'
                                        */
  real_T Memory2_InitialCondition_j;   /* Expression: 0
                                        * Referenced by: '<S62>/Memory2'
                                        */
  real_T followDistRateLimit_RisingLim;/* Expression: 1
                                        * Referenced by: '<S62>/followDistRateLimit'
                                        */
  real_T followDistRateLimit_FallingLim;/* Expression: -1
                                         * Referenced by: '<S62>/followDistRateLimit'
                                         */
  real_T followDistRateLimit_IC;       /* Expression: 0
                                        * Referenced by: '<S62>/followDistRateLimit'
                                        */
  real_T Gain1_Gain_d;                 /* Expression: 3.6
                                        * Referenced by: '<S1>/Gain1'
                                        */
  real_T Gain2_Gain_e;                 /* Expression: 2.23694
                                        * Referenced by: '<S1>/Gain2'
                                        */
  uint8_T ManualSwitch_CurrentSetting;
                              /* Computed Parameter: ManualSwitch_CurrentSetting
                               * Referenced by: '<S1>/Manual Switch'
                               */
};

/* Real-time Model Data Structure */
struct tag_RTM_RS_DS_CM11_SimulinkRS_T {
  const char_T *errorStatus;
  RTWSolverInfo solverInfo;
  X_RS_DS_CM11_SimulinkRS_T *contStates;
  int_T *periodicContStateIndices;
  real_T *periodicContStateRanges;
  real_T *derivs;
  boolean_T *contStateDisabled;
  boolean_T zCCacheNeedsReset;
  boolean_T derivCacheNeedsReset;
  boolean_T CTOutputIncnstWithState;
  real_T odeF[1][4];
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
extern P_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_P;

/* Block signals (default storage) */
extern B_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_B;

/* Continuous states (default storage) */
extern X_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_X;

/* Block states (default storage) */
extern DW_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_DW;

/* Zero-crossing (trigger) state */
extern PrevZCX_RS_DS_CM11_SimulinkRS_T RS_DS_CM11_SimulinkRS_PrevZCX;

/* External data declarations for dependent source files */
extern const VehDataBus RS_DS_CM11_SimulinkRS_rtZVehDataBus;/* VehDataBus ground */

/* Model entry point functions */
extern void RS_DS_CM11_SimulinkRS_initialize(void);
extern void RS_DS_CM11_SimulinkRS_output(void);
extern void RS_DS_CM11_SimulinkRS_update(void);
extern void RS_DS_CM11_SimulinkRS_terminate(void);

/* Real-time Model object */
extern RT_MODEL_RS_DS_CM11_SimulinkRS_T *const RS_DS_CM11_SimulinkRS_M;

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
 * '<Root>' : 'RS_DS_CM11_SimulinkRS'
 * '<S1>'   : 'RS_DS_CM11_SimulinkRS/CarMaker'
 * '<S2>'   : 'RS_DS_CM11_SimulinkRS/Open CarMaker GUI'
 * '<S3>'   : 'RS_DS_CM11_SimulinkRS/CarMaker/CM_FIRST'
 * '<S4>'   : 'RS_DS_CM11_SimulinkRS/CarMaker/CM_LAST'
 * '<S5>'   : 'RS_DS_CM11_SimulinkRS/CarMaker/DrivMan'
 * '<S6>'   : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle'
 * '<S7>'   : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core'
 * '<S8>'   : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Translate CM Signal to Bus as Feedback to SUMO//VISSIM'
 * '<S9>'   : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSimHILCycle'
 * '<S10>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/Subsystem'
 * '<S11>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/VehicleControl'
 * '<S12>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Actuation and Outputs'
 * '<S13>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Brake'
 * '<S14>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer'
 * '<S15>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle'
 * '<S16>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/PowerTrain'
 * '<S17>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/VehicleSource_BuiltIn'
 * '<S18>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Brake/CreateBus Vhcl.Wheel.PT'
 * '<S19>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/CarAndTrailerUpd'
 * '<S20>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces'
 * '<S21>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Kinematics'
 * '<S22>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Kinetics'
 * '<S23>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Steering'
 * '<S24>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer'
 * '<S25>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/CarAndTrailerUpd/CreateBus Brake.IF.In'
 * '<S26>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/External Suspension Forces'
 * '<S27>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Suspension Forces'
 * '<S28>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires'
 * '<S29>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Vehicle Forces'
 * '<S30>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireFL'
 * '<S31>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireFR'
 * '<S32>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireRL'
 * '<S33>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireRR'
 * '<S34>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Car.Hitch.FrcTrq'
 * '<S35>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Motion'
 * '<S36>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.PoI'
 * '<S37>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Steering'
 * '<S38>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.FL'
 * '<S39>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.FR'
 * '<S40>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.RL'
 * '<S41>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.RR'
 * '<S42>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control'
 * '<S43>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller'
 * '<S44>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Compare To Constant1'
 * '<S45>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Compare To Constant3'
 * '<S46>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Fall Negative'
 * '<S47>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Rise Positive'
 * '<S48>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Fall Negative/Negative'
 * '<S49>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/IPG Vehicle/Driver and Drive Cycle/Driver and PI with FF Control/Vehicle Speed Controller/Detect Rise Positive/Positive'
 * '<S50>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/MATLAB Function'
 * '<S51>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/Status//Control_Out [TCP (1)]'
 * '<S52>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In'
 * '<S53>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP Out'
 * '<S54>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Ethernet Setup (1)'
 * '<S55>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_In [TCP (1)]'
 * '<S56>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Receive_Out [TCP (1)]'
 * '<S57>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Status//Control_In [TCP (1)]'
 * '<S58>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP In/Transmit_In [TCP (1)]'
 * '<S59>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Core/dSPACE TCP Out/Transmit_Out [TCP (1)]'
 * '<S60>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Translate CM Signal to Bus as Feedback to SUMO//VISSIM/MATLAB Function'
 * '<S61>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/RealSim Translate CM Signal to Bus as Feedback to SUMO//VISSIM/RealSim CreatBus'
 * '<S62>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/Subsystem/PID Speed Driver'
 * '<S63>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/Subsystem/PID Speed Driver/Compare To Constant'
 * '<S64>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/Subsystem/PID Speed Driver/Compare To Constant1'
 * '<S65>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/Subsystem/PID Speed Driver/Compare To Constant3'
 * '<S66>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/VehicleControl/CreateBus Vhcl.Misc'
 * '<S67>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/VehicleControl/CreateBus Vhcl.Wheel.In'
 * '<S68>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/VehicleControl/CreateBus VhclCtrl'
 * '<S69>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/VehicleControl/CreateBus VhclCtrl.PT'
 * '<S70>'  : 'RS_DS_CM11_SimulinkRS/CarMaker/VehicleControl/CreateBus VhclCtrl.Steering'
 */
#endif                                 /* RTW_HEADER_RS_DS_CM11_SimulinkRS_h_ */
