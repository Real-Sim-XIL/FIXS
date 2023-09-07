##############################################################################
### File                  : Configuration.mk
### Compatibility         : DSMAKE 6.26
### Description           : Configuration makefile.
###
### Copyright 2019, dSPACE GmbH. All rights reserved.
###
##############################################################################

# ============================================================================
# NEVER CHANGE THIS LINE:
IMPLEMENTATION_MAKEFILE_TEMPLATE_VERSION = 22
# ============================================================================

# -----------------------------------------------------------------------------
# Make predefinitions.
# -----------------------------------------------------------------------------

%if '$(MAKEVERSION)' < '612'
% abort ERROR: Makefile "$(MAKEFILE)" requires at least DSMAKE Version 6.12.
%endif

.REJECT_RULES

.NOLIBRARY :

.OPTIONS : MacroQuoted
.OPTIONS : TargetQuoted
.OPTIONS : TargetKeepCase

WS_APPEND_SEMIKOLON     = W+_WS_OUTER_+, =+_WS_INNER_+,+_WS_OUTER_+= ,Q-,>;,+_WS_INNER_+= ,Q+
WS_PREPEND_STR          = W+_WS_OUTER_+, =+_WS_INNER_+,+_WS_OUTER_+= ,Q-,<$(WS_STR),+_WS_INNER_+= ,Q+
WS_PREPEND_CC_INCL_OPT  = W+_WS_OUTER_+, =+_WS_INNER_+,+_WS_OUTER_+= ,Q-,<$(CFG_CC_INCL_OPT),+_WS_INNER_+= ,Q+
WS_PREPEND_CPP_INCL_OPT = W+_WS_OUTER_+, =+_WS_INNER_+,+_WS_OUTER_+= ,Q-,<$(CFG_CPP_INCL_OPT),+_WS_INNER_+= ,Q+

WS_PACK_TARGET_ARGS   = "=+_WS_DBL_QUOTE_+
WS_UNPACK_TARGET_ARGS = +_WS_DBL_QUOTE_+="

# Use as: MACRO_WITHOUT_QUOTES = $(MACRO_WITH_QUOTES,$(WS_DEQUOTE))
WS_DEQUOTE = "=

NEWLINE  = X X
NEWLINE := $(NEWLINE,W\n,X=)

# -----------------------------------------------------------------------------
# Work directory of makefile.
# -----------------------------------------------------------------------------

WORK_DIRPATH = "$(MAKEDIR)"

# -----------------------------------------------------------------------------
# Configuration section.
# -----------------------------------------------------------------------------

# MAKE_CFG_SECTION_BEGIN

DSPACE_ROOT                     = C:\Program Files\dSPACE RCPHIL 2021-B

DSPACE_CONFIG                   = C:\ProgramData\dSPACE\f3a006af-1ea4-494a-8f4a-004575529a70

DSPACE_DECRYPTED_ROOT           = C:\ProgramData\dSPACE\f3a006af-1ea4-494a-8f4a-004575529a70

TMK_CFG_TARGET_APPL_TYPE        = SimulinkModelImplementation

TMK_CFG_TARGET_PLATFORM_TYPE    = SCALEXIO_LNX

TMK_CFG_MAKE_INCLUDE_CONTEXTS   =  RS_DS_CM11_SimulinkRS Main

TMK_CFG_HIDE_GLOBAL_SYMBOLS     = 0

CFG_IN_BOARD_ARCHITECTURE       = 32

# Kept global symbols lists
TMK_CFG_RS_DS_CM11_SimulinkRS_KEPT_GLOBAL_SYMBOLS_FILEPATH = C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_SimulinkRS\Build\RS_DS_CM11_SimulinkRS\Compile\RS_DS_CM11_SimulinkRS_gs.txt


# Main targets
TMK_CFG_RS_DS_CM11_SimulinkRS_MAIN_TARGETS = RS_DS_CM11_SimulinkRS


# Included makefiles
TMK_CFG_RS_DS_CM11_SimulinkRS_MAKE_INCLUDE_FILEPATHS =  \
 "C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_SimulinkRS\Build\RS_DS_CM11_SimulinkRS\Compile\RS_DS_CM11_SimulinkRS\RS_DS_CM11_SimulinkRS_lib.mk"
TMK_CFG_Main_MAKE_INCLUDE_FILEPATHS =  \
 "C:\CM_Projects\RealSimCm11Prj\ConfigDesk\RS_DS_CM11_SimulinkRS\Build\RS_DS_CM11_SimulinkRS\Compile\RS_DS_CM11_SimulinkRS_ext.mk"


TMK_CFG_ABORT_MUTEX_NAME        = CfgDeskAbortMutex

TMK_MAX_NUMBER_OF_PARALLEL_JOBS = -1

TMK_PARALLEL_JOBS_EXEC_PATH     = C:\Program Files\dSPACE RCPHIL 2021-B\ConfigurationDesk\Implementation\bin\

PARCOMP_EXEC = "$(TMK_PARALLEL_JOBS_EXEC_PATH)\DsParallelJobs.exe"

# MAKE_CFG_SECTION_END

# -----------------------------------------------------------------------------
# Command to execute shell
# -----------------------------------------------------------------------------

EXEC_SHELL = -- call
FIND = %SystemRoot%\System32\find.exe /I

# -----------------------------------------------------------------------------
# Source directories.
# -----------------------------------------------------------------------------

CC_SRC_GLOBAL_DIRPATHS = \
 $(CFG_BOARD_DRIVER_DIRPATH) \
 $(CFG_BOARD_RTOS_DIRPATH)

# -----------------------------------------------------------------------------
# Header directories.
# -----------------------------------------------------------------------------

CC_HDR_GLOBAL_DIRPATHS = \
 $(CFG_BOARD_RTOS_SRC_HDR_DIRPATHS) \
 $(CFG_COMPILER_HDR_DIRPATHS)

MKMF_HDR_DIRPATHS = \
 $(ITF_CUSTOM_DIRPATHS) \
 $(ITF_CC_HDR_DIRPATHS) \
 $(CFG_BOARD_RTOS_SRC_HDR_DIRPATHS)

# -----------------------------------------------------------------------------
# File extensions.
# -----------------------------------------------------------------------------

CC_SRC_EXT          = $(CFG_C_FILE_EXT)
CPP_SRC_EXT         = $(CFG_CPP_FILE_EXT)

CC_HDR_EXT          = h

CC_OBJ_EXT          = $(CFG_OBJ_FILE_EXT)
CPP_OBJ_EXT         = $(CFG_OBJ_FILE_EXT_CPP)

MRK_EXT             = mrk

# -----------------------------------------------------------------------------
# Data file with result files information for outside processes.
# -----------------------------------------------------------------------------

DAT_FILE    = $(OUTPUT_FILE_NAME).dat

# -----------------------------------------------------------------------------
# CC Compile options
# -----------------------------------------------------------------------------

CC_ALL_OPTS = \
 $(CFG_CC_CTRL_OPTS) \
 $(CFG_CC_CODEFORMAT_OPTS) \
 $(CFG_CC_PREPROC_OPTS) \
 $(CFG_CC_MSG_OPTS) \
 $(CFG_CC_FLT_PT_OPT) \
 $(CC_OPT_OPTS) \
 $(ITF_CC_ADD_OPTS)

# -----------------------------------------------------------------------------
# C++ Compile options
# -----------------------------------------------------------------------------

CPP_ALL_OPTS = \
 $(CFG_CPP_CTRL_OPTS) \
 $(CFG_CPP_CODEFORMAT_OPTS) \
 $(CFG_CPP_PREPROC_OPTS) \
 $(CFG_CPP_MSG_OPTS) \
 $(CFG_CPP_FLT_PT_OPT) \
 $(CPP_OPT_OPTS) \
 $(ITF_CPP_ADD_OPTS)

# -----------------------------------------------------------------------------
# Commando lines.
# -----------------------------------------------------------------------------

CC_CMD  = "$(CFG_CC,$(WS_DEQUOTE))"  $(CC_ALL_OPTS)  -o "$(OUT_OBJ_FILE,A,$(WS_DEQUOTE))" $(IN_SOURCE_FILE,F)
CPP_CMD = "$(CFG_CPP,$(WS_DEQUOTE))"  $(CPP_ALL_OPTS) -o "$(OUT_OBJ_FILE,A,$(WS_DEQUOTE))" $(IN_SOURCE_FILE,F)

# -----------------------------------------------------------------------------
# Include RTLib and compiler configuration.
# -----------------------------------------------------------------------------

# CFG_IN_*: Input parameter for DsBuildConfiguration.mk.
CFG_IN_APPL_NAME    = $(OUTPUT_FILE_NAME)
CFG_IN_BOARD_RTOS   = Linux
APPL_IMAGE_FILE     = $(OUTPUT_FILE_NAME).$(CFG_BOARD_APPL_IMAGE_EXT)

%set PLATFORM_CFG_INVALID = 1
%if member('$(TMK_CFG_TARGET_APPL_TYPE)', 'VEcuImplementation' 'SimulinkModelImplementation' 'FmuImplementation' 'BscImplementation')
%   if '$(TMK_CFG_TARGET_APPL_TYPE)' == 'VEcuImplementation'
%      set CFG_IN_VET_APPLICATION = true
%   endif
%   undef PLATFORM_CFG_INVALID
%endif

%ifdef PLATFORM_CFG_INVALID
%   echo Error in makefile "$(MAKEFILE)": Unsupported combination of settings for target application type "$(TMK_CFG_TARGET_APPL_TYPE)" and target platform type "$(TMK_CFG_TARGET_PLATFORM_TYPE)".
%endif

%set TMK_CFG_BUILDCONFIG_INCLUDE = "$(DSPACE_ROOT)\SCALEXIO\DsBuildConfiguration.mk"

%if exists ($(TMK_CFG_BUILDCONFIG_INCLUDE))
  # Turn off optimization for unused global symbols.
% set DS_NO_SUBSET_LIB = 1

% include $(TMK_CFG_BUILDCONFIG_INCLUDE)
%else
% echo Error in makefile "$(MAKEFILE)"
% echo Additional build configuration makefile $(TMK_CFG_BUILDCONFIG_INCLUDE) does not exist
% abort 1
%endif
