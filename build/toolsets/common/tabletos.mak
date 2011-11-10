###################################################################################################
#—борка под BlackBerry TabletOS
###################################################################################################
ifeq ($(strip $(TABLETOS_NDK)),)
  $(error "Please set TABLETOS_NDK variable in your environment")
endif

ifeq ($(strip $(TABLETOS_ARCHITECTURE)),)
  $(error "Please set TABLETOS_ARCHITECTURE variable in your environment")
endif

ifeq ($(strip $(TABLETOS_NDK_TOOL_PREFIX)),)
  $(error "Please set TABLETOS_NDK_TOOL_PREFIX variable in your environment")
endif

ifeq ($(strip $(TABLETOS_DEVHOST)),)
  ifneq (,$(filter Win%,$(OS)))  
    TABLETOS_DEVHOST         := win32
    TABLETOS_DEVARCHITECTURE := x86
  else
    $(error "Please set TABLETOS_DEVHOST variable in your environment")
  endif
endif

###################################################################################################
# онстанты
###################################################################################################
QNX_TARGET               := $(TABLETOS_NDK)/target/qnx6
QNX_HOST                 := $(TABLETOS_NDK)/host/$(TABLETOS_DEVHOST)/$(TABLETOS_DEVARCHITECTURE)
PROFILES                 += unistd tabletos has_windows has_iconv gles egl no_dll
REMOTE_DEBUG_DIR         ?= //accounts/devuser/funner
EXE_SUFFIX               :=
DLL_SUFFIX               := .so
DLL_PREFIX               := lib
TABLETOS_NDK             := $(subst \,/,$(TABLETOS_NDK))
TABLETOS_NDK_GCC         := $(TABLETOS_NDK)/host/$(TABLETOS_DEVHOST)/$(TABLETOS_DEVARCHITECTURE)/usr
COMPILER_GCC             := $(TABLETOS_NDK_GCC)/bin/$(TABLETOS_NDK_TOOL_PREFIX)gcc
LINKER_GCC               := $(TABLETOS_NDK_GCC)/bin/$(TABLETOS_NDK_TOOL_PREFIX)g++ 
LIB_GCC                  := $(TABLETOS_NDK_GCC)/bin/$(TABLETOS_NDK_TOOL_PREFIX)ar
PACKAGER                 := $(TABLETOS_NDK_GCC)/bin/blackberry-nativepackager
DEPLOYER                 := $(TABLETOS_NDK_GCC)/bin/blackberry-deploy
CONNECTOR                := $(TABLETOS_NDK_GCC)/bin/blackberry-connect
COMMON_CPPFLAGS          += -fexceptions -frtti
COMMON_CFLAGS            += -fPIC -DTABLETOS -O2 -Wno-strict-aliasing -I$(TABLETOS_NDK)/target/target-override/usr/include -I$(TABLETOS_NDK)/target/qnx6/usr/include/freetype2
COMMON_LINK_FLAGS        += -Wl,--no-undefined -L$(TABLETOS_NDK)/target/target-override/$(TABLETOS_ARCHITECTURE)/lib -L$(TABLETOS_NDK)/target/target-override/$(TABLETOS_ARCHITECTURE)/usr/lib -Wl,-L,$(DIST_BIN_DIR)
TABLETOS_HOST            := $(strip $(TABLETOS_HOST))
TABLETOS_USER            := $(strip $(TABLETOS_USER))
TABLETOS_PORT            := $(strip $(TABLETOS_PORT))
VALID_TARGET_TYPES       += tabletos-bar
TESTS_LAUNCHER           := $(DIST_LIB_DIR)/funner.application.bar

ifneq (,$(filter Win%,$(OS)))
TABLETOS_KEY         := $(BUILD_DIR)/platforms/tabletos/id_rsa.ppk
else
TABLETOS_KEY         := $(BUILD_DIR)/platforms/tabletos/id_rsa
endif

export QNX_HOST
export QNX_TARGET

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#ѕереопределени€ вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,$3,,$5 $(foreach link,$4,-Wl,-u,$(link)))
endef

define tools.link.shared-lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,,,$5 $(foreach dir,$3,-Wl,-L,$(dir)) -shared $(foreach link,$4,-Wl,-u,$(link)))
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link.dll
-shared
endef

###################################################################################################
#ќтладка на устройстве
###################################################################################################

# опирование файла на устройство (им€ локальных файлов, им€ удалЄнного каталога)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
$(call ssh_run,"mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(TABLETOS_USER)@$(TABLETOS_HOST),,$(TABLETOS_PORT),$(TABLETOS_KEY)) && \
$(call ssh_copy,$(strip $1),$(TABLETOS_USER)@$(TABLETOS_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT),,$(TABLETOS_PORT),$(TABLETOS_KEY)) && \
$(call ssh_run,"chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(TABLETOS_USER)@$(TABLETOS_HOST),,$(TABLETOS_PORT),$(TABLETOS_KEY))
endef

#¬ыполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(call ssh_run,"export PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)",$(TABLETOS_USER)@$(TABLETOS_HOST),,$(TABLETOS_PORT),$(TABLETOS_KEY))
endef

#¬ыполнение команды из пакета (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run.tabletos_package
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
export PATH=$$PATH:/$(subst :,,$(call convert_path,$(TABLETOS_NDK_GCC)/bin)) && $(notdir $(DEPLOYER)) -launchApp -device $(TABLETOS_HOST) -password $(TABLETOS_PASSWORD) -package $(TESTS_LAUNCHER) && \
echo "$$(echo $$SUBST_COMMAND) $$(echo $$SUBST_DIR_RESULT) $(args)" | plink -P 1663 -raw $(TABLETOS_HOST)
endef

###################################################################################################
#—борка приложени€
###################################################################################################

#ќбработка цели tabletos-bar (им€ цели)
define process_target.tabletos-bar
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty application name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif
  
  $$(eval $$(call process_target_with_sources,$1))  

  $1.EXE_FILE      := $$($1.TMP_DIR)/$$($1.NAME)$(EXE_SUFFIX)
  TARGET_FILES     := $$(TARGET_FILES) $$($1.EXE_FILE)
  $1.TARGET_DLLS   := $$($1.DLLS:%=$$($1.OUT_DIR)/$(DLL_PREFIX)%$(DLL_SUFFIX))
  $1.BAR_FILE      := $$(DIST_LIB_DIR)/$$($1.NAME).bar
  DIST_DIRS        := $$(DIST_DIRS) $$(dir $$($1.BAR_FILE))
  $1.RES_DIR       := $$(COMPONENT_DIR)$$($1.RES_DIR)
  $1.MANIFEST_FILE := $$(COMPONENT_DIR)$$($1.MANIFEST_FILE)
  $1.RES_FILES     := $$(wildcard $$($1.RES_DIR)/*)  

  build: $$($1.BAR_FILE)
  
  ifeq (,$$($1.EXECUTION_DIR))
    $1.EXECUTION_DIR := $$($1.OUT_DIR)
  endif
  
  $$($1.EXE_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS) $$($1.OBJECT_FILES)
		@echo Linking $$(notdir $$@)...
		@$$(call $$(if $$($1.LINK_TOOL),$$($1.LINK_TOOL),$(LINK_TOOL)),$$@,$$($1.OBJECT_FILES) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))
		
  $$($1.BAR_FILE): $$($1.EXE_FILE) $$($1.RES_FILES) $$($1.MANIFEST_FILE)
		@echo Packaging $$(notdir $$@)...
		@export PATH=$$$$PATH:/$(subst :,,$(call convert_path,$(TABLETOS_NDK_GCC)/bin)) && $(notdir $(PACKAGER)) -package $$@ -devMode $$($1.MANIFEST_FILE) -C $$($1.TMP_DIR) $$($1.EXE_FILE) -C $$($1.RES_DIR) $$($1.RES_FILES) -e $(TABLETOS_NDK)/target/target-override/x86/usr/lib/libbps.so lib/libbps.so.1 -e $(TABLETOS_NDK)/target/target-override/x86/usr/lib/libOpenAL.so lib/libOpenAL.so.1
		
#Install package
  install: INSTALL.$1
  uninstall: UNINSTALL.$1

  .PHONY: INSTALL.$1 UNINSTALL.$1

  INSTALL.$1: $$($1.TMP_DIR) $$($1.INSTALLATION_FLAG)

  $$($1.INSTALLATION_FLAG): $$($1.BAR_FILE)
		@echo Installing $$(notdir $$<)...
		@export PATH=$$$$PATH:/$(subst :,,$(call convert_path,$(TABLETOS_NDK_GCC)/bin)) && $(notdir $(DEPLOYER)) -uninstallApp -device $(TABLETOS_HOST) -password $(TABLETOS_PASSWORD) -package $$($1.BAR_FILE)
		@export PATH=$$$$PATH:/$(subst :,,$(call convert_path,$(TABLETOS_NDK_GCC)/bin)) && $(notdir $(DEPLOYER)) -installApp -device $(TABLETOS_HOST) -password $(TABLETOS_PASSWORD) -package $$($1.BAR_FILE)
		@touch $$@

  $1.UNINSTALL:
		@export PATH=$$$$PATH:/$(subst :,,$(call convert_path,$(TABLETOS_NDK_GCC)/bin)) && $(notdir $(DEPLOYER)) -uninstallApp -device $(TABLETOS_HOST) -password $(TABLETOS_PASSWORD) -package $$($1.BAR_FILE)  
		
#Run package
  run: RUN.$1
  
  .PHONY: RUN.$1
  
  RUN.$1: INSTALL.$1
		@export PATH=$$$$PATH:/$(subst :,,$(call convert_path,$(TABLETOS_NDK_GCC)/bin)) && $(notdir $(DEPLOYER)) -launchApp -device $(TABLETOS_HOST) -password $(TABLETOS_PASSWORD) -package $$($1.BAR_FILE)
#		@echo "/accounts/devuser/funner/tmp/tabletos-x86/XTL.STL.TESTS/tests/stl/accum1 /accounts/devuser/funner/tmp/tabletos-x86/XTL.STL.TESTS  " | plink -P 1663 -raw $(TABLETOS_HOST)

endef

.PHONY: connect

connect:
		@echo Connecting to TabletOS device...
		@$(CONNECTOR) $(if $(TABLETOS_PASSWORD),-devicePassword $(TABLETOS_PASSWORD)) -targetHost $(TABLETOS_HOST) -sshPublicKey $(BUILD_DIR)/platforms/tabletos/id_rsa.pub
