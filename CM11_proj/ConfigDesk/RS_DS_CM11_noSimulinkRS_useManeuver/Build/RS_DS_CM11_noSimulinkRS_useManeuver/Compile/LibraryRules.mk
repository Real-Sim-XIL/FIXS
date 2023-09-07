##############################################################################
### File                  : LibraryRules.mk
### Compatibility         : DSMAKE 6.26
### Description           : Make Libraries rules makefile.
###
### Copyright 2018, dSPACE GmbH. All rights reserved.
###
##############################################################################

# =============================================================================
# Make library main rule
# =============================================================================

MakeLib : MakeLibPreparation MakeCheckMarker_All MakeCompile MakeLibArchive MakeFinishCleanUp

# =============================================================================
# Make library preparation
# =============================================================================

MakeLibPreparation :
    # Save the current working directory
    %set RETURN_DIR = $(WORK_DIRPATH)

# =============================================================================
# Start the library generation
# =============================================================================

MakeLibArchive : $(CC_LIB_FILE)

# =============================================================================
# Create the new library or update the library if the objects are new or the
# dependencies have changed
# =============================================================================

$(CC_LIB_FILE) .ALWAYS : $(OBJS)
    %set _LIB_NAME := $(.TARGET)
    %set _LIB_UPDATED = 0

    # =============================================================================
    # Create library (that is, archive with object files) if the source files have
    # changed, or if the library is built from object files only.
    # =============================================================================
    %if '$(.NEWSOURCES)' != '' || '$(.SOURCES,F)' == ''
        # Delete existing library to ensure obsolete objects are removed from library.
        %if exists($(_LIB_NAME))
            %do MakeDel FILE_LIST="$(_LIB_NAME,$(WS_PACK_TARGET_ARGS))"
        %endif

        # Create librarian/archiver list file
        %set LIB_LIST_FILE = $(WORK_DIRPATH)\$(_LIB_NAME,B).lst
        %echo $(.SOURCES,F) $(OBJ_FILEPATHS:\\=/) > $(LIB_LIST_FILE)

        # Create library
        %set AR_CMD := "$(CFG_AR,$(WS_DEQUOTE))" -rc $(_LIB_NAME) @"$(LIB_LIST_FILE,$(WS_DEQUOTE))"
        %do MakeShellLine CMD_DIR=$(WORK_DIRPATH) SHORT_NAME=$(_LIB_NAME,B)_ar SHELL_CMD="$(AR_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Making library $(_LIB_NAME,F) failed"

        # Generate a table of contents.
        %set TABLE_CMD = "$(CFG_AR,$(WS_DEQUOTE))" -t $(_LIB_NAME) > $(_LIB_NAME,B).tbl
        %do MakeShellLine CMD_DIR=$(WORK_DIRPATH) SHORT_NAME=$(_LIB_NAME,B)_ar-t SHELL_CMD="$(TABLE_CMD,$(WS_PACK_TARGET_ARGS))" ERR_MSG="Making library $(_LIB_NAME,F) failed"

        %set _LIB_UPDATED = 1
    %endif

    # Return to the saved working directory
    %set WORK_DIRPATH = $(RETURN_DIR)
	%undef RETURN_DIR

    %if '$(_LIB_UPDATED)' == '0'
        %echo Library "$(_LIB_NAME,F,$(WS_DEQUOTE))" is up to date
    %else
        %echo Making library "$(_LIB_NAME,F,$(WS_DEQUOTE))" finished 
    %endif