###################################################################################################
#Сборка под BeagleBoard
###################################################################################################
ifeq ($(strip $(BEAGLEBOARD_SDK)),)
  $(error "Please set BEAGLEBOARD_SDK variable in your environment")
endif

ifeq ($(strip $(BEAGLE_BOARD_USER)),)
  $(error "Please set BEAGLE_BOARD_USER variable in your environment")
endif

ifeq ($(strip $(BEAGLE_BOARD_PASSWORD)),)
  $(error "Please set BEAGLE_BOARD_PASSWORD variable in your environment")
endif

ifeq ($(strip $(BEAGLE_BOARD_HOST)),)
  $(error "Please set BEAGLE_BOARD_HOST variable in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
PROFILES          += unistd beagle-board
REMOTE_DEBUG_DIR  ?= //work/funner
EXE_SUFFIX        :=
DLL_SUFFIX        := .so
DLL_PREFIX        :=
BEAGLEBOARD_SDK   := $(subst \,/,$(BEAGLEBOARD_SDK))
COMPILER_GCC      := $(BEAGLEBOARD_SDK)/bin/arm-none-linux-gnueabi-gcc
LINKER_GCC        := $(BEAGLEBOARD_SDK)/bin/arm-none-linux-gnueabi-g++
LIB_GCC           := $(BEAGLEBOARD_SDK)/bin/arm-none-linux-gnueabi-ar
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -DBEAGLEBOARD -DARM -DARM7 -Wno-uninitialized -Wno-psabi
COMMON_LINK_FLAGS += -lm -pthread

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Переопределения вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,,$4,$5 $(foreach dir,$3,-Wl,-L,$(dir)),$6,$7,$8,$9)
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
plink $(BEAGLE_BOARD_USER)@$(BEAGLE_BOARD_HOST) -pw $(BEAGLE_BOARD_PASSWORD) "mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)" && \
pscp -scp -r -batch -pw $(BEAGLE_BOARD_PASSWORD) $1 $(BEAGLE_BOARD_USER)@$(BEAGLE_BOARD_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)
endef

#Выполнение команды (команда, каталог запуска, базовый каталог, дополнительные пути поиска библиотек и приложений)
define tools.run
export SUBST_DIR_STRING=$$(cd $2 && pwd) SUBST_DIR_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$SUBST_DIR_SUBSTRING/} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) SUBST_CMD_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$SUBST_CMD_SUBSTRING/} && \
plink $(BEAGLE_BOARD_USER)@$(BEAGLE_BOARD_HOST) -pw $(BEAGLE_BOARD_PASSWORD) "chmod 755 $$(echo $$SUBST_COMMAND) && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo	$$SUBST_COMMAND) $(subst $(firstword $1),,$1)"
endef
