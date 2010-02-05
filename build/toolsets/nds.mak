###################################################################################################
#Проверка наличия конфигурации
###################################################################################################
ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifeq ($(strip $(DEVKITPRO)),)
  $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPRO")
endif

###################################################################################################
#Константы сборки под NDS
###################################################################################################
ARCH              := -mthumb -mthumb-interwork
LIBNDS            := $(DEVKITPRO)/libnds
OBJCOPY           := $(DEVKITARM)/bin/arm-eabi-objcopy.exe
NDSTOOL           := $(DEVKITARM)/bin/ndstool.exe
COMMON_CFLAGS     += -g -Wall -O2 -mcpu=arm9tdmi -mtune=arm9tdmi -fomit-frame-pointer -ffast-math $(ARCH) -DARM9 -I$(LIBNDS)/include
COMMON_LINK_FLAGS += -specs=ds_arm9.specs -g $(ARCH) -mno-fpu -L$(LIBNDS)/lib -lnds9
PROFILES          += nds
COMPILER_GCC      := $(DEVKITARM)/bin/arm-eabi-gcc.exe
LINKER_GCC        := $(DEVKITARM)/bin/arm-eabi-g++.exe
LIB_GCC           := $(DEVKITARM)/bin/arm-eabi-ar.exe
EXE_SUFFIX        := .nds
PROFILES          += unistd no_windows haswchar

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки)
###################################################################################################
define tools.link
$(RM) $(basename $1).arm9 $(basename $1).elf && \
$(call tools.g++.link,$(basename $1).elf,$2,$3,$4,$5,$6,$7,$8,$9) &&\
$(OBJCOPY) -O binary $(basename $1).elf $(basename $1).arm9 &&\
$(NDSTOOL) -c $(basename $1).nds -9 $(basename $1).arm9 && \
$(RM) $(basename $1).arm9 $(basename $1).elf
endef
