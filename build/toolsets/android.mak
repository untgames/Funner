###################################################################################################
#Сборка под Android
###################################################################################################
ifeq ($(strip $(ANDROID_NDK)),)
  $(error "Please set ANDROID_NDK variable in your environment")
endif

ifeq ($(strip $(ANDROID_SDK)),)
  $(error "Please set ANDROID_SDK variable in your environment")
endif

ifeq ($(strip $(CYGHOME)),)
  $(error "Please set CYGHOME in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
PROFILES          += android no_dll unistd
#REMOTE_DEBUG_DIR  ?= //sdcard/tests/funner
REMOTE_DEBUG_DIR  ?= //data/misc/tests/funner
EXE_SUFFIX        :=
DLL_SUFFIX        := so #no suffix, this is for dll correct dll dispatch at link
DLL_PREFIX        :=
NDK_HOST          := windows
ANDROID_PLATFORM  := android-3
NDK_ROOT          := /$(subst :,,$(call convert_path,$(ANDROID_NDK)))
SDK_ROOT          := /$(subst :,,$(call convert_path,$(ANDROID_SDK)))
ARM_EABI_DIR      := $(NDK_ROOT)/build/prebuilt/$(NDK_HOST)/arm-eabi-4.2.1
CYGWIN_BIN        := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
GCC_TOOLS_DIR     := $(ARM_EABI_DIR)/bin
COMPILER_GCC      := $(GCC_TOOLS_DIR)/arm-eabi-gcc.exe
LINKER_GCC        := $(GCC_TOOLS_DIR)/arm-eabi-gcc.exe
LIB_GCC           := $(GCC_TOOLS_DIR)/arm-eabi-ar
ANDROID_TOOLS_DIR := $(SDK_ROOT)/tools
ADB               := $(ANDROID_TOOLS_DIR)/adb
ADDITIONAL_PATHS  += $(CYGWIN_BIN)
BUILD_PATHS       := $(CYGWIN_BIN):$(GCC_TOOLS_DIR):$(ARM_EABI_DIR)/libexec/gcc/arm-eabi/4.2.1
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -mandroid -ffunction-sections -fdata-sections -Os -g \
                    --sysroot=$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm \
                    -fPIC -fvisibility=hidden -D__NEW__ -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ \
                    -D__ARM_ARCH_5TE__ -DANDROID -DSK_RELEASE -DNDEBUG \
                    -UDEBUG -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -fpic -ffunction-sections \
                    -funwind-tables -fstack-protector -fmessage-length=0 -Bdynamic -fno-strict-aliasing
COMMON_LINK_FLAGS += --sysroot=$(NDK_ROOT)/build/platforms/android-4/arch-arm
COMMON_LINK_FLAGS += -Bdynamic -Wl,-dynamic-linker,//system/bin/linker
COMMON_LINK_FLAGS += -Wl,--gc-sections -Wl,-z,nocopyreloc
COMMON_LINK_FLAGS += -Wl,-L,$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib
COMMON_LINK_FLAGS += -Wl,-rpath-link=$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib
COMMON_LINK_FLAGS += -nostdlib -lc -llog -lm -lstdc++ \
                     --no-undefined -z $(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib/crtbegin_dynamic.o
CYGWIN            := nodosfilewarning

export CYGWIN

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

###################################################################################################
#Отладка на устройстве
###################################################################################################

#Копирование файла на устройство (имя локального файла, имя удалённого файла)
define tools.install
 echo -n "Install $(notdir $1): " &&  $(ADB) push $1 $(REMOTE_DEBUG_DIR)/$2
endef

#Выполнение команды (команда, каталог запуска, базовый каталог, дополнительные пути поиска библиотек и приложений, файл результатов)
define tools.run
$(ADB) shell "chmod 775 $(REMOTE_DEBUG_DIR)/$3/$(notdir $(firstword $1)) && cd $(REMOTE_DEBUG_DIR)/$3/ && $(REMOTE_DEBUG_DIR)/$3/$(notdir $(firstword $1)) $(subst $(firstword $1),,$1) $(if $5, > $(REMOTE_DEBUG_DIR)/$3/$(notdir $5))" $(if $5, && $(ADB) pull $(REMOTE_DEBUG_DIR)/$3/$(notdir $5) "$(CURDIR)/$5")
endef
