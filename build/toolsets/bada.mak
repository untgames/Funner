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
BADA_APP_NAME          := FunnerTestApplication
BADA_APP_ID            := 93bt1p123e
BADA_APP_SECRET        := 9C645DDBA19C71BAD1204DA4DAA7A0B9
BADA_SDK_HOME          := $(call convert_path,$(BADA_SDK_HOME))
BADA_TOOLCHAIN_DIR     := $(BADA_SDK_HOME)/Tools/Toolchains/$(BADA_TOOLCHAIN)
BADA_TOOLCHAIN_BIN_DIR := $(BADA_TOOLCHAIN_DIR)/bin
COMPILER_GCC           := $(BADA_TOOLCHAIN_BIN_DIR)/$(BADA_TOOLCHAIN_PREFIX)gcc
LINKER_GCC             := $(BADA_TOOLCHAIN_BIN_DIR)/$(BADA_TOOLCHAIN_PREFIX)g++
LIB_GCC                := $(BADA_TOOLCHAIN_BIN_DIR)/$(BADA_TOOLCHAIN_PREFIX)ar
COMMON_CFLAGS          += -fno-strict-aliasing -DDEFAULT_APP_ID='L"$(BADA_APP_ID)"' -DDEFAULT_APP_SECRET='L"$(BADA_APP_SECRET)"' -DDEFAULT_APP_NAME='L"$(BADA_APP_NAME)"' -DBADA -I$(BADA_SDK_HOME)/Include 
COMMON_LINK_FLAGS      +=
DLL_SUFFIX             := .so

###################################################################################################
#Подключение родительского скрипта сборки
###################################################################################################
include $(TOOLSETS_DIR)/g++.mak
