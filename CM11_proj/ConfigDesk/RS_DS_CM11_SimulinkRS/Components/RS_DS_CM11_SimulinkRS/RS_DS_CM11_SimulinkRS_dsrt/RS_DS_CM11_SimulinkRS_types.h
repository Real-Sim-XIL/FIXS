/*
 * RS_DS_CM11_SimulinkRS_types.h
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

#ifndef RTW_HEADER_RS_DS_CM11_SimulinkRS_types_h_
#define RTW_HEADER_RS_DS_CM11_SimulinkRS_types_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#include "zero_crossing_types.h"

/* Model Code Variants */
#ifndef DEFINED_TYPEDEF_FOR_VehDataBus_
#define DEFINED_TYPEDEF_FOR_VehDataBus_

typedef struct {
  uint8_T id[50];
  real_T idLength;
  uint8_T type[50];
  real_T typeLength;
  uint8_T vehicleClass[50];
  real_T vehicleClassLength;
  real_T speed;
  real_T acceleration;
  real_T positionX;
  real_T positionY;
  real_T positionZ;
  real_T heading;
  real_T color;
  uint8_T linkId[50];
  real_T linkIdLength;
  real_T laneId;
  real_T distanceTravel;
  real_T speedDesired;
  real_T accelerationDesired;
  real_T hasPrecedingVehicle;
  uint8_T precedingVehicleId[50];
  real_T precedingVehicleIdLength;
  real_T precedingVehicleDistance;
  real_T precedingVehicleSpeed;
  uint8_T signalLightId[50];
  real_T signalLightIdLength;
  real_T signalLightHeadId;
  real_T signalLightDistance;
  real_T signalLightColor;
  real_T speedLimit;
  real_T speedLimitNext;
  real_T speedLimitChangeDistance;
  uint8_T linkIdNext[50];
  real_T linkIdNextLength;
  real_T grade;
  real_T activeLaneChange;
} VehDataBus;

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_GphLW54WCcJjc0lEaqz83B_
#define DEFINED_TYPEDEF_FOR_struct_GphLW54WCcJjc0lEaqz83B_

typedef struct {
  real_T speedInit;
  real_T tLookahead;
  real_T smoothWindow;
} struct_GphLW54WCcJjc0lEaqz83B;

#endif

#ifndef struct_tag_ss9j38eBfDiWAKQLv2egin
#define struct_tag_ss9j38eBfDiWAKQLv2egin

struct tag_ss9j38eBfDiWAKQLv2egin
{
  real_T id;
  real_T type;
  real_T vehicleClass;
  real_T speed;
  real_T acceleration;
  real_T positionX;
  real_T positionY;
  real_T positionZ;
  real_T heading;
  real_T color;
  real_T linkId;
  real_T laneId;
  real_T distanceTravel;
  real_T speedDesired;
  real_T accelerationDesired;
  real_T hasPrecedingVehicle;
  real_T precedingVehicleId;
  real_T precedingVehicleDistance;
  real_T precedingVehicleSpeed;
  real_T signalLightId;
  real_T signalLightHeadId;
  real_T signalLightDistance;
  real_T signalLightColor;
  real_T speedLimit;
  real_T speedLimitNext;
  real_T speedLimitChangeDistance;
  real_T linkIdNext;
  real_T grade;
  real_T activeLaneChange;
};

#endif                                 /* struct_tag_ss9j38eBfDiWAKQLv2egin */

#ifndef typedef_ss9j38eBfDiWAKQLv2egin_RS_DS_CM11_SimulinkRS_T
#define typedef_ss9j38eBfDiWAKQLv2egin_RS_DS_CM11_SimulinkRS_T

typedef struct tag_ss9j38eBfDiWAKQLv2egin
  ss9j38eBfDiWAKQLv2egin_RS_DS_CM11_SimulinkRS_T;

#endif              /* typedef_ss9j38eBfDiWAKQLv2egin_RS_DS_CM11_SimulinkRS_T */

#ifndef struct_tag_BlgwLpgj2bjudmbmVKWwDE
#define struct_tag_BlgwLpgj2bjudmbmVKWwDE

struct tag_BlgwLpgj2bjudmbmVKWwDE
{
  uint32_T f1[8];
};

#endif                                 /* struct_tag_BlgwLpgj2bjudmbmVKWwDE */

#ifndef typedef_cell_wrap_RS_DS_CM11_SimulinkRS_T
#define typedef_cell_wrap_RS_DS_CM11_SimulinkRS_T

typedef struct tag_BlgwLpgj2bjudmbmVKWwDE cell_wrap_RS_DS_CM11_SimulinkRS_T;

#endif                           /* typedef_cell_wrap_RS_DS_CM11_SimulinkRS_T */

#ifndef struct_tag_6iFn6TkvYyvkmIVgwoJKOG
#define struct_tag_6iFn6TkvYyvkmIVgwoJKOG

struct tag_6iFn6TkvYyvkmIVgwoJKOG
{
  int32_T isInitialized;
  cell_wrap_RS_DS_CM11_SimulinkRS_T inputVarSize[3];
  real_T VehicleMessageFieldDefInputVec[29];
  ss9j38eBfDiWAKQLv2egin_RS_DS_CM11_SimulinkRS_T VehicleMessageFieldDef;
  real_T msgHeaderSize;
  real_T msgEachHeaderSize;
  real_T vehMsgIdentifer;
  real_T enableDebug;
  VehDataBus VehicleDataEmpty;
  real_T simStatePrevious;
  real_T tPrevious;
  real_T isVehicleInNetworkPrevious;
  VehDataBus VehicleDataPrevious;
};

#endif                                 /* struct_tag_6iFn6TkvYyvkmIVgwoJKOG */

#ifndef typedef_RealSimDepack_RS_DS_CM11_SimulinkRS_T
#define typedef_RealSimDepack_RS_DS_CM11_SimulinkRS_T

typedef struct tag_6iFn6TkvYyvkmIVgwoJKOG RealSimDepack_RS_DS_CM11_SimulinkRS_T;

#endif                       /* typedef_RealSimDepack_RS_DS_CM11_SimulinkRS_T */

#ifndef struct_tag_TcBKy0MY0UYgBVMhSYbj4C
#define struct_tag_TcBKy0MY0UYgBVMhSYbj4C

struct tag_TcBKy0MY0UYgBVMhSYbj4C
{
  int32_T isInitialized;
  cell_wrap_RS_DS_CM11_SimulinkRS_T inputVarSize[3];
  real_T VehicleMessageFieldDefInputVec[29];
  ss9j38eBfDiWAKQLv2egin_RS_DS_CM11_SimulinkRS_T VehicleMessageFieldDef;
  real_T msgHeaderSize;
  real_T msgEachHeaderSize;
};

#endif                                 /* struct_tag_TcBKy0MY0UYgBVMhSYbj4C */

#ifndef typedef_RealSimPack_RS_DS_CM11_SimulinkRS_T
#define typedef_RealSimPack_RS_DS_CM11_SimulinkRS_T

typedef struct tag_TcBKy0MY0UYgBVMhSYbj4C RealSimPack_RS_DS_CM11_SimulinkRS_T;

#endif                         /* typedef_RealSimPack_RS_DS_CM11_SimulinkRS_T */

#ifndef struct_tag_RMjsg0DbGVtaj1UZjgt5lB
#define struct_tag_RMjsg0DbGVtaj1UZjgt5lB

struct tag_RMjsg0DbGVtaj1UZjgt5lB
{
  int32_T isInitialized;
  cell_wrap_RS_DS_CM11_SimulinkRS_T inputVarSize[7];
  real_T initialSpeed;
  real_T speedLookAheadHorizon;
  real_T speedUpperBound;
  real_T timePrevious;
  real_T speedPrevious;
  real_T timeNext;
  real_T speedNext;
  real_T connectionState;
  real_T timeSimulator;
  real_T simulatorStartTime;
  real_T timeStepSimulator;
  real_T timeStepSimulatorPrevious;
  real_T timeReceivePrevious;
  real_T simState;
  real_T RealSimDelay;
  real_T speedLookAheadPrevious;
  real_T accelerationDesiredPrevious;
  real_T speedInterpolationMode;
};

#endif                                 /* struct_tag_RMjsg0DbGVtaj1UZjgt5lB */

#ifndef typedef_RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T
#define typedef_RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T

typedef struct tag_RMjsg0DbGVtaj1UZjgt5lB
  RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T;

#endif                  /* typedef_RealSimInterpSpeed_RS_DS_CM11_SimulinkRS_T */

/* Parameters (default storage) */
typedef struct P_RS_DS_CM11_SimulinkRS_T_ P_RS_DS_CM11_SimulinkRS_T;

/* Forward declaration for rtModel */
typedef struct tag_RTM_RS_DS_CM11_SimulinkRS_T RT_MODEL_RS_DS_CM11_SimulinkRS_T;

#endif                           /* RTW_HEADER_RS_DS_CM11_SimulinkRS_types_h_ */
