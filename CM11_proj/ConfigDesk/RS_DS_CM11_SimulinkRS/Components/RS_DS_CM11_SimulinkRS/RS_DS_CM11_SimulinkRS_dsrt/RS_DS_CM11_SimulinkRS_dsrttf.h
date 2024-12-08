/*********************** dSPACE target specific file *************************

   Include file RS_DS_CM11_SimulinkRS_dsrttf.h:

   Extern declarations of task functions.

   Fri May 19 17:14:26 2023

   Copyright 2019, dSPACE GmbH. All rights reserved.

 *****************************************************************************/

#ifndef _DSRT_RS_DS_CM11_SimulinkRS_DSRTTF_HEADER_
#define _DSRT_RS_DS_CM11_SimulinkRS_DSRTTF_HEADER_
#ifdef EXTERN_C
#undef EXTERN_C
#endif

#ifdef __cplusplus
#define EXTERN_C                       extern "C"
#else
#define EXTERN_C                       extern
#endif

EXTERN_C void RS_DS_CM11_SimulinkRS_DSRTMdlOutputs0(void);
EXTERN_C void RS_DS_CM11_SimulinkRS_DSRTMdlUpdate0(void);

#endif                          /* _DSRT_RS_DS_CM11_SimulinkRS_DSRTTF_HEADER_ */