###################################################################################################
#Build for linux g++
###################################################################################################

###################################################################################################
#Constants
###################################################################################################
EXE_SUFFIX        :=
DLL_SUFFIX        := .so
DLL_PREFIX        := lib
DLL_PATH          := LD_LIBRARY_PATH
COMMON_CFLAGS     += -DLINUX -fPIC -fvisibility=hidden -m32
COMMON_CFLAGS     += -fno-strict-aliasing
COMMON_LINK_FLAGS += -Wl,-rpath -Wl,'$$ORIGIN' -Wl,--gc-sections -m32
LIB_GCC           := ar
SHELL             := /bin/bash

PROFILES += linux linux-x86 unistd x11 glx haswchar pthread_static_library has_windows x86

ADDITIONAL_PATHS=$$(pwd)/$(BUILD_DIR)tools/linux

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Files linking (output file name, files list, static libraries directories list,
#include link symbols list, link flags)
###################################################################################################
define tools.link.dll
-shared -Wl,-undefined -Wl,error
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,,$5 $(foreach link,$4,-Wl,-u,$(link)),$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR))
endef
