/*
 * RS_DS_CM11_noSimulinkRS_useManeuver.h
 *
 * Code generation for model "RS_DS_CM11_noSimulinkRS_useManeuver".
 *
 * Model version              : 5.3
 * Simulink Coder version : 9.6 (R2021b) 14-May-2021
 * C source code generated on : Tue Sep 26 10:58:04 2023
 *
 * Target selection: dsrt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_RS_DS_CM11_noSimulinkRS_useManeuver_h_
#define RTW_HEADER_RS_DS_CM11_noSimulinkRS_useManeuver_h_
#include <string.h>
#ifndef RS_DS_CM11_noSimulinkRS_useManeuver_COMMON_INCLUDES_
#define RS_DS_CM11_noSimulinkRS_useManeuver_COMMON_INCLUDES_
#include <Global.h>
#include <CarMaker4SL.h>
#include <TextUtils.h>
#include <DataDict.h>
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                /* RS_DS_CM11_noSimulinkRS_useManeuver_COMMON_INCLUDES_ */

#include "RS_DS_CM11_noSimulinkRS_useManeuver_types.h"

/* Shared type includes */
#include "multiword_types.h"

/* Macros for accessing real-time model data structure */
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

/* Block signals (default storage) */
typedef struct {
  real_T IoIn;                         /* '<S3>/IoIn' */
  real_T Environment_o1;               /* '<S3>/Environment' */
  real_T Environment_o2[14];           /* '<S3>/Environment' */
  real_T DrivMan_o1;                   /* '<S5>/DrivMan' */
  real_T DrivMan_o2[17];               /* '<S5>/DrivMan' */
  real_T VehicleControl_o1;            /* '<S7>/VehicleControl' */
  real_T VehicleControl_o2[29];        /* '<S7>/VehicleControl' */
  real_T VehicleControlUpd_o1;         /* '<S7>/VehicleControlUpd' */
  real_T VehicleControlUpd_o2;         /* '<S7>/VehicleControlUpd' */
  real_T VehicleControlUpd_o3[5];      /* '<S7>/VehicleControlUpd' */
  real_T VehicleControlUpd_o4[12];     /* '<S7>/VehicleControlUpd' */
  real_T VehicleControlUpd_o5[11];     /* '<S7>/VehicleControlUpd' */
  real_T VehicleControlUpd_o6[2];      /* '<S7>/VehicleControlUpd' */
  real_T VehicleControlUpd_o7[12];     /* '<S7>/VehicleControlUpd' */
  real_T Steering_o1;                  /* '<S17>/Steering' */
  real_T Steering_o2[21];              /* '<S17>/Steering' */
  real_T Kinematics_o1;                /* '<S15>/Kinematics' */
  real_T Kinematics_o2[7];             /* '<S15>/Kinematics' */
  real_T Kinematics_o3[7];             /* '<S15>/Kinematics' */
  real_T Kinematics_o4[7];             /* '<S15>/Kinematics' */
  real_T Kinematics_o5[7];             /* '<S15>/Kinematics' */
  real_T Kinematics_o6[8];             /* '<S15>/Kinematics' */
  real_T Kinematics_o7[8];             /* '<S15>/Kinematics' */
  real_T Kinematics_o8[8];             /* '<S15>/Kinematics' */
  real_T Kinematics_o9[8];             /* '<S15>/Kinematics' */
  real_T Forces_o1;                    /* '<S23>/Forces' */
  real_T Forces_o2[12];                /* '<S23>/Forces' */
  real_T Forces_o3[13];                /* '<S23>/Forces' */
  real_T Forces_o4[16];                /* '<S23>/Forces' */
  real_T SuspensionControlUnit_o1;     /* '<S21>/Suspension Control Unit' */
  real_T SuspensionControlUnit_o2[16]; /* '<S21>/Suspension Control Unit' */
  real_T SuspensionForceElements_o1;   /* '<S21>/Suspension Force Elements' */
  real_T SuspensionForceElements_o2[24];/* '<S21>/Suspension Force Elements' */
  real_T SuspensionForceElementsUpdate;
                                  /* '<S21>/Suspension Force Elements Update' */
  real_T Kinetics_o1;                  /* '<S16>/Kinetics' */
  real_T Kinetics_o2[12];              /* '<S16>/Kinetics' */
  real_T Kinetics_o3[6];               /* '<S16>/Kinetics' */
  real_T Kinetics_o4[6];               /* '<S16>/Kinetics' */
  real_T Kinetics_o5[4];               /* '<S16>/Kinetics' */
  real_T Kinetics_o6[4];               /* '<S16>/Kinetics' */
  real_T Trailer_o1;                   /* '<S18>/Trailer' */
  real_T Trailer_o2[4];                /* '<S18>/Trailer' */
  real_T Trailer_o3[15];               /* '<S18>/Trailer' */
  real_T Trailer_o4[18];               /* '<S18>/Trailer' */
  real_T Trailer_o5[14];               /* '<S18>/Trailer' */
  real_T Trailer_o6[14];               /* '<S18>/Trailer' */
  real_T Trailer_o7[14];               /* '<S18>/Trailer' */
  real_T Trailer_o8[14];               /* '<S18>/Trailer' */
  real_T Trailer_o9[6];                /* '<S18>/Trailer' */
  real_T CarAndTrailerUpd_o1;          /* '<S13>/CarAndTrailerUpd' */
  real_T CarAndTrailerUpd_o2[9];       /* '<S13>/CarAndTrailerUpd' */
  real_T Sync;                         /* '<S8>/Brake' */
  real_T Brake_o2[16];                 /* '<S8>/Brake' */
  real_T BrakeUpd_o1;                  /* '<S8>/BrakeUpd' */
  real_T BrakeUpd_o2[20];              /* '<S8>/BrakeUpd' */
  real_T Sync_e;                       /* '<S10>/PowerTrain' */
  real_T PowerTrain_o2[42];            /* '<S10>/PowerTrain' */
  real_T PowerTrainUpd;                /* '<S10>/PowerTrainUpd' */
  real_T CM_User;                      /* '<S4>/CM_User' */
  real_T IoOut;                        /* '<S4>/IoOut' */
  real_T VehSpd;                       /* '<S1>/Read CM Dict' */
  real_T VehSpdKph;                    /* '<S1>/Gain1' */
  real_T VehSpdMph;                    /* '<S1>/Gain2' */
  real_T CM_Time;                      /* '<S1>/Read CM Dict3' */
  real_T SimCore_State;                /* '<S1>/Read CM Dict7' */
  real_T distanceTravel;               /* '<S1>/Read CM Dict8' */
  real_T Trf_FOL_Targ_Dtct;            /* '<S1>/Read CM Dict1' */
  real_T Trf_FOL_dDist;                /* '<S1>/Read CM Dict2' */
  real_T Trf_FOL_dSpeed;               /* '<S1>/Read CM Dict4' */
  real_T Trf_FOL_ObjId;                /* '<S1>/Read CM Dict5' */
  real_T Trf_FOL_State;                /* '<S1>/Read CM Dict6' */
  boolean_T Compare;                   /* '<S11>/Compare' */
} B_RS_DS_CM11_noSimulinkRS_useManeuver_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  struct {
    void *Entry;
  } ReadCMDict_PWORK;                  /* '<S1>/Read CM Dict' */

  struct {
    void *Entry;
  } ReadCMDict3_PWORK;                 /* '<S1>/Read CM Dict3' */

  struct {
    void *Entry;
  } ReadCMDict7_PWORK;                 /* '<S1>/Read CM Dict7' */

  struct {
    void *Entry;
  } ReadCMDict8_PWORK;                 /* '<S1>/Read CM Dict8' */

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
} DW_RS_DS_CM11_noSimulinkRS_useManeuver_T;

/* Parameters (default storage) */
struct P_RS_DS_CM11_noSimulinkRS_useManeuver_T_ {
  real_T VehicleSource_BuiltIn_const;
                                  /* Mask Parameter: VehicleSource_BuiltIn_const
                                   * Referenced by: '<S11>/Constant'
                                   */
  real_T Gain1_Gain;                   /* Expression: 3.6
                                        * Referenced by: '<S1>/Gain1'
                                        */
  real_T Gain2_Gain;                   /* Expression: 2.23694
                                        * Referenced by: '<S1>/Gain2'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_RS_DS_CM11_noSimulinkRS_useManeuver_T {
  const char_T *errorStatus;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    boolean_T stopRequestedFlag;
  } Timing;
};

/* Block parameters (default storage) */
extern P_RS_DS_CM11_noSimulinkRS_useManeuver_T
  RS_DS_CM11_noSimulinkRS_useManeuver_P;

/* Block signals (default storage) */
extern B_RS_DS_CM11_noSimulinkRS_useManeuver_T
  RS_DS_CM11_noSimulinkRS_useManeuver_B;

/* Block states (default storage) */
extern DW_RS_DS_CM11_noSimulinkRS_useManeuver_T
  RS_DS_CM11_noSimulinkRS_useManeuver_DW;

/* Model entry point functions */
extern void RS_DS_CM11_noSimulinkRS_useManeuver_initialize(void);
extern void RS_DS_CM11_noSimulinkRS_useManeuver_output(void);
extern void RS_DS_CM11_noSimulinkRS_useManeuver_update(void);
extern void RS_DS_CM11_noSimulinkRS_useManeuver_terminate(void);

/* Real-time Model object */
extern RT_MODEL_RS_DS_CM11_noSimulinkRS_useManeuver_T *const
  RS_DS_CM11_noSimulinkRS_useManeuver_M;

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
 * '<Root>' : 'RS_DS_CM11_noSimulinkRS_useManeuver'
 * '<S1>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker'
 * '<S2>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/Open CarMaker GUI'
 * '<S3>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/CM_FIRST'
 * '<S4>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/CM_LAST'
 * '<S5>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/DrivMan'
 * '<S6>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle'
 * '<S7>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/VehicleControl'
 * '<S8>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/Brake'
 * '<S9>'   : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer'
 * '<S10>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/PowerTrain'
 * '<S11>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/VehicleSource_BuiltIn'
 * '<S12>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/Brake/CreateBus Vhcl.Wheel.PT'
 * '<S13>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/CarAndTrailerUpd'
 * '<S14>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces'
 * '<S15>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Kinematics'
 * '<S16>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Kinetics'
 * '<S17>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Steering'
 * '<S18>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer'
 * '<S19>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/CarAndTrailerUpd/CreateBus Brake.IF.In'
 * '<S20>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/External Suspension Forces'
 * '<S21>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Suspension Forces'
 * '<S22>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires'
 * '<S23>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Vehicle Forces'
 * '<S24>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireFL'
 * '<S25>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireFR'
 * '<S26>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireRL'
 * '<S27>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Forces/Tires/TireRR'
 * '<S28>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Car.Hitch.FrcTrq'
 * '<S29>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Motion'
 * '<S30>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.PoI'
 * '<S31>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Steering'
 * '<S32>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.FL'
 * '<S33>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.FR'
 * '<S34>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.RL'
 * '<S35>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/IPG Vehicle/CarAndTrailer/Trailer/CreateBus Vhcl.Wheel.RR'
 * '<S36>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/VehicleControl/CreateBus Vhcl.Misc'
 * '<S37>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/VehicleControl/CreateBus Vhcl.Wheel.In'
 * '<S38>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/VehicleControl/CreateBus VhclCtrl'
 * '<S39>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/VehicleControl/CreateBus VhclCtrl.PT'
 * '<S40>'  : 'RS_DS_CM11_noSimulinkRS_useManeuver/CarMaker/VehicleControl/CreateBus VhclCtrl.Steering'
 */
#endif                   /* RTW_HEADER_RS_DS_CM11_noSimulinkRS_useManeuver_h_ */
