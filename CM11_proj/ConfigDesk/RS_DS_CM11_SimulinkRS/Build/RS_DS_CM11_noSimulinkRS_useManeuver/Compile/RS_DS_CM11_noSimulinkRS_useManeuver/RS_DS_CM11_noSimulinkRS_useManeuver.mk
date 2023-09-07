CC_OPT_SET_SUPPORTED_VALUES = DEFAULT NONE CUSTOM 
INCLUDE_MAKEFILE_VERSION    = 1 
LIB_TARGET                  = RS_DS_CM11_noSimulinkRS_useManeuver 
SRCS= RS_DS_CM11_noSimulinkRS_useManeuver.c RS_DS_CM11_noSimulinkRS_useManeuver_data.c \
		  RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.c \
		  RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.c CM_Main.c \
		  CM_Vehicle.c User.c IO.c app_tmp.c
MRK_FILES                   = RS_DS_CM11_noSimulinkRS_useManeuver.mrk RS_DS_CM11_noSimulinkRS_useManeuver_data.mrk RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.mrk RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.mrk RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.mrk RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.mrk CM_Main.mrk CM_Vehicle.mrk User.mrk IO.mrk app_tmp.mrk  
OBJS= RS_DS_CM11_noSimulinkRS_useManeuver.o86 RS_DS_CM11_noSimulinkRS_useManeuver_data.o86 \
		  RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.o86 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.o86 \
		  RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.o86 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.o86 CM_Main.o86 \
		  CM_Vehicle.o86 User.o86 IO.o86 app_tmp.o86
CC_SRC_DIRPATHS = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 "C:\Program Files\MATLAB\R2021b\simulink\src" \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl 
 
CC_HDR_DIRPATHS = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 "C:\Program Files\MATLAB\R2021b\extern\include" \
 "C:\Program Files\MATLAB\R2021b\simulink\include" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 C:\CM_Projects\RealSimCm11Prj\include \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b 
 
CC_LIB_FILE                 = C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Build\RS_DS_CM11_noSimulinkRS_useManeuver\Compile\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver.a 
OBJ_FILEPATHS               =  
LIB_DIRPATHS                =    
CUSTOM_DIRPATHS             =  C:\IPG\carmaker\win64-11.0.1\include C:\CM_Projects\RealSimCm11Prj\src_cm4sl C:\CM_Projects\RealSimCm11Prj\include 
CC_ADD_OPTS                 =   -include ipgrt.h 
CPP_ADD_OPTS                =   -include ipgrt.h 
CC_ADD_DEFINES              = RS_CAVE RS_DEBUG RS_DSPACE DSPACE DSRTLX _DSRTLX CM_HIL CM_NUMVER=110001 CM4SLDS DATA_PORT_ACCESS_POINT_API_VERSION=2 Model=RS_DS_CM11_noSimulinkRS_useManeuver MT=0 NUMST=1 NCSTATES=0 TID01EQ=0 USE_RTMODEL RT NO_PRINTF 
CC_ADD_UNDEFS               =  
CC_OPT_SET                  = Default 
CC_CUSTOM_OPT_OPTS          =  

all : MakeLib 

depend : 

%include ..\Configuration.mk 
%include ..\LibraryRules.mk 
%include ..\CommonRules.mk 

CC_ALL_OPTS = \
 -c \
 -fno-common \
 -fno-strict-aliasing \
 -pipe \
 -march=sandybridge \
 -ffast-math \
 -mno-avx \
 -msse2 \
 -nostdinc \
 -mfpmath=sse \
 -fomit-frame-pointer \
 -mstackrealign \
 "-isystemC:\Program Files\dSPACE RCPHIL 2021-B\SCALEXIO\Include" \
 "-isystemC:\Program Files\dSPACE RCPHIL 2021-B\SCALEXIO\Include" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\i686-linux-gnu\include\c++\5.5.0" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\i686-linux-gnu\include\c++\5.5.0\i686-linux-gnu" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\include" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\lib\gcc\i686-linux-gnu\5.5.0\include" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\lib\gcc\i686-linux-gnu\5.5.0\include-fixed" \
 -Werror-implicit-function-declaration \
 -D_DSx86_32 \
 -D_DS2510 \
 -DDS_PLATFORM_X86 \
 -DDS_PLATFORM_LE \
 -DDS_PLATFORM_POSIX \
 -DDS_PLATFORM_LINUX \
 -DDS_PLATFORM_DSRTLINUX \
 -DDS_PLATFORM_CN \
 -DDS_PLATFORM_HCN \
 -DDS_PLATFORM_SMART \
 -DDS_PLATFORM_SMARTHIL \
 -DDS_PLATFORM_SMP \
 -D__HOSTNAME__=\""DESK0120048"\" \
 -D__INTEGRATION__=\""5.2.1"\" \
 -DDS_RTLIB_VERSION_MAR=5 \
 -DDS_RTLIB_VERSION_MIR=2 \
 -DDS_QNX_VERSION=650 \
 -DQNX_VERSION=6.5.0 \
 -D_FILE_OFFSET_BITS=64 \
 -O2 \
 -finline-functions \
 -D_INLINE \
 -include \
 ipgrt.h \
 -IC:\IPG\carmaker\win64-11.0.1\include \
 -IC:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 -IC:\CM_Projects\RealSimCm11Prj\include \
 -IC:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 -IC:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 -IC:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 -I"C:\Program Files\MATLAB\R2021b\extern\include" \
 -I"C:\Program Files\MATLAB\R2021b\simulink\include" \
 -I"C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 -I"C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 -IC:\CM_Projects\RealSimCm11Prj\include \
 -IC:\IPG\carmaker\win64-11.0.1\include \
 -IC:\IPG\carmaker\win64-11.0.1\Matlab\R2021b \
 -I"C:\Program Files\dSPACE RCPHIL 2021-B\SCALEXIO\Include" \
 -DRS_CAVE \
 -DRS_DEBUG \
 -DRS_DSPACE \
 -DDSPACE \
 -DDSRTLX \
 -D_DSRTLX \
 -DCM_HIL \
 -DCM_NUMVER=110001 \
 -DCM4SLDS \
 -DDATA_PORT_ACCESS_POINT_API_VERSION=2 \
 -DModel=RS_DS_CM11_noSimulinkRS_useManeuver \
 -DMT=0 \
 -DNUMST=1 \
 -DNCSTATES=0 \
 -DTID01EQ=0 \
 -DUSE_RTMODEL \
 -DRT \
 -DNO_PRINTF 
 
CPP_ALL_OPTS = \
 -c \
 -fno-common \
 -fno-strict-aliasing \
 -pipe \
 -march=sandybridge \
 -ffast-math \
 -mno-avx \
 -msse2 \
 -nostdinc \
 -mfpmath=sse \
 -fomit-frame-pointer \
 -mstackrealign \
 "-isystemC:\Program Files\dSPACE RCPHIL 2021-B\SCALEXIO\Include" \
 "-isystemC:\Program Files\dSPACE RCPHIL 2021-B\SCALEXIO\Include" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\i686-linux-gnu\include\c++\5.5.0" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\i686-linux-gnu\include\c++\5.5.0\i686-linux-gnu" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\include" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\lib\gcc\i686-linux-gnu\5.5.0\include" \
 -isystem"C:\Program Files\dSPACE RCPHIL 2021-B\Compiler\Linux440_550\target\i686-linux-gnu\lib\gcc\i686-linux-gnu\5.5.0\include-fixed" \
 -fno-operator-names \
 -Wall \
 -D_DSx86_32 \
 -D_DS2510 \
 -DDS_PLATFORM_X86 \
 -DDS_PLATFORM_LE \
 -DDS_PLATFORM_POSIX \
 -DDS_PLATFORM_LINUX \
 -DDS_PLATFORM_DSRTLINUX \
 -DDS_PLATFORM_CN \
 -DDS_PLATFORM_HCN \
 -DDS_PLATFORM_SMART \
 -DDS_PLATFORM_SMARTHIL \
 -DDS_PLATFORM_SMP \
 -D__HOSTNAME__=\""DESK0120048"\" \
 -D__INTEGRATION__=\""5.2.1"\" \
 -DDS_RTLIB_VERSION_MAR=5 \
 -DDS_RTLIB_VERSION_MIR=2 \
 -DDS_QNX_VERSION=650 \
 -DQNX_VERSION=6.5.0 \
 -D_FILE_OFFSET_BITS=64 \
 -O2 \
 -finline-functions \
 -D_INLINE \
 -include \
 ipgrt.h \
 -IC:\IPG\carmaker\win64-11.0.1\include \
 -IC:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 -IC:\CM_Projects\RealSimCm11Prj\include \
 -IC:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 -IC:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 -IC:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 -I"C:\Program Files\MATLAB\R2021b\extern\include" \
 -I"C:\Program Files\MATLAB\R2021b\simulink\include" \
 -I"C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 -I"C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 -IC:\CM_Projects\RealSimCm11Prj\include \
 -IC:\IPG\carmaker\win64-11.0.1\include \
 -IC:\IPG\carmaker\win64-11.0.1\Matlab\R2021b \
 -I"C:\Program Files\dSPACE RCPHIL 2021-B\SCALEXIO\Include" \
 -DRS_CAVE \
 -DRS_DEBUG \
 -DRS_DSPACE \
 -DDSPACE \
 -DDSRTLX \
 -D_DSRTLX \
 -DCM_HIL \
 -DCM_NUMVER=110001 \
 -DCM4SLDS \
 -DDATA_PORT_ACCESS_POINT_API_VERSION=2 \
 -DModel=RS_DS_CM11_noSimulinkRS_useManeuver \
 -DMT=0 \
 -DNUMST=1 \
 -DNCSTATES=0 \
 -DTID01EQ=0 \
 -DUSE_RTMODEL \
 -DRT \
 -DNO_PRINTF 
 
# ============================================================================= 
# MKMF Section 
# ============================================================================= 

# ----------------------------------------------------------------------------- 
# MKMF Configuration 
# ----------------------------------------------------------------------------- 

.TYPE.C   = .c .cpp 
.TYPE.OBJ = .o86 .o86 

.PATH.c = \
 C:\IPG\carmaker\win64-11.0.1\include; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; \
 C:\CM_Projects\RealSimCm11Prj\include; \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver; \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt; \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src"; \
 "C:\Program Files\MATLAB\R2021b\simulink\src"; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; 
 
.PATH.cpp = \
 C:\IPG\carmaker\win64-11.0.1\include; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; \
 C:\CM_Projects\RealSimCm11Prj\include; \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver; \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt; \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src"; \
 "C:\Program Files\MATLAB\R2021b\simulink\src"; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; 
 
.PATH.h = \
 C:\IPG\carmaker\win64-11.0.1\include; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; \
 C:\CM_Projects\RealSimCm11Prj\include; \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver; \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; \
 "C:\Program Files\MATLAB\R2021b\extern\include"; \
 "C:\Program Files\MATLAB\R2021b\simulink\include"; \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src"; \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common"; \
 C:\CM_Projects\RealSimCm11Prj\include; \
 C:\IPG\carmaker\win64-11.0.1\include; \
 C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b; 
 

.HDRPATH.c = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 "C:\Program Files\MATLAB\R2021b\extern\include" \
 "C:\Program Files\MATLAB\R2021b\simulink\include" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 C:\CM_Projects\RealSimCm11Prj\include \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include \
 | \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 "C:\Program Files\MATLAB\R2021b\extern\include" \
 "C:\Program Files\MATLAB\R2021b\simulink\include" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 C:\CM_Projects\RealSimCm11Prj\include \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include 
 
.HDRPATH.cpp = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 "C:\Program Files\MATLAB\R2021b\extern\include" \
 "C:\Program Files\MATLAB\R2021b\simulink\include" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 C:\CM_Projects\RealSimCm11Prj\include \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include \
 | \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 "C:\Program Files\MATLAB\R2021b\extern\include" \
 "C:\Program Files\MATLAB\R2021b\simulink\include" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src" \
 "C:\Program Files\MATLAB\R2021b\rtw\c\src\ext_mode\common" \
 C:\CM_Projects\RealSimCm11Prj\include \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include 
 

.CDEFINES.c  = -D_DSx86_32 -D_DS2510 -DDS_PLATFORM_X86 -DDS_PLATFORM_LE -DDS_PLATFORM_POSIX -DDS_PLATFORM_LINUX -DDS_PLATFORM_DSRTLINUX -DDS_PLATFORM_CN -DDS_PLATFORM_HCN -DDS_PLATFORM_SMART -DDS_PLATFORM_SMARTHIL -DDS_PLATFORM_SMP -D__HOSTNAME__=\DESK0120048\ -D__INTEGRATION__=\5.2.1\ -DDS_RTLIB_VERSION_MAR=5 -DDS_RTLIB_VERSION_MIR=2 -DDS_QNX_VERSION=650 -DQNX_VERSION=6.5.0 -D_FILE_OFFSET_BITS=64 -D_INLINE  
.CDEFINES.cpp = -D__cplusplus -D_DSx86_32 -D_DS2510 -DDS_PLATFORM_X86 -DDS_PLATFORM_LE -DDS_PLATFORM_POSIX -DDS_PLATFORM_LINUX -DDS_PLATFORM_DSRTLINUX -DDS_PLATFORM_CN -DDS_PLATFORM_HCN -DDS_PLATFORM_SMART -DDS_PLATFORM_SMARTHIL -DDS_PLATFORM_SMP -D__HOSTNAME__=\DESK0120048\ -D__INTEGRATION__=\5.2.1\ -DDS_RTLIB_VERSION_MAR=5 -DDS_RTLIB_VERSION_MIR=2 -DDS_QNX_VERSION=650 -DQNX_VERSION=6.5.0 -D_FILE_OFFSET_BITS=64 -D_INLINE  

MKMF_SRCS  = RS_DS_CM11_noSimulinkRS_useManeuver.c RS_DS_CM11_noSimulinkRS_useManeuver_data.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.c CM_Main.c CM_Vehicle.c User.c IO.c app_tmp.c 
MKMF_LINELEN = 118 
MKMF_TABLEN  = 2 

# ----------------------------------------------------------------------------- 
# MKMF Replacements 
# ----------------------------------------------------------------------------- 

HDRS= multiword_types.h RS_DS_CM11_noSimulinkRS_useManeuver.h RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.h \
		  RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.h RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.h \
		  RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.h RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
		  RS_DS_CM11_noSimulinkRS_useManeuver_types.h rtmodel.h rtwtypes.h IOVec.h User.h VirEnv_Wrapper.h ADASRP.h adtf.h \
		  apo.h can_interface.h can_rbs.h CarMaker.h CarMaker4SL.h CM_CCP.h CM_Sfun_Utils.h CM_XCP.h CMThread.h \
		  CycleControl.h DataDict.h DataStorage.h DirectVarAccess.h DrivMan.h dsio.h Environment.h EnvUtils.h \
		  ExternalInputs.h FailSafeTester.h flex.h flex_rbs.h FMUQueue_IF.h GCS.h Global.h infoc.h InfoParam.h InfoUtils.h \
		  ioconf.h ipgdriver.h LinMap.h Log.h MathUtils.h MData.h ModelManager.h Periodic.h QuantAudit.h rbs.h road.h \
		  roadjunction.h roadpath.h roadrefline.h roadsegment.h roadspline.h roadtypedefs.h Scratchpad.h SessionCmds.h \
		  SimCore.h SimNet.h spline.h system.h TextUtils.h Traffic.h TrafficLight.h Vehicle.h VehicleControl.h MatSupp.h \
		  tmwtypes.h rtw_continuous.h rtw_solver.h

EXTHDRS= c:\IPG\carmaker\win64-11.0.1\include\Car\Brake.h c:\IPG\carmaker\win64-11.0.1\include\Car\Brake_HydESP.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Car\Car.h c:\IPG\carmaker\win64-11.0.1\include\Car\Car_CM4SL.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Car\PowerFlow.h c:\IPG\carmaker\win64-11.0.1\include\Car\PowerTrain.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Car\Steering.h c:\IPG\carmaker\win64-11.0.1\include\Car\Susp.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Car\Trailer.h c:\IPG\carmaker\win64-11.0.1\include\Car\Trailer_Brake.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Car\Vehicle_Car.h c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Aero.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\PylonDetect.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Assembly.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Camera.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_CameraRSI.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Collision.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_FSpace.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_GNav.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Inertial.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_LidarRSI.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Line.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Object.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_ObjectByLane.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Radar.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_RadarRSI.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_SAngle.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_TSign.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_USonicRSI.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Surrounding.h c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Tire.h \
		  c:\IPG\carmaker\win64-11.0.1\include\Vehicle\VehicleControlApps.h

# [EOF] 
 
### OPUS MKMF:  Do not remove this line!  Generated dependencies follow.

RS_DS_CM11_noSimulinkRS_useManeuver.o86: RS_DS_CM11_noSimulinkRS_useManeuver.c \
	 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver.h \
			 Global.h \
			 CarMaker4SL.h \
				 SimCore.h \
			 TextUtils.h \
			 DataDict.h \
			 rtwtypes.h \
				 tmwtypes.h \
			 rtw_continuous.h \
				 rtwtypes.h \
					 # tmwtypes.h \
			 rtw_solver.h \
				 rtw_continuous.h \
					 # rtwtypes.h \
			 RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
				 rtwtypes.h \
					 # tmwtypes.h \
				 multiword_types.h \
					 rtwtypes.h \
						 # tmwtypes.h \
			 multiword_types.h \
				 # rtwtypes.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
			 rtwtypes.h \
				 # tmwtypes.h \
			 multiword_types.h \
				 # rtwtypes.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver.h \
		 # Global.h \
		 # CarMaker4SL.h \
		 # TextUtils.h \
		 # DataDict.h \
		 # rtwtypes.h \
		 # rtw_continuous.h \
		 # rtw_solver.h \
		 # RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
		 # multiword_types.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
		 # rtwtypes.h \
		 # multiword_types.h \
	 MatSupp.h \
		 infoc.h \
		 DataDict.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h

RS_DS_CM11_noSimulinkRS_useManeuver_data.o86: RS_DS_CM11_noSimulinkRS_useManeuver_data.c \
	 RS_DS_CM11_noSimulinkRS_useManeuver.h \
		 Global.h \
		 CarMaker4SL.h \
			 SimCore.h \
		 TextUtils.h \
		 DataDict.h \
		 rtwtypes.h \
			 tmwtypes.h \
		 rtw_continuous.h \
			 rtwtypes.h \
				 # tmwtypes.h \
		 rtw_solver.h \
			 rtw_continuous.h \
				 # rtwtypes.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
			 rtwtypes.h \
				 # tmwtypes.h \
			 multiword_types.h \
				 rtwtypes.h \
					 # tmwtypes.h \
		 multiword_types.h \
			 # rtwtypes.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
		 rtwtypes.h \
			 # tmwtypes.h \
		 multiword_types.h \
			 # rtwtypes.h

RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.o86: RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.c \
	 rtmodel.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver.h \
			 Global.h \
			 CarMaker4SL.h \
				 SimCore.h \
			 TextUtils.h \
			 DataDict.h \
			 rtwtypes.h \
				 tmwtypes.h \
			 rtw_continuous.h \
				 rtwtypes.h \
					 # tmwtypes.h \
			 rtw_solver.h \
				 rtw_continuous.h \
					 # rtwtypes.h \
			 RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
				 rtwtypes.h \
					 # tmwtypes.h \
				 multiword_types.h \
					 rtwtypes.h \
						 # tmwtypes.h \
			 multiword_types.h \
				 # rtwtypes.h \
	 rtwtypes.h \
		 # tmwtypes.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.h \
		 rtwtypes.h \
			 # tmwtypes.h

RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.o86: RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.c \
	 RS_DS_CM11_noSimulinkRS_useManeuver.h \
		 Global.h \
		 CarMaker4SL.h \
			 SimCore.h \
		 TextUtils.h \
		 DataDict.h \
		 rtwtypes.h \
			 tmwtypes.h \
		 rtw_continuous.h \
			 rtwtypes.h \
				 # tmwtypes.h \
		 rtw_solver.h \
			 rtw_continuous.h \
				 # rtwtypes.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
			 rtwtypes.h \
				 # tmwtypes.h \
			 multiword_types.h \
				 rtwtypes.h \
					 # tmwtypes.h \
		 multiword_types.h \
			 # rtwtypes.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
		 rtwtypes.h \
			 # tmwtypes.h \
		 multiword_types.h \
			 # rtwtypes.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.h

RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.o86: RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.c \
	 RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver.h \
		 Global.h \
		 CarMaker4SL.h \
			 SimCore.h \
		 TextUtils.h \
		 DataDict.h \
		 rtwtypes.h \
			 tmwtypes.h \
		 rtw_continuous.h \
			 rtwtypes.h \
				 # tmwtypes.h \
		 rtw_solver.h \
			 rtw_continuous.h \
				 # rtwtypes.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
			 rtwtypes.h \
				 # tmwtypes.h \
			 multiword_types.h \
				 rtwtypes.h \
					 # tmwtypes.h \
		 multiword_types.h \
			 # rtwtypes.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
		 rtwtypes.h \
			 # tmwtypes.h \
		 multiword_types.h \
			 # rtwtypes.h

RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.o86: RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.c \
	 RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver.h \
			 Global.h \
			 CarMaker4SL.h \
				 SimCore.h \
			 TextUtils.h \
			 DataDict.h \
			 rtwtypes.h \
				 tmwtypes.h \
			 rtw_continuous.h \
				 rtwtypes.h \
					 # tmwtypes.h \
			 rtw_solver.h \
				 rtw_continuous.h \
					 # rtwtypes.h \
			 RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
				 rtwtypes.h \
					 # tmwtypes.h \
				 multiword_types.h \
					 rtwtypes.h \
						 # tmwtypes.h \
			 multiword_types.h \
				 # rtwtypes.h \
		 RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
			 rtwtypes.h \
				 # tmwtypes.h \
			 multiword_types.h \
				 # rtwtypes.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver.h \
		 # Global.h \
		 # CarMaker4SL.h \
		 # TextUtils.h \
		 # DataDict.h \
		 # rtwtypes.h \
		 # rtw_continuous.h \
		 # rtw_solver.h \
		 # RS_DS_CM11_noSimulinkRS_useManeuver_types.h \
		 # multiword_types.h \
	 RS_DS_CM11_noSimulinkRS_useManeuver_private.h \
		 # rtwtypes.h \
		 # multiword_types.h

CM_Main.o86: CM_Main.c \
	 Global.h \
	 system.h \
	 CarMaker.h \
		 system.h \
		 infoc.h \
		 apo.h \
		 TextUtils.h \
		 EnvUtils.h \
		 MathUtils.h \
			 Log.h \
		 CMThread.h \
		 Log.h \
		 InfoUtils.h \
			 infoc.h \
		 DataDict.h \
		 DataStorage.h \
		 Scratchpad.h \
		 QuantAudit.h \
		 SessionCmds.h \
		 DirectVarAccess.h \
			 DataDict.h \
		 ExternalInputs.h \
		 MData.h \
		 spline.h \
		 LinMap.h \
		 SimCore.h \
		 Periodic.h \
			 SimCore.h \
		 Vehicle.h \
			 SimCore.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 road.h \
					 roadtypedefs.h \
					 roadspline.h \
					 roadrefline.h \
					 roadsegment.h \
					 roadjunction.h \
					 roadpath.h \
				 GCS.h \
		 Environment.h \
			 GCS.h \
		 GCS.h \
		 DrivMan.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 VehicleControl.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 Traffic.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 # road.h \
				 # GCS.h \
		 TrafficLight.h \
	 ModelManager.h \
		 DataDict.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Inertial.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_SAngle.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Object.h \
		 Traffic.h \
			 # GCS.h \
			 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_FSpace.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 # road.h \
		 # GCS.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_TSign.h \
		 road.h \
			 # roadtypedefs.h \
			 # roadspline.h \
			 # roadrefline.h \
			 # roadsegment.h \
			 # roadjunction.h \
			 # roadpath.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Line.h \
		 road.h \
			 # roadtypedefs.h \
			 # roadspline.h \
			 # roadrefline.h \
			 # roadsegment.h \
			 # roadjunction.h \
			 # roadpath.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Collision.h \
		 Vehicle.h \
			 # SimCore.h \
			 # GCS.h \
			 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 Traffic.h \
			 # GCS.h \
			 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_GNav.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\PylonDetect.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Radar.h \
		 road.h \
			 # roadtypedefs.h \
			 # roadspline.h \
			 # roadrefline.h \
			 # roadsegment.h \
			 # roadjunction.h \
			 # roadpath.h \
		 Traffic.h \
			 # GCS.h \
			 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 LinMap.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Surrounding.h \
		 road.h \
			 # roadtypedefs.h \
			 # roadspline.h \
			 # roadrefline.h \
			 # roadsegment.h \
			 # roadjunction.h \
			 # roadpath.h \
		 Traffic.h \
			 # GCS.h \
			 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
		 Environment.h \
			 # GCS.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_USonicRSI.h \
		 SimCore.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_RadarRSI.h \
		 SimCore.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_LidarRSI.h \
		 SimCore.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_CameraRSI.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Camera.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_ObjectByLane.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Assembly.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake.h \
		 SimCore.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Car\Trailer_Brake.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake.h \
			 # SimCore.h \
	 CarMaker4SL.h \
		 SimCore.h \
	 CM_Sfun_Utils.h \
		 road.h \
			 # roadtypedefs.h \
			 # roadspline.h \
			 # roadrefline.h \
			 # roadsegment.h \
			 # roadjunction.h \
			 # roadpath.h \
		 Environment.h \
			 # GCS.h \
	 InfoUtils.h \
		 # infoc.h \
	 InfoParam.h \
	 MatSupp.h \
		 infoc.h \
		 DataDict.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
	 CycleControl.h \
	 adtf.h \
	 ADASRP.h \
	 User.h \
		 Global.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
	 IOVec.h \
	 CM_XCP.h \
	 CM_CCP.h \
	 FMUQueue_IF.h \
		 ModelManager.h \
			 # DataDict.h \
	 SimNet.h

CM_Vehicle.o86: CM_Vehicle.c \
	 Global.h \
	 CarMaker.h \
		 system.h \
		 infoc.h \
		 apo.h \
		 TextUtils.h \
		 EnvUtils.h \
		 MathUtils.h \
			 Log.h \
		 CMThread.h \
		 Log.h \
		 InfoUtils.h \
			 infoc.h \
		 DataDict.h \
		 DataStorage.h \
		 Scratchpad.h \
		 QuantAudit.h \
		 SessionCmds.h \
		 DirectVarAccess.h \
			 DataDict.h \
		 ExternalInputs.h \
		 MData.h \
		 spline.h \
		 LinMap.h \
		 SimCore.h \
		 Periodic.h \
			 SimCore.h \
		 Vehicle.h \
			 SimCore.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 road.h \
					 roadtypedefs.h \
					 roadspline.h \
					 roadrefline.h \
					 roadsegment.h \
					 roadjunction.h \
					 roadpath.h \
				 GCS.h \
		 Environment.h \
			 GCS.h \
		 GCS.h \
		 DrivMan.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 VehicleControl.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 Traffic.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 # road.h \
				 # GCS.h \
		 TrafficLight.h \
	 CarMaker4SL.h \
		 SimCore.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Car\Vehicle_Car.h \
		 ModelManager.h \
			 DataDict.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Aero.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Steering.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake.h \
			 SimCore.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake_HydESP.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\PowerTrain.h \
			 DrivMan.h \
				 # Vehicle.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\PowerFlow.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Tire.h \
			 ModelManager.h \
				 # DataDict.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Trailer.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Trailer_Brake.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake.h \
				 # SimCore.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Car.h \
			 GCS.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 # road.h \
				 # GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Car\Susp.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Susp.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\VehicleControlApps.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Car\Car_CM4SL.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Tire.h \
			 # ModelManager.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Susp.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\VehicleControlApps.h \
	 User.h \
		 Global.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h

User.o86: User.c \
	 VirEnv_Wrapper.h \
	 Global.h \
	 ioconf.h \
	 dsio.h \
	 CarMaker.h \
		 system.h \
		 infoc.h \
		 apo.h \
		 TextUtils.h \
		 EnvUtils.h \
		 MathUtils.h \
			 Log.h \
		 CMThread.h \
		 Log.h \
		 InfoUtils.h \
			 infoc.h \
		 DataDict.h \
		 DataStorage.h \
		 Scratchpad.h \
		 QuantAudit.h \
		 SessionCmds.h \
		 DirectVarAccess.h \
			 DataDict.h \
		 ExternalInputs.h \
		 MData.h \
		 spline.h \
		 LinMap.h \
		 SimCore.h \
		 Periodic.h \
			 SimCore.h \
		 Vehicle.h \
			 SimCore.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 road.h \
					 roadtypedefs.h \
					 roadspline.h \
					 roadrefline.h \
					 roadsegment.h \
					 roadjunction.h \
					 roadpath.h \
				 GCS.h \
		 Environment.h \
			 GCS.h \
		 GCS.h \
		 DrivMan.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 VehicleControl.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 Traffic.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 # road.h \
				 # GCS.h \
		 TrafficLight.h \
	 c:\IPG\carmaker\win64-11.0.1\include\Car\Vehicle_Car.h \
		 ModelManager.h \
			 DataDict.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Aero.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Steering.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake.h \
			 SimCore.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake_HydESP.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\PowerTrain.h \
			 DrivMan.h \
				 # Vehicle.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\PowerFlow.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Tire.h \
			 ModelManager.h \
				 # DataDict.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Trailer.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Trailer_Brake.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Car\Brake.h \
				 # SimCore.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Car.h \
			 GCS.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 # road.h \
				 # GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Car\Susp.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Car\Susp.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\VehicleControlApps.h \
	 ADASRP.h \
	 MatSupp.h \
		 infoc.h \
		 DataDict.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
	 rbs.h \
		 DataDict.h \
		 flex_rbs.h \
			 DataDict.h \
			 rbs.h \
				 # DataDict.h \
				 # flex_rbs.h \
				 can_rbs.h \
					 DataDict.h \
					 rbs.h \
						 # DataDict.h \
						 # flex_rbs.h \
						 # can_rbs.h \
		 # can_rbs.h \
	 IOVec.h \
	 User.h \
		 Global.h \
		 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h

IO.o86: IO.c \
	 Global.h \
	 CarMaker.h \
		 system.h \
		 infoc.h \
		 apo.h \
		 TextUtils.h \
		 EnvUtils.h \
		 MathUtils.h \
			 Log.h \
		 CMThread.h \
		 Log.h \
		 InfoUtils.h \
			 infoc.h \
		 DataDict.h \
		 DataStorage.h \
		 Scratchpad.h \
		 QuantAudit.h \
		 SessionCmds.h \
		 DirectVarAccess.h \
			 DataDict.h \
		 ExternalInputs.h \
		 MData.h \
		 spline.h \
		 LinMap.h \
		 SimCore.h \
		 Periodic.h \
			 SimCore.h \
		 Vehicle.h \
			 SimCore.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 road.h \
					 roadtypedefs.h \
					 roadspline.h \
					 roadrefline.h \
					 roadsegment.h \
					 roadjunction.h \
					 roadpath.h \
				 GCS.h \
		 Environment.h \
			 GCS.h \
		 GCS.h \
		 DrivMan.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 VehicleControl.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 Traffic.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 # road.h \
				 # GCS.h \
		 TrafficLight.h \
	 dsio.h \
	 ioconf.h \
	 can_interface.h \
	 FailSafeTester.h \
	 flex.h \
	 rbs.h \
		 DataDict.h \
		 flex_rbs.h \
			 DataDict.h \
			 rbs.h \
				 # DataDict.h \
				 # flex_rbs.h \
				 can_rbs.h \
					 DataDict.h \
					 rbs.h \
						 # DataDict.h \
						 # flex_rbs.h \
						 # can_rbs.h \
		 # can_rbs.h \
	 CM_XCP.h \
	 CM_CCP.h \
	 IOVec.h

app_tmp.o86: app_tmp.c \
	 Global.h \
	 infoc.h \
	 CarMaker.h \
		 system.h \
		 infoc.h \
		 apo.h \
		 TextUtils.h \
		 EnvUtils.h \
		 MathUtils.h \
			 Log.h \
		 CMThread.h \
		 Log.h \
		 InfoUtils.h \
			 infoc.h \
		 DataDict.h \
		 DataStorage.h \
		 Scratchpad.h \
		 QuantAudit.h \
		 SessionCmds.h \
		 DirectVarAccess.h \
			 DataDict.h \
		 ExternalInputs.h \
		 MData.h \
		 spline.h \
		 LinMap.h \
		 SimCore.h \
		 Periodic.h \
			 SimCore.h \
		 Vehicle.h \
			 SimCore.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 road.h \
					 roadtypedefs.h \
					 roadspline.h \
					 roadrefline.h \
					 roadsegment.h \
					 roadjunction.h \
					 roadpath.h \
				 GCS.h \
		 Environment.h \
			 GCS.h \
		 GCS.h \
		 DrivMan.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 VehicleControl.h \
			 Vehicle.h \
				 # SimCore.h \
				 # GCS.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\MBSUtils.h \
				 # c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
		 Traffic.h \
			 GCS.h \
			 c:\IPG\carmaker\win64-11.0.1\include\Vehicle\Sensor_Road.h \
				 # road.h \
				 # GCS.h \
		 TrafficLight.h \
	 ipgdriver.h \
	 road.h \
		 # roadtypedefs.h \
		 # roadspline.h \
		 # roadrefline.h \
		 # roadsegment.h \
		 # roadjunction.h \
		 # roadpath.h
