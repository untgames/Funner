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
REMOTE_DEBUG_DIR  ?= //sdcard/funner
EXE_SUFFIX        :=
DLL_SUFFIX        := so #no suffix, this is for dll correct dll dispatch at link
DLL_PREFIX        :=
NDK_HOST          := windows
ANDROID_PLATFORM  := android-8
NDK_ROOT          := /$(subst :,,$(call convert_path,$(ANDROID_NDK)))
SDK_ROOT          := /$(subst :,,$(call convert_path,$(ANDROID_SDK)))
ARM_EABI_DIR      := $(NDK_ROOT)/build/prebuilt/$(NDK_HOST)/arm-eabi-4.4.0
CYGWIN_BIN        := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
GCC_TOOLS_DIR     := $(ARM_EABI_DIR)/bin
COMPILER_GCC      := $(GCC_TOOLS_DIR)/arm-eabi-gcc.exe
LINKER_GCC        := $(GCC_TOOLS_DIR)/arm-eabi-gcc.exe
LIB_GCC           := $(GCC_TOOLS_DIR)/arm-eabi-ar
ANDROID_TOOLS_DIR := $(SDK_ROOT)/tools
ADB               := $(ANDROID_TOOLS_DIR)/adb
ADDITIONAL_PATHS  += $(CYGWIN_BIN)
BUILD_PATHS       := $(CYGWIN_BIN):$(GCC_TOOLS_DIR):$(ARM_EABI_DIR)/libexec/gcc/arm-eabi/4.4.0
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_CFLAGS     += -mandroid -ffunction-sections -fdata-sections -Os -g \
                    --sysroot=$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm \
                    -fPIC -fvisibility=hidden -D__NEW__ -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ \
                    -D__ARM_ARCH_5TE__ -DANDROID -DARM -DSK_RELEASE -DNDEBUG \
                    -UDEBUG -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -fpic -ffunction-sections \
                    -funwind-tables -fstack-protector -fmessage-length=0 -Bdynamic -fno-strict-aliasing
COMMON_LINK_FLAGS += -Wl,-L,$(NDK_ROOT)/build/prebuilt/$(NDK_HOST)/arm-eabi-4.4.0/lib/gcc/arm-eabi/4.4.0
COMMON_LINK_FLAGS += --sysroot=$(NDK_ROOT)/build/platforms/android-4/arch-arm
COMMON_LINK_FLAGS += -Bdynamic -Wl,-dynamic-linker,//system/bin/linker
COMMON_LINK_FLAGS += -Wl,--gc-sections -Wl,-z,nocopyreloc
COMMON_LINK_FLAGS += -Wl,-L,$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib
COMMON_LINK_FLAGS += -Wl,-rpath-link=$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib
COMMON_LINK_FLAGS += -nostdlib -lc -lm -lsupc++ -lgcc \
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

#Копирование файла на устройство (имя локальных файлов, имя удалённого каталога)
define tools.install
 export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
 $(ADB) shell "mount -o remount,rw -t vfat /dev/block//vold/179:0 /sdcard && export PATH=/data/busybox:\$\$$PATH && mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)" && \
 $(foreach file,$1, echo -n "Install $(notdir $(file)): " && $(ADB) push $(file) $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT) && $(ADB) shell "export PATH=/data/busybox:\$\$$PATH && chmod -R 777 $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)" && ) true
endef

#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
define tools.run
 export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
 export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
 export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
 export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
 export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
 $(ADB) shell "mount -o remount,rw -t vfat /dev/block//vold/179:0 /sdcard && export OLDPATH=\$\$$PATH:\.:$$PATH_SEARCH && export PATH=//data/busybox:\$\$$PATH && export LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)" | sed "s/.$$//"
endef
