##############################################################################
### File                  : RS_DS_CM11_noSimulinkRS_impl.mk
### Compatibility         : DSMAKE 6.26
### Description           : Implementation makefile.
###
### Copyright 2019, dSPACE GmbH. All rights reserved.
###
##############################################################################

# ============================================================================
# NEVER CHANGE THIS LINE:
IMPLEMENTATION_MAKEFILE_TEMPLATE_VERSION = 23
# ============================================================================

# =============================================================================
# Start rule of the makefile.
# =============================================================================

Start : MakeMain

%include Configuration.mk

INTERFACE_IN_DS_PLATFORM_TYPE        = $(TMK_CFG_TARGET_PLATFORM_TYPE)
INTERFACE_IN_LIB_TARGETS             =

# -----------------------------------------------------------------------------
# Further global settings
# -----------------------------------------------------------------------------

MAKE_SUCCESS_MESSAGE = Making "$(OUTPUT_FILE_NAME)" finished
MAKE_ABORT_MESSAGE   = Error:   Making "$(OUTPUT_FILE_NAME)" failed $(NEWLINE)$(NEWLINE)

# =============================================================================
# Include test makefile
# =============================================================================

MAKE_TEST_HOOKS_FILEPATH = .\make_test_hooks.mk
%if exists ($(MAKE_TEST_HOOKS_FILEPATH))
%       include $(MAKE_TEST_HOOKS_FILEPATH)
%endif

# -----------------------------------------------------------------------------
# Include and process makefile fragments.
# -----------------------------------------------------------------------------

# Macros for input definitions from make includes.
ITF_CC_OPT_SET_SUPPORTED_VALUES = DEFAULT NONE CUSTOM

ITF_CC_SRC_FILES        =
ITF_CC_SRC_DIRPATHS     =
ITF_CC_HDR_DIRPATHS     =
ITF_LIB_DIRPATHS        =
ITF_CUSTOM_FILEPATHS    =
ITF_CC_ADD_OPTS         =
ITF_CPP_ADD_OPTS        =
ITF_CC_ADD_DEFINES      =
ITF_CC_ADD_UNDEFS       =
ITF_CC_OPT_SET          =
ITF_CC_CUSTOM_OPT_OPTS  =
ITF_OBJ_FILEPATHS       =
ITF_LIB_FILEPATHS       =
ITF_LIB_FILES           =
ITF_LIB_MAIN_TARGET     =

%foreach IDX_INCLUDE_CONTEXT in $(TMK_CFG_MAKE_INCLUDE_CONTEXTS)
%foreach IDX_MAKE_INCLUDE_FILE in $(TMK_CFG_$(IDX_INCLUDE_CONTEXT)_MAKE_INCLUDE_FILEPATHS)

    # Predefine interface macros.
    # Note, following commands need to left assigned.
INTERFACE_CC_SRC_FILES             =
INTERFACE_CC_SRC_DIRPATHS          =
INTERFACE_CC_HDR_DIRPATHS          =
INTERFACE_LIB_DIRPATHS             =
INTERFACE_CUSTOM_DIRPATHS          =
INTERFACE_CC_ADD_OPTS              =
INTERFACE_CPP_ADD_OPTS             =
INTERFACE_CC_ADD_DEFINES           =
INTERFACE_CC_ADD_UNDEFS            =
INTERFACE_CC_OPT_SET               =
INTERFACE_CC_CUSTOM_OPT_OPTS       =
INTERFACE_OBJ_FILEPATHS            =
INTERFACE_LIB_FILEPATHS            =
INTERFACE_LIB_FILES                =
INTERFACE_CLEAR_MACROS             =
INTERFACE_IMPLEMENTATION_TYPE      =
INTERFACE_LIB_MAIN_TARGET          =
INTERFACE_LIB_TARGET               =
INTERFACE_INCLUDE_MAKEFILE_VERSION =

    # Include makefile.
    # Macro INTERFACE_IN_INCLUDE_FILEPATH lets the include file know its own absolute location.
    # Can be used to resolve relative path references.
%   set INTERFACE_IN_INCLUDE_FILEPATH = $(IDX_MAKE_INCLUDE_FILE)
%   if file($(IDX_MAKE_INCLUDE_FILE))
      # Include makefile exists.
%     include $(IDX_MAKE_INCLUDE_FILE)
%   else
%     echo Error in makefile "$(MAKEFILE)"
%     echo Include makefile does not exist:
%     echo   $(INTERFACE_IN_INCLUDE_FILEPATH)
%     abort 1
%   endif

    # Evaluate interface macros.
%   set TMP_INTERFACE_CC_SRC_FILES             := $(INTERFACE_CC_SRC_FILES)
%   set TMP_INTERFACE_CC_SRC_DIRPATHS          := $(INTERFACE_CC_SRC_DIRPATHS,A)
%   set TMP_INTERFACE_CC_HDR_DIRPATHS          := $(INTERFACE_CC_HDR_DIRPATHS,A)
%   set TMP_INTERFACE_LIB_DIRPATHS             := $(INTERFACE_LIB_DIRPATHS,A)
%   set TMP_INTERFACE_CUSTOM_DIRPATHS          := $(INTERFACE_CUSTOM_DIRPATHS,A)
%   set TMP_INTERFACE_CC_ADD_OPTS              := $(INTERFACE_CC_ADD_OPTS)
%   set TMP_INTERFACE_CPP_ADD_OPTS             := $(INTERFACE_CPP_ADD_OPTS)
%   set TMP_INTERFACE_CC_ADD_DEFINES           := $(INTERFACE_CC_ADD_DEFINES)
%   set TMP_INTERFACE_CC_ADD_UNDEFS            := $(INTERFACE_CC_ADD_UNDEFS)
%   set TMP_INTERFACE_CC_OPT_SET               := $(INTERFACE_CC_OPT_SET)
%   set TMP_INTERFACE_CC_CUSTOM_OPT_OPTS       := $(INTERFACE_CC_CUSTOM_OPT_OPTS)
%   set TMP_INTERFACE_OBJ_FILEPATHS            := $(INTERFACE_OBJ_FILEPATHS,A)
%   set TMP_INTERFACE_LIB_FILEPATHS            := $(INTERFACE_LIB_FILEPATHS,A)
%   set TMP_INTERFACE_LIB_FILES                := $(INTERFACE_LIB_FILES)
%   set TMP_INTERFACE_CLEAR_MACROS             := $(INTERFACE_CLEAR_MACROS)
%   set TMP_INTERFACE_IMPLEMENTATION_TYPE      := $(INTERFACE_IMPLEMENTATION_TYPE)
%   set TMP_INTERFACE_LIB_MAIN_TARGET          := $(INTERFACE_LIB_MAIN_TARGET)
%   set TMP_INTERFACE_LIB_TARGET               := $(INTERFACE_LIB_TARGET)

%   set TMP_INTERFACE_INCLUDE_MAKEFILE_VERSION := $(INTERFACE_INCLUDE_MAKEFILE_VERSION)

    # Clear interface makros.
%   undef INTERFACE_CC_SRC_FILES
%   undef INTERFACE_CC_SRC_DIRPATHS
%   undef INTERFACE_CC_HDR_DIRPATHS
%   undef INTERFACE_LIB_DIRPATHS
%   undef INTERFACE_CUSTOM_DIRPATHS
%   undef INTERFACE_CC_ADD_OPTS
%   undef INTERFACE_CPP_ADD_OPTS
%   undef INTERFACE_CC_ADD_DEFINES
%   undef INTERFACE_CC_ADD_UNDEFS
%   undef INTERFACE_CC_OPT_SET
%   undef INTERFACE_CC_CUSTOM_OPT_OPTS
%   undef INTERFACE_OBJ_FILEPATHS
%   undef INTERFACE_LIB_FILEPATHS
%   undef INTERFACE_LIB_FILES
%   undef INTERFACE_CLEAR_MACROS
%   undef INTERFACE_LIB_MAIN_TARGET
%   undef INTERFACE_LIB_TARGET
%   undef INTERFACE_INCLUDE_MAKEFILE_VERSION

    # Assign values of include file to internal macros.
%   set ITF_CC_SRC_FILES             += $(TMP_INTERFACE_CC_SRC_FILES)
%   set ITF_CC_SRC_DIRPATHS          += $(TMP_INTERFACE_CC_SRC_DIRPATHS)

%   set ITF_LIB_DIRPATHS             += $(TMP_INTERFACE_LIB_DIRPATHS)
%   set ITF_CUSTOM_DIRPATHS          += $(TMP_INTERFACE_CUSTOM_DIRPATHS)
%   set ITF_CC_ADD_OPTS              += $(TMP_INTERFACE_CC_ADD_OPTS)
%   set ITF_CPP_ADD_OPTS             += $(TMP_INTERFACE_CPP_ADD_OPTS)
%   set ITF_CC_ADD_UNDEFS            += $(TMP_INTERFACE_CC_ADD_UNDEFS)
%   set ITF_CC_ADD_DEFINES           += $(TMP_INTERFACE_CC_ADD_DEFINES)

%   set INTERFACE_IN_LIB_TARGETS     += $(TMP_INTERFACE_LIB_TARGET)

%   if '$(TMP_INTERFACE_LIB_MAIN_TARGET)' != ''
        # BSC main library has higher priority than SIC. Take the BSC main library here
%       if '$(TMP_INTERFACE_IMPLEMENTATION_TYPE)' == 'BscImplementation'
%           set ITF_LIB_MAIN_TARGET = $(TMP_INTERFACE_LIB_MAIN_TARGET)
        # Set the SIC main target only if empty. This could be set by BSC. 
%       elseif '$(ITF_LIB_MAIN_TARGET)' == '' && '$(TMP_INTERFACE_IMPLEMENTATION_TYPE)' == 'SimulinkImplementation'
%           set ITF_LIB_MAIN_TARGET = $(TMP_INTERFACE_LIB_MAIN_TARGET)
%       endif
%   elseif '$(ITF_LIB_MAIN_TARGET)' == ''
%       set ITF_LIB_MAIN_TARGET = $(OUTPUT_FILE_NAME)
%   endif

%   if !null(TMP_INTERFACE_INCLUDE_MAKEFILE_VERSION)
%     if $(TMP_INTERFACE_INCLUDE_MAKEFILE_VERSION) > $(ITF_INCLUDE_MAKEFILE_VERSION)
%       set ITF_INCLUDE_MAKEFILE_VERSION = $(TMP_INTERFACE_INCLUDE_MAKEFILE_VERSION)
%     endif  
%   endif

# NOTE: Macros INTERFACE_CC_OPT_SET and INTERFACE_CC_CUSTOM_OPT_OPTS do NOT sum up. 
#       Only one value can be used. Thus the 'last one wins'.
# NOTE: Consider INTERFACE_CC_CUSTOM_OPT_OPTS only if also INTERFACE_CC_OPT_SET is actually set.
%   if !null(TMP_INTERFACE_CC_OPT_SET)
      # Check for supported values of INTERFACE_CC_OPT_SET:
%     if !member($(TMP_INTERFACE_CC_OPT_SET), $(ITF_CC_OPT_SET_SUPPORTED_VALUES))
%       echo Error in makefile "$(MAKEFILE)".
%       echo Included makefile "$(IDX_MAKE_INCLUDE_FILE,$(WS_DEQUOTE))" specifies:
%       echo INTERFACE_CC_OPT_SET = $(TMP_INTERFACE_CC_OPT_SET)
%       echo However, only values "$(ITF_CC_OPT_SET_SUPPORTED_VALUES)" are supported for INTERFACE_CC_OPT_SET.
%       echo $(MAKE_ABORT_MESSAGE)
%       abort 1
%     endif
%     set ITF_CC_OPT_SET         = $(TMP_INTERFACE_CC_OPT_SET)
%     set ITF_CC_CUSTOM_OPT_OPTS = $(TMP_INTERFACE_CC_CUSTOM_OPT_OPTS)
%   endif

%   if '$(TMK_CFG_HIDE_GLOBAL_SYMBOLS)' == '1' && '$(IDX_INCLUDE_CONTEXT)' != 'Main'
        # Support of Simulink implementation containers older than 2019-B
%       if '$(TMP_INTERFACE_LIB_MAIN_TARGET)' != ''
            # BSC main library has higher priority than SIC. Take the BSC main library here
%           if '$(TMP_INTERFACE_IMPLEMENTATION_TYPE)' == 'BscImplementation'
%               set INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_MAIN_TARGET = $(TMP_INTERFACE_LIB_MAIN_TARGET)
            # Set the SIC main target only if empty. This could be set by BSC. 
%           elseif '$(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_MAIN_TARGET)' == '' && '$(TMP_INTERFACE_IMPLEMENTATION_TYPE)' == 'SimulinkImplementation'
%               set INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_MAIN_TARGET = $(TMP_INTERFACE_LIB_MAIN_TARGET)
%           endif
%       endif
%       set INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_OBJ_FILEPATHS += $(TMP_INTERFACE_OBJ_FILEPATHS)
%       set INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_LIB_FILEPATHS += $(TMP_INTERFACE_LIB_FILEPATHS)
%       foreach IDX_LIB_TARGET in $(TMP_INTERFACE_LIB_TARGET)
%           if !member($(INTERFACE_LIB_$(IDX_LIB_TARGET)_FILELOCATION), $(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_LIB_FILEPATHS))
%               set INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_LIB_FILEPATHS += "$(INTERFACE_LIB_$(IDX_LIB_TARGET)_FILELOCATION,$(WS_DEQUOTE))"
%           endif
%       endfor
%   else
%       set ITF_CC_HDR_DIRPATHS += $(TMP_INTERFACE_CC_HDR_DIRPATHS)
%       set ITF_OBJ_FILEPATHS   += $(TMP_INTERFACE_OBJ_FILEPATHS) $(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_OBJ_FILEPATHS)
%       set ITF_LIB_FILEPATHS   += $(TMP_INTERFACE_LIB_FILEPATHS) $(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_LIB_FILEPATHS)
        # if the library location is present only in INTERFACE_LIB_$(IDX_LIB_TARGET)_FILELOCATION makro (TL-SIC)
%       foreach IDX_LIB_TARGET in $(TMP_INTERFACE_LIB_TARGET)
%           if !member($(INTERFACE_LIB_$(IDX_LIB_TARGET)_FILELOCATION), $(ITF_LIB_FILEPATHS))
%               set ITF_LIB_FILEPATHS += "$(INTERFACE_LIB_$(IDX_LIB_TARGET)_FILELOCATION,$(WS_DEQUOTE))"
%           endif
%       endfor
%   endif

%   set ITF_LIB_FILES += $(TMP_INTERFACE_LIB_FILES)

    # Clear makros listed in INTERFACE_CLEAR_MACROS.
%   if ! %null(TMP_INTERFACE_CLEAR_MACROS)
%       foreach IDX_INTERFACE_MACRO in $(TMP_INTERFACE_CLEAR_MACROS)
%           undef $(IDX_INTERFACE_MACRO)
%       endfor
%   endif

%   undef INTERFACE_IN_INCLUDE_FILEPATH

    # Workaround for SIC with model referencing and custom code
%   if %null(ITF_INCLUDE_MAKEFILE_VERSION) && %null(_DIRPATHS_INCLUDED)
%       set CC_SRC_GLOBAL_DIRPATHS = $(ITF_CUSTOM_DIRPATHS) $(ITF_CC_SRC_DIRPATHS) $(CC_SRC_GLOBAL_DIRPATHS)
%       set CC_HDR_GLOBAL_DIRPATHS = $(ITF_CUSTOM_DIRPATHS) $(ITF_CC_HDR_DIRPATHS) $(CC_HDR_GLOBAL_DIRPATHS)
%       set _DIRPATHS_INCLUDED   = true
%   endif

%endfor # $(MAKE_INCLUDE_FILEPATHS)
%endfor # $(MAKE_INCLUDE_CONTEXTS)

# =============================================================================
# Makefile flags.
# =============================================================================

LINK_ON =


# =============================================================================
# Container library interface makros.
# =============================================================================

%   if "$(OUTPUT_FILE_TYPE)" == "LIBRARY"
%       if !member($(ITF_LIB_MAIN_TARGET), $(INTERFACE_IN_LIB_TARGETS))
%           set INTERFACE_IN_LIB_TARGETS                             += $(ITF_LIB_MAIN_TARGET)
%           set INTERFACE_LIB_$(ITF_LIB_MAIN_TARGET)_FILELOCATION     = $(ITF_LIB_MAIN_TARGET).$(CFG_LIB_FILE_EXT))
%           set INTERFACE_LIB_$(ITF_LIB_MAIN_TARGET)_CC_HDR_DIRPATHS  = $(ITF_CUSTOM_DIRPATHS) $(ITF_CC_HDR_DIRPATHS)
%           set INTERFACE_LIB_$(ITF_LIB_MAIN_TARGET)_CC_SRC_DIRPATHS  = $(ITF_CUSTOM_DIRPATHS) $(ITF_CC_SRC_DIRPATHS)
%           set INTERFACE_LIB_$(ITF_LIB_MAIN_TARGET)_CC_SRC_FILES     = $(ITF_CC_SRC_FILES)
%       endif
%       foreach IDX_MAIN_TARGET in $(TMK_CFG_$(ITF_LIB_MAIN_TARGET)_MAIN_TARGETS) $(ITF_LIB_MAIN_TARGET)
%           set ITF_OBJ_FILEPATHS += $(INTERFACE_LIB_$(IDX_MAIN_TARGET)_CC_SRC_FILES,M\.$(CC_SRC_EXT)$$,S'\.$(CC_SRC_EXT)$$'.$(CC_OBJ_EXT)',F,<$(WORK_DIRPATH,>\$(IDX_MAIN_TARGET))\)
%           set ITF_OBJ_FILEPATHS += $(INTERFACE_LIB_$(IDX_MAIN_TARGET)_CC_SRC_FILES,M\.$(CPP_SRC_EXT)$$,S'\.$(CPP_SRC_EXT)$$'.$(CPP_OBJ_EXT)',F,<$(WORK_DIRPATH,>\$(IDX_MAIN_TARGET))\)
%       endfor
%  endif

LNK_OBJ_FILES = $(CFG_ADD_LNK_OBJ_LIST,*F) $(ITF_OBJ_FILEPATHS)
LIB_FILEPATHS = $(ITF_LIB_FILEPATHS)
LIB_FILES     = $(ITF_LIB_FILES)

# -----------------------------------------------------------------------------
# Data file with result files information for outside processes.
# -----------------------------------------------------------------------------

DAT_ON = 1
DAT_FILE    = $(OUTPUT_FILE_NAME).dat

# -----------------------------------------------------------------------------
# C/C++ Optimization options
# -----------------------------------------------------------------------------

# Predefine default opimization (= full optimization)
%set    CC_OPT_OPTS  = $(CFG_CC_OPT_FULL_OPTS)
%set    CPP_OPT_OPTS = $(CFG_CPP_OPT_FULL_OPTS)

%if !null(ITF_CC_OPT_SET)
% if     "$(ITF_CC_OPT_SET)" == "DEFAULT"
    # Values as defined above.

% elseif "$(ITF_CC_OPT_SET)" == "NONE"
%   set CC_OPT_OPTS  = $(CFG_CC_OPT_NONE_OPTS)
%   set CPP_OPT_OPTS = $(CFG_CPP_OPT_NONE_OPTS)

% else   "$(ITF_CC_OPT_SET)" == "CUSTOM"
%   set CC_OPT_OPTS  = $(ITF_CC_CUSTOM_OPT_OPTS)
%   set CPP_OPT_OPTS = $(ITF_CC_CUSTOM_OPT_OPTS)

% endif
%endif

# -----------------------------------------------------------------------------
# Linker options
# -----------------------------------------------------------------------------

%set CFG_LNK_OPTS += -Wl,--warn-common

%foreach _IDX_ITF_LIB_DIRPATHS in $(LIB_DIRPATHS)
    %set CFG_LNK_OPTS += -Wl,-L$(_IDX_ITF_LIB_DIRPATHS)
%endfor

% include CommonRules.mk
