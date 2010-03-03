###################################################################################################
#Проверка наличия конфигурации
###################################################################################################
ifeq ($(strip $(BADA_SDK_HOME)),)
  $(error "Please set BADA_SDK_HOME variable in your environment")
endif

ifeq ($(strip $(BADA_TOOLCHAIN)),)
  $(error "No BADA_TOOLCHAIN variable detected")
endif

ifeq ($(strip $(BADA_TOOLCHAIN_PREFIX)),)
  $(error "No BADA_TOOLCHAIN_PREFIX variable detected")
endif

###################################################################################################
#Константы
###################################################################################################
PROFILES               += bada
BADA_SDK_HOME          := $(call convert_path,$(BADA_SDK_HOME))
BADA_TOOLCHAIN_DIR     := $(BADA_SDK_HOME)/Tools/Toolchains/$(BADA_TOOLCHAIN)
BADA_TOOLCHAIN_BIN_DIR := $(BADA_TOOLCHAIN_DIR)/bin
COMPILER_GCC           := $(BADA_TOOLCHAIN_BIN_DIR)/$(BADA_TOOLCHAIN_PREFIX)gcc
LINKER_GCC             := $(BADA_TOOLCHAIN_BIN_DIR)/$(BADA_TOOLCHAIN_PREFIX)g++
LIB_GCC                := $(BADA_TOOLCHAIN_BIN_DIR)/$(BADA_TOOLCHAIN_PREFIX)ar
COMMON_CFLAGS          += -DBADA -fno-strict-aliasing
COMMON_LINK_FLAGS      +=
DLL_SUFFIX             := .so

###################################################################################################
#Подключение родительского скрипта сборки
###################################################################################################
include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки)
###################################################################################################
#define tools.link.dll
#-shared -Wl,-undefined -Wl,error
#endef

#define tools.link
#$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR))
#endef
