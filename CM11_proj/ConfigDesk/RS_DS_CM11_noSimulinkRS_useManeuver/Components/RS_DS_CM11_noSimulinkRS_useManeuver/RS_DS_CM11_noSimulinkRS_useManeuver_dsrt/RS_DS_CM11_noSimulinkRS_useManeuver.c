/*
 * RS_DS_CM11_noSimulinkRS_useManeuver.c
 *
 * Code generation for model "RS_DS_CM11_noSimulinkRS_useManeuver".
 *
 * Model version              : 5.2
 * Simulink Coder version : 9.6 (R2021b) 14-May-2021
 * C source code generated on : Fri May 19 16:10:59 2023
 *
 * Target selection: dsrt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver.h"
#include "RS_DS_CM11_noSimulinkRS_useManeuver_private.h"
#include "MatSupp.h"

void CarMaker_Startup (void);
void CarMaker_FinishStartup (void);
void CarMaker_Cleanup (void);

/* Block signals (default storage) */
B_RS_DS_CM11_noSimulinkRS_useManeuver_T RS_DS_CM11_noSimulinkRS_useManeuver_B;

/* Block states (default storage) */
DW_RS_DS_CM11_noSimulinkRS_useManeuver_T RS_DS_CM11_noSimulinkRS_useManeuver_DW;

/* Real-time model */
static RT_MODEL_RS_DS_CM11_noSimulinkRS_useManeuver_T
  RS_DS_CM11_noSimulinkRS_useManeuver_M_;
RT_MODEL_RS_DS_CM11_noSimulinkRS_useManeuver_T *const
  RS_DS_CM11_noSimulinkRS_useManeuver_M =
  &RS_DS_CM11_noSimulinkRS_useManeuver_M_;

/* Model output function */
void RS_DS_CM11_noSimulinkRS_useManeuver_output(void)
{
  /* Outputs for Atomic SubSystem: '<Root>/CarMaker' */
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
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[13] = *p++;
  }

  /* S-Function (CM_Sfun): '<S5>/DrivMan' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[2];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[7];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[15] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[16] = *p++;
  }

  /* S-Function (CM_Sfun): '<S7>/VehicleControl' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[3];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[15];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[16];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[15] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[16] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[17] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[18] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[19] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[20] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[21] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[22] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[23] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[24] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[25] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[26] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[27] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[28] = *p++;
  }

  /* S-Function (CM_Sfun): '<S7>/VehicleControlUpd' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[4];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[15];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[16];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[17];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[18];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[19];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[20];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[21];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[22];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[23];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[24];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[25];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[26];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[27];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[28];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o2 = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o6[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o6[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[11] = *p++;
  }

  /* S-Function (CM_Sfun): '<S17>/Steering' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[5];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[4];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[15] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[16] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[17] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[18] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[19] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[20] = *p++;
  }

  /* S-Function (CM_Sfun): '<S15>/Kinematics' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[6];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[15];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[7] = *p++;
  }

  /* S-Function (CM_Sfun): '<S23>/Forces' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[7];
    double *p;
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[15] = *p++;
  }

  /* S-Function (CM_Sfun): '<S21>/Suspension Control Unit' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[8];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[7];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[15] = *p++;
  }

  /* S-Function (CM_Sfun): '<S21>/Suspension Force Elements' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[9];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[15];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[15] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[16] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[17] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[18] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[19] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[20] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[21] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[22] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[23] = *p++;
  }

  /* S-Function (CM_Sfun): '<S21>/Suspension Force Elements Update' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[10];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[15];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[16];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[17];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[18];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[19];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[20];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[21];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[22];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[23];
    rinf->Func();
  }

  /* S-Function (CM_Sfun): '<S16>/Kinetics' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[11];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[15];
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
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[3] = *p++;
  }

  /* S-Function (CM_Sfun): '<S18>/Trailer' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[12];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[3];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[15] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[16] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[17] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[5] = *p++;
  }

  /* S-Function (CM_Sfun): '<S13>/CarAndTrailerUpd' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[13];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[15];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[16];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[17];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[5];
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
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[8] = *p++;
  }

  /* S-Function (CM_Sfun): '<S8>/Brake' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[18];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o6[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o6[1];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[15] = *p++;
  }

  /* S-Function (CM_Sfun): '<S8>/BrakeUpd' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[19];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[15];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[15] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[16] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[17] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[18] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[19] = *p++;
  }

  /* S-Function (CM_Sfun): '<S10>/PowerTrain' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[20];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[15];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[16];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[17];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[18];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[19];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[11];
    rinf->Func();

    /* Copy back outputs from CarMaker */
    p = rinf->Outputs;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[0] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[1] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[2] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[3] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[4] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[5] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[6] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[7] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[8] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[9] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[10] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[11] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[12] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[13] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[14] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[15] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[16] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[17] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[18] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[19] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[20] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[21] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[22] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[23] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[24] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[25] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[26] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[27] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[28] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[29] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[30] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[31] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[32] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[33] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[34] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[35] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[36] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[37] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[38] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[39] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[40] = *p++;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[41] = *p++;
  }

  /* S-Function (CM_Sfun): '<S10>/PowerTrainUpd' */
  {
    const tRoleInfo *rinf = &CM_RoleInfos.v[21];
    double *p;

    /* Copy inputs to CarMaker */
    p = rinf->Inputs;
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[0];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[1];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[2];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[3];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[4];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[5];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[6];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[7];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[8];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[9];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[10];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[11];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[12];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[13];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[14];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[15];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[16];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[17];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[18];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[19];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[20];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[21];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[22];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[23];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[24];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[25];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[26];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[27];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[28];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[29];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[30];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[31];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[32];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[33];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[34];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[35];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[36];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[37];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[38];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[39];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[40];
    *p++ = RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[41];
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

  /* RelationalOperator: '<S11>/Compare' incorporates:
   *  Constant: '<S11>/Constant'
   */
  RS_DS_CM11_noSimulinkRS_useManeuver_B.Compare =
    (RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o2 !=
     RS_DS_CM11_noSimulinkRS_useManeuver_P.VehicleSource_BuiltIn_const);

  /* Stop: '<S6>/Stop Simulation' */
  if (RS_DS_CM11_noSimulinkRS_useManeuver_B.Compare) {
    rtmSetStopRequested(RS_DS_CM11_noSimulinkRS_useManeuver_M, 1);
  }

  /* End of Stop: '<S6>/Stop Simulation' */

  /* S-Function (read_dict): '<S1>/Read CM Dict' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpd = e->GetFunc(e->Var);
  }

  /* Gain: '<S1>/Gain1' */
  RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpdKph =
    RS_DS_CM11_noSimulinkRS_useManeuver_P.Gain1_Gain *
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpd;

  /* Gain: '<S1>/Gain2' */
  RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpdMph =
    RS_DS_CM11_noSimulinkRS_useManeuver_P.Gain2_Gain *
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpd;

  /* S-Function (read_dict): '<S1>/Read CM Dict3' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict3_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CM_Time = e->GetFunc(e->Var);
  }

  /* S-Function (read_dict): '<S1>/Read CM Dict7' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict7_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SimCore_State = e->GetFunc(e->Var);
  }

  /* S-Function (read_dict): '<S1>/Read CM Dict8' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict8_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.distanceTravel = e->GetFunc(e->Var);
  }

  /* S-Function (read_dict): '<S1>/Read CM Dict1' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict1_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_Targ_Dtct = e->GetFunc(e->Var);
  }

  /* S-Function (read_dict): '<S1>/Read CM Dict2' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict2_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_dDist = e->GetFunc(e->Var);
  }

  /* S-Function (read_dict): '<S1>/Read CM Dict4' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict4_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_dSpeed = e->GetFunc(e->Var);
  }

  /* S-Function (read_dict): '<S1>/Read CM Dict5' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict5_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_ObjId = e->GetFunc(e->Var);
  }

  /* S-Function (read_dict): '<S1>/Read CM Dict6' */
  {
    tDDictEntry *e;
    e = (tDDictEntry *)
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict6_PWORK.Entry;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_State = e->GetFunc(e->Var);
  }

  /* End of Outputs for SubSystem: '<Root>/CarMaker' */
}

/* Model update function */
void RS_DS_CM11_noSimulinkRS_useManeuver_update(void)
{
  /* (no update code required) */
}

/* Model initialize function */
void RS_DS_CM11_noSimulinkRS_useManeuver_initialize(void)
{
  /* Registration code */

  /* initialize real-time model */
  (void) memset((void *)RS_DS_CM11_noSimulinkRS_useManeuver_M, 0,
                sizeof(RT_MODEL_RS_DS_CM11_noSimulinkRS_useManeuver_T));

  /* block I/O */
  (void) memset(((void *) &RS_DS_CM11_noSimulinkRS_useManeuver_B), 0,
                sizeof(B_RS_DS_CM11_noSimulinkRS_useManeuver_T));

  {
    int32_T i;
    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o2[i] = 0.0;
    }

    for (i = 0; i < 17; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o2[i] = 0.0;
    }

    for (i = 0; i < 29; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o2[i] = 0.0;
    }

    for (i = 0; i < 5; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o3[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o4[i] = 0.0;
    }

    for (i = 0; i < 11; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o5[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o7[i] = 0.0;
    }

    for (i = 0; i < 21; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o2[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o2[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o3[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o4[i] = 0.0;
    }

    for (i = 0; i < 7; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o5[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o6[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o7[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o8[i] = 0.0;
    }

    for (i = 0; i < 8; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o9[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o2[i] = 0.0;
    }

    for (i = 0; i < 13; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o3[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o4[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o2[i] = 0.0;
    }

    for (i = 0; i < 24; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o2[i] = 0.0;
    }

    for (i = 0; i < 12; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o2[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o3[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o4[i] = 0.0;
    }

    for (i = 0; i < 15; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o3[i] = 0.0;
    }

    for (i = 0; i < 18; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o4[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o5[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o6[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o7[i] = 0.0;
    }

    for (i = 0; i < 14; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o8[i] = 0.0;
    }

    for (i = 0; i < 6; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o9[i] = 0.0;
    }

    for (i = 0; i < 9; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o2[i] = 0.0;
    }

    for (i = 0; i < 16; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.Brake_o2[i] = 0.0;
    }

    for (i = 0; i < 20; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o2[i] = 0.0;
    }

    for (i = 0; i < 42; i++) {
      RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrain_o2[i] = 0.0;
    }

    RS_DS_CM11_noSimulinkRS_useManeuver_B.IoIn = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Environment_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.DrivMan_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControl_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o2 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o6[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehicleControlUpd_o6[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Steering_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinematics_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Forces_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionControlUnit_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElements_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SuspensionForceElementsUpdate = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[2] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o5[3] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[2] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Kinetics_o6[3] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[0] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[1] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[2] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trailer_o2[3] = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CarAndTrailerUpd_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Sync = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.BrakeUpd_o1 = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Sync_e = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.PowerTrainUpd = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CM_User = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.IoOut = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpd = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpdKph = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.VehSpdMph = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.CM_Time = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.SimCore_State = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.distanceTravel = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_Targ_Dtct = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_dDist = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_dSpeed = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_ObjId = 0.0;
    RS_DS_CM11_noSimulinkRS_useManeuver_B.Trf_FOL_State = 0.0;
  }

  /* states (dwork) */
  (void) memset((void *)&RS_DS_CM11_noSimulinkRS_useManeuver_DW, 0,
                sizeof(DW_RS_DS_CM11_noSimulinkRS_useManeuver_T));

  {
    /* user code (registration function declaration) */
    /*Initialize global TRC pointers. */
    RS_DS_CM11_noSimulinkRS_useManeuver_rti_init_trc_pointers();
  }

  {
    /* user code (Start function Header) */
    CarMaker_Startup();

    /* Start for Atomic SubSystem: '<Root>/CarMaker' */

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict' */
    {
      char **namevec = CM_Names2StrVec("Car.v", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict3' */
    {
      char **namevec = CM_Names2StrVec("Time", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict3_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict7' */
    {
      char **namevec = CM_Names2StrVec("SC.State", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict7_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict8' */
    {
      char **namevec = CM_Names2StrVec("Vhcl.Distance", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict8_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict1' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.Targ_Dtct", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict1_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict2' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dDist", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict2_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict4' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.dSpeed", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict4_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict5' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.ObjId", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict5_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* Start for S-Function (read_dict): '<S1>/Read CM Dict6' */
    {
      char **namevec = CM_Names2StrVec("Driver.ReCon.Trf_FOL.State", NULL);
      RS_DS_CM11_noSimulinkRS_useManeuver_DW.ReadCMDict6_PWORK.Entry = (void *)
        MdlQuants_Request(namevec[0]);
      CM_FreeStrVec(namevec);
    }

    /* End of Start for SubSystem: '<Root>/CarMaker' */

    /* user code (Start function Trailer) */
    CarMaker_FinishStartup();
  }
}

/* Model terminate function */
void RS_DS_CM11_noSimulinkRS_useManeuver_terminate(void)
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
