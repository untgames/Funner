###################################################################################################
#Сборка под Alchemy
###################################################################################################
ifeq ($(strip $(ALCHEMY_HOME)),)
  $(error "Please set ALCHEMY_HOME variable in your environment")
endif

ifeq ($(strip $(FLEX_HOME)),)
  $(error "Please set FLEX_HOME variable in your environment")
endif

ifeq ($(strip $(CYGHOME)),)
  $(error "Please set CYGHOME in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
CYGWIN_BIN        := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
ALCHEMY_SDK       := /$(subst :,,$(call convert_path,$(ALCHEMY_HOME)))
FLEX_SDK          := /$(subst :,,$(call convert_path,$(FLEX_HOME)))
BUILD_PATHS       := $(ALCHEMY_SDK)/achacks:$(ALCHEMY_SDK)/bin:$(FLEX_SDK)/bin:$(CYGWIN_BIN)
PROFILES          += unistd haswchar no_dll alchemy
EXE_SUFFIX        := .swc
DLL_SUFFIX        :=
DLL_PREFIX        :=
COMPILER_GCC      := /cygdrive$(ALCHEMY_SDK)/achacks/gcc
LINKER_GCC        := /cygdrive$(ALCHEMY_SDK)/achacks/g++
LIB_GCC           := /cygdrive$(ALCHEMY_SDK)/achacks/ar
ADL               := /cygdrive$(FLEX_SDK)/bin/adl
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -DALCHEMY
COMMON_LINK_FLAGS += -swc -Wl,-L,$(DIST_BIN_DIR)
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
