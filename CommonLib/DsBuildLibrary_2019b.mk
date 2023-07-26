########################################################################################################################
# File:
#   DsBuildLibrary.mk
#
# Author:
#   Ralf Grosse Boerger, Thorsten Brehm, Volker Lang
#
# Description:
#   This Makefile batch compiles the source files specified by the user and adds all
#   resulting object files into a runtime library using the GNU archiver (ar).
#   Depending of the target platform the built libraries are named in the following way:
#     DSx86_32:         lib<output_filename>.a
#     DS1007, DS1202:   <output_filename>.a
#     all other:        <output_filename>.lib
#
# Parameters:
#   output_filename    :  Name of the library to be generated. The suffix ".lib"/".a" is added automatically.
#   source_files       :  List of the source files to be compiled/linked.
#   custom_c_options   :  Additional custom C compiler options.
#   custom_cpp_options :  Additional custom C++ compiler options.
#   custom_obj_files   :  Additional custom object files.
#   target             :  Target platform (DS1005, DS1006, DS1007, DS1103, DS1104, DS1401, DS1202, DSx86_32)
#
# Usage:
#   dsmake -f DsBuildLibrary.mk [output_filename=<library filename without pre- and suffix>] source_files="<list of files>"
#
# Using large number of source files:
#   If the library consists of a large number of source files it may be impossible to specify them in a single command 
#   line without exceeding the maximum command line length of the operating system.
#   In this case the source files can be specified in an additional makefile, which must fulfill the conditions 
#   described below.
#   The makefile must be named 'DsBuildLibrarySrc.mk' and located in the same directory in which DsBuildLibrary.mk
#   is invoked. Specify the source files in the following format:
#
#     CUSTOM_SRC_FILES += <source_file1> <source_file2> <source_file3> <source_file4> \
#                         <source_file5> <source_file6> <source_file7> <source_file8>
#    
# Copyright 2018, dSPACE GmbH. All rights reserved.
#
########################################################################################################################

.OPTIONS : TargetQuoted
.OPTIONS : MacroQuoted

# --- default rule --------------------------------------------------------------------------------
default_target:                compile_and_archive

#--- Source files to be batch compiled and archived.
#--- Specify the full path for each source which is not located in the current working directory.
CUSTOM_SRC_FILES = $(source_files)

#--- Optional additional, pre-compiled object files to be added to the library.
%foreach obj in $(custom_obj_files)
    CUSTOM_OBJS += "$(obj,__WHITE_X_SPACE__= )"
%endfor

#--- Additional C compiler options
CUSTOM_C_OPTS = $(custom_c_options)

#--- Additional C++ compiler options
CUSTOM_CPP_OPTS = $(custom_cpp_options)

#--- enable C++ support
RTLIB_IS_CPP_APPL = ON

########################################################################################################################

%if %exist("DsBuildLibrarySrc.mk")
%include DsBuildLibrarySrc.mk       # makefile containing additional source file names
%endif


ABORT           = no
LIB_FILENAME    = MergedObjects
RESPONSE_FILE   = response.rsp      # name of the response file



# -------------------------------------------------------------------------------------------------
#  Check for (optional) parameters and build environment sanity
# -------------------------------------------------------------------------------------------------

%if "$(target)" == ""
%    echo
%    echo NOTICE: no target (DS1005, DS1006, DSx86_32 etc.) was specified!
%    set ABORT = yes
%endif

%if "$(output_filename)" == ""
%    echo
%    echo NOTICE: no output filename was specified, using default filename
%elif $(target,UC) == DSx86_32
# GNU linker requires the "lib" prefix, otherwise "-l" option cannot be used to link the library
%    set LIB_FILENAME = lib$(output_filename)
%elif $(target,UC) == DS2511
# GNU linker requires the "lib" prefix, otherwise "-l" option cannot be used to link the library
%    set LIB_FILENAME = lib$(output_filename)
%elif $(target,UC) == DS1403
# GNU linker requires the "lib" prefix, otherwise "-l" option cannot be used to link the library
%    set LIB_FILENAME = lib$(output_filename)
%else
%    set LIB_FILENAME = $(output_filename)
%endif

# check for source files
%if "$(CUSTOM_SRC_FILES)" == ""
%    echo ERROR: no source files specified!
%    set ABORT = yes
%endif

%if ("$(X86_ROOT)" == "")&&($(target,UC) == DS1006)
%    echo
%    echo ERROR: the environment variable X86_ROOT is not set!
%    set ABORT = yes
%endif

%if "$(DSPACE_ROOT)" == ""
%    echo
%    echo ERROR: the dSPACE root directory environment variable DSPACE_ROOT is not set!
%    set ABORT = yes
%endif

%if "$(ABORT)" == "yes"
%    abort
%endif

#include platform specific configuration data
%if %member($(target,UC), DS1103 DS1007 DS1202)
%  set CFG_MK_TEMPLATE_PATH = $(DSPACE_ROOT)\$(target)
%elif $(target,UC) == DS2511
%  set LIB_FILENAME = lib$(output_filename)
%  set CFG_MK_TEMPLATE_PATH = $(DSPACE_ROOT)\DS2511
%elif $(target,UC) == DSx86_32
%  set LIB_FILENAME = lib$(output_filename)
%  set CFG_MK_TEMPLATE_PATH = $(DSPACE_ROOT)\SCALEXIO
%elif $(target,UC) == DS1403
%  set LIB_FILENAME = lib$(output_filename)
%  set CFG_MK_TEMPLATE_PATH = $(DSPACE_ROOT)\DS1403
%else
%  set CFG_MK_TEMPLATE_PATH = $(DSPACE_ROOT)\$(target)\RTLib
%endif

%include "$(CFG_MK_TEMPLATE_PATH)\DsBuildConfiguration.mk"      # platform specific configuration data

LIB_OBJ_FILES += $(CUSTOM_SRC_FILES,M\.$(CFG_C_FILE_EXT)$$, R,>.$(CFG_OBJ_FILE_EXT))
LIB_OBJ_FILES += $(CUSTOM_SRC_FILES,M\.$(CFG_CPP_FILE_EXT)$$, R,>.$(CFG_OBJ_FILE_EXT_CPP))
LIB_OBJ_FILES += $(CUSTOM_OBJS)

CFG_CC_OPTIONS += $(CUSTOM_C_OPTS)
CFG_CPP_OPTIONS += $(CUSTOM_CPP_OPTS)

########################################################################################################################
#
# Rules for batch compile & archive
#
########################################################################################################################

compile_and_archive:           show_stats startup archive

show_stats:
    %echo 
    %echo Output filename                       : $(LIB_FILENAME).$(CFG_LIB_FILE_EXT)
    %echo Compiler root directory               : $(CFG_COMPILER_DIRPATH)
    %echo dSPACE Software root directory        : $(DSPACE_ROOT)
    %echo

startup:                       
                               %echo building library "$(LIB_FILENAME).$(CFG_LIB_FILE_EXT)" ...
                               if exist $(LIB_FILENAME).$(CFG_LIB_FILE_EXT) del $(LIB_FILENAME).$(CFG_LIB_FILE_EXT)

#--- create library ------------------------------------------------------------
archive                        .LIBRARY : $(LIB_OBJ_FILES)


                               %echo   archiving : $(.NEWSOURCES)
                               %if ($(CFG_BOARD_TYPE) == 1006) || ($(CFG_BOARD_TYPE) == x86_32) || ($(CFG_BOARD_TYPE) == 2511) || ($(CFG_BOARD_TYPE) == 1007) || ($(CFG_BOARD_TYPE) == 1202) || ($(CFG_BOARD_TYPE) == 1403)
                                    %set ARCHIVER             = $(CFG_AR)
                                    %set CFG_AR_PRE_OPTIONS   = $(CFG_AR_RPLOBJ_OPT) $(LIB_FILENAME).$(CFG_LIB_FILE_EXT)                                    
                               %else
                                    %set ARCHIVER             = "$(DSPACE_ROOT)\Exe\DsExtPPCLib.exe"      # use gnu archiver also for Microtec objects
                                    %set CFG_AR_PRE_OPTIONS   = -rsv $(LIB_FILENAME).$(CFG_LIB_FILE_EXT)
                                    
                               %endif
                               
                               # create response file for the archiver                              
                               echo $(CFG_AR_PRE_OPTIONS) >$(RESPONSE_FILE)
                               %foreach obj in $(.NEWSOURCES) # using a for-loop to avoid command lines exceeding max. length
                                  echo $(obj) >>$(RESPONSE_FILE)
                               %endfor                               
                               # invoke archiver with generated response file
                               $(CFG_INVOKE) $(ARCHIVER) @$(RESPONSE_FILE)
                               
                               %set BUILD_FLAG = "yes"
                               if exist $(RESPONSE_FILE) del $(RESPONSE_FILE)
                               %echo library successfully built.
                               %echo

%include "$(CFG_MK_TEMPLATE_PATH)\DsBuildCommonRules.mk"    # common makefile rules
