#******************************************************************************
#**  CarMaker - Version 11.0.1
#**  Vehicle Dynamics Simulation Toolkit
#**
#**  Copyright (C)   IPG Automotive GmbH
#**                  Bannwaldallee 60             Phone  +49.721.98520.0
#**                  76185 Karlsruhe              Fax    +49.721.98520.99
#**                  Germany                      WWW    www.ipg-automotive.com
#******************************************************************************
#**
#**  Application Makefile
#**  Run with 'make V=1 ...' for a verbose build.
#**
#******************************************************************************

include C:/IPG/carmaker/win64-11.0.1/include/MakeDefs.win64


# @@PLUGIN-BEGIN-MATLAB@@ - Automatically generated code - don't edit!
# @@PLUGIN-END@@

# Matlab version and Matlab installation directory.
# Uncomment only when overriding or not using CarMaker's model plugin mechanism.
#MATSUPP_MATVER =	R2019a
#MAT_HOME =		C:/Program\ Files/MATLAB/R2019a

ifndef MAT_HOME
$(error Matlab version and directory in MATSUPP_MATVER/MAT_HOME required)
endif


include $(CARMAKER_DIR_MK)/CM4SL/MakeDefs4SL


APP_VER =		"Car_Generic ?.?"
APP_NAME =		libcarmaker4sl.$(MAT_EXT)

#OPT_CFLAGS =		-g -O1

# Use the following line if you want to #include Matlab header files.
# Be sure to #include Matlab header files _before_ #including CarMaker4SL.h.
#CFLAGS +=	$(MAT_CFLAGS)

LD_LIBS =		$(CAR4SL_LIB) \
			$(CARMAKER4SL_LIB) $(DRIVER_LIB) $(ROAD_LIB) $(TIRE_LIB)
OBJS =			User.cm4sl.o CM_Main.cm4sl.o CM_Vehicle.cm4sl.o
#OBJS +=		IO.cm4sl.o

# Prepend local include/library directory to include path:
# PREINC_CFLAGS +=	-I../include -I../lib/$(ARCH) -I../lib

# Append local include/library directory to include path:
# INC_CFLAGS +=		-I../include -I../lib/$(ARCH) -I../lib


### Linking with RTW-built Simulink models

#LD_LIBS +=		$(MATSUPP_LIB)

#OBJS += libSimuModel_$(ARCH).a

# @@PLUGIN-BEGIN-LIBS@@ - Automatically generated code - don't edit!
# @@PLUGIN-END@@

### END (Linking with RTW-built Simulink models)


default:	$(APP_NAME)

$(APP_NAME):	$(OBJS_$(ARCH)) $(OBJS) $(LD_LIBS_MK) app_tmp.cm4sl.o
	$(QECHO) " LD     $@"
ifneq ($(filter linux linux64, $(ARCH)), )
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -o $@ \
		-shared -Wl,-Bsymbolic,--allow-shlib-undefined -u CarMaker4SL_CMLib \
		$(OBJS_$(ARCH)) $(OBJS) \
		$(LD_LIBS) $(SUPP4SL_LIB) $(MAT_LIBS) \
		app_tmp.cm4sl.o $(LD_LIBS_OS)
endif
ifneq ($(filter win32 win64, $(ARCH)), )
	$Q $(CC) $(CFLAGS) $(LDFLAGS) -o $@ \
		-static-libgcc -static-libstdc++ \
		-shared -Wl,-Bsymbolic,--allow-shlib-undefined \
		$(OBJS_$(ARCH)) $(OBJS) \
		$(LD_LIBS) $(SUPP4SL_LIB) $(MAT_LIBS) \
		app_tmp.cm4sl.o $(LD_LIBS_OS)
endif

clean:
	-rm -f 	*~ *% *.o *.obj *.res *.$(SO_EXT) *.$(MAT_EXT) core

app_tmp.c:	Makefile $(OBJS_$(ARCH)) $(OBJS) $(LD_LIBS_MK)
	$(QECHO) " MK     $@"
	$Q $(CREATE_INFO_CMD)

depend .depend: Makefile
	$(QECHO) " MK     $@"
	@echo -n "" >.depend
ifneq ($(wildcard *.c),)
	$Q-$(CC)  $(CFLAGS)   $(CFLAGS_CM4SL)   $(DEPCFLAGS)   *.c   >>.depend 2>/dev/null
endif
ifneq ($(wildcard *.cpp),)
	$Q-$(CXX) $(CXXFLAGS) $(CXXFLAGS_CM4SL) $(DEPCXXFLAGS) *.cpp >>.depend 2>/dev/null
endif
include .depend
