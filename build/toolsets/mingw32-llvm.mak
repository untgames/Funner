###################################################################################################
#Check configuration existence
###################################################################################################
ifeq ($(strip $(MINGW_LLVM)),)
  $(error "Please set MINGW_LLVM variable in your environment")
endif

###################################################################################################
#Constants
###################################################################################################
PROFILES          += mingw win32 has_windows haswchar x86 g++x86 x86_win32 llvm
MINGW_LLVM        := $(call convert_path,$(MINGW_LLVM))
MINGW_BIN         := $(MINGW_LLVM)/bin
COMPILER_GCC      := $(MINGW_BIN)/i386-mingw32-llvm-gcc
LINKER_GCC        := $(MINGW_BIN)/i386-mingw32-llvm-g++
LIB_GCC           := $(MINGW_BIN)/llvm-ar
RANLIB_GCC        := $(MINGW_BIN)/llvm-ranlib
COMMON_CFLAGS     += -mwin32 -I "$(MINGW_LLVM)/include" -I "$(MINGW_LLVM)/include/c++/4.2.1" -I "$(MINGW_LLVM)/include/c++/4.2.1/i386-mingw32"
COMMON_LINK_FLAGS += -Wl,--stack,128000 -L "$(MINGW_LLVM)/lib"
EXCLUDE_LIBS      := uuid

###################################################################################################
#Include parent build script
###################################################################################################
include $(TOOLSETS_DIR)/g++.mak
include $(TOOLSETS_DIR)/common/mono.mak

###################################################################################################
#Replacements
###################################################################################################
define tools.lib
$(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9) && $(RANLIB_GCC) $1
endef

#define tools.c++compile
#$(call tools.g++.c++compile,$1,$2,$3,$4,$5,$(subst --no-warn,-w,$6),$7,$8,$9)
#endef
