###################################################################################################
#—борка под Emscripten SDK
###################################################################################################
ifeq ($(strip $(EMSCRIPTEN_SDK)),)
  $(error "Please set EMSCRIPTEN_SDK variable in your environment")
endif

ifeq ($(strip $(PYTHON2_SDK)),)
  $(error "Please set PYTHON2_SDK variable in your environment")
endif

###################################################################################################
# онстанты
###################################################################################################
PROFILES          += unistd emscripten has_windows no_dll no_threads
EXE_SUFFIX        := .js
DLL_SUFFIX        :=
DLL_PREFIX        :=
NCLIENT_GCC       := $(subst \,/,$(EMSCRIPTEN_SDK))/emscripten/1.12.0
EMCC              := $(NCLIENT_GCC)/emcc
EMAR              := $(NCLIENT_GCC)/emar
ifneq (,$(filter Win%,$(OS)))
COMPILER_GCC      := $(ROOT)/build/platforms/emscripten/cc
LIB_GCC           := $(ROOT)/build/platforms/emscripten/ar
else
COMPILER_GCC      := $(NCLIENT_GCC)/emcc
LIB_GCC           := $(NCLIENT_GCC)/emar
endif
LINKER_GCC        := $(COMPILER_GCC)
NODE_TOOL         := $(subst \,/,$(EMSCRIPTEN_SDK))/node/0.10.17_32bit/node
COMMON_CPPFLAGS   += -s DISABLE_EXCEPTION_CATCHING=0
COMMON_CFLAGS     +=
COMMON_LINK_FLAGS +=
BUILD_PATHS       += /$(subst :,,$(call convert_path,$(EMSCRIPTEN_SDK)/emscripten/1.12.0)):/$(subst :,,$(call convert_path,$(PYTHON2_SDK)))
ADDITIONAL_PATHS  += $(BUILD_PATHS)

export EMCC
export EMAR

include $(TOOLSETS_DIR)/g++.mak

PROFILES := $(filter-out g++,$(PROFILES))

###################################################################################################
#ѕереопределени€ вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,$3,,$5 $(foreach link,$4,-Wl,-u,$(link)))
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

#¬ыполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run
$(call prepare_to_execute,$2,$3) && $(NODE_TOOL) $(CURDIR)/$(firstword $1)
endef
