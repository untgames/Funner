###################################################################################################
#Check configuration existence
###################################################################################################
ifeq ($(strip $(CYGHOME)),)
  $(error "Please set CYGHOME in your environment")
endif

CYGHOME := /$(subst :,,$(call convert_path,$(CYGHOME)))

###################################################################################################
#Constants
###################################################################################################
PROFILES         += cygwin unistd haswchar pthread_static_library x86 g++x86
CYGWIN_BIN       := $(CYGHOME)/bin
COMPILER_GCC     := export PATH=$(CYGWIN_BIN):$$PATH && gcc
LINKER_GCC       := export PATH=$(CYGWIN_BIN):$$PATH && g++
LIB_GCC          := export PATH=$(CYGWIN_BIN):$$PATH && ar
ADDITIONAL_PATHS += $(CYGWIN_BIN)

###################################################################################################
#Include parent build script
###################################################################################################
include $(TOOLSETS_DIR)/g++.mak
