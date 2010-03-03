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
PROFILES               += bada egl gles
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
