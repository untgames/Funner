###################################################################################################
#Сборка под Alchemy
###################################################################################################
ifeq ($(strip $(ALCHEMY_HOME)),)
  $(error "Please set ALCHEMY_HOME variable in your environment")
endif

ifeq ($(strip $(CYGHOME)),)
  $(error "Please set CYGHOME in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
CYGWIN_BIN        := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
ALCHEMY_HOME      := /$(subst :,,$(call convert_path,$(ALCHEMY_HOME)))
BUILD_PATHS       := $(ALCHEMY_HOME)/achacks:$(ALCHEMY_HOME)/bin:$(CYGWIN_BIN)
PROFILES          += unistd haswchar no_dll alchemy
EXE_SUFFIX        :=
DLL_SUFFIX        := .so
DLL_PREFIX        := lib
COMPILER_GCC      := /cygdrive$(ALCHEMY_HOME)/achacks/gcc
LINKER_GCC        := /cygdrive$(ALCHEMY_HOME)/achacks/g++
LIB_GCC           := /cygdrive$(ALCHEMY_HOME)/achacks/ar
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -DALCHEMY
COMMON_LINK_FLAGS += -Wl,-L,$(DIST_BIN_DIR)
CYGWIN            := nodosfilewarning

export CYGWIN

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Переопределения вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(CYGWIN_BIN)/sh -c '$(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)'
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(CYGWIN_BIN)/sh -c '$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9)'
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && $(CYGWIN_BIN)/sh -c '$(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)'
endef
