###################################################################################################
#Сборка под Android
###################################################################################################
ifeq ($(strip $(ANDROID_NDK)),)
  $(error "Please set ANDROID_NDK variable in your environment")
endif

ifeq ($(strip $(ANDROID_NDK_HOST)),)
  $(error "Please set ANDROID_NDK_HOST variable in your environment")
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
PROFILES                   += android no_dll unistd egl gles
REMOTE_DEBUG_DIR           ?= //sdcard/funner
EXE_SUFFIX                 :=
DLL_SUFFIX                 := .so
DLL_PREFIX                 := lib
ANDROID_PLATFORM           := android-9
NDK_ROOT                   := /$(subst :,,$(call convert_path,$(ANDROID_NDK)))
SDK_ROOT                   := /$(subst :,,$(call convert_path,$(ANDROID_SDK)))
JAVA_SDK                   := /$(subst :,,$(call convert_path,$(JAVA_SDK)))
PLATFORM_DIR               := $(NDK_ROOT)/platforms/$(ANDROID_PLATFORM)
ANDROID_PLATFORM_TOOLS_DIR := $(call convert_path,$(ANDROID_SDK))/platform-tools
ARM_EABI_DIR               := $(NDK_ROOT)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/$(ANDROID_NDK_HOST)
CYGWIN_BIN                 := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
GCC_TOOLS_DIR              := $(ARM_EABI_DIR)/bin
COMPILER_GCC               := $(GCC_TOOLS_DIR)/arm-linux-androideabi-gcc
LINKER_GCC                 := $(GCC_TOOLS_DIR)/arm-linux-androideabi-g++
LIB_GCC                    := $(GCC_TOOLS_DIR)/arm-linux-androideabi-ar
ANDROID_TOOLS_DIR          := $(SDK_ROOT)/tools
ADB                        := $(ANDROID_PLATFORM_TOOLS_DIR)/adb
APK_BUILDER                := $(ANDROID_SDK)/tools/apkbuilder
DX_TOOL                    := $(ANDROID_PLATFORM_TOOLS_DIR)/dx
JAVA_JAR                   := "$(JAVA_SDK)/bin/jar"
JAVA_CC                    := "$(JAVA_SDK)/bin/javac"
JAVA_AAPT                  := $(ANDROID_PLATFORM_TOOLS_DIR)/aapt
JAVA_JAR_SIGNER            := "$(JAVA_SDK)/bin/jarsigner"
ZIP_ALIGNER                := $(ANDROID_SDK)/tools/zipalign
ADDITIONAL_PATHS           += $(CYGWIN_BIN)
BUILD_PATHS                := $(CYGWIN_BIN):$(GCC_TOOLS_DIR):$(ARM_EABI_DIR)/libexec/gcc/arm-linux-androideabi/4.4.3
COMMON_JAVA_FLAGS          += -g
COMMON_CPPFLAGS            += -fexceptions -frtti
COMMON_CFLAGS              += -ffunction-sections -funwind-tables -fstack-protector -fpic -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64
COMMON_CFLAGS              += -march=armv5te -mtune=xscale -msoft-float -mthumb
COMMON_CFLAGS              += -Wno-psabi -Wa,--noexecstack
COMMON_CFLAGS              += -fvisibility=hidden -fvisibility-inlines-hidden
COMMON_CFLAGS              += -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__ -DANDROID -DARM -UDEBUG
COMMON_CFLAGS              += --sysroot=$(PLATFORM_DIR)/arch-arm
COMMON_CFLAGS              += -I$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/include
COMMON_CFLAGS              += -I$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/libs/armeabi/include
COMMON_LINK_FLAGS          += --sysroot=$(PLATFORM_DIR)/arch-arm
COMMON_LINK_FLAGS          += -Wl,-L,$(ARM_EABI_DIR)/lib/gcc/arm-linux-androideabi/4.4.3
COMMON_LINK_FLAGS          += -Wl,-L,$(ARM_EABI_DIR)/lib/thumb
COMMON_LINK_FLAGS          += -Wl,-L,$(PLATFORM_DIR)/arch-arm/usr/lib
COMMON_LINK_FLAGS          += -Wl,-rpath-link=$(PLATFORM_DIR)/arch-arm/usr/lib
COMMON_LINK_FLAGS          += -lc -lm -lstdc++ -lgcc -lsupc++
COMMON_LINK_FLAGS          += -Wl,--no-undefined
ANDROID_EXE_LINK_FLAGS     += -z $(PLATFORM_DIR)/arch-arm/usr/lib/crtbegin_dynamic.o
ANDROID_SO_LINK_FLAGS       = -Wl,-soname,$(notdir $1) -shared -Wl,--no-undefined -Wl,-z,noexecstack

CYGWIN                     := nodosfilewarning
VALID_TARGET_TYPES         += android-pak android-jar
ANDROID_KEY_STORE          := $(BUILD_DIR)platforms/android/my-release-key.keystore
ANDROID_KEY_PASS           := android
ANDROID_JAR                := $(ANDROID_SDK)/platforms/$(ANDROID_PLATFORM)/android.jar
DEFAULT_PACKAGE_PREFIX     := com.untgames.
GDB_SERVER_FLAG_FILE       := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/gdb-installed
GDB_SERVER_FILE            := $(ARM_EABI_DIR)/../gdbserver

export CYGWIN

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Переопределения вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link.shared-lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,,,$5 $(foreach dir,$3,-Wl,-L,$(dir)) $(ANDROID_SO_LINK_FLAGS) $(foreach link,$4,-Wl,-u,$(link)),$6,$7,$8,$9)
endef

define tools.link.dll
$(ANDROID_SO_LINK_FLAGS)
endef

define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,,,$5 $(foreach dir,$3,-Wl,-L,$(dir)) $(if $(filter %$(DLL_SUFFIX),$1),,$(ANDROID_EXE_LINK_FLAGS)) $(foreach link,$4,-Wl,-u,$(link)),$6,$7,$8,$9)
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
 $(ADB) shell "mount -o remount,rw -t vfat /dev/block//vold/179:0 /sdcard && export PATH=/sdcard/busybox:\$\$$PATH && /sdcard/busybox mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)" && \
 $(foreach file,$1, echo -n "Install $(notdir $(file)): " && $(ADB) push $(file) $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT) && $(ADB) shell "export PATH=/sdcard/busybox:\$\$$PATH && /sdcard/busybox chmod -R 777 $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)" && ) true
endef

#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
define tools.run
 export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
 export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
 export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
 export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
 export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
 $(ADB) shell "mount -o remount,rw -t vfat /dev/block//vold/179:0 /sdcard && export OLDPATH=\$\$$PATH:\.:$$PATH_SEARCH && export PATH=//sdcard/busybox:\$\$$PATH && export LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && /sdcard/busybox mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)" | sed "s/.$$//"
endef

#Выполнение команды из пакета (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
define tools.run.android_package
 export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
 export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
 export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
 export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
 export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
 $(ADB) shell logcat -c && \
 $(ADB) shell "mount -o remount,rw -t vfat /dev/block//vold/179:0 /sdcard && export OLDPATH=\$\$$PATH:\.:$$PATH_SEARCH && export PATH=//sdcard/busybox:\$\$$PATH && export LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && /sdcard/busybox mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && am start -a android.intent.action.VIEW -c android.intent.category.LAUNCHER -n $(DEFAULT_PACKAGE_PREFIX)funner.application/.EngineActivity -e 'program' '$$(echo $$SUBST_COMMAND)' -e 'workdir' '$$SUBST_DIR_RESULT' -e 'args' '$(subst $(firstword $1),,$1)'" | sed "s/.$$//" && \
 sleep 1 && \
 $(ADB) shell "\\/sdcard/busybox/sh -c 'while ps | \\/sdcard/busybox/grep $(DEFAULT_PACKAGE_PREFIX)funner.application; do sleep 1; done'" > nul && \
 $(ADB) logcat -s -d -v raw System.out:I -v raw stdout:I
endef
# $(ADB) logcat 

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
  $1.TARGET            := $(DIST_LIB_DIR)/$$($1.NAME).apk
  $1.TMP_DIR           := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.UNSIGNED_TARGET   := $$($1.TMP_DIR)/$$($1.NAME).unsigned.apk
  $1.SIGNED_TARGET     := $$($1.TMP_DIR)/$$($1.NAME).signed.apk
  $1.PACKAGED_RES_FILE := $$($1.TMP_DIR)/resource.pak
  $1.DEX_FILE          := $$($1.TMP_DIR)/code.dex
  $1.JARS              := $$($1.JARS:%=%.jar)
  $1.JAR_DIRS          := $$(call specialize_paths,$$($1.JAR_DIRS)) $(DIST_LIB_DIR)
  $1.JARS              := $$(foreach jar,$$($1.JARS),$$(if $$(wildcard $$($1.JAR_DIRS:%=%/$$(jar))),$$(wildcard $$($1.JAR_DIRS:%=%/$$(jar))),$$(jar)))
  $1.JARS              := $$($1.JARS)
  $1.CLASSES_DIR       := $$($1.TMP_DIR)/classes
  $1.CLASSES_FLAG      := $$($1.TMP_DIR)/compilation-flag
  $1.COMPILER_FLAGS    := $(COMMON_JAVA_FLAGS) $$($1.COMPILER_FLAGS)
  $1.MANIFEST_FILE     := $$(call specialize_paths,$$($1.MANIFEST_FILE))
  $1.R_DIR             := $$($1.TMP_DIR)/r_files
  $1.RES_DIR           := $$(call specialize_paths,$$($1.RES_DIR))
  $1.DLL_DIRS          := $$(call specialize_paths,$$($1.DLL_DIRS)) $(DIST_BIN_DIR)
  $1.DLLS              := $$($1.DLLS:%=$(DLL_PREFIX)%$(DLL_SUFFIX))
  $1.TARGET_DLLS       := $$($1.DLLS:%=$$($1.TMP_DIR)/bin/lib/armeabi/%)
  $1.INSTALLATION_FLAG := $$($1.TMP_DIR)/installation-flag
  TMP_DIRS             := $$(TMP_DIRS) $$($1.TMP_DIR) $$($1.CLASSES_DIR) $$($1.R_DIR) $$($1.TMP_DIR)/bin/lib/armeabi
  
  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))    
  
ifeq (,$$(wildcard $$($1.MANIFEST_FILE)))
  $$(error Manifest file '$$($1.MANIFEST_FILE)' not found)
endif

#Build package
  build: BUILD.$1
  
  .PHONY: BUILD.$1
  
  BUILD.$1: $$($1.CLASSES_DIR) $$($1.R_DIR) $$($1.TARGET)

  $$($1.PACKAGED_RES_FILE): $$($1.MANIFEST_FILE) 
		@echo Compile resources for $$(notdir $$($1.TARGET))...
		@$(JAVA_AAPT) package -f -M $$($1.MANIFEST_FILE) -F $$@ -I $(ANDROID_JAR) -S $$($1.RES_DIR) -m -J $$($1.R_DIR)
		
  .PHONY: create-dirs
  
  $$($1.CLASSES_FLAG): $$($1.SOURCE_FILES) $$($1.JARS) $$($1.PACKAGED_RES_FILE)
		@echo Compile sources for $$(notdir $$($1.TARGET))...
		@export R_FILES=$$$$(/usr/bin/find $$($1.R_DIR) -name '*.java') && $(JAVA_CC) $$($1.SOURCE_FILES) $$$$R_FILES $$($1.COMPILER_FLAGS) -d $$($1.CLASSES_DIR) -classpath '$(ANDROID_JAR)$$(if $$($1.JARS),;$$(subst ; ,;,$$($1.JARS:%=%;)))'
		@touch $$@
  
  $$($1.DEX_FILE): $$($1.CLASSES_FLAG)
		@echo Convert $$(notdir $$@) to Dalvik bytecodes...
ifeq ($(ANDROID_NDK_HOST),windows)
		@export TMP_DIR_PREFIX=$$$$($$(call get_system_dir,$$($1.TMP_DIR))) && cmd //C "$(DX_TOOL).bat --dex --output=$$$$TMP_DIR_PREFIX\$$(notdir $$@) $$$$TMP_DIR_PREFIX\$$(notdir $$($1.CLASSES_DIR))"
else
		@$(DX_TOOL) --dex --output=$$@ $$($1.CLASSES_DIR)
endif

  $$($1.UNSIGNED_TARGET): $$($1.DEX_FILE) $$($1.PACKAGED_RES_FILE) $$($1.TARGET_DLLS)
		@echo Create unsigned APK $$(notdir $$@)...
ifeq ($(ANDROID_NDK_HOST),windows)		
		@export LIB_DIR_PREFIX=$$$$($$(call get_system_dir,$$(DIST_LIB_DIR))) TMP_DIR_PREFIX=$$$$($$(call get_system_dir,$$($1.TMP_DIR))) && if ! cmd //C "$(APK_BUILDER) %TMP_DIR_PREFIX%\$$(notdir $$@) -u -z %TMP_DIR_PREFIX%\$$(notdir $$($1.PACKAGED_RES_FILE)) -f %TMP_DIR_PREFIX%\$$(notdir $$($1.DEX_FILE)) -rf %TMP_DIR_PREFIX%\\bin $$(foreach jar,$$($1.JARS),-rj %LIB_DIR_PREFIX%\\$$(notdir $$(jar)))"; then $(RM) $$@; exit 1; fi
else
		@if ! $$$$($(APK_BUILDER) $$@ -u -z $$($1.PACKAGED_RES_FILE) -f $$($1.DEX_FILE) -rf $$($1.TMP_DIR)/bin $$(foreach jar,$$($1.JARS),-rj $$(jar))); then $(RM) $$@; exit 1; fi
endif

  $$($1.SIGNED_TARGET): $$($1.UNSIGNED_TARGET)
		@echo Sign $$(notdir $$@)...
		@$(JAVA_JAR_SIGNER) -keystore $(ANDROID_KEY_STORE) -storepass $(ANDROID_KEY_PASS) -keypass $(ANDROID_KEY_PASS) -signedjar $$@ $$< alias_name
		
  $$($1.TARGET): $$($1.SIGNED_TARGET)
		@echo Align $$(notdir $$@)...
		@$(RM) $$@
		@$(ZIP_ALIGNER) 4 $$< $$@
		
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
		@$(ADB) shell am start -a android.intent.action.VIEW -c android.intent.category.LAUNCHER -n $(DEFAULT_PACKAGE_PREFIX)$$($1.NAME)/.EngineActivity -e "program" "value"
		@sleep 1
		@$(ADB) shell "\\/sdcard/busybox/sh -c 'while ps | \\/sdcard/busybox/grep $(DEFAULT_PACKAGE_PREFIX)$$($1.NAME); do sleep 1; done'" > nul
		@$(ADB) logcat -s -d -v raw System.out:I -v raw stdout:I
#		@$(ADB) shell setprop log.redirect-stdio true

endef

#Обработка цели android-jar (имя цели)
define process_target.android-jar
  $1.SOURCE_DIRS := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_android_java_source_dir,$1,$$(dir))))

  $1.PACKAGE_NAME      := $$(if $$($1.PACKAGE_NAME),$$($1.PACKAGE_NAME),$(DEFAULT_PACKAGE_PREFIX)$$($1.NAME))
  $1.TARGET            := $(DIST_LIB_DIR)/$$($1.NAME).jar
  $1.TMP_DIR           := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.JARS              := $$(call specialize_paths,$$($1.JARS))
  $1.CLASSES_DIR       := $$($1.TMP_DIR)/classes
  $1.CLASSES_FLAG      := $$($1.TMP_DIR)/compilation-flag
  $1.COMPILER_FLAGS    := $(COMMON_JAVA_FLAGS) $$($1.COMPILER_FLAGS)
  TMP_DIRS             := $$(TMP_DIRS) $$($1.TMP_DIR) $$($1.CLASSES_DIR)
  
#Build package
  build: BUILD.$1
  
  .PHONY: BUILD.$1
  
  BUILD.$1: $$($1.TARGET)

  $$($1.CLASSES_FLAG): $$($1.SOURCE_FILES)
		@echo Compile sources for $$(notdir $$($1.TARGET))...
		@$(RM) -r $$($1.CLASSES_DIR)
		@mkdir -p $$($1.CLASSES_DIR)
		@$(JAVA_CC) $$($1.SOURCE_FILES) $$($1.COMPILER_FLAGS) -d $$($1.CLASSES_DIR) -classpath /$(subst :,,$(call convert_path,$(ANDROID_JAR)))
		@touch $$@

  $$($1.TARGET): $$($1.CLASSES_FLAG)
		@echo Jar $$(notdir $$@)...
		@$(RM) $$@
		@$(JAVA_JAR) cf $$@ -C $$($1.CLASSES_DIR) .
		
endef

install: $(GDB_SERVER_FLAG_FILE)

$(GDB_SERVER_FLAG_FILE): $(GDB_SERVER_FILE)
	@echo Install gdbserver...
	@$(ADB) push $(GDB_SERVER_FILE) $(REMOTE_DEBUG_DIR)
	@$(ADB) shell /sdcard/busybox  chmod 777 $(REMOTE_DEBUG_DIR)/$(notdir $(GDB_SERVER_FILE))
	@touch $@
