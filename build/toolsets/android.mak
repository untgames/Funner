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

ifeq ($(strip $(JAVA_SDK)),)
  $(error "Please set JAVA_SDK variable in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
PROFILES           += android no_dll unistd haswchar
REMOTE_DEBUG_DIR   ?= //sdcard/funner
EXE_SUFFIX         :=
DLL_SUFFIX         := so #no suffix, this is for dll correct dll dispatch at link
DLL_PREFIX         :=
NDK_HOST           := windows
ANDROID_PLATFORM   := android-8
NDK_ROOT           := /$(subst :,,$(call convert_path,$(ANDROID_NDK)))
SDK_ROOT           := /$(subst :,,$(call convert_path,$(ANDROID_SDK)))
JAVA_SDK           := /$(subst :,,$(call convert_path,$(JAVA_SDK)))
ARM_EABI_DIR       := $(NDK_ROOT)/build/prebuilt/$(NDK_HOST)/arm-eabi-4.4.0
CYGWIN_BIN         := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
GCC_TOOLS_DIR      := $(ARM_EABI_DIR)/bin
COMPILER_GCC       := $(GCC_TOOLS_DIR)/arm-eabi-gcc.exe
LINKER_GCC         := $(GCC_TOOLS_DIR)/arm-eabi-gcc.exe
LIB_GCC            := $(GCC_TOOLS_DIR)/arm-eabi-ar
ANDROID_TOOLS_DIR  := $(SDK_ROOT)/tools
ADB                := $(ANDROID_TOOLS_DIR)/adb
APK_BUILDER        := $(ANDROID_SDK)/tools/apkbuilder.bat
DX_TOOL            := $(ANDROID_SDK)/platforms/$(ANDROID_PLATFORM)/tools/dx.bat
JAVA_CC            := "$(JAVA_SDK)/bin/javac"
JAVA_AAPT          := $(ANDROID_SDK)/platforms/$(ANDROID_PLATFORM)/tools/aapt
JAVA_JAR_SIGNER    := "$(JAVA_SDK)/bin/jarsigner"
ADDITIONAL_PATHS   += $(CYGWIN_BIN)
BUILD_PATHS        := $(CYGWIN_BIN):$(GCC_TOOLS_DIR):$(ARM_EABI_DIR)/libexec/gcc/arm-eabi/4.4.0
COMMON_JAVA_FLAGS  += -g
COMMON_CPPFLAGS    += -fexceptions -frtti
COMMON_CFLAGS      += -mandroid -ffunction-sections -fdata-sections -Os -g -Wno-psabi \
                     --sysroot=$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm \
                     -fPIC -fvisibility=hidden -D__NEW__ -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ \
                     -D__ARM_ARCH_5TE__ -DANDROID -DARM -DSK_RELEASE -DNDEBUG \
                     -UDEBUG -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -fpic -ffunction-sections \
                     -funwind-tables -fstack-protector -fmessage-length=0 -Bdynamic -fno-strict-aliasing
COMMON_LINK_FLAGS  += -Wl,-L,$(NDK_ROOT)/build/prebuilt/$(NDK_HOST)/arm-eabi-4.4.0/lib/gcc/arm-eabi/4.4.0
COMMON_LINK_FLAGS  += -Wl,-L,$(NDK_ROOT)/build/prebuilt/$(NDK_HOST)/arm-eabi-4.4.0/arm-eabi/lib/thumb
COMMON_LINK_FLAGS  += --sysroot=$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm
COMMON_LINK_FLAGS  += -Bdynamic -Wl,-dynamic-linker,//system/bin/linker
COMMON_LINK_FLAGS  += -Wl,--gc-sections -Wl,-z,nocopyreloc
COMMON_LINK_FLAGS  += -Wl,-L,$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib
COMMON_LINK_FLAGS  += -Wl,-rpath-link=$(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib
COMMON_LINK_FLAGS  += -nostdlib -lc -lm -lstdc++ -lgcc \
                      --no-undefined -z $(NDK_ROOT)/build/platforms/$(ANDROID_PLATFORM)/arch-arm/usr/lib/crtbegin_dynamic.o
CYGWIN             := nodosfilewarning
VALID_TARGET_TYPES += android-pak
ANDROID_KEY_STORE  := $(BUILD_DIR)platforms/android/my-release-key.keystore
ANDROID_KEY_PASS   := android
ANDROID_JAR        := $(ANDROID_SDK)/platforms/$(ANDROID_PLATFORM)/android.jar
DEFAULT_PACKAGE_PREFIX := com.untgames.android.
GDB_SERVER_FLAG_FILE   := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/gdb-installed
GDB_SERVER_FILE        := $(ANDROID_NDK)/build/prebuilt/$(NDK_HOST)/arm-eabi-4.4.0/bin/gdbserver

export CYGWIN

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Переопределения вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,,,$5 $(foreach dir,$3,-Wl,-L,$(dir)) $(foreach link,$4,-Wl,-u,$(link)),$6,$7,$8,$9)
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

###################################################################################################
#Сборка пакетов
###################################################################################################

#Обработка каталога с java исходниками (имя цели, имя каталога)
define process_android_java_source_dir 
  ifneq (,$$(wildcard $2/sources.mak))
    SOURCE_FILES :=
  
    include $2/sources.mak    

    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $$(SOURCE_FILES:%=$2/%))
  else
    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $2/*.java)
  endif
endef

#Обработка цели android-apk (имя цели)
define process_target.android-pak
  $1.SOURCE_DIRS := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_android_java_source_dir,$1,$$(dir))))

  $1.PACKAGE_NAME      := $$(if $$($1.PACKAGE_NAME),$$($1.PACKAGE_NAME),$(DEFAULT_PACKAGE_PREFIX)$$($1.NAME))
  $1.TARGET            := $(DIST_BIN_DIR)/$$($1.NAME).apk
  $1.TMP_DIR           := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.UNSIGNED_TARGET   := $$($1.TMP_DIR)/$$($1.NAME).unsigned.apk
  $1.PACKAGED_RES_FILE := $$($1.TMP_DIR)/resource.pak
  $1.DEX_FILE          := $$($1.TMP_DIR)/code.dex
  $1.JARS              := $$(call specialize_paths,$$($1.JARS))
  $1.CLASSES_DIR       := $$($1.TMP_DIR)/classes
  $1.CLASSES_FLAG      := $$($1.TMP_DIR)/compilation-flag
  $1.COMPILER_FLAGS    := $(COMMON_JAVA_FLAGS) $$($1.COMPILER_FLAGS)
  $1.MANIFEST_FILE     := $$(call specialize_paths,$$($1.MANIFEST_FILE))
  $1.R_DIR             := $$($1.TMP_DIR)/r_files
  $1.RES_DIR           := $$(call specialize_paths,$$($1.RES_DIR))
  $1.INSTALLATION_FLAG := $$($1.TMP_DIR)/installation-flag
  TMP_DIRS             := $$(TMP_DIRS) $$($1.TMP_DIR) $$($1.CLASSES_DIR) $$($1.R_DIR)
  
ifeq (,$$(wildcard $$($1.MANIFEST_FILE)))
  $$(error Manifest file '$$($1.MANIFEST_FILE)' not found)
endif

#Build package
  build: BUILD.$1
  
  .PHONY: BUILD.$1
  
  BUILD.$1: $$($1.CLASSES_DIR) $$($1.R_DIR) $$($1.TARGET)

  $$($1.PACKAGED_RES_FILE): $$($1.MANIFEST_FILE) 
		@echo Compile resources for $$(notdir $$($1.TARGET))...
		@export MANIFEST_DIR_PREFIX=$$$$($$(call get_system_dir,$$(dir $$($1.MANIFEST_FILE)))) && cmd //C '$(subst /,\,$(JAVA_AAPT)) package -f -M %MANIFEST_DIR_PREFIX%\$$(notdir $$($1.MANIFEST_FILE)) -F $$@ -I $(ANDROID_JAR) -S $$($1.RES_DIR) -m -J $$($1.R_DIR)'
  
  $$($1.CLASSES_FLAG): $$($1.TMP_DIR) $$($1.SOURCE_FILES) $$($1.PACKAGED_RES_FILE)
		@echo Compile sources for $$(notdir $$($1.TARGET))...
		@export R_FILES=$$$$(/bin/find $$($1.R_DIR) -name '*.java') && $(JAVA_CC) $$($1.SOURCE_FILES) $$$$R_FILES $$($1.COMPILER_FLAGS) -d $$($1.CLASSES_DIR) -classpath /$(subst :,,$(call convert_path,$(ANDROID_JAR)))
		@touch $$@
  
  $$($1.DEX_FILE): $$($1.CLASSES_FLAG)
		@echo Convert $$(notdir $$@) to Dalvik bytecodes...
		@export TMP_DIR_PREFIX=$$$$($$(call get_system_dir,$$($1.TMP_DIR))) && cmd //C '$(DX_TOOL) --dex --output=%TMP_DIR_PREFIX%\$$(notdir $$@) %TMP_DIR_PREFIX%\$$(notdir $$($1.CLASSES_DIR))'		
  
  $$($1.UNSIGNED_TARGET): $$($1.DEX_FILE) $$($1.PACKAGED_RES_FILE)
		@echo Create unsigned APK $$(notdir $$@)...
		@export TMP_DIR_PREFIX=$$$$($$(call get_system_dir,$$($1.TMP_DIR))) && cmd //C '$(APK_BUILDER) %TMP_DIR_PREFIX%\$$(notdir $$@) -u -z %TMP_DIR_PREFIX%\$$(notdir $$($1.PACKAGED_RES_FILE)) -f %TMP_DIR_PREFIX%\$$(notdir $$($1.DEX_FILE))' # -rf ${source.dir}  -rj  ${libraries.dir}

  $$($1.TARGET): $$($1.UNSIGNED_TARGET)
		@echo Sign $$(notdir $$@)...
		@$(JAVA_JAR_SIGNER) -keystore $(ANDROID_KEY_STORE) -storepass $(ANDROID_KEY_PASS) -keypass $(ANDROID_KEY_PASS) -signedjar $$@ $$< alias_name
		
#Install package
  install: INSTALL.$1
  uninstall: UNINSTALL.$1

  .PHONY: INSTALL.$1 UNINSTALL.$1

  INSTALL.$1: $$($1.TMP_DIR) $$($1.INSTALLATION_FLAG)
  
  $$($1.INSTALLATION_FLAG): $$($1.TARGET)
		@echo Install $$(notdir $$<)...
		@$(ADB) uninstall $$<
		@$(ADB) install -r $$<
		@touch $$@
		
  $1.UNINSTALL:
		@$(ADB) uninstall $$($1.TARGET)
		
#Run package
  run: RUN.$1
  
  .PHONY: RUN.$1
  
  RUN.$1: INSTALL.$1
		@$(ADB) shell logcat -c		
		@$(ADB) shell am start -a android.intent.action.VIEW -c android.intent.category.LAUNCHER -n $(DEFAULT_PACKAGE_PREFIX)$$($1.NAME)/.SkeletonActivity -e "test" "value"
		@sleep 1
#		@$(ADB) shell "\\/data/busybox/sh -c 'while ! ps | \\/data/busybox/grep $(DEFAULT_PACKAGE_PREFIX)$$($1.NAME); do sleep 1; done'" > nul
		@$(ADB) shell "\\/data/busybox/sh -c 'while ps | \\/data/busybox/grep $(DEFAULT_PACKAGE_PREFIX)$$($1.NAME); do sleep 1; done'" > nul
		@$(ADB) logcat -s -d -v raw System.out:I
#		@$(ADB) shell setprop log.redirect-stdio true

endef

install: $(GDB_SERVER_FLAG_FILE)

$(GDB_SERVER_FLAG_FILE): $(GDB_SERVER_FILE)
	@echo Install gdbserver...
	@$(ADB) push $(GDB_SERVER_FILE) $(REMOTE_DEBUG_DIR)
	@$(ADB) shell chmod 777 $(REMOTE_DEBUG_DIR)/$(notdir $(GDB_SERVER_FILE))
	@touch $@
