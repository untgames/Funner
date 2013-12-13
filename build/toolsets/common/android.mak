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

ifneq (,$(filter Win%,$(OS)))

ifeq ($(strip $(CYGHOME)),)
  $(error "Please set CYGHOME in your environment")
endif

endif

ifeq ($(strip $(JAVA_SDK)),)
  $(error "Please set JAVA_SDK variable in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
PROFILES                   += android no_dll unistd egl gles has_windows
SDCARD_DIR                 := //mnt/sdcard
REMOTE_DEBUG_DIR           ?= $(SDCARD_DIR)/funner
EXE_SUFFIX                 :=
DLL_SUFFIX                 := .so
DLL_PREFIX                 := lib
ANDROID_NDK_PLATFORM       := android-9
ANDROID_SDK_PLATFORM       := android-13
NDK_ROOT                   := /$(subst :,,$(call convert_path,$(ANDROID_NDK)))
SDK_ROOT                   := /$(subst :,,$(call convert_path,$(ANDROID_SDK)))
JAVA_ROOT                  := /$(subst :,,$(call convert_path,$(JAVA_SDK)))
PLATFORM_DIR               := $(NDK_ROOT)/platforms/$(ANDROID_NDK_PLATFORM)
ANDROID_PLATFORM_TOOLS_DIR := $(call convert_path,$(ANDROID_SDK))/platform-tools
ANDROID_BUILD_TOOLS_DIR    := $(call convert_path,$(ANDROID_SDK))/build-tools/18.1.1
ABI_DIR                    := $(NDK_ROOT)/toolchains/$(ANDROID_TOOLCHAIN)-$(ANDROID_TOOLCHAIN_VERSION)/prebuilt/$(ANDROID_NDK_HOST)
GCC_TOOLS_DIR              := $(ABI_DIR)/bin
COMPILER_GCC               := $(GCC_TOOLS_DIR)/$(ANDROID_TOOLS_PREFIX)-gcc
LINKER_GCC                 := $(GCC_TOOLS_DIR)/$(ANDROID_TOOLS_PREFIX)-g++
LIB_GCC                    := $(GCC_TOOLS_DIR)/$(ANDROID_TOOLS_PREFIX)-ar
ADDR2LINE                  := $(GCC_TOOLS_DIR)/$(ANDROID_TOOLS_PREFIX)-addr2line
ANDROID_TOOLS_DIR          := $(SDK_ROOT)/tools
ADB                        := $(ANDROID_PLATFORM_TOOLS_DIR)/adb
AIDL                       := $(ANDROID_PLATFORM_TOOLS_DIR)/aidl
APK_BUILDER                := $(BUILD_DIR)platforms/android/apkbuilder
DX_TOOL                    := $(ANDROID_BUILD_TOOLS_DIR)/dx
JAVA_JAR                   := "$(JAVA_ROOT)/bin/jar"
JAVA_CC                    := "$(JAVA_ROOT)/bin/javac"
JAVA_AAPT                  := $(ANDROID_BUILD_TOOLS_DIR)/aapt
JAVA_JAR_SIGNER            := "$(JAVA_ROOT)/bin/jarsigner"
ZIP_ALIGNER                := $(ANDROID_SDK)/tools/zipalign
BUILD_PATHS                := $(GCC_TOOLS_DIR):$(ABI_DIR)/libexec/gcc/$(ANDROID_TOOLCHAIN)/$(ANDROID_TOOLCHAIN_VERSION)

COMMON_JAVA_FLAGS          += -g
COMMON_CPPFLAGS            += -fexceptions -frtti
#COMMON_CFLAGS              += -g
COMMON_CFLAGS              += -Os -ffunction-sections -funwind-tables -fstack-protector -fpic -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 #-gdwarf-2
COMMON_CFLAGS              += -Wno-psabi -Wa,--noexecstack
COMMON_CFLAGS              += -fvisibility=hidden
COMMON_CFLAGS              += -DANDROID -UDEBUG
COMMON_CFLAGS              += --sysroot=$(PLATFORM_DIR)/arch-$(ANDROID_ARCH)
COMMON_LINK_FLAGS          += --sysroot=$(PLATFORM_DIR)/arch-$(ANDROID_ARCH)
COMMON_LINK_FLAGS          += -Wl,-L,$(ABI_DIR)/lib/gcc/$(ANDROID_TOOLCHAIN)/$(ANDROID_TOOLCHAIN_VERSION)
COMMON_LINK_FLAGS          += -Wl,-L,$(ABI_DIR)/lib/thumb
COMMON_LINK_FLAGS          += -Wl,-L,$(PLATFORM_DIR)/arch-$(ANDROID_ARCH)/usr/lib
COMMON_LINK_FLAGS          += -Wl,-L,$(DIST_BIN_DIR)
COMMON_LINK_FLAGS          += -Wl,-rpath-link=$(PLATFORM_DIR)/arch-$(ANDROID_ARCH)/usr/lib
COMMON_LINK_FLAGS          += -lc -lm -lstdc++ -lgcc -lsupc++
COMMON_LINK_FLAGS          += -Wl,--no-undefined
COMMON_LINK_FLAGS          += -s

ifneq (,$(wildcard $(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/libs/$(ANDROID_ABI)/include))
COMMON_CFLAGS              += -I$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/include
COMMON_CFLAGS              += -I$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/libs/$(ANDROID_ABI)/include
COMMON_LINK_FLAGS          += -Wl,-L,$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/libs/$(ANDROID_ABI)
else
COMMON_CFLAGS              += -I$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_TOOLCHAIN_VERSION)/include
COMMON_CFLAGS              += -I$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_TOOLCHAIN_VERSION)/libs/$(ANDROID_ABI)/include
COMMON_LINK_FLAGS          += -Wl,-L,$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/$(ANDROID_TOOLCHAIN_VERSION)/libs/$(ANDROID_ABI)
endif

ANDROID_SO_LINK_FLAGS       = -Wl,-soname,$(notdir $1) -shared -Wl,--no-undefined -Wl,-z,noexecstack
VALID_TARGET_TYPES         += android-pak android-jar
ANDROID_AIDL               := $(ANDROID_SDK)/platforms/$(ANDROID_SDK_PLATFORM)/framework.aidl
ANDROID_KEY_STORE          := $(BUILD_DIR)platforms/android/my-release-key.keystore
ANDROID_KEY_PASS           := android
ANDROID_JAR                := $(ANDROID_SDK)/platforms/$(ANDROID_SDK_PLATFORM)/android.jar
ANDROID_SDKLIB_JAR         := $(ANDROID_SDK)/tools/lib/sdklib.jar
DEFAULT_PACKAGE_PREFIX     := com.untgames.
GDB_SERVER_FLAG_FILE       := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/gdb-installed
GDB_SERVER_FILE            := $(ABI_DIR)/../gdbserver
GDB_CLIENT                 := $(GCC_TOOLS_DIR)/$(ANDROID_TOOLCHAIN)-gdb
BUSYBOX_FLAG_FILE          := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/busybox-installed
DEFAULT_BUSYBOX_PATH       := $(REMOTE_DEBUG_DIR)/busybox

ifeq (,$(BUSYBOX_PATH))
BUSYBOX_PATH               := $(DEFAULT_BUSYBOX_PATH)
else
ROOTED                     := true
endif

ifneq (,$(filter Win%,$(OS)))
CYGWIN_BIN                 := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
ADDITIONAL_PATHS           += $(CYGWIN_BIN)
BUILD_PATHS                := $(CYGWIN_BIN):$(BUILD_PATHS)
CYGWIN                     := nodosfilewarning
PATH_SEPARATOR             := ;

export CYGWIN

else

PATH_SEPARATOR             := :

endif

include $(TOOLSETS_DIR)/g++.mak

ifeq (,$(ROOTED))

define sudo
su -c '$1'
endef

define remount
 su -c 'mount -o remount,rw -t vfat /dev/block//vold/179:0 $(SDCARD_DIR)'
endef

else

define sudo
$1
endef

define remount
 mount -o remount,rw -t vfat /dev/block//vold/179:0 $(SDCARD_DIR)
endef

endif

###################################################################################################
#Переопределения вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link.shared-lib
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,,,$5 $(foreach dir,$3,-Wl,-L,$(dir)) $(ANDROID_SO_LINK_FLAGS) $(foreach link,$4,-Wl,-u,$(link)),$6,$7,$8,$9)
endef

define tools.link.exe
export PATH=$(BUILD_PATHS):$$PATH && $(call tools.g++.link,$1,$2,,,$5 $(foreach dir,$3,-Wl,-L,$(dir)) $(ANDROID_EXE_LINK_FLAGS) $(foreach link,$4,-Wl,-u,$(link)),$6,$7,$8,$9)
endef

define tools.link.dll
endef

define tools.link
$(if $(filter %$(DLL_SUFFIX),$1),$(call tools.link.shared-lib,$1,$2,$3,$4,$5,$6,$7,$8,$9),$(call tools.link.exe,$1,$2,$3,$4,$5,$6,$7,$8,$9))
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && export FILE_COUNTER=0 FILE_LIST="" && for file in $2; do export FILE_COUNTER=$$(($$FILE_COUNTER + 1)) && FILE_LIST="$$FILE_LIST $$file"; if [ $$FILE_COUNTER -eq 256 ]; then $(call tools.g++.lib,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9); export FILE_COUNTER=0 FILE_LIST=""; fi; done && $(call tools.g++.lib,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9)
endef

###################################################################################################
#Отладка на устройстве
###################################################################################################

#Копирование файла на устройство (имя локальных файлов, имя удалённого каталога)
define tools.install
 export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
 $(ADB) shell "$(call remount)" && \
 $(ADB) shell "export PATH=$(BUSYBOX_PATH):\$\$$PATH && $(call sudo,$(BUSYBOX_PATH) mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT))" && \
 $(foreach file,$1, echo -n "Install $(notdir $(file)): " && $(ADB) push $(file) $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT) && $(ADB) shell "export PATH=$(BUSYBOX_PATH):\$\$$PATH && $(call sudo,$(BUSYBOX_PATH) chmod -R 777 $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT))" && ) true
endef

#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run
 export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
 export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
 export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
 export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
 export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
 $(ADB) shell "$(call remount)" && $(ADB) shell "export OLDPATH=\$\$$PATH:\.:$$PATH_SEARCH && export PATH=$(BUSYBOX_PATH):\$\$$PATH && export LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && $(BUSYBOX_PATH) mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)" | sed "s/.$$//"
endef

#Выполнение команды из пакета (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run.android_package
 export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
 export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
 export DLLS="$(foreach path,$4,$$(export SUBST_DLLS=$$(cd $(dir $(path)) && pwd)/$(notdir $(path)) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_DLLS/#$$ROOT_SUBSTRING/}))" && \
 export DLLS=$${DLLS/\ /:} && \
 export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
 export OLD_APP_PID=`$(ADB) shell ps | grep $(DEFAULT_PACKAGE_PREFIX)funner.application | awk '{print $$2}'` && \
 $(ADB) shell "$(call sudo,kill $$OLD_APP_PID)" && \
 $(ADB) shell logcat -c && \
 $(ADB) shell "$(call remount) && $(BUSYBOX_PATH) mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && am start -a android.intent.action.VIEW -c android.intent.category.LAUNCHER -n $(DEFAULT_PACKAGE_PREFIX)funner.application/.EngineActivity -e 'program' '$$(echo $$SUBST_COMMAND)' -e 'workdir' '$$SUBST_DIR_RESULT' -e 'libraries' '$$DLLS' -e 'args' '$(subst $(firstword $1),,$1)'" | sed "s/.$$//" && \
 sleep 1 && \
 ( $(ADB) logcat -s -v raw funner:E -v raw funner:I & ) && \
 while $(ADB) shell ps | grep $(DEFAULT_PACKAGE_PREFIX)funner.application; do sleep 1; done > nul
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
    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $2/*.java) $$(wildcard $2/*.aidl)
  endif
endef

#Генерация java файлов из aidl (source_file, out_file)
define aidl_gen
  $2: $1
		@echo Generating $$(notdir $1)...
		@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
		@$(AIDL) -p$(ANDROID_AIDL) $$< $$@
endef

#Обработка цели android-apk (имя цели)
define process_target.android-pak
  $1.SOURCE_DIRS := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_android_java_source_dir,$1,$$(dir))))

  $1.PACKAGE_NAME      := $$(if $$($1.PACKAGE_NAME),$$($1.PACKAGE_NAME),$(DEFAULT_PACKAGE_PREFIX)$$($1.NAME))
  $1.TARGET            := $(DIST_LIB_DIR)/$$($1.NAME).apk
  $1.TMP_DIR           := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.AIDL_GEN_DIR      := $(COMPONENT_DIR)$$($1.AIDL_GEN_DIR)
  $1.AIDL_FILES        := $$(filter %.aidl,$$($1.SOURCE_FILES))
  $1.AIDL_JAVA_FILES   := $$(foreach aidl,$$($1.AIDL_FILES),$$($1.AIDL_GEN_DIR)/$$(basename $$(notdir $$(aidl))).java)
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
  $1.JAR_CLASSES_FLAG  := $$($1.TMP_DIR)/jar-extract-flag
  $1.COMPILER_FLAGS    := $(COMMON_JAVA_FLAGS) $$($1.COMPILER_FLAGS)
  $1.MANIFEST_FILE     := $$(call specialize_paths,$$($1.MANIFEST_FILE))
  $1.R_DIR             := $$($1.TMP_DIR)/r_files
  $1.RES_DIR           := $$(call specialize_paths,$$($1.RES_DIR))
  $1.DLL_DIRS          := $$(call specialize_paths,$$($1.DLL_DIRS)) $(DIST_BIN_DIR)
  $1.DLLS              := $$($1.DLLS:%=$(DLL_PREFIX)%$(DLL_SUFFIX))
  $1.ABI_DIR           := $$($1.TMP_DIR)/bin/lib/$(ANDROID_ABI)
  $1.TARGET_DLLS       := $$($1.DLLS:%=$$($1.ABI_DIR)/%)
  $1.INSTALLATION_FLAG := $$($1.TMP_DIR)/installation-flag
  $1.SOURCE_FILES      := $$(filter-out %.aidl,$$($1.SOURCE_FILES)) $$($1.AIDL_JAVA_FILES)
  TMP_DIRS             := $$(TMP_DIRS) $$($1.TMP_DIR) $$($1.CLASSES_DIR) $$($1.R_DIR) $$($1.ABI_DIR)
 
  $$(foreach aidl,$$($1.AIDL_FILES),$$(eval $$(call aidl_gen,$$(aidl),$$($1.AIDL_GEN_DIR)/$$(basename $$(notdir $$(aidl))).java)))

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
  
  $$($1.CLASSES_FLAG): $$($1.SOURCE_FILES) $$($1.JARS) $$($1.PACKAGED_RES_FILE) $$($1.JAR_CLASSES_FLAG)
		@echo Compile sources for $$(notdir $$($1.TARGET))...
		@export R_FILES=$$$$(/usr/bin/find $$($1.R_DIR) -name '*.java') && $(JAVA_CC) $$($1.SOURCE_FILES) $$$$R_FILES $$($1.COMPILER_FLAGS) -d $$($1.CLASSES_DIR) -classpath '$(ANDROID_JAR)$$(if $$($1.JARS),$(PATH_SEPARATOR)$$(subst ; ,$(PATH_SEPARATOR),$$($1.JARS:%=%$(PATH_SEPARATOR))))'
		@touch $$@

  $$($1.JAR_CLASSES_FLAG): $$($1.JARS)
		@echo Unpack jar files...
		@cd $$($1.CLASSES_DIR) && $$(foreach jar,$$($1.JARS), $(JAVA_JAR) xf "$(CURDIR)/$$(jar)" && ) true
		@touch $$@
  
  $$($1.DEX_FILE): $$($1.CLASSES_FLAG)
		@echo Convert $$(notdir $$@) to Dalvik bytecodes...
ifneq (,$(filter windows%,$(ANDROID_NDK_HOST)))
		@export TMP_DIR_PREFIX=$$$$($$(call get_system_dir,$$($1.TMP_DIR))) && cmd //C "$(DX_TOOL).bat --dex --output=$$$$TMP_DIR_PREFIX\$$(notdir $$@) $$$$TMP_DIR_PREFIX\$$(notdir $$($1.CLASSES_DIR))"
else
		@$(DX_TOOL) --dex --output=$$@ $$($1.CLASSES_DIR)
endif

  $$($1.UNSIGNED_TARGET): $$($1.DEX_FILE) $$($1.PACKAGED_RES_FILE) $$($1.TARGET_DLLS)
		@echo Create unsigned APK $$(notdir $$@)...
ifneq (,$(filter windows%,$(ANDROID_NDK_HOST)))
		@export PATH="$(JAVA_ROOT)/bin":$$$$PATH SDKLIB_JAR=$(subst /,\\,$(subst \,\\,$(ANDROID_SDKLIB_JAR))) LIB_DIR_PREFIX=$$$$(cmd "\\/C" "cd . && cd") TMP_DIR_PREFIX=$$$$($$(call get_system_dir,$$($1.TMP_DIR))) && if ! cmd //C "$(subst /,\,$(APK_BUILDER)).bat %TMP_DIR_PREFIX%\$$(notdir $$@) -u -z %TMP_DIR_PREFIX%\$$(notdir $$($1.PACKAGED_RES_FILE)) -f %TMP_DIR_PREFIX%\$$(notdir $$($1.DEX_FILE)) -rf %TMP_DIR_PREFIX%\\bin $$(foreach jar,$$($1.JARS),-rj %LIB_DIR_PREFIX%\\$$(subst /,\,$$(jar)))"; then $(RM) $$@; exit 1; fi
else
		@export PATH="$(JAVA_ROOT)/bin":$$$$PATH SDKLIB_JAR=$(ANDROID_SDKLIB_JAR) && if ! $$$$($(APK_BUILDER) $$@ -u -z $$($1.PACKAGED_RES_FILE) -f $$($1.DEX_FILE) -rf $$($1.TMP_DIR)/bin $$(foreach jar,$$($1.JARS),-rj $$(jar))); then $(RM) $$@; exit 1; fi
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
		@$(ADB) shell "$(BUSYBOX_PATH)/sh -c 'while ps | $(BUSYBOX_PATH)/grep $(DEFAULT_PACKAGE_PREFIX)$$($1.NAME); do sleep 1; done'" > nul
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
  $1.AIDL_GEN_DIR      := $(COMPONENT_DIR)$$($1.AIDL_GEN_DIR)
  $1.AIDL_FILES        := $$(filter %.aidl,$$($1.SOURCE_FILES))
  $1.AIDL_JAVA_FILES   := $$(foreach aidl,$$($1.AIDL_FILES),$$($1.AIDL_GEN_DIR)/$$(basename $$(notdir $$(aidl))).java)
  $1.JARS              := $$($1.JARS:%=%.jar)
  $1.JAR_DIRS          := $$(call specialize_paths,$$($1.JAR_DIRS)) $(DIST_LIB_DIR)
  $1.JARS              := $$(foreach jar,$$($1.JARS),$$(if $$(wildcard $$($1.JAR_DIRS:%=%/$$(jar))),$$(wildcard $$($1.JAR_DIRS:%=%/$$(jar))),$$(jar)))
  $1.JARS              := $$($1.JARS)
  $1.CLASSES_DIR       := $$($1.TMP_DIR)/classes
  $1.CLASSES_FLAG      := $$($1.TMP_DIR)/compilation-flag
  $1.COMPILER_FLAGS    := $(COMMON_JAVA_FLAGS) $$($1.COMPILER_FLAGS)
  $1.SOURCE_FILES      := $$(filter-out %.aidl,$$($1.SOURCE_FILES)) $$($1.AIDL_JAVA_FILES)
  TMP_DIRS             := $$(TMP_DIRS) $$($1.TMP_DIR) $$($1.CLASSES_DIR)
  
  $$(foreach aidl,$$($1.AIDL_FILES),$$(eval $$(call aidl_gen,$$(aidl),$$($1.AIDL_GEN_DIR)/$$(basename $$(notdir $$(aidl))).java)))
  
#Build package
  build: BUILD.$1
  
  .PHONY: BUILD.$1
  
  BUILD.$1: $$($1.TARGET)

  $$($1.CLASSES_FLAG): $$($1.SOURCE_FILES)
		@echo Compile sources for $$(notdir $$($1.TARGET))...
		@$(RM) -r $$($1.CLASSES_DIR)
		@mkdir -p $$($1.CLASSES_DIR)
		@$(JAVA_CC) $$($1.SOURCE_FILES) $$($1.COMPILER_FLAGS) -d $$($1.CLASSES_DIR) -classpath '$(ANDROID_JAR)$$(if $$($1.JARS),$(PATH_SEPARATOR)$$(subst $(PATH_SEPARATOR) ,$(PATH_SEPARATOR),$$(subst ; ,$(PATH_SEPARATOR),$$($1.JARS:%=%$(PATH_SEPARATOR)))))'
		@touch $$@

  $$($1.TARGET): $$($1.CLASSES_FLAG)
		@echo Jar $$(notdir $$@)...
		@$(RM) $$@
		@$(JAVA_JAR) cf $$@ -C $$($1.CLASSES_DIR) .
		
endef

#install: $(GDB_SERVER_FLAG_FILE) $(BUSYBOX_FLAG_FILE)
ifeq ($(REMOTE_DEBUG_DIR)/busybox,$(BUSYBOX_PATH))
install:  $(BUSYBOX_FLAG_FILE)
endif

$(GDB_SERVER_FLAG_FILE): $(GDB_SERVER_FILE) $(BUSYBOX_FLAG_FILE)
	@echo Install gdbserver...
	@$(ADB) push $(GDB_SERVER_FILE) $(REMOTE_DEBUG_DIR)
	@$(ADB) shell "$(call remount)"
	@$(ADB) shell "$(call sudo,$(BUSYBOX_PATH) chmod 777 $(REMOTE_DEBUG_DIR)/$(notdir $(GDB_SERVER_FILE)))"
	@touch $@

$(BUSYBOX_FLAG_FILE): $(BUSYBOX_FILE)
	@echo Install busybox...
	@$(ADB) push $(BUSYBOX_FILE) $(REMOTE_DEBUG_DIR)
	@$(ADB) shell "$(call remount)"
	@$(ADB) shell "$(call sudo,chmod 777 $(BUSYBOX_PATH))"
	@touch $@

.PHONY: addr2line

addr2line:
#ifeq (,$(addr))
#	@echo Address not specified
#	@exit 1
#endif
	@$(ADDR2LINE) -e $(DIST_BIN_DIR)/$(file) -C -f -i -s $(addr)
#	@$(GCC_TOOLS_DIR)/$(ANDROID_TOOLCHAIN)-nm $(DIST_BIN_DIR)/$(file)
