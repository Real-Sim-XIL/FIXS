##############################################################################
### File                  : CommonRules.mk
### Compatibility         : DSMAKE 6.26
### Description           : Common rules makefile.
###
### Copyright 2019, dSPACE GmbH. All rights reserved.
###
##############################################################################

# =============================================================================
# Make main rule
# =============================================================================

# Default target of make process.
MakeMain : MakeStart MakeCreateMakefiles MakeDependencyGeneration MakeExecuteMakefiles MakeBuild MakeFinish

MakeBuild :
    %if "$(OUTPUT_FILE_TYPE)" != "LIBRARY"
        %do MakeObjCopy
        %do MakeApplImageFiles
    %else
         %do MakeFinalList
    %endif

# =============================================================================
# Make start rule
# =============================================================================

MakeStart : MakeStartMessage MakeStartCheck

MakeStartMessage :
    %echo Making "$(OUTPUT_FILE_NAME)" ...

MakeStartCheck :
    %echo Make directory is "$(WORK_DIRPATH,$(WS_DEQUOTE))"
    %echo Target compiler is "$(CFG_COMPILER_NAME)" at "$(CFG_COMPILER_DIRPATH,$(WS_DEQUOTE))"
    %if !(exists("$(CFG_CC,$(WS_DEQUOTE))") && exists("$(CFG_ASM,$(WS_DEQUOTE))") && exists("$(CFG_LNK,$(WS_DEQUOTE))"))
        %echo ERROR: $(CFG_COMPILER_NAME) compiler is not installed/decrypted. Use dSPACE Installation Manager to complete the installation.
        %do MakeAbort
    %endif

MakeCreateMakefiles : MakeCreateLibMakefiles
    %if "$(OUTPUT_FILE_TYPE)" != "LIBRARY"
        %do MakeCreateObjMakefiles
    %endif

# =============================================================================
# Create Interface Makefiles
# =============================================================================
MakeCreateObjMakefiles :
    %set TMP_MAKEFILE_CONTENT =

    %set TMP_MAKEFILE_CONTENT  =CC_OPT_SET_SUPPORTED_VALUES = $(ITF_CC_OPT_SET_SUPPORTED_VALUES) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=INCLUDE_MAKEFILE_VERSION    = $(ITF_INCLUDE_MAKEFILE_VERSION) $(NEWLINE)

    %set TMP_MAKEFILE_CONTENT +=SRCS                        = $(ITF_CC_SRC_FILES) $(NEWLINE)

    %set TMP_MAKEFILE_CONTENT +=MRK_FILES                   = $(ITF_CC_SRC_FILES,M\.$(CC_SRC_EXT)$$,S'\.$(CC_SRC_EXT)$$'.$(MRK_EXT)') $(ITF_CC_SRC_FILES,M\.$(CPP_SRC_EXT)$$,S'\.$(CPP_SRC_EXT)$$'.$(MRK_EXT)') $(NEWLINE)

    # Note, don't swap the macros of the following dependency line.
    %set TMP_MAKEFILE_CONTENT +=OBJS                        = $(ITF_CC_SRC_FILES,M\.$(CPP_SRC_EXT)$$,S'\.$(CPP_SRC_EXT)$$'.$(CPP_OBJ_EXT)') $(ITF_CC_SRC_FILES,M\.$(CC_SRC_EXT)$$,S'\.$(CC_SRC_EXT)$$'.$(CC_OBJ_EXT)') $(NEWLINE)

    %set LNK_OBJ_FILES        += $(ITF_CC_SRC_FILES,M\.$(CC_SRC_EXT)$$,S'\.$(CC_SRC_EXT)$$'.$(CC_OBJ_EXT)') $(ITF_CC_SRC_FILES,M\.$(CPP_SRC_EXT)$$,S'\.$(CPP_SRC_EXT)$$'.$(CPP_OBJ_EXT)')
    
    %do MakeSplitMacro MACRONAME=CC_SRC_DIRPATHS MACROVALUES="$(ITF_CC_SRC_DIRPATHS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
    
    %do MakeSplitMacro MACRONAME=CC_HDR_DIRPATHS MACROVALUES="$(ITF_CC_HDR_DIRPATHS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
    
    %set TMP_MAKEFILE_CONTENT +=LIB_DIRPATHS                = $(ITF_LIB_DIRPATHS) $(NEWLINE)
    %foreach _IDX_ITF_LIB_DIRPATHS in $(ITF_LIB_DIRPATHS)
        %set CFG_LNK_OPTS += -Wl,-L$(_IDX_ITF_LIB_DIRPATHS)
    %endfor

    %set TMP_MAKEFILE_CONTENT +=CUSTOM_DIRPATHS             = $(ITF_CUSTOM_DIRPATHS) $(NEWLINE)
        %foreach _IDX_ITF_CUSTOM_DIRPATHS in $(ITF_CUSTOM_DIRPATHS)
        %set CFG_LNK_OPTS += -Wl,-L$(_IDX_ITF_CUSTOM_DIRPATHS)
    %endfor

    %set TMP_MAKEFILE_CONTENT +=CC_ADD_OPTS                 = $(ITF_CC_ADD_OPTS) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=CPP_ADD_OPTS                = $(ITF_CPP_ADD_OPTS) $(NEWLINE)

    %set CC_ADD_DEFINES        = $(ITF_CC_ADD_DEFINES)
    %set TMP_MAKEFILE_CONTENT +=CC_ADD_DEFINES              = $(CC_ADD_DEFINES) $(NEWLINE)

    %set CC_ADD_UNDEFS         = $(ITF_CC_ADD_UNDEFS)
    %set TMP_MAKEFILE_CONTENT +=CC_ADD_UNDEFS               = $(CC_ADD_UNDEFS) $(NEWLINE)

    %set TMP_MAKEFILE_CONTENT +=CC_OPT_SET                  = $(ITF_CC_OPT_SET) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=CC_CUSTOM_OPT_OPTS          = $(ITF_CC_CUSTOM_OPT_OPTS) $(NEWLINE)

    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=all : MakeCheckMarker_All MakeCompile $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=depend : $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=%include Configuration.mk $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=%include CommonRules.mk $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)

    %set TMP_CC_ALL_OPTS = $(CC_ALL_OPTS) \
        $(ITF_CUSTOM_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
        $(ITF_CC_HDR_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
        $(CC_HDR_GLOBAL_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
        $(CC_ADD_DEFINES,<$(CFG_CC_DEF_OPT)) \
        $(CC_ADD_UNDEFS,<$(CFG_CC_UNDEF_OPT))

    %do MakeSplitMacro MACRONAME=CC_ALL_OPTS MACROVALUES="$(TMP_CC_ALL_OPTS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT

    %set TMP_CPP_ALL_OPTS = $(CPP_ALL_OPTS) \
        $(ITF_CUSTOM_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
        $(ITF_CC_HDR_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
        $(CC_HDR_GLOBAL_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
        $(CC_ADD_DEFINES,<$(CFG_CC_DEF_OPT)) \
        $(CC_ADD_UNDEFS,<$(CFG_CC_UNDEF_OPT))

    %do MakeSplitMacro MACRONAME=CPP_ALL_OPTS MACROVALUES="$(TMP_CPP_ALL_OPTS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT

    %undef CC_ADD_UNDEFS
    %undef CC_ADD_DEFINES

    %set TMP_MAKEFILE_CONTENT +=\# ============================================================================= $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# MKMF Section $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# ============================================================================= $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# MKMF Configuration $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=.TYPE.C   = .$(CC_SRC_EXT) .$(CPP_SRC_EXT) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=.TYPE.OBJ = .$(CC_OBJ_EXT) .$(CPP_OBJ_EXT) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %do MakeSplitMacro MACRONAME=.PATH.$(CC_SRC_EXT) MACROVALUES="$(ITF_CC_SRC_DIRPATHS,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS)) $(ITF_CUSTOM_DIRPATHS,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
    %do MakeSplitMacro MACRONAME=.PATH.$(CPP_SRC_EXT) MACROVALUES="$(ITF_CC_SRC_DIRPATHS,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS)) $(ITF_CUSTOM_DIRPATHS,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
    %do MakeSplitMacro MACRONAME=.PATH.$(CC_HDR_EXT) MACROVALUES="$(ITF_CC_HDR_DIRPATHS,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS)) $(ITF_CUSTOM_DIRPATHS,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set HDRPATH_CC_SRC_EXT = \
        $(ITF_CC_HDR_DIRPATHS) \
        $(ITF_CUSTOM_DIRPATHS) | \
        $(ITF_CC_HDR_DIRPATHS) \
        $(ITF_CUSTOM_DIRPATHS)
    %do MakeSplitMacro MACRONAME=.HDRPATH.$(CC_SRC_EXT) MACROVALUES="$(HDRPATH_CC_SRC_EXT,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
    %set HDRPATH_CPP_SRC_EXT  = \
        $(ITF_CC_HDR_DIRPATHS) \
        $(ITF_CUSTOM_DIRPATHS) | \
        $(ITF_CC_HDR_DIRPATHS) \
        $(ITF_CUSTOM_DIRPATHS)
    %do MakeSplitMacro MACRONAME=.HDRPATH.$(CPP_SRC_EXT) MACROVALUES="$(HDRPATH_CPP_SRC_EXT,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=.CDEFINES.$(CC_SRC_EXT)  = $(CC_ALL_OPTS,M$(CFG_CC_DEF_OPT)) $(CC_ALL_OPTS,M$(CFG_CC_UNDEF_OPT)) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=.CDEFINES.$(CPP_SRC_EXT) = $(CFG_CC_DEF_OPT)__cplusplus $(CPP_ALL_OPTS,M$(CFG_CPP_DEF_OPT)) $(CPP_ALL_OPTS,M$(CFG_CPP_UNDEF_OPT)) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=MKMF_SRCS  = $(ITF_CC_SRC_FILES) $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=MKMF_LINELEN = 118 $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=MKMF_TABLEN  = 2 $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# MKMF Replacements $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=HDRS= $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=EXTHDRS= $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=SRCS= $(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
    %set TMP_MAKEFILE_CONTENT +=\# [EOF] $(NEWLINE)

    %echo $(TMP_MAKEFILE_CONTENT) > $(OUTPUT_FILE_NAME).mk

    %undef TMP_MAKEFILE_CONTENT

    %set CfgDeskMakefiles += $(OUTPUT_FILE_NAME).mk
    %set CfgDeskDependencyGenerationList += $(OUTPUT_FILE_NAME).mk

# =============================================================================
# Create Library Makefiles
# =============================================================================
MakeCreateLibMakefiles :
    %echo Creating Makefiles ...
    %foreach IDX_TARGET in $(INTERFACE_IN_LIB_TARGETS)
        # Create library working directory
        %set CfgDeskMakeFile =  $(IDX_TARGET)\$(IDX_TARGET).mk

        %if '$(IDX_TARGET)' != ''
            %if !exists($(IDX_TARGET))
                %do MakeMkDir NEW_DIR=$(IDX_TARGET)
            %endif
        %endif
        
        %set TMP_MAKEFILE_CONTENT =

        %set TMP_MAKEFILE_CONTENT  =CC_OPT_SET_SUPPORTED_VALUES = $(ITF_CC_OPT_SET_SUPPORTED_VALUES) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=INCLUDE_MAKEFILE_VERSION    = $(ITF_INCLUDE_MAKEFILE_VERSION) $(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=LIB_TARGET                  = $(IDX_TARGET) $(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=SRCS                        = $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_FILES) $(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=MRK_FILES                   = $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_FILES,M\.$(CC_SRC_EXT)$$,S'\.$(CC_SRC_EXT)$$'.$(MRK_EXT)',F) $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_FILES,M\.$(CPP_SRC_EXT)$$,S'\.$(CPP_SRC_EXT)$$'.$(MRK_EXT)',F) $(NEWLINE)

        # Note, don't swap the macros of the following dependency line.
        %set TMP_MAKEFILE_CONTENT +=OBJS                        = $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_FILES,M\.$(CPP_SRC_EXT)$$,S'\.$(CPP_SRC_EXT)$$'.$(CPP_OBJ_EXT)',F) $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_FILES,M\.$(CC_SRC_EXT)$$,S'\.$(CC_SRC_EXT)$$'.$(CC_OBJ_EXT)',F) $(NEWLINE)

        %do MakeSplitMacro MACRONAME=CC_SRC_DIRPATHS MACROVALUES="$(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_DIRPATHS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
        
        %do MakeSplitMacro MACRONAME=CC_HDR_DIRPATHS MACROVALUES="$(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT

        %set TMP_MAKEFILE_CONTENT +=CC_LIB_FILE                 = $(INTERFACE_LIB_$(IDX_TARGET)_FILELOCATION) $(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=OBJ_FILEPATHS               = $(INTERFACE_LIB_$(IDX_TARGET)_OBJ_FILEPATHS) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=LIB_DIRPATHS                = $(ITF_LIB_DIRPATHS) $(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=CUSTOM_DIRPATHS             = $(ITF_CUSTOM_DIRPATHS) $(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=CC_ADD_OPTS                 = $(ITF_CC_ADD_OPTS) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=CPP_ADD_OPTS                = $(ITF_CPP_ADD_OPTS) $(NEWLINE)

        %set CC_ADD_DEFINES        = $(ITF_CC_ADD_DEFINES) $(INTERFACE_LIB_$(IDX_TARGET)_CC_ADD_DEFINES)
        %set TMP_MAKEFILE_CONTENT +=CC_ADD_DEFINES              = $(CC_ADD_DEFINES) $(NEWLINE)

        %set CC_ADD_UNDEFS         = $(ITF_CC_ADD_UNDEFS) $(INTERFACE_LIB_$(IDX_TARGET)_CC_ADD_UNDEFS)
        %set TMP_MAKEFILE_CONTENT +=CC_ADD_UNDEFS               = $(CC_ADD_UNDEFS) $(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=CC_OPT_SET                  = $(ITF_CC_OPT_SET) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=CC_CUSTOM_OPT_OPTS          = $(ITF_CC_CUSTOM_OPT_OPTS) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=all : MakeLib $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=depend : $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)

        %set TMP_MAKEFILE_CONTENT +=%include ..\Configuration.mk $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=%include ..\LibraryRules.mk $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=%include ..\CommonRules.mk $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)

        %set TMP_CC_ALL_OPTS = $(CC_ALL_OPTS) \
            $(ITF_CUSTOM_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
            $(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
            $(CC_HDR_GLOBAL_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
            $(CC_ADD_DEFINES,<$(CFG_CC_DEF_OPT)) \
            $(CC_ADD_UNDEFS,<$(CFG_CC_UNDEF_OPT))

        %do MakeSplitMacro MACRONAME=CC_ALL_OPTS MACROVALUES="$(TMP_CC_ALL_OPTS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT

        %set TMP_CPP_ALL_OPTS = $(CPP_ALL_OPTS) \
            $(ITF_CUSTOM_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
            $(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
            $(CC_HDR_GLOBAL_DIRPATHS,A,$(WS_PREPEND_CC_INCL_OPT)) \
            $(CC_ADD_DEFINES,<$(CFG_CC_DEF_OPT)) \
            $(CC_ADD_UNDEFS,<$(CFG_CC_UNDEF_OPT))

        %do MakeSplitMacro MACRONAME=CPP_ALL_OPTS MACROVALUES="$(TMP_CPP_ALL_OPTS,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT

        %undef CC_ADD_UNDEFS
        %undef CC_ADD_DEFINES

        %set TMP_MAKEFILE_CONTENT +=\# ============================================================================= $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# MKMF Section $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# ============================================================================= $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# MKMF Configuration $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=.TYPE.C   = .$(CC_SRC_EXT) .$(CPP_SRC_EXT) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=.TYPE.OBJ = .$(CC_OBJ_EXT) .$(CPP_OBJ_EXT) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %do MakeSplitMacro MACRONAME=.PATH.$(CC_SRC_EXT) MACROVALUES="$(ITF_CUSTOM_DIRPATHS,A,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS)) $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_DIRPATHS,A,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
        %do MakeSplitMacro MACRONAME=.PATH.$(CPP_SRC_EXT) MACROVALUES="$(ITF_CUSTOM_DIRPATHS,A,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS)) $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_DIRPATHS,A,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
        %do MakeSplitMacro MACRONAME=.PATH.$(CC_HDR_EXT) MACROVALUES="$(ITF_CUSTOM_DIRPATHS,A,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS)) $(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS,A,$(WS_APPEND_SEMIKOLON),$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set HDRPATH_CC_SRC_EXT = \
            $(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS) \
            $(ITF_CUSTOM_DIRPATHS) | \
            $(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS) \
            $(ITF_CUSTOM_DIRPATHS)
        %do MakeSplitMacro MACRONAME=.HDRPATH.$(CC_SRC_EXT) MACROVALUES="$(HDRPATH_CC_SRC_EXT,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
        %set HDRPATH_CPP_SRC_EXT = \
            $(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS) \
            $(ITF_CUSTOM_DIRPATHS) | \
            $(INTERFACE_LIB_$(IDX_TARGET)_CC_HDR_DIRPATHS) \
            $(ITF_CUSTOM_DIRPATHS)
        %do MakeSplitMacro MACRONAME=.HDRPATH.$(CPP_SRC_EXT) MACROVALUES="$(HDRPATH_CPP_SRC_EXT,$(WS_PACK_TARGET_ARGS))" CONTENT=TMP_MAKEFILE_CONTENT
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=.CDEFINES.$(CC_SRC_EXT)  = $(CC_ALL_OPTS,M$(CFG_CC_DEF_OPT)) $(CC_ALL_OPTS,M$(CFG_CC_UNDEF_OPT)) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=.CDEFINES.$(CPP_SRC_EXT) = $(CFG_CC_DEF_OPT)__cplusplus $(CPP_ALL_OPTS,M$(CFG_CPP_DEF_OPT)) $(CPP_ALL_OPTS,M$(CFG_CPP_UNDEF_OPT)) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=MKMF_SRCS  = $(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_FILES) $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=MKMF_LINELEN = 118 $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=MKMF_TABLEN  = 2 $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# MKMF Replacements $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# ----------------------------------------------------------------------------- $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=HDRS= $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=EXTHDRS= $(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=$(NEWLINE)
        %set TMP_MAKEFILE_CONTENT +=\# [EOF] $(NEWLINE)

        %echo $(TMP_MAKEFILE_CONTENT) > $(CfgDeskMakeFile)

        %undef TMP_MAKEFILE_CONTENT

        %set CfgDeskMakefiles += $(CfgDeskMakeFile)

        # Add makefile only to the list of makefiles for dependency computation
        # when there are sourcefiles
        %if '$(INTERFACE_LIB_$(IDX_TARGET)_CC_SRC_FILES)' != ''
            %set CfgDeskDependencyGenerationList += $(CfgDeskMakeFile) 
        %endif
    %endfor

# =============================================================================
# Write the macro values on several lines
# =============================================================================
MakeSplitMacro :
    %set $(CONTENT) +=$(MACRONAME) =
    %foreach ITEM in $(MACROVALUES,$(WS_UNPACK_TARGET_ARGS))
        %set $(CONTENT) += \$(NEWLINE)
        %set $(CONTENT) +=    $(ITEM)
    %endfor
    %set $(CONTENT) += $(NEWLINE)
    %set $(CONTENT) += $(NEWLINE)

# =============================================================================
# MakeExecuteMakefiles
# =============================================================================

# Check if we have libraries to be build.
MakeExecuteMakefiles :
    %foreach file in $(CfgDeskMakefiles)
        %set RETURN_DIR = "$(WORK_DIRPATH,$(WS_DEQUOTE))"
        %set filepath = "$(file,A)"
        %set WORK_DIRPATH = $(filepath,D)
        %set DSMAKE_LIB_CMD = "$(CFG_MAKE,$(WS_DEQUOTE))" -f $(file,F)
        %do MakeCreateParallelJob PARCOMP_FILE_NAME=$(OUTPUT_FILE_NAME)_em START_MSG="Making Makefile \"$(file)\" ..." SHELL_CMD="$(DSMAKE_LIB_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Making \"$(file)\" failed"
        %set WORK_DIRPATH = $(RETURN_DIR)
        %undef RETURN_DIR
    %endfor
    %do MakeParallelJobsExecute PARCOMP_FILE_NAME=$(OUTPUT_FILE_NAME)_em

MakeObjCopy :
    %foreach IDX_INCLUDE_CONTEXT in $(TMK_CFG_MAKE_INCLUDE_CONTEXTS,N"Main")
        %set TMP_LIB_MAIN_TARGET =
        %if '$(TMK_CFG_HIDE_GLOBAL_SYMBOLS)' == '1'
        
            %if '$(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_MAIN_TARGET)' != ''
                %set TMP_LIB_MAIN_TARGET = $(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_MAIN_TARGET)
            %endif
            
            %set TMP_LIB_OBJ_FILEPATHS  =
            %set TMP_LIB_LIB_FILEPATHS  =
            
            %set TMP_LIB_MAIN_TARGET += $(TMK_CFG_$(IDX_INCLUDE_CONTEXT)_MAIN_TARGETS)
            %if !member($(IDX_INCLUDE_CONTEXT), $(TMP_LIB_MAIN_TARGET))
                %set TMP_LIB_MAIN_TARGET += $(IDX_INCLUDE_CONTEXT)
            %endif
            
            %foreach IDX_LIB_MAIN_TARGET in $(TMP_LIB_MAIN_TARGET) 
                # Create librarian/archiver list file
                %set TMP_LIB_CC_SRC_FILES   = $(INTERFACE_LIB_$(IDX_LIB_MAIN_TARGET)_CC_SRC_FILES,M\.$(CC_SRC_EXT)$$)
                %set TMP_LIB_CPP_SRC_FILES  = $(INTERFACE_LIB_$(IDX_LIB_MAIN_TARGET)_CC_SRC_FILES,M\.$(CPP_SRC_EXT)$$)
                
                %set TMP_LIB_OBJ_FILEPATHS += $(TMP_LIB_CC_SRC_FILES,S'\.$(CC_SRC_EXT)$$'.$(CC_OBJ_EXT)',F,<$(WORK_DIRPATH,>\$(IDX_LIB_MAIN_TARGET))\)
                %set TMP_LIB_OBJ_FILEPATHS += $(TMP_LIB_CPP_SRC_FILES,S'\.$(CPP_SRC_EXT)$$'.$(CPP_OBJ_EXT)',F,<$(WORK_DIRPATH,>\$(IDX_LIB_MAIN_TARGET))\)
            %endfor

            %set TMP_LIB_OBJ_FILEPATHS += $(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_OBJ_FILEPATHS)
            %set TMP_LIB_LIB_FILEPATHS += $(INTERFACE_LIB_$(IDX_INCLUDE_CONTEXT)_LIB_FILEPATHS)

            %set LIB_LIST_FILE = $(WORK_DIRPATH,>\$(IDX_INCLUDE_CONTEXT))_ld.lst
            # Note: the order of object and library files is important
            %echo $(TMP_LIB_OBJ_FILEPATHS:\\=/) $(TMP_LIB_LIB_FILEPATHS:\\=/) > $(LIB_LIST_FILE)

            # Create library and generate a table of contents
            %set LD_CMD := "$(CFG_LDLNK,$(WS_DEQUOTE))" $(CFG_LDLNK_OPTS) --start-group @$(LIB_LIST_FILE) --end-group $(CFG_LDLNK_OUTFILE_OPT) $(IDX_INCLUDE_CONTEXT).$(CFG_OBJ_FILE_EXT)
            %do MakeShellLine CMD_DIR=$(WORK_DIRPATH) SHORT_NAME=$(IDX_INCLUDE_CONTEXT)_ld SHELL_CMD="$(LD_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Making object \"$(IDX_INCLUDE_CONTEXT,$(WS_DEQUOTE))\" failed"

            # Hide global symbols
            %set OCOPY_CMD = "$(CFG_OBJCOPY,$(WS_DEQUOTE))" $(CFG_OBJCOPY_KEEP_OPT)="$(TMK_CFG_$(IDX_INCLUDE_CONTEXT)_KEPT_GLOBAL_SYMBOLS_FILEPATH,$(WS_DEQUOTE))" $(IDX_INCLUDE_CONTEXT).$(CFG_OBJ_FILE_EXT)
            %do MakeShellLine CMD_DIR=$(WORK_DIRPATH) SHORT_NAME=$(IDX_INCLUDE_CONTEXT,B)_objcopy SHELL_CMD="$(OCOPY_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Object copy for "$(IDX_INCLUDE_CONTEXT)" failed."

            %set LNK_OBJ_FILES += $(IDX_INCLUDE_CONTEXT).$(CFG_OBJ_FILE_EXT)
        %endif
    %endfor

# =============================================================================
# Check Options rule
# =============================================================================

MakeCheckMarker_All :  $(MRK_FILES)

%.$(MRK_EXT) .ALWAYS : %.$(CC_SRC_EXT)
    %set MRK_FILE         = $(WORK_DIRPATH,>\$(.TARGET,1,F))
    %set OUT_OBJ_FILE     = $(.SOURCE,F,S'\.$(CC_SRC_EXT)'.$(CC_OBJ_EXT)')
    %set IN_SOURCE_FILE   = $(.SOURCE,A)
    %set TEST_CMD         = $(CC_CMD,S'\>'\\>')
    %set TEST_SRC         = $(.SOURCE,A)
    %do MakeCheckMarkerFile

%.$(MRK_EXT) .ALWAYS : %.$(CPP_SRC_EXT)
    %set MRK_FILE         = $(WORK_DIRPATH,>\$(.TARGET,1,F))
    %set OUT_OBJ_FILE     = $(.SOURCE,F,S'\.$(CPP_SRC_EXT)'.$(CPP_OBJ_EXT)')
    %set IN_SOURCE_FILE   = $(.SOURCE,A)
    %set TEST_CMD         = $(CPP_CMD,S'\>'\\>')
    %set TEST_SRC         = $(.SOURCE,A)
    %do MakeCheckMarkerFile

MakeCheckMarkerFile :
    %set TMP_INFO_FILE = $(WORK_DIRPATH,>\$(.SOURCE,B).info)
    # file modified time
    FOR %A IN ($(TEST_SRC)) DO @ECHO %~tA > $(TMP_INFO_FILE)
    # file created time
    dir /t:c /-c $(TEST_SRC) | $(FIND) "$(TEST_SRC,F,$(WS_DEQUOTE))" >> $(TMP_INFO_FILE)
    %set TEST_FILE = source_file: $(TEST_SRC) command_line: $(TEST_CMD) file_info: $(TMP_INFO_FILE,@)
    %do MakeDel FILE_LIST="$(TMP_INFO_FILE,$(WS_PACK_TARGET_ARGS))"
    %set TEST_FILE = $(TEST_FILE, W\n)
    %set UPDATE_MARKER = 0
    %if file($(MRK_FILE))
        # NOTE: Macro modifier "@" would read in the file but collapses all spaces to single spaces.
        #       Instead "@'\(.*\)'\1'," reads in the file without change.
        %set CURR_MRK_FILE := $(MRK_FILE,@'\(.*\)'\1'g,S'\>'\\>',W\n)
        %if '$(TEST_FILE)' != '$(CURR_MRK_FILE)'
            %set UPDATE_MARKER = 1
        %endif
    %else
        %set UPDATE_MARKER = 1
    %endif

    %if $(UPDATE_MARKER)
        %do MakeDel FILE_LIST="$(WORK_DIRPATH,>\$(OUT_OBJ_FILE),$(WS_PACK_TARGET_ARGS))"
        %echo $(TEST_FILE) > $(MRK_FILE)
    %endif

# =============================================================================
# Compile rules
# =============================================================================

MakeCompile : $(OBJS)
    %do MakeParallelJobsExecute PARCOMP_FILE_NAME=MakeCompile

# C sources deps.
%.$(CC_OBJ_EXT) : %.$(CC_SRC_EXT)
    %set OUT_OBJ_FILE     = $(WORK_DIRPATH,>\$(.TARGET,1,F))
    %set IN_SOURCE_FILE   = $(.SOURCE,A)
    %do MakeCreateParallelJob PARCOMP_FILE_NAME=MakeCompile START_MSG="Compiling \"$(.SOURCE,A,$(WS_DEQUOTE))\" ..." SHELL_CMD="$(CC_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Compiling failed  \"$(.SOURCE,$(WS_DEQUOTE))\""

# C++ sources deps.
%.$(CPP_OBJ_EXT) : %.$(CPP_SRC_EXT)
    %set OUT_OBJ_FILE     = $(WORK_DIRPATH,>\$(.TARGET,1,F))
    %set IN_SOURCE_FILE   = $(.SOURCE,A)
    %do MakeCreateParallelJob PARCOMP_FILE_NAME=MakeCompile START_MSG="Compiling \"$(.SOURCE,A,$(WS_DEQUOTE))\" ..." SHELL_CMD="$(CPP_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Compiling failed  \"$(.SOURCE,$(WS_DEQUOTE))\""

MakeParallelJobsExecute :
    %if %length(PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME)) > 0
        %set PARCOMP_CMDFILE = "$(WORK_DIRPATH,$(WS_DEQUOTE))\$(PARCOMP_FILE_NAME).xml"
        %set PARCOMP_ERRFILE = "$(WORK_DIRPATH,$(WS_DEQUOTE))\$(PARCOMP_FILE_NAME).err"
        %set PARCOMP_LINES =\<?xml version="1.0"?\>$(NEWLINE)
        %set PARCOMP_LINES +=\<jobs\>$(NEWLINE)
        %set PARCOMP_LINES +=\<abortmutex\>\<mutexname\>$(TMK_CFG_ABORT_MUTEX_NAME)\</mutexname\>\<abortmsg\>Abort requested. Waiting for already started jobs to finish.\</abortmsg\>\</abortmutex\>$(NEWLINE)
        %set PARCOMP_LINES += $(PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME))
        %set PARCOMP_LINES +=\</jobs\>$(NEWLINE)
        %echo $(PARCOMP_LINES)>$(PARCOMP_CMDFILE)
        # additional parallel compile options, e.g. maxJobs=n to limit the number of
        # concurrent compile operations
        $(EXEC_SHELL) $(PARCOMP_EXEC) -cmdFile=$(PARCOMP_CMDFILE) -errFile=$(PARCOMP_ERRFILE) -maxJobs=$(TMK_MAX_NUMBER_OF_PARALLEL_JOBS)
        %if $(status) == 3
            %echo Compilation aborted.
            %do MakeAbort
        %endif
        %if $(status) == 1 || $(status) == 2
            %set _STATUS = $(status)
            %echo Compilation was not successful. ($(_STATUS))
            %do MakeAbort
        %endif
    %endif

# =============================================================================
# Link rules
# =============================================================================

MakeApplImageFiles :
    # Prepare dat-file (containing result file paths)
    %do MakeDel FILE_LIST="$(DAT_FILE,$(WS_PACK_TARGET_ARGS))"
    %echo \<?xml version="1.0" encoding="utf-8"?\> >> $(DAT_FILE)
    %echo \<resultfilepaths\> >> $(DAT_FILE)

    %set APPL_VAR            = $(APPL_VARS)
    %set APPL_IMAGE_MAP_FILE = $(OUTPUT_FILE_NAME).$(CFG_BOARD_APPL_IMAGE_MAP_EXT)
    %set APPL_RESULT_DIRPATH = $(APPL_VAR_RESULT_DIRPATH_$(APPL_VAR))
    %echo Creating application image file ...
    %do MakeApplImageLink
    %do MakeApplCopyResults
    %do MakeDatFileApplEntries
    %echo \</resultfilepaths\> >> $(DAT_FILE)

MakeApplImageLink :
    %set CFG_IN_LNK_OBJ_FILES  := $(LNK_OBJ_FILES)
    %set CFG_IN_LIB_FILE_PATHS =
    %foreach IDX_LIB_FILEPATHS in $(LIB_FILEPATHS)
    %   set CFG_IN_LIB_FILE_PATHS += $(IDX_LIB_FILEPATHS)
    %endfor
    %set CFG_IN_LIB_FILES      = $(LIB_FILES)
    %do cfg_lnk_appl
    #%if ! null(CFG_LNK_APPL)
    #    %do MakeShellLine SHORT_NAME=$(APPL_IMAGE_FILE,B)_lnk SHELL_CMD="$(CFG_LNK_APPL,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Linking application image file \"$(APPL_IMAGE_FILE,$(WS_DEQUOTE))\" failed"
    #%endif

MakeDatFileApplEntries :
    %echo       \<applicationvariant_$(APPL_VAR,LC) name="$(APPL_VAR,LC)" >> $(DAT_FILE)
    %echo           application_image_file="$(APPL_RESULT_DIRPATH,Q-,$(WS_DEQUOTE))\$(APPL_IMAGE_FILE)" >> $(DAT_FILE)
    %echo           link_map_file="$(APPL_RESULT_DIRPATH,Q-,$(WS_DEQUOTE))\$(APPL_IMAGE_MAP_FILE)" >> $(DAT_FILE)
    %echo       /\> >> $(DAT_FILE)

MakeApplCopyResults :
    %set DEST_DIR = $(APPL_RESULT_DIRPATH)\.
    %do MakeMkDir NEW_DIR=$(DEST_DIR)
    %do MakeCopy SRC_FILE=$(APPL_IMAGE_FILE) DEST_FILE=$(DEST_DIR)
    %do MakeCopy SRC_FILE=$(APPL_IMAGE_MAP_FILE) DEST_FILE=$(DEST_DIR)


# =============================================================================
# File Handling Rules
# =============================================================================

MakeDel :
    %foreach IDX_FILE in $(FILE_LIST,$(WS_UNPACK_TARGET_ARGS))
        %if exists($(IDX_FILE))
            --del "$(IDX_FILE,A,$(WS_DEQUOTE))"
        %endif
    %endfor

MakeCopy :
    %set TMP_COPY_CMD_FILE = copy.cmd
    %set TMP_COPY_OUT_FILE = copy.out
    %set COPY_CMD=copy /y "$(SRC_FILE)" "$(DEST_FILE)"
    %echo $(COPY_CMD,S'\>'\\>') > $(TMP_COPY_CMD_FILE)
    --$(COPY_CMD) > $(TMP_COPY_OUT_FILE) 2>&1
    %set TMP_STATUS = $(status)
    %if $(TMP_STATUS)
        --type $(TMP_COPY_CMD_FILE)
        --type $(TMP_COPY_OUT_FILE)
        %do MakeAbort
    %endif

MakeMkDir :
    %set NEW_DIR = "$(NEW_DIR,$(WS_DEQUOTE))"
    %set MKDIR_CMD = mkdir $(NEW_DIR)
    %if ! dir($(NEW_DIR))
        %do MakeShellLine SHORT_NAME="$(NEW_DIR,B)_mkdir" SHELL_CMD="$(MKDIR_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Make directory failed"
    %endif

# =============================================================================
# List Rules
# =============================================================================

MakeListFile :
    %do MakeDel FILE_LIST="$(LST_LST_FILE,$(WS_PACK_TARGET_ARGS))"
    %foreach LST_FILE in $(ALL_LST_FILES)
        %set LST_FILE = $(LST_FILE,<.\)
        %echo $(LST_FILE,A)>>$(LST_LST_FILE)
    %endfor

# =============================================================================
# Log Rules
# =============================================================================

MakeShellLine :
    %set TMP_CMD_FILE = $(WORK_DIRPATH,>\$(SHORT_NAME).cmd,A)
    %set TMP_OUT_FILE = $(WORK_DIRPATH,>\$(SHORT_NAME).out,A)
    %echo > $(TMP_OUT_FILE)
    %set SHELL_CMD := $(SHELL_CMD,$(WS_UNPACK_TARGET_ARGS))
    %if !null(CMD_DIR)
        %set CMD_DIR := $(CMD_DIR,$(WS_UNPACK_TARGET_ARGS))
    %else
        %set CMD_DIR := $(WORK_DIRPATH,$(WS_UNPACK_TARGET_ARGS))
    %endif
    %echo Command: > $(TMP_CMD_FILE)
    %echo $(SHELL_CMD,>=\>,<=\<,|=:) >> $(TMP_CMD_FILE)
    %set DIR_RETURN = $(WORK_DIRPATH)
    %if !null(CMD_DIR)
        %chdir "$(CMD_DIR,$(WS_DEQUOTE))"
    %endif
    $(EXEC_SHELL) $(SHELL_CMD)
    %set TMP_STATUS = $(status)
    %if !null(CMD_DIR)
        %chdir "$(DIR_RETURN,$(WS_DEQUOTE))"
    %endif
    %if $(TMP_STATUS)
        %echo $(ERR_MSG)
        %do MakeAbort
    %endif

MakeCreateParallelJob :
    %set SHELL_CMD := $(SHELL_CMD,$(WS_UNPACK_TARGET_ARGS))
    %set PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME) +=    \<job genre="execute"\>$(NEWLINE)
    %set workingdir = $(.SOURCE,A,D,$(WS_DEQUOTE))
    %if '$(workingdir)' == ''
        %set workingdir = $(WORK_DIRPATH,$(WS_DEQUOTE))
    %endif
    %set PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME) +=        \<workingdir\>$(workingdir,\\=/)\</workingdir\>$(NEWLINE)
    %set PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME) +=        \<startmsg\>$(START_MSG)\</startmsg\>$(NEWLINE)
    %set PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME) +=        \<command\>$(SHELL_CMD)\</command\>$(NEWLINE)
    %set PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME) +=        \<errmsg\>$(ERR_MSG)\</errmsg\>$(NEWLINE)
    %set PARCOMP_CMDEXEC_$(PARCOMP_FILE_NAME) +=    \</job\>$(NEWLINE)

# =============================================================================
# Make Finish rules
# =============================================================================

MakeFinalList:
    %set HELPER_FILES_OUTPUT_DIR := $(WORK_DIRPATH)
    %set LIB_RESULT_LIST = $(HELPER_FILES_OUTPUT_DIR,>\lib.txt)
    %set OBJ_RESULT_LIST = $(HELPER_FILES_OUTPUT_DIR,>\obj.txt)
    %set HDR_RESULT_LIST = $(HELPER_FILES_OUTPUT_DIR,>\hdr.txt)

    %echo > $(LIB_RESULT_LIST)
    %echo > $(OBJ_RESULT_LIST)
    %echo > $(HDR_RESULT_LIST)

    %foreach IDX in $(LIB_FILEPATHS)
        %set LIB_IS_FOUND = 1
        %foreach IDX_MAIN_TARGET in $(TMK_CFG_$(ITF_LIB_MAIN_TARGET)_MAIN_TARGETS) $(ITF_LIB_MAIN_TARGET)
            %if "$(INTERFACE_LIB_$(IDX_MAIN_TARGET)_FILELOCATION,A,$(WS_DEQUOTE))" != "$(IDX,A,$(WS_DEQUOTE))"
                %set LIB_IS_FOUND = 0
            %endif
        %endfor
        %if $(LIB_IS_FOUND) == 0
            %echo "$(IDX,A,$(WS_DEQUOTE))" >> $(LIB_RESULT_LIST)
        %endif
    %endfor
    %foreach IDX in $(LIB_FILES)
        %echo "$(IDX,$(WS_DEQUOTE))" >> $(LIB_RESULT_LIST)
    %endfor
    %foreach IDX in $(ITF_OBJ_FILEPATHS)
        %echo $(IDX) >> $(OBJ_RESULT_LIST)
    %endfor
    %foreach IDX in $(CC_HDR_GLOBAL_DIRPATHS)
        %echo $(IDX) >> $(HDR_RESULT_LIST)
    %endfor
    %foreach IDX in $(ITF_CC_HDR_DIRPATHS)
        %echo $(IDX) >> $(HDR_RESULT_LIST)
    %endfor
    %do MakeDel FILE_LIST="$(DAT_FILE,$(WS_PACK_TARGET_ARGS))"

    %echo \<?xml version="1.0" encoding="utf-8"?\> >> $(DAT_FILE)
    %echo \<resultfilepaths\> >> $(DAT_FILE)
    %set APPL_VAR            = $(APPL_VARS)

    %echo       \<applicationvariant_$(APPL_VAR,LC) name="$(APPL_VAR,LC)" >> $(DAT_FILE)
    %echo           library_list_file="$(LIB_RESULT_LIST,Q-,$(WS_DEQUOTE))" >> $(DAT_FILE)
    %echo           object_list_file="$(OBJ_RESULT_LIST,Q-,$(WS_DEQUOTE))" >> $(DAT_FILE)
    %echo           header_list_file="$(HDR_RESULT_LIST,Q-,$(WS_DEQUOTE))" >> $(DAT_FILE)
    %echo       /\> >> $(DAT_FILE)

    %echo \</resultfilepaths\> >> $(DAT_FILE)


MakeFinish : MakeFinishCleanUp MakeFinishMsg

MakeFinishCleanUp :
    # Delete temporary files
    %ifndef DEBUG_ON
        %set ALL_FILES = *.*
        %set KEPT_FILES  =N"*.$(CC_OBJ_EXT)"
        %set KEPT_FILES +=,N"*.$(CPP_OBJ_EXT)"
        %set KEPT_FILES +=,N"*.dat"
        %set KEPT_FILES +=,N"*.$(CFG_BOARD_APPL_IMAGE_MAP_EXT)"
        %set KEPT_FILES +=,N"*.$(CFG_LIB_FILE_EXT)"
        %set KEPT_FILES +=,N"*.$(MRK_EXT)"
        %set KEPT_FILES +=,N"*.mk"
        %set KEPT_FILES +=,N"*.txt"
        %set DEL_FILES = $(ALL_FILES,*F,$(KEPT_FILES))
        %do MakeDel FILE_LIST="$(DEL_FILES,$(WS_PACK_TARGET_ARGS))"
    %endif

MakeFinishMsg :
    %echo $(MAKE_SUCCESS_MESSAGE)

MakeAbort :
    %abort

# =============================================================================
# Make Auto Dependency Rule
# =============================================================================

MakeDependencyGeneration .NOALWAYS :
    %foreach file in $(CfgDeskDependencyGenerationList)
        %set RETURN_DIR = "$(WORK_DIRPATH,$(WS_DEQUOTE))"
        %set filepath = "$(file,A)"
        %set WORK_DIRPATH = $(filepath,D)
        %set MKMF_CMD = "$(CFG_DP,$(WS_DEQUOTE))" -c -dS -pS -oH+ -r -q -f $(file,F)
        %do MakeCreateParallelJob PARCOMP_FILE_NAME=$(OUTPUT_FILE_NAME)_dg START_MSG="Generating dependencies in \"$(file)\" ..." SHELL_CMD="$(MKMF_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Generating dependencies in \"$(file)\" failed"
        %set WORK_DIRPATH = $(RETURN_DIR)
        %undef RETURN_DIR
    %endfor
    %do MakeParallelJobsExecute PARCOMP_FILE_NAME=$(OUTPUT_FILE_NAME)_dg
