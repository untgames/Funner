###################################################################################################
#�������� ������� ������������
###################################################################################################
ifeq ($(strip $(CYGHOME)),)
  $(error "Please set CYGHOME in your environment")
endif

CYGHOME := /$(subst :,,$(call convert_path,$(CYGHOME)))

###################################################################################################
#���������
###################################################################################################
PROFILES         += cygwin unistd haswchar
CYGWIN_BIN       := $(CYGHOME)/bin
COMPILER_GCC     := export PATH=$(CYGWIN_BIN):$$PATH && gcc
LINKER_GCC       := export PATH=$(CYGWIN_BIN):$$PATH && g++
LIB_GCC          := export PATH=$(CYGWIN_BIN):$$PATH && ar
ADDITIONAL_PATHS += $(CYGWIN_BIN)

###################################################################################################
#����������� ������������� ������� ������
###################################################################################################
include $(TOOLSETS_DIR)/g++.mak
