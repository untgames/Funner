###################################################################################################
#Проверка наличия конфигурации
###################################################################################################
ifeq ($(strip $(DEVKIT_PSP)),)
  $(error "Please set DEVKIT_PSP variable in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
PROFILES          += psp has_windows haswchar no_dll
PSP_BIN           := $(DEVKIT_PSP)/bin
PSP_SDK           := $(DEVKIT_PSP)/psp/sdk
COMPILER_GCC      := $(PSP_BIN)/psp-gcc
LINKER_GCC        := $(PSP_BIN)/psp-gcc
LIB_GCC           := $(PSP_BIN)/psp-ar
EXE_SUFFIX        ?=
DLL_SUFFIX        ?= .so
COMMON_CFLAGS     += -D_PSP_FW_VERSION=150
COMMON_CFLAGS     += -G0
COMMON_CFLAGS     += -I$(PSP_SDK)/include
COMMON_LINK_FLAGS += -Wl,-L,$(PSP_SDK)/lib
#COMMON_LINK_FLAGS += -Wl,-L,$(PSP_SDK)/../lib
COMMON_LINK_FLAGS += -specs=$(PSP_SDK)/lib/prxspecs -Wl,-q,-T$(PSP_SDK)/lib/linkfile.prx
COMMON_LINK_FLAGS += -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk -lstdc++ -lm -lc -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility -lpspuser -lpspkernel -lfunner.extern.psp_addons

###################################################################################################
#Подключение родительского скрипта сборки
###################################################################################################
include $(TOOLSETS_DIR)/g++.mak
