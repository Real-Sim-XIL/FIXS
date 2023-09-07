CC_OPT_SET_SUPPORTED_VALUES = DEFAULT NONE CUSTOM 
INCLUDE_MAKEFILE_VERSION    = 1 
SRCS= mdladapter_mdlfunctions.cpp mdlCode_ap.cpp data_in_ap.cpp data_out_ap.cpp fm_entry_ap.cpp fm_exit_ap.cpp \
		  dssimengine_accesspoint.cpp dssimengine_api.cpp dssimengine_main.cpp rtosal_task.cpp rtosal_taskap.cpp \
		  rtosal_simengineap.cpp IOCode_Data.cpp
MRK_FILES                   =  mdladapter_mdlfunctions.mrk mdlCode_ap.mrk data_in_ap.mrk data_out_ap.mrk fm_entry_ap.mrk fm_exit_ap.mrk dssimengine_accesspoint.mrk dssimengine_api.mrk dssimengine_main.mrk rtosal_task.mrk rtosal_taskap.mrk rtosal_simengineap.mrk IOCode_Data.mrk 
OBJS= mdladapter_mdlfunctions.o86 mdlCode_ap.o86 data_in_ap.o86 data_out_ap.o86 fm_entry_ap.o86 fm_exit_ap.o86 \
		  dssimengine_accesspoint.o86 dssimengine_api.o86 dssimengine_main.o86 rtosal_task.o86 rtosal_taskap.o86 \
		  rtosal_simengineap.o86 IOCode_Data.o86
CC_SRC_DIRPATHS = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" 
 
CC_HDR_DIRPATHS = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" 
 
LIB_DIRPATHS                =    
CUSTOM_DIRPATHS             =  C:\IPG\carmaker\win64-11.0.1\include C:\CM_Projects\RealSimCm11Prj\src_cm4sl C:\CM_Projects\RealSimCm11Prj\include 
CC_ADD_OPTS                 =   -include ipgrt.h 
CPP_ADD_OPTS                =   -include ipgrt.h 
CC_ADD_DEFINES              = RS_CAVE RS_DEBUG RS_DSPACE DSPACE DSRTLX _DSRTLX CM_HIL CM_NUMVER=110001 CM4SLDS DATA_PORT_ACCESS_POINT_API_VERSION=2 
CC_ADD_UNDEFS               =  
CC_OPT_SET                  = Default 
CC_CUSTOM_OPT_OPTS          =  

all : MakeCheckMarker_All MakeCompile 

depend : 

%include Configuration.mk 
%include CommonRules.mk 

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
 -IC:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 -I"C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" \
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
 -DDATA_PORT_ACCESS_POINT_API_VERSION=2 
 
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
 -IC:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 -I"C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" \
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
 -DDATA_PORT_ACCESS_POINT_API_VERSION=2 
 
# ============================================================================= 
# MKMF Section 
# ============================================================================= 

# ----------------------------------------------------------------------------- 
# MKMF Configuration 
# ----------------------------------------------------------------------------- 

.TYPE.C   = .c .cpp 
.TYPE.OBJ = .o86 .o86 

.PATH.c = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode; \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src"; \
 C:\IPG\carmaker\win64-11.0.1\include; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; \
 C:\CM_Projects\RealSimCm11Prj\include; 
 
.PATH.cpp = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode; \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src"; \
 C:\IPG\carmaker\win64-11.0.1\include; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; \
 C:\CM_Projects\RealSimCm11Prj\include; 
 
.PATH.h = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode; \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src"; \
 C:\IPG\carmaker\win64-11.0.1\include; \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl; \
 C:\CM_Projects\RealSimCm11Prj\include; 
 

.HDRPATH.c = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include \
 | \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include 
 
.HDRPATH.cpp = \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include \
 | \
 C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\RS_DS_CM11_noSimulinkRS\SysIntCode \
 "C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src" \
 C:\IPG\carmaker\win64-11.0.1\include \
 C:\CM_Projects\RealSimCm11Prj\src_cm4sl \
 C:\CM_Projects\RealSimCm11Prj\include 
 

.CDEFINES.c  = -D_DSx86_32 -D_DS2510 -DDS_PLATFORM_X86 -DDS_PLATFORM_LE -DDS_PLATFORM_POSIX -DDS_PLATFORM_LINUX -DDS_PLATFORM_DSRTLINUX -DDS_PLATFORM_CN -DDS_PLATFORM_HCN -DDS_PLATFORM_SMART -DDS_PLATFORM_SMARTHIL -DDS_PLATFORM_SMP -D__HOSTNAME__=\DESK0120048\ -D__INTEGRATION__=\5.2.1\ -DDS_RTLIB_VERSION_MAR=5 -DDS_RTLIB_VERSION_MIR=2 -DDS_QNX_VERSION=650 -DQNX_VERSION=6.5.0 -D_FILE_OFFSET_BITS=64 -D_INLINE  
.CDEFINES.cpp = -D__cplusplus -D_DSx86_32 -D_DS2510 -DDS_PLATFORM_X86 -DDS_PLATFORM_LE -DDS_PLATFORM_POSIX -DDS_PLATFORM_LINUX -DDS_PLATFORM_DSRTLINUX -DDS_PLATFORM_CN -DDS_PLATFORM_HCN -DDS_PLATFORM_SMART -DDS_PLATFORM_SMARTHIL -DDS_PLATFORM_SMP -D__HOSTNAME__=\DESK0120048\ -D__INTEGRATION__=\5.2.1\ -DDS_RTLIB_VERSION_MAR=5 -DDS_RTLIB_VERSION_MIR=2 -DDS_QNX_VERSION=650 -DQNX_VERSION=6.5.0 -D_FILE_OFFSET_BITS=64 -D_INLINE  

MKMF_SRCS  =   mdladapter_mdlfunctions.cpp mdlCode_ap.cpp data_in_ap.cpp data_out_ap.cpp fm_entry_ap.cpp fm_exit_ap.cpp dssimengine_accesspoint.cpp dssimengine_api.cpp dssimengine_main.cpp rtosal_task.cpp rtosal_taskap.cpp rtosal_simengineap.cpp IOCode_Data.cpp 
MKMF_LINELEN = 118 
MKMF_TABLEN  = 2 

# ----------------------------------------------------------------------------- 
# MKMF Replacements 
# ----------------------------------------------------------------------------- 

HDRS= data_in_ap.h data_out_ap.h DS_BusDef.h dssimengine_accesspoint.h dssimengine_defines.h fm_entry_ap.h \
		  fm_exit_ap.h IOCode_Data.h mdlCode_ap.h rtosal_simengineap.h rtosal_task.h rtosal_taskap.h dssimengine_api.h \
		  dssimengine_main.h rtosal_msg.h

EXTHDRS=

SRCS= mdladapter_mdlfunctions.cpp mdlCode_ap.cpp data_in_ap.cpp data_out_ap.cpp fm_entry_ap.cpp fm_exit_ap.cpp \
		  dssimengine_accesspoint.cpp dssimengine_api.cpp dssimengine_main.cpp rtosal_task.cpp rtosal_taskap.cpp \
		  rtosal_simengineap.cpp IOCode_Data.cpp

# [EOF] 
 
### OPUS MKMF:  Do not remove this line!  Generated dependencies follow.

mdladapter_mdlfunctions.o86: mdladapter_mdlfunctions.cpp \
	 dssimengine_api.h

mdlCode_ap.o86: mdlCode_ap.cpp \
	 IOCode_Data.h \
	 mdlCode_ap.h \
		 DS_BusDef.h

data_in_ap.o86: data_in_ap.cpp \
	 IOCode_Data.h \
	 data_in_ap.h \
		 DS_BusDef.h

data_out_ap.o86: data_out_ap.cpp \
	 IOCode_Data.h \
	 data_out_ap.h \
		 DS_BusDef.h

fm_entry_ap.o86: fm_entry_ap.cpp \
	 IOCode_Data.h \
	 fm_entry_ap.h \
		 DS_BusDef.h

fm_exit_ap.o86: fm_exit_ap.cpp \
	 IOCode_Data.h \
	 fm_exit_ap.h \
		 DS_BusDef.h

dssimengine_accesspoint.o86: dssimengine_accesspoint.cpp \
	 dssimengine_accesspoint.h \
	 rtosal_simengineap.h \
	 rtosal_task.h \
	 dssimengine_api.h \
	 IOCode_Data.h \
	 dssimengine_defines.h

dssimengine_api.o86: dssimengine_api.cpp \
	 dssimengine_main.h \
	 dssimengine_api.h

dssimengine_main.o86: dssimengine_main.cpp \
	 dssimengine_api.h \
	 dssimengine_main.h \
	 dssimengine_accesspoint.h

rtosal_task.o86: rtosal_task.cpp \
	 dssimengine_api.h \
	 rtosal_msg.h \
	 rtosal_taskap.h \
	 IOCode_Data.h \
	 rtosal_task.h

rtosal_taskap.o86: rtosal_taskap.cpp \
	 rtosal_task.h \
	 rtosal_simengineap.h \
	 IOCode_Data.h \
	 rtosal_taskap.h

rtosal_simengineap.o86: rtosal_simengineap.cpp \
	 rtosal_task.h \
	 dssimengine_api.h \
	 rtosal_msg.h \
	 IOCode_Data.h \
	 rtosal_simengineap.h

IOCode_Data.o86: IOCode_Data.cpp \
	 dssimengine_accesspoint.h \
	 IOCode_Data.h
