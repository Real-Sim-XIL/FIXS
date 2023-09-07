/****************************************************************************\ 
 *** 
 *** File                  : data_out_ap.cpp
 *** 
 *** Generated by dSPACE Tools.
 *** (c) Copyright 2013, dSPACE GmbH. All rights reserved.
\****************************************************************************/

#if !defined(DATA_OUT_AP_CPP_)
#  define    DATA_OUT_AP_CPP_


/* -- Include directives. -- */
#  include "IOCode_Data.h"
#  include <DsApplicationInterface.h>
#  include "string.h"
#  include "data_out_ap.h"

#  if defined(EXTERN_C_DECL)
#    undef EXTERN_C_DECL
#  endif
#  if defined(__cplusplus)
#    define EXTERN_C_DECL extern "C"
#  else
#    define EXTERN_C_DECL extern
#  endif


/* -- Variable declarations in module scope. -- */

/* Pragma to suppress GCC stack reuse which leads to problems with signal chain code */
#ifdef __GNUC__
#pragma GCC optimize "-fstack-reuse=none"
#endif

/* -- Definitions of exported functions. -- */

  void
ap_write_RS_DS_CM11_SimulinkRS_DataOutport1_P1_S1(
    UInt32* x) 
{
/* ModelPort:{Receive_Out [TCP (1)].Vector Size}, PortId:{3a4e41e0-0059-4009-8606-30e05f8817b4} */
UInt32 localVariable_1;
{
    localVariable_1 = *x;
}
{
tCP_1_Receive_DataSize = localVariable_1;
}

}

  void
ap_write_RS_DS_CM11_SimulinkRS_DataOutport3_P1_S1(
    UInt8* x) 
{
/* ModelPort:{Transmit_Out [TCP (1)].Data Vector}, PortId:{abefa9c2-5971-43e2-92ea-8b98002a54ba} */
UInt8* localVariable_1;
{
    localVariable_1 = x;
}
{
UInt8 * tmpVector = localVariable_1;
std::memcpy(tCP_1_Transmit_DataVector, tmpVector, 200 * (sizeof *tmpVector));
}

}

  void
ap_write_RS_DS_CM11_SimulinkRS_DataOutport3_P2_S1(
    UInt32* x) 
{
/* ModelPort:{Transmit_Out [TCP (1)].Vector Size}, PortId:{cddb9d5a-43dc-4246-b7b3-04f9827e0c24} */
UInt32 localVariable_1;
{
    localVariable_1 = *x;
}
{
tCP_1_Transmit_DataSize = localVariable_1;
}

}

  void
ap_write_RS_DS_CM11_SimulinkRS_DataOutport2_P1_S1(
    Bool* x) 
{
/* ModelPort:{Status/Control_Out [TCP (1)].Connection Request}, PortId:{50966301-89aa-49cd-9e83-22a02fa0d1d9} */
Bool localVariable_1;
{
    localVariable_1 = *x;
}
{
tCP_1_ConnectionRequest = (Int32)localVariable_1;
}

}

#endif                                 /* DATA_OUT_AP_CPP_ */
/* [EOF] */
