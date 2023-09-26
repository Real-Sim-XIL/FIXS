﻿##############################################################################
### Template file         : LibraryInterface.tmk
### File                  : RS_DS_CM11_noSimulinkRS_useManeuver.mk
### Compatibility         : DSMAKE 6.26
### Description           : Library makefile.
###
### Copyright 2016, dSPACE GmbH. All rights reserved.
###
##############################################################################

MAKEFILE_DIR = "$(INTERFACE_IN_INCLUDE_FILEPATH,D,"=)"

TMP_INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_SRC_DIRPATHS    =  \
 ".\..\..\..\..\Components\RS_DS_CM11_noSimulinkRS_useManeuver" \
 ".\..\..\..\..\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt" \
 ".\..\..\..\..\..\..\..\..\Program Files\MATLAB\R2021b\rtw\c\src" \
 ".\..\..\..\..\..\..\..\..\Program Files\MATLAB\R2021b\simulink\src" \
 ".\..\..\..\..\..\..\src_cm4sl"

TMP_INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_HDR_DIRPATHS    =  \
 ".\..\..\..\..\Components\RS_DS_CM11_noSimulinkRS_useManeuver" \
 ".\..\..\..\..\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt" \
 ".\..\..\..\..\..\..\src_cm4sl" \
 ".\..\..\..\..\..\..\..\..\Program Files\MATLAB\R2021b\extern\include" \
 ".\..\..\..\..\..\..\..\..\Program Files\MATLAB\R2021b\simulink\include" \
 ".\..\..\..\..\..\..\..\..\Program Files\MATLAB\R2021b\rtw\c\src" \
 ".\..\..\..\..\..\..\..\..\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 ".\..\..\..\..\..\..\include" \
 ".\..\..\..\..\..\..\..\..\IPG\carmaker\win64-11.0.1\include" \
 ".\..\..\..\..\..\..\..\..\IPG\carmaker\win64-11.0.1\Matlab\R2021b"

#-------------------------------------------------------------------------------
# Interface Settings
#-------------------------------------------------------------------------------

INTERFACE_INCLUDE_MAKEFILE_VERSION = 1

% set INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_FILELOCATION = $(MAKEFILE_DIR,>\RS_DS_CM11_noSimulinkRS_useManeuver.a)

INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_SRC_FILES       =  \
 RS_DS_CM11_noSimulinkRS_useManeuver.c \
 RS_DS_CM11_noSimulinkRS_useManeuver_data.c \
 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.c \
 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.c \
 RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.c \
 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.c \
 CM_Main.c \
 CM_Vehicle.c \
 User.c \
 IO.c \
 app_tmp.c

% foreach IDX_DIRPATH in $(TMP_INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_SRC_DIRPATHS)
%   set INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_SRC_DIRPATHS += $(MAKEFILE_DIR,>\$(IDX_DIRPATH,"=),A)
% endfor
% set INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_SRC_DIRPATHS += 

% foreach IDX_DIRPATH in $(TMP_INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_HDR_DIRPATHS)
%   set INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_HDR_DIRPATHS += $(MAKEFILE_DIR,>\$(IDX_DIRPATH,"=),A)
% endfor
% set INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_HDR_DIRPATHS += 

INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_OBJ_FILEPATHS      = 

INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_ADD_DEFINES     =  \
 Model=RS_DS_CM11_noSimulinkRS_useManeuver \
 MT=0 \
 NUMST=1 \
 NCSTATES=0 \
 TID01EQ=0 \
 USE_RTMODEL \
 RT \
 NO_PRINTF

INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_ADD_UNDEFS      = 

# Interface information about created library
INTERFACE_LIB_TARGET        = RS_DS_CM11_noSimulinkRS_useManeuver

% set INTERFACE_LIB_FILEPATHS += $(MAKEFILE_DIR,>\RS_DS_CM11_noSimulinkRS_useManeuver.a)

INTERFACE_LIB_FILEPATHS     +=  \
 "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libcar4sl.a" \
 "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libcarmaker4sl.a" \
 "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libipgdriver.a" \
 "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libipgroad.a" \
 "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libipgtire.a" \
 "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\liburiparser.a" \
 "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libz-dsrtlx.a" \
 "C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b\libmatsupp-dsrtlx.a"

% undef IDX_DIRPATH
% undef TMP_INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_SRC_DIRPATHS
% undef TMP_INTERFACE_LIB_RS_DS_CM11_noSimulinkRS_useManeuver_CC_HDR_DIRPATHS
% undef MAKEFILE_DIR

# [EOF]