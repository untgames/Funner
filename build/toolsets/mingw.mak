###################################################################################################
#Check configuration existence
###################################################################################################
ifeq ($(strip $(MINGW)),)
  $(error "Please set MINGW variable in your environment")
endif

CYGHOME := /$(subst :,,$(call convert_path,$(MINGW)))

###################################################################################################
#Constants
###################################################################################################
PROFILES          += mingw win32 has_windows haswchar x86 g++x86 x86_win32
MINGW             := $(call convert_path,$(MINGW))
MINGW_BIN         := $(MINGW)/bin
COMPILER_GCC      := $(MINGW_BIN)/gcc.exe
LINKER_GCC        := $(MINGW_BIN)/g++.exe
LIB_GCC           := $(MINGW_BIN)/ar
COMMON_CFLAGS     += -mwin32
COMMON_LINK_FLAGS += -Wl,--stack,128000
EXCLUDE_LIBS      := uuid

###################################################################################################
#Include parent build script
###################################################################################################
include $(TOOLSETS_DIR)/g++.mak
include $(TOOLSETS_DIR)/common/mono.mak
