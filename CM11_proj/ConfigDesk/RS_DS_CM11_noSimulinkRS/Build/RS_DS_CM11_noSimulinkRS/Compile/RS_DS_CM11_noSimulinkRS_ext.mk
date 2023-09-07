################################################################################
### Template file         : CoreMakeManagement_ext.tmk
### File                  : RS_DS_CM11_noSimulinkRS_ext.mk
### Compatibility         : DSMAKE 6.12
### Description           : Application specific makefile.
###
### Copyright 2015, dSPACE GmbH. ALL rights reserved.
###
################################################################################

#-------------------------------------------------------------------------------
# APPL Specific Settings
#-------------------------------------------------------------------------------

OUTPUT_FILE_NAME              = RS_DS_CM11_noSimulinkRS
OUTPUT_FILE_TYPE              = APPLICATION

# Application variants.
APPL_VARS                          =

# Variant: DEFAULT
APPL_VARS                         += DEFAULT
APPL_VAR_NAME_DEFAULT              = RS_DS_CM11_noSimulinkRS_DEFAULT
APPL_VAR_OPT_Linkage_DEFAULT       = DEFAULT
APPL_VAR_RESULT_DIRPATH_DEFAULT    = "C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_noSimulinkRS\Build\Results.tmp"



#-------------------------------------------------------------------------------
# Interface Settings
#-------------------------------------------------------------------------------

INTERFACE_CC_SRC_FILES        += \
 mdladapter_mdlfunctions.cpp \
 mdlCode_ap.cpp \
 data_in_ap.cpp \
 data_out_ap.cpp \
 fm_entry_ap.cpp \
 fm_exit_ap.cpp \
 dssimengine_accesspoint.cpp \
 dssimengine_api.cpp \
 dssimengine_main.cpp \
 rtosal_task.cpp \
 rtosal_taskap.cpp \
 rtosal_simengineap.cpp \
 IOCode_Data.cpp

INTERFACE_CC_SRC_DIRPATHS     += \
 ".\..\SysIntCode" \
 ".\..\..\..\..\..\..\..\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src"

INTERFACE_CC_HDR_DIRPATHS     += \
 ".\..\SysIntCode" \
 ".\..\..\..\..\..\..\..\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\EmbeddedSW\Src"

INTERFACE_LIB_DIRPATHS        +=

INTERFACE_CUSTOM_DIRPATHS     += \
 ".\..\..\..\..\..\..\..\IPG\carmaker\win64-11.0.1\include" \
 ".\..\..\..\..\..\src_cm4sl" \
 ".\..\..\..\..\..\include"

INTERFACE_OBJ_FILEPATHS       +=

INTERFACE_LIB_FILEPATHS       +=

INTERFACE_LIB_FILES           += \
 "dscandrv" \
 "dslindrv" \
 "dsethernetinterfacemanagementdrv" \
 "RealSimDsLib_2021b"

INTERFACE_CC_ADD_OPTS         += -include ipgrt.h

INTERFACE_CPP_ADD_OPTS        += -include ipgrt.h

INTERFACE_CC_ADD_DEFINES      += RS_CAVE RS_DEBUG RS_DSPACE DSPACE DSRTLX _DSRTLX CM_HIL CM_NUMVER=110001 CM4SLDS DATA_PORT_ACCESS_POINT_API_VERSION=2

INTERFACE_CC_ADD_UNDEFS       +=

INTERFACE_CC_OPT_SET          = Default

INTERFACE_CC_CUSTOM_OPT_OPTS  = 


# Clear Macros
# Don't use $() here!
INTERFACE_CLEAR_MACROS        +=


#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

dssimengine_accesspoint.cpp : dssimengine_accesspoint.h

dssimengine_api.cpp : dssimengine_api.h

dssimengine_main.cpp : dssimengine_main.h

dssimengine_api.cpp : dssimengine_main.h

dssimengine_accesspoint.cpp : dssimengine_defines.h




# [EOF]
