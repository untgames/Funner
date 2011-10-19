###################################################################################################
#Сборка под BlackBerry TabletOS
###################################################################################################
ifeq ($(strip $(TABLETOS_NDK)),)
  $(error "Please set TABLETOS_NDK variable in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
QNX_TARGET               := $(TABLETOS_NDK)/target/qnx6
QNX_HOST                 := $(TABLETOS_NDK)/host/win32/x86
PROFILES                 += unistd tabletos has_windows has_iconv gles egl
REMOTE_DEBUG_DIR         ?= //accounts/devuser/funner
EXE_SUFFIX               :=
DLL_SUFFIX               := .so
DLL_PREFIX               := lib
TABLETOS_NDK_TOOL_PREFIX := i486-pc-nto-qnx6.5.0-
TABLETOS_NDK             := $(subst \,/,$(TABLETOS_NDK))
TABLETOS_NDK_GCC         := $(TABLETOS_NDK)/host/win32/x86/usr
COMPILER_GCC             := $(TABLETOS_NDK_GCC)/bin/$(TABLETOS_NDK_TOOL_PREFIX)gcc
LINKER_GCC               := $(TABLETOS_NDK_GCC)/bin/$(TABLETOS_NDK_TOOL_PREFIX)g++
LIB_GCC                  := $(TABLETOS_NDK_GCC)/bin/$(TABLETOS_NDK_TOOL_PREFIX)ar
COMMON_CPPFLAGS          += -fexceptions -frtti
COMMON_CFLAGS            += -DTABLETOS -O2 -Wno-strict-aliasing -I$(TABLETOS_NDK)/target/target-override/usr/include
COMMON_LINK_FLAGS        += -Wl,-L,$(DIST_BIN_DIR)
TABLETOS_HOST            := $(strip $(TABLETOS_HOST))
TABLETOS_USER            := $(strip $(TABLETOS_USER))
TABLETOS_PORT            := $(strip $(TABLETOS_PORT))

ifneq (,$(filter Win%,$(OS)))
TABLETOS_KEY         := $(BUILD_DIR)/platforms/tabletos/id_rsa.ppk
else
TABLETOS_KEY         := $(BUILD_DIR)/platforms/tabletos/id_rsa
endif

export QNX_HOST
export QNX_TARGET

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Переопределения вызовов утилит
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

define tools.link.dll
-shared
endef

###################################################################################################
#Отладка на устройстве
###################################################################################################

#Копирование файла на устройство (имя локальных файлов, имя удалённого каталога)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
$(call ssh_run,"mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(TABLETOS_USER)@$(TABLETOS_HOST),,$(TABLETOS_PORT),$(TABLETOS_KEY)) && \
$(call ssh_copy,$(strip $1),$(TABLETOS_USER)@$(TABLETOS_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT),,$(TABLETOS_PORT),$(TABLETOS_KEY)) && \
$(call ssh_run,"chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(TABLETOS_USER)@$(TABLETOS_HOST),,$(TABLETOS_PORT),$(TABLETOS_KEY))
endef

#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(call ssh_run,"export PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)",$(TABLETOS_USER)@$(TABLETOS_HOST),,$(TABLETOS_PORT),$(TABLETOS_KEY))
endef
