##############################################################################
### Template file         : dsrt.tmf
### File                  : RS_DS_CM11_noSimulinkRS_useManeuver.mk
### Compatibility         : DSMAKE 6.12
### Description           : Model specific makefile.
###
###
### Copyright 2019, dSPACE GmbH. All rights reserved.
###
##############################################################################

# ============================================================================
# NEVER CHANGE THIS LINE:
DSRT_MAKEFILE_TEMPLATE_VERSION = 11
# ============================================================================

# Version to control the handling of include paths in application process makefiles
%set INTERFACE_INCLUDE_MAKEFILE_VERSION := 2

# -----------------------------------------------------------------------------
# Make predefinitions.
# -----------------------------------------------------------------------------

.REJECT_RULES
.OPTIONS : MacroQuoted
.OPTIONS : TargetQuoted
.OPTIONS : TargetKeepCase

# -----------------------------------------------------------------------------
# Turn off warning for tokens declared as obsolete by MATLAB R2019a
# -----------------------------------------------------------------------------
# include a comment line in the .tmf files that includes
# NO_WARN_LEGACY_TOKENS
# This part shall be removed when the oldest supported MATLAB version is MATLAB R2019a

# -----------------------------------------------------------------------------
# Macros read by RTW.
# -----------------------------------------------------------------------------

# RTW specific environment specifications
# SECTION_DEFAULTS_RTW_BEGIN
MAKECMD                        = dsmake
HOST                           = PC
BUILD                          = no
BUILD_SUCCESS                  = DSRT: MAKE PROCESS SUCCEEDED
BUILD_ERROR                    = [sprintf('DSRT MAKE ERROR: Make process failed for model RS_DS_CM11_noSimulinkRS_useManeuver.')]
SYS_TARGET_FILE                = any
PROJ_MARKER_FILE               = no
VERBOSE_BUILD_OFF_TREATMENT    = PRINT_OUTPUT_ALWAYS
# SECTION_DEFAULTS_RTW_END


# -----------------------------------------------------------------------------
# Tokens expaned by RTW.
# -----------------------------------------------------------------------------

# RTW specific customization macros
# SECTION_CUSTOMIZATION_RTW_BEGIN
DS_PROTECTION                     = OFF
MW_MAKEFILE                       = RS_DS_CM11_noSimulinkRS_useManeuver.mk
MW_BUILDARGS                      =  MAT_FILE=0 COMBINE_OUTPUT_UPDATE_FCNS=0 INCLUDE_MDL_TERMINATE_FCN=1 MULTI_INSTANCE_CODE=0 OPTS="-DTID01EQ=0"
# MW_MATLAB_ROOT                  = "C:\Program Files\MATLAB\R2021b"
# MW_MATLAB_BIN                   = "C:\Program Files\MATLAB\R2021b\bin"
MATLAB_ROOT                       = C:\PROGRA~1\MATLAB\R2021b
MATLAB_BIN_ALT                    = C:\PROGRA~1\MATLAB\R2021b\bin
MW_RELEASE_VERSION                = R2021b
MW_RELEASE_VERSION               := $(MW_RELEASE_VERSION,S'\.'_',W_,LC,S'^r'')
# MW_COMPUTER                     = PCWIN64
MW_MODEL                          = RS_DS_CM11_noSimulinkRS_useManeuver
MW_MODEL_MODULES                  = RS_DS_CM11_noSimulinkRS_useManeuver_data.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.c RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.c CM_Main.c CM_Vehicle.c User.c IO.c app_tmp.c
# MW_MODEL_MODULES_OBJ            = RS_DS_CM11_noSimulinkRS_useManeuver_data.obj RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmdlfcn.obj RS_DS_CM11_noSimulinkRS_useManeuver_dsrtmf.obj RS_DS_CM11_noSimulinkRS_useManeuver_dsrttf.obj RS_DS_CM11_noSimulinkRS_useManeuver_dsrtvdf.obj CM_Main.obj CM_Vehicle.obj User.obj IO.obj app_tmp.obj 
MW_S_FUNCTIONS                    = 
# MW_S_FUNCTIONS_OBJ              = 
MW_S_FUNCTIONS_LIB                = 
# MW_SOLVER                       = 
# MW_SOLVER_OBJ                   = 
MW_NUMST                          = 1
MW_TID01EQ                        = 0
MW_NCSTATES                       = 0
# MW_EXT_MODE                     = 0
MW_MULTITASKING                   = 0
# Support for parallel computing
MASTER_ANCHOR_DIR                 = 
START_DIR                         = C:\CM_Projects\RS_FIXS_Ford\CM11_proj\ConfigDesk\RS_DS_CM11_noSimulinkRS_useManeuver\Components\RS_DS_CM11_noSimulinkRS_useManeuver
MW_TARGET_LANG_EXT                = c
# MW_TGT_FCN_LIB                  = None
# MW_EXTMODE_STATIC_ALLOC         = 0
# MW_TMW_EXTMODE_TESTING          = 0
# MW_EXTMODE_STATIC_ALLOC_SIZE    = 1000000
# MW_EXTMODE_TRANSPORT            = 0
# Shared library for utility code
MW_SHARED_LIB                     = 
MW_SHARED_LIB_SRCS_WILD           = 
MW_SHARED_LIB_SRCS_DIR            = 
MW_SHARED_LIB_OBJS_DIR            = 

MW_RELATIVE_PATH_TO_ANCHOR        = ..

MW_MODELLIB                       = 
MW_MODELREF_LINK_LIBS             = 
MW_MODELREF_TARGET_TYPE           = NONE
MW_MODELREFS                      = 
MW_MODELREF_LINK_RSPFILE          = RS_DS_CM11_noSimulinkRS_useManeuver_ref.rsp
MW_MODELREF_INC_PATH              = 
# SECTION_CUSTOMIZATION_RTW_END

# -----------------------------------------------------------------------------
# Remove repeated items in MW_MODEL_MODULES to avoid linkage problems
# -----------------------------------------------------------------------------
%set TMP_UNIQUE_MODEL_MODULES =
%foreach ITEM in $(MW_MODEL_MODULES)
    # check if current ITEM is already contained in unique list.
    %if !member($(ITEM),$(TMP_UNIQUE_MODEL_MODULES))
        #avoid multiple source inclusions
        %set TMP_UNIQUE_MODEL_MODULES += $(ITEM)
    %endif
%endfor
# overwrite original list with unique one
%set MW_MODEL_MODULES := $(TMP_UNIQUE_MODEL_MODULES)
# remove temporal macro
%undef TMP_UNIQUE_MODEL_MODULES

# -----------------------------------------------------------------------------
# DSRT target specific configuration.
# -----------------------------------------------------------------------------

# In some dsmake versions (up to 6.26) the ,A modifier (abs. path) leaves behind a trailing \ character at the end.
# The S'\\$$'' removes such trailing \ character if it exists.
%set TMP_CHECK_START_DIR := $(INTERFACE_IN_INCLUDE_FILEPATH,D,>\$(MW_RELATIVE_PATH_TO_ANCHOR),A,S'\\$$'',$(WS_DEQUOTE))
%set TMP_CHECK_MATLAB_ROOT := "$(TMP_CHECK_START_DIR)\ml"
%if !(exists("$(TMP_CHECK_MATLAB_ROOT,A,$(WS_DEQUOTE))"))
   %set USE_LOCAL_SOURCES = no
%else
   %set USE_LOCAL_SOURCES = yes
%endif

%if '$(USE_LOCAL_SOURCES)' == 'yes'
%  set START_DIR := $(INTERFACE_IN_INCLUDE_FILEPATH,D,>\$(MW_RELATIVE_PATH_TO_ANCHOR),A,S'\\$$'',$(WS_DEQUOTE))
%  set MATLAB_ROOT := $(START_DIR)\ml
%endif

MODEL_MAKEFILE_BUILD_DIRPATH  = $(INTERFACE_IN_INCLUDE_FILEPATH,D)
MODEL_MAKEFILE_WORK_DIRPATH   = $(MODEL_MAKEFILE_BUILD_DIRPATH, >\$(MW_RELATIVE_PATH_TO_ANCHOR),A)

%ifdef CFG_VERSION_POSTFIX
% if $(CFG_VERSION_POSTFIX) != 1 
    QNX_VERSION_POSTFIX = _$(CFG_VERSION_POSTFIX)
% endif
%endif

DSRT_TARGET = $(SYS_TARGET_FILE,B)

WS_ESC = \\=\\\
WS_WILD = .=\.

# -----------------------------------------------------------------------------
# Model referencing settings.
# -----------------------------------------------------------------------------

# Evaluate Target Type for Model Referencing.

%if     '$(MW_MODELREF_TARGET_TYPE)' == 'NONE'
   # Root Model or no Model Referencing
%  set   _DSRT_IS_ROOT_MODEL = 1
%elseif '$(MW_MODELREF_TARGET_TYPE)' == 'RTW'
   # Referenced Model for Model Referencing
%  undef _DSRT_IS_ROOT_MODEL
%endif

%if !null(MW_MODELREFS)
   # This model includes other models/model libraries.
%  set   _DSRT_HAS_MODELREFS = 1
%else
   # No models referenced by this model.
%  undef _DSRT_HAS_MODELREFS
%endif

# -----------------------------------------------------------------------------
# Common search directories.
# -----------------------------------------------------------------------------

%set DSRT_COMMON_HDR_DIRPATHS := $(MODEL_MAKEFILE_BUILD_DIRPATH)
%set DSRT_COMMON_HDR_DIRPATHS += $(MODEL_MAKEFILE_WORK_DIRPATH,D)
%set DSRT_COMMON_HDR_DIRPATHS += "$(MATLAB_ROOT)\simulink\include"
%set DSRT_COMMON_HDR_DIRPATHS += "$(MATLAB_ROOT)\extern\include"
%set DSRT_COMMON_HDR_DIRPATHS += "$(MATLAB_ROOT)\rtw\c\src"
%set DSRT_COMMON_HDR_DIRPATHS += "$(MATLAB_ROOT)\rtw\c\src\ext_mode\common"
%set DSRT_COMMON_HDR_DIRPATHS += \
    "$(START_DIR)" \
    "$(START_DIR)\RS_DS_CM11_noSimulinkRS_useManeuver_dsrt" \
    "C:\CM_Projects\RS_FIXS_Ford\CM11_proj\src_cm4sl_ds" \
    "$(MATLAB_ROOT)\extern\include" \
    "$(MATLAB_ROOT)\simulink\include" \
    "$(MATLAB_ROOT)\rtw\c\src" \
    "$(MATLAB_ROOT)\rtw\c\src\ext_mode\common" \
    "C:\CM_Projects\RS_FIXS_Ford\CM11_proj\include" \
    "C:\IPG\carmaker\win64-11.0.1\include" \
    "C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b"

%if !null(MW_MODELREF_INC_PATH) 
%set DSRT_COMMON_HDR_DIRPATHS    += $(MODEL_MAKEFILE_BUILD_DIRPATH,>\$(MW_MODELREF_INC_PATH))
%elseif dir($(MODEL_MAKEFILE_BUILD_DIRPATH,>\referenced_model_includes))
# in MATLAB R2019a the MW_MODELREF_INC_PATH macro is always empty
# Temporal Fix/Hack for MATLAB 2019a It is assumed that the folder with all referenced model header is called \referenced_model_includes
%set DSRT_COMMON_HDR_DIRPATHS    += $(MODEL_MAKEFILE_BUILD_DIRPATH,>\referenced_model_includes)
%endif

%if !null(MW_SHARED_LIB_SRCS_DIR) 
%if !(%exists("$(MW_SHARED_LIB_SRCS_DIR,$(WS_DEQUOTE))"))
%set DSRT_COMMON_HDR_DIRPATHS    += $(MODEL_MAKEFILE_BUILD_DIRPATH,>\$(MW_SHARED_LIB_SRCS_DIR))
%else
%set DSRT_COMMON_HDR_DIRPATHS    += "$(MW_SHARED_LIB_SRCS_DIR,$(WS_DEQUOTE))"
%endif
%endif

%set DSRT_COMMON_SRC_DIRPATHS := $(DSRT_COMMON_HDR_DIRPATHS)
%set DSRT_COMMON_SRC_DIRPATHS += \
    "$(MATLAB_ROOT)\rtw\c\src" \
    "$(MATLAB_ROOT)\simulink\src" \
    "C:\CM_Projects\RS_FIXS_Ford\CM11_proj\src_cm4sl_ds"

%if '$(USE_LOCAL_SOURCES)' == 'yes'
%  set DSRT_COMMON_HDR_DIRPATHS += "$(START_DIR)\otherFiles"
%  set DSRT_COMMON_HDR_DIRPATHS += "$(START_DIR)\PI\misc"
%  set DSRT_COMMON_SRC_DIRPATHS += "$(START_DIR)\otherFiles"
%  set DSRT_COMMON_SRC_DIRPATHS += "$(START_DIR)\PI\misc"
%  set TMP_START_DIR = $(START_DIR,$(WS_ESC))
%  set TMP_START_DIR = $(TMP_START_DIR,$(WS_WILD))
%  set DSRT_COMMON_HDR_DIRPATHS = $(DSRT_COMMON_HDR_DIRPATHS,M^$(TMP_START_DIR))
%  set DSRT_COMMON_SRC_DIRPATHS = $(DSRT_COMMON_SRC_DIRPATHS,M^$(TMP_START_DIR))
%endif

# -----------------------------------------------------------------------------
# Common compiler defines.
# -----------------------------------------------------------------------------

TMP_INTERFACE_CC_ADD_DEFINES    = USE_RTMODEL
TMP_INTERFACE_CC_ADD_DEFINES   += MODEL=$(MW_MODEL)
TMP_INTERFACE_CC_ADD_DEFINES   += MT=$(MW_MULTITASKING)
TMP_INTERFACE_CC_ADD_DEFINES   += RT
TMP_INTERFACE_CC_ADD_DEFINES   += NUMST=$(MW_NUMST)
TMP_INTERFACE_CC_ADD_DEFINES   += NCSTATES=$(MW_NCSTATES)
TMP_INTERFACE_CC_ADD_DEFINES   += NO_PRINTF
TMP_INTERFACE_CC_ADD_DEFINES   += TID01EQ=$(MW_TID01EQ)

# -----------------------------------------------------------------------------
# Shared library for utility code
# -----------------------------------------------------------------------------
%if !null(_DSRT_IS_ROOT_MODEL)
%if !null(MW_SHARED_LIB_SRCS_DIR)
%if !null(MW_SHARED_LIB)
%set MW_SHARED_LIB_BASENAME := $(MW_SHARED_LIB,B)

# MMPAP: Rename shared library "basename" if already present in INTERFACE_IN_LIB_TARGETS
%foreach ITEM in $(INTERFACE_IN_LIB_TARGETS)
%    if member($(MW_SHARED_LIB_BASENAME), $(INTERFACE_IN_LIB_TARGETS))
%        set MW_SHARED_LIB_BASENAME = $(MW_SHARED_LIB_BASENAME)_1
%    else
%        break
%    endif
%endfor

%set MW_SHARED_LIB_FILEPATH =$(MODEL_MAKEFILE_BUILD_DIRPATH,>\$(MW_SHARED_LIB))
# add renamed basename to the INTERFACE_LIB_TARGET list.
%set INTERFACE_LIB_TARGET +=$(MW_SHARED_LIB_BASENAME)
# save shared library in the model folder. Do not use renamed MW_SHARED_LIB_BASENAME, use MW_SHARED_LIB instead.
# Generate macros to build MW_SHARED_LIB_BASENAME library.
%set INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_FILELOCATION     =$(MW_SHARED_LIB_FILEPATH)
%set INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_SRC_FILES     =$(MODEL_MAKEFILE_BUILD_DIRPATH,>\$(MW_SHARED_LIB_SRCS_WILD),A,*F)
%if !(%exists("$(MW_SHARED_LIB_SRCS_DIR,$(WS_DEQUOTE))"))
%   set INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_SRC_DIRPATHS  =$(MODEL_MAKEFILE_BUILD_DIRPATH,>\$(MW_SHARED_LIB_SRCS_DIR),A)
%else
%   set INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_SRC_DIRPATHS  ="$(MW_SHARED_LIB_SRCS_DIR,$(WS_DEQUOTE))"
%endif
%set INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_HDR_DIRPATHS  =$(DSRT_COMMON_HDR_DIRPATHS)
%set INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_ADD_DEFINES   =$(TMP_INTERFACE_CC_ADD_DEFINES)
%endif

%endif
%ifdef DEBUG_ON
% echo DEBUG: INTERFACE_LIB_TARGET $(INTERFACE_LIB_TARGET)
% echo DEBUG: INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_FILELOCATION $(INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_FILELOCATION)
% echo DEBUG: INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_SRC_FILES $(INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_SRC_FILES)
% echo DEBUG: INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_SRC_DIRPATHS $(INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_SRC_DIRPATHS)
% echo DEBUG: INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_HDR_DIRPATHS $(INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_HDR_DIRPATHS)
% echo DEBUG: INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_ADD_DEFINES $(INTERFACE_LIB_$(MW_SHARED_LIB_BASENAME)_CC_ADD_DEFINES)
%endif
%endif # _DSRT_IS_ROOT_MODEL

# -----------------------------------------------------------------------------
# Sources and objects for RTW Custom Run-Time libraries
# -----------------------------------------------------------------------------

%if !null(_DSRT_IS_ROOT_MODEL)

# List of RTW Custom Run-Time Libraries and related sources.


# RTW Custom Run-Time Libraries from rtwmakecfg.m (to be built).


# RTW Custom Run-Time Libraries from rtwmakecfg.m (pre-compiled, locally build if unavailable).


%endif # _DSRT_IS_ROOT_MODEL

# -----------------------------------------------------------------------------
# Interface macros for root/main model.
# -----------------------------------------------------------------------------
%if !null(_DSRT_IS_ROOT_MODEL)

%set LIB_BASENAME = RS_DS_CM11_noSimulinkRS_useManeuver

% foreach ITEM in $(INTERFACE_IN_LIB_TARGETS)
%    if member($(LIB_BASENAME), $(INTERFACE_IN_LIB_TARGETS))
%        set LIB_BASENAME = $(LIB_BASENAME)_1
%    else
%        break
%    endif
% endfor

% set INTERFACE_LIB_TARGET += $(LIB_BASENAME)
% set INTERFACE_LIB_MAIN_TARGET = $(LIB_BASENAME)
% set INTERFACE_IMPLEMENTATION_TYPE = SimulinkImplementation
% set INTERFACE_LIB_$(LIB_BASENAME)_FILELOCATION = $(MODEL_MAKEFILE_BUILD_DIRPATH,>\$(LIB_BASENAME).$(CFG_LIB_FILE_EXT))

# Search path for header files and include path.
INTERFACE_LIB_$(LIB_BASENAME)_CC_HDR_DIRPATHS := $(DSRT_COMMON_HDR_DIRPATHS)
INTERFACE_CC_HDR_DIRPATHS := $(DSRT_COMMON_HDR_DIRPATHS)

# Search path for source files.
INTERFACE_LIB_$(LIB_BASENAME)_CC_SRC_DIRPATHS := $(DSRT_COMMON_SRC_DIRPATHS)

# Interface macros for integration with ConfigurationDesk.
INTERFACE_LIB_$(LIB_BASENAME)_CC_SRC_FILES := RS_DS_CM11_noSimulinkRS_useManeuver.$(MW_TARGET_LANG_EXT) 
INTERFACE_LIB_$(LIB_BASENAME)_CC_SRC_FILES += $(MW_MODEL_MODULES)
INTERFACE_LIB_$(LIB_BASENAME)_CC_SRC_FILES += $(MW_S_FUNCTIONS)

INTERFACE_LIB_$(LIB_BASENAME)_CC_ADD_DEFINES := $(TMP_INTERFACE_CC_ADD_DEFINES)

INTERFACE_OBJ_FILEPATHS +=


%endif # _DSRT_IS_ROOT_MODEL

# -----------------------------------------------------------------------------
# Interface macros for library targets of referenced models.
# -----------------------------------------------------------------------------

%if null(_DSRT_IS_ROOT_MODEL)

% set LIB_BASENAME = $(MW_MODELLIB,B)

% foreach ITEM in $(INTERFACE_IN_LIB_TARGETS)
%    if member($(LIB_BASENAME), $(INTERFACE_IN_LIB_TARGETS))
%        set LIB_BASENAME = $(LIB_BASENAME)_1
%    else
%        break
%    endif
% endfor

# Library targets of referenced models.
% set INTERFACE_LIB_TARGET                 += $(LIB_BASENAME)

% set TMP_REF_MODEL_BUILD_DIR := $(INTERFACE_IN_INCLUDE_FILEPATH,D)

% set INTERFACE_LIB_$(LIB_BASENAME)_FILELOCATION     := $(TMP_REF_MODEL_BUILD_DIR,>\$(LIB_BASENAME)$(MW_MODELLIB,E))
% set INTERFACE_LIB_$(LIB_BASENAME)_CC_SRC_FILES     := $(MW_MODEL_MODULES)
% set INTERFACE_LIB_$(LIB_BASENAME)_CC_SRC_FILES     += $(MW_S_FUNCTIONS)
% set INTERFACE_LIB_$(LIB_BASENAME)_CC_SRC_DIRPATHS  := $(DSRT_COMMON_SRC_DIRPATHS)
% set INTERFACE_LIB_$(LIB_BASENAME)_CC_HDR_DIRPATHS  := $(DSRT_COMMON_HDR_DIRPATHS)
% set INTERFACE_LIB_$(LIB_BASENAME)_CC_ADD_DEFINES   := $(TMP_INTERFACE_CC_ADD_DEFINES)

%endif # !_DSRT_IS_ROOT_MODEL

local_start_$(MW_MODEL):
%if '$(DS_PROTECTION)' == 'ON'
    %echo $(BUILD_SUCCESS)
%endif
# [EOF]
