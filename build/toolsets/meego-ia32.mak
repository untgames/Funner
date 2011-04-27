###################################################################################################
#Сборка под MeeGo
###################################################################################################
ifeq ($(strip $(MEEGO_SDK)),)
  $(error "Please set MEEGO_SDK variable in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
MEEGO_VERSION     := 1.1
PROFILES          += unistd meego meego_ia32 x11 linux glx has_windows
REMOTE_DEBUG_DIR  ?= //home/leny/funner
EXE_SUFFIX        :=
DLL_SUFFIX        := .so
DLL_PREFIX        := lib
MEEGO_SDK         := $(subst \,/,$(MEEGO_SDK))
MEEGO_GCC         := $(MEEGO_SDK)/MADDE/targets/meego-netbook-ia32-1.1
MEEGO_SYSROOT     := $(MEEGO_SDK)/MADDE/sysroots/meego-core-ia32-madde-sysroot-$(MEEGO_VERSION)-fs
COMPILER_GCC      := $(MEEGO_GCC)/bin/gcc
LINKER_GCC        := $(MEEGO_GCC)/bin/g++
LIB_GCC           := $(MEEGO_GCC)/bin/ar
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -DMEEGO "-I$(MEEGO_SYSROOT)/usr/include" -O2 -funroll-loops
COMMON_LINK_FLAGS += "-L$(MEEGO_SYSROOT)/usr/lib" "-L$(MEEGO_SYSROOT)/lib" "-L$(MEEGO_SYSROOT)/usr/lib" -O2 -lm -lpthread -lstdc++ -lrt -Wl,-L,$(DIST_BIN_DIR)
MEEGO_HOST        := $(strip $(MEEGO_HOST))
MEEGO_USER        := $(strip $(MEEGO_USER))
MEEGO_PASSWORD    := $(strip $(MEEGO_PASSWORD))
MEEGO_PORT        := $(strip $(MEEGO_PORT))
MEEGO_DISPLAY     := $(strip $(MEEGO_DISPLAY))

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
$(call ssh_run,"mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(MEEGO_USER)@$(MEEGO_HOST),$(MEEGO_PASSWORD),$(MEEGO_PORT)) && \
$(call ssh_copy,$(strip $1),$(MEEGO_USER)@$(MEEGO_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT),$(MEEGO_PASSWORD),$(MEEGO_PORT)) && \
$(call ssh_run,"chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(MEEGO_USER)@$(MEEGO_HOST),$(MEEGO_PASSWORD),$(MEEGO_PORT))
endef

#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(call ssh_run,"export DISPLAY=$(MEEGO_DISPLAY) PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)",$(MEEGO_USER)@$(MEEGO_HOST),$(MEEGO_PASSWORD),$(MEEGO_PORT))
endef
