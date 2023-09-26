/*********************** dSPACE target specific header file ********************
   Include file RS_DS_CM11_noSimulinkRS_dsmpbap.h:

   Definitions used for access points

   Tue Sep 19 14:33:07 2023

   (c) Copyright 2020, dSPACE GmbH. All rights reserved.

 *******************************************************************************/

#ifndef _DSMPB_RS_DS_CM11_noSimulinkRS_HEADER_
#define _DSMPB_RS_DS_CM11_noSimulinkRS_HEADER_
#include "rtwtypes.h"
#ifdef EXTERN_C
#undef EXTERN_C
#endif

#ifdef __cplusplus
#define EXTERN_C                       extern "C"
#else
#define EXTERN_C                       extern
#endif

/* External declarations for access points prototypes */

/*                                                                            */
/* Declarations of read/write and trigger access points                       */
/*                                                                            */

/*                                                                            */
/* Declarations of function module access points                              */
/*                                                                            */
#ifdef HAVE_RS_DS_CM11_noSimulinkRS_DSMPBAP_CUSTOM
#include "RS_DS_CM11_noSimulinkRS_dsmpbap_custom.h"
#endif
#endif
