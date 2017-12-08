###################################################################################################
#Build for Emscripten SDK
###################################################################################################
ifeq ($(strip $(EMSCRIPTEN_SDK)),)
  $(error "Please set EMSCRIPTEN_SDK variable in your environment")
endif

ifeq ($(strip $(PYTHON2_SDK)),)
  $(error "Please set PYTHON2_SDK variable in your environment")
endif

###################################################################################################
#Constants
###################################################################################################
PROFILES          += emscripten unistd has_windows no_dll no_threads
EXE_SUFFIX        := .js
DLL_SUFFIX        := .so
DLL_PREFIX        :=
EMSCRIPTEN_TAG    := tag-1.37.24
EMSCRIPTEN_ROOT   := $(subst \,/,$(EMSCRIPTEN_SDK))/emscripten/$(EMSCRIPTEN_TAG)
EMCC              := $(EMSCRIPTEN_ROOT)/emcc
EMAR              := $(EMSCRIPTEN_ROOT)/emar
ifneq (,$(filter Win%,$(OS)))
COMPILER_GCC      := $(ROOT)/build/platforms/emscripten/cc
LIB_GCC           := $(ROOT)/build/platforms/emscripten/ar
else
COMPILER_GCC      := $(EMCC)
LIB_GCC           := $(EMAR)
endif
LINKER_GCC        := $(COMPILER_GCC)
NODE_TOOL         := $(subst \,/,$(EMSCRIPTEN_SDK))/node/4.1.1_64bit/bin/node
TOTAL_MEMORY      := $(shell expr 256 \* 1024 \* 1024) # 256 MiB
COMMON_CPPFLAGS   += -s TOTAL_MEMORY=$(TOTAL_MEMORY)
COMMON_CPPFLAGS   += -s DISABLE_EXCEPTION_CATCHING=0
COMMON_CPPFLAGS   += -s USE_PTHREADS=0
COMMON_CPPFLAGS   += -s ASSERTIONS=1
COMMON_CPPFLAGS   += -O2
COMMON_LINK_FLAGS += -O2
COMMON_LINK_FLAGS += -s TOTAL_MEMORY=$(TOTAL_MEMORY)
BUILD_PATHS       += /$(subst :,,$(call convert_path,$(EMSCRIPTEN_ROOT))):/$(subst :,,$(call convert_path,$(PYTHON2_SDK)))
ADDITIONAL_PATHS  += $(BUILD_PATHS)

export EMCC
export EMAR

include $(TOOLSETS_DIR)/g++.mak

PROFILES := $(filter-out g++,$(PROFILES))

###################################################################################################
#Override utilities calling
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,$3,,$5 $(foreach link,$4,-u $(link)))
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && export FILE_COUNTER=0 FILE_LIST="" && for file in $2; do export FILE_COUNTER=$$(($$FILE_COUNTER + 1)) && FILE_LIST="$$FILE_LIST $$file"; if [ $$FILE_COUNTER -eq 256 ]; then $(call tools.g++.lib,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9); export FILE_COUNTER=0 FILE_LIST=""; fi; done && $(call tools.g++.lib,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9)
endef

#Execute commands (command, launch directory, additional search paths for libraries and executables, dlls list)
define tools.run
$(call prepare_to_execute,$2,$3) && $(NODE_TOOL) $(CURDIR)/$(firstword $1)
endef
