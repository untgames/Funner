###################################################################################################
#Сборка под WindowsRT
###################################################################################################

#Links
#http://msdn.microsoft.com/en-us/library/windows/desktop/hh446767(v=vs.85).aspx
#http://msdn.microsoft.com/en-us/library/windows/desktop/jj835835(v=vs.85).aspx
#http://msdn.microsoft.com/en-us/library/windows/desktop/jj835832(v=vs.85).aspx
#http://msdn.microsoft.com/en-us/library/windows/apps/jj552945.aspx

###################################################################################################
#Выбор конфигурации MSVC
###################################################################################################
ifneq (,$(VS110COMNTOOLS))
  MSVC_PATH         ?= $(VS110COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS110COMNTOOLS)../../Common7/Ide
  PROFILES          += haswchar
endif

ifeq (,$(MSVS_COMMON_PATH))
  $(error 'Microsoft Visual Studio not detected (empty MSVC_PATH)')
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

ifeq (,$(WINRT_SDK))
  $(error 'Microsoft WindowsRT SDK not detected (empty WINRT_SDK)')
endif

ifeq (,$(TARGET_PLATFORM))
  $(error 'TARGET_PLATFORM has not been defined')
endif

ifneq (,$(WINRT_SDK))
  SDK_DIR ?= $(call convert_path,$(WINRT_SDK))
endif

MSVC_BIN_PATH      := $(MSVC_PATH)/bin$(if $(filter x86,$(CPU_ARCH)),)
HOST_PLATFORM      := x86
COMMON_CFLAGS      += -W3 -Ox -wd4996 -nologo -FC -D "WINRT" -D "WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP" -MD -AI "$(WINRT_SDK)\References\CommonConfiguration\Neutral"
COMMON_CFLAGS      += -AI "$(MSVC_PATH)\vcpackages"
COMMON_LINK_FLAGS  += -DYNAMICBASE

###################################################################################################
#Константы
###################################################################################################
LIB_SUFFIX     := .lib
OBJ_SUFFIX     := .obj
EXE_SUFFIX     := .exe
DLL_SUFFIX     := .dll
DLL_LIB_SUFFIX := .lib
DLL_PREFIX     :=
PROFILES       += msvc has_windows win8 winrt no_dll
SOURCE_FILES_SUFFIXES += asm
VALID_TARGET_TYPES += win8-appx

###################################################################################################
#Конфигурация переменных расположения библиотек
###################################################################################################
INCLUDE := $(WINRT_SDK)/include;$(WINRT_SDK)/include/shared;$(WINRT_SDK)/include/um;$(WINRT_SDK)/include/winrt;$(MSVC_PATH)/include;$(MSVC_PATH)/atlmfc/include;$(INCLUDE)
LIB     := $(WINRT_SDK)/lib/win8/um/$(CPU_ARCH);$(MSVC_PATH)/lib$(if $(filter x86,$(CPU_ARCH)),,/$(CPU_ARCH));$(MSVC_PATH)/atlmfc/lib$(if $(filter x86,$(CPU_ARCH)),,/$(CPU_ARCH));$(LIB)

export INCLUDE
export LIB

###################################################################################################
#Компиляция исходников (список исходников, список подключаемых каталогов, список подключаемых файлов, каталог с объектными файлами,
#список дефайнов, флаги компиляции, pch файл, список каталогов с dll)
###################################################################################################
define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" \
$(if $(filter %.c,$1)$(filter %.cpp,$1),&& "$(MSVC_BIN_PATH)/cl" -c -Fo"$4\\" $(patsubst %,-I"%",$2) $(patsubst %,-FI"%",$3) $(COMMON_CFLAGS) $6 $(if $(filter -clr,$6),$(foreach dir,$8 $(DIST_BIN_DIR),-AI $(dir)),-EHsc) $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.c,$1) $(filter %.cpp,$1) $(if $7,-FI"$7" -Yc"$7" -Fp"$4\\")) \
$(if $(filter %.asm,$1),&& "$(MSVC_BIN_PATH)/ml" -nologo -c -Fo"$4\\" $(patsubst %,-I"%",$2) $6 $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.asm,$1))
endef

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки, def файл)
###################################################################################################
define tools.link
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/link" -nologo -out:"$1" $(if $(filter %.dll,$1),-dll) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"_%",$4) $5 $2 $(COMMON_LINK_FLAGS) $(if $(map),-MAP:$(basename $1).map -MAPINFO:EXPORTS) $(if $6,-DEF:"$6")
endef

define tools.link.shared-lib
$(call tools.link,$1,$2,$3,$4,$5) -dll
endef

###################################################################################################
#Сборка библиотеки (имя выходного файла, список файлов)
###################################################################################################
define tools.lib.generic
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/lib" -nologo -out:$1 $2
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && export FILE_COUNTER=0 FILE_LIST="" && for file in $2; do export FILE_COUNTER=$$(($$FILE_COUNTER + 1)) && FILE_LIST="$$FILE_LIST $$file"; if [ $$FILE_COUNTER -eq 256 ]; then $(call tools.lib.generic,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9); export FILE_COUNTER=0 FILE_LIST="$1"; fi; done && $(call tools.lib.generic,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9)
endef

#Обработка цели win8-msbuild-appx (имя цели)
define process_target.win8-appx
  $1.TMP_DIR                 := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.OUT_DIR                 := $$(if $$($1.OUT_DIR),$$(COMPONENT_DIR)/$$($1.OUT_DIR),$$(DIST_LIB_DIR))
  $1.EXE_FILE                := $$($1.TMP_DIR)/$$($1.NAME)$(EXE_SUFFIX)
  $1.WINMD_FILE              := $$(basename $$($1.EXE_FILE)).winmd
  $1.PACKAGE_FILE            := $$($1.OUT_DIR)/$$($1.NAME).appx
  $1.UNSIGNED_APPX_FILE      := $$($1.TMP_DIR)/$$($1.NAME).unsigned.appx
  $1.SIGNED_APPX_FILE        := $$($1.TMP_DIR)/$$($1.NAME).appx
  $1.MANIFEST_FILE           := $(COMPONENT_DIR)$$($1.MANIFEST_FILE)
  $1.PFX_FILE                := $(COMPONENT_DIR)$$($1.PFX_FILE)
  TARGET_FILES               := $$(TARGET_FILES) $$($1.PACKAGE_FILE)
  $1.TARGET_DLLS             := $$($1.DLLS:%=$$($1.OUT_DIR)/$(DLL_PREFIX)%$(DLL_SUFFIX))
  $1.CER_FILE                := $$($1.TMP_DIR)/$$(basename $$(notdir $$($1.PFX_FILE))).cer
  $1.PVK_FILE                := $$($1.TMP_DIR)/$$(basename $$(notdir $$($1.PFX_FILE))).pvk
  $1.LINK_FLAGS              := $$($1.LINK_FLAGS) -subsystem:windows -winmd -winmdfile:$$($1.WINMD_FILE)
  $1.APPX_LIST               := $$($1.EXE_FILE) $$($1.WINMD_FILE)
  $1.SOURCE_DATA_ROOT        := $(COMPONENT_DIR)$$($1.SOURCE_DATA_ROOT)
  $1.SOURCE_DATA_FILES       := $$($1.SOURCE_DATA_FILES:%=$(COMPONENT_DIR)%) $$($1.MANIFEST_FILE)
  $1.DST_DATA_FILES_DIR      := $$($1.TMP_DIR)/assets
  $1.APPX_INSTALLATION_FLAG  := $$($1.TMP_DIR)/appx-installation-flag
  $1.CER_INSTALLATION_FLAG   := $$($1.TMP_DIR)/cer-installation-flag

ifneq (,$$($1.IGNORE_MISSING_SOURCE_DATA_FILES))
  $1.SOURCE_DATA_FILES := $$(wildcard $$($1.SOURCE_DATA_FILES))
endif

  $1.TARGET_DATA_FILES := $$(foreach file,$$($1.SOURCE_DATA_FILES),$$(patsubst $$($1.SOURCE_DATA_ROOT)/%,$$($1.DST_DATA_FILES_DIR)/%,$$(file)))
  $1.TARGET_DATA_FILES := $$($1.TARGET_DATA_FILES) $$(foreach file,$$($1.APPX_LIST),$$(patsubst $$($1.TMP_DIR)/%,$$($1.DST_DATA_FILES_DIR)/%,$$(file)))

  $$(eval $$(call process_target_with_sources,$1))

  build: BUILD.$1

  .PHONY: BUILD.$1

  BUILD.$1: $$($1.PACKAGE_FILE)

  $$($1.DST_DATA_FILES_DIR)/%: $$($1.SOURCE_DATA_ROOT)/%
	@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
	@echo "Copy $$<..."
	@cp -fr "$$<" "$$(dir $$@)"

  $$($1.DST_DATA_FILES_DIR)/%: $$($1.TMP_DIR)/%
	@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
	@echo "Copy $$<..."
	@cp -fr "$$<" "$$(dir $$@)"

  $$($1.EXE_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS) $$($1.OBJECT_FILES)
	@echo Linking $$(notdir $$@)...
	@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
	@$$(call $$(if $$($1.LINK_TOOL),$$($1.LINK_TOOL),$(LINK_TOOL)),$$@,$$($1.OBJECT_FILES) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))

  $$($1.CER_FILE): #$(COMPONENT_DIR)component.mak
	@echo Creating certificate file $$(notdir $$@)...
	@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
	@$(RM) $$@ $$($1.PVK_FILE)
	@"$(WINRT_SDK)/bin/$(HOST_PLATFORM)/MakeCert" $$($1.CER_FILE) -r -n "$$($1.PUBLISHER)" -$$$$ individual -sv $$($1.PVK_FILE) -pe -cy end

  $$($1.PFX_FILE): $$($1.CER_FILE)
	@echo Creating key file $$(notdir $$@)...
	@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
	@$(RM) $$@
	@"$(WINRT_SDK)/bin/$(HOST_PLATFORM)/Pvk2Pfx" -pvk $$($1.PVK_FILE) -spc $$($1.CER_FILE) -pfx $$@

  $$($1.UNSIGNED_APPX_FILE): $$($1.EXE_FILE) $$($1.MANIFEST_FILE) $$($1.PFX_FILE) $$($1.TARGET_DATA_FILES)
	@echo Packaging $$(notdir $$@)...
	@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
	@"$(WINRT_SDK)/bin/$(HOST_PLATFORM)/MakeAppx" pack -d $$($1.DST_DATA_FILES_DIR) -o -p $$@

  $$($1.PACKAGE_FILE): $$($1.UNSIGNED_APPX_FILE)
	@echo Signing $$(notdir $$@)...
	@test -d "$$(dir $$@)" || mkdir -p "$$(dir $$@)"
	@cp -f $$($1.UNSIGNED_APPX_FILE) $$($1.SIGNED_APPX_FILE)
	@"$(WINRT_SDK)/bin/$(HOST_PLATFORM)/SignTool" sign -a -v -fd SHA256 -f $$($1.PFX_FILE) $$($1.SIGNED_APPX_FILE)
	@mv -f $$($1.SIGNED_APPX_FILE) $$@

  install: INSTALL_APPX.$1
  uninstall: UNINSTALL_APPX.$1

  .PHONY: INSTALL_APPX.$1 UNINSTALL_APPX.$1

  INSTALL_APPX.$1: $$($1.APPX_INSTALLATION_FLAG)

  $$($1.APPX_INSTALLATION_FLAG): $$($1.PACKAGE_FILE) $$($1.CER_INSTALLATION_FLAG)
	@echo Installing $$(notdir $$($1.PACKAGE_FILE))...
	@$(if $(INSTALLATION_FLAGS)$(INSTALLATION_FLAG),$(RM) $(INSTALLATION_FLAGS) $(INSTALLATION_FLAG))
	@chcp.com 850 > $$($1.TMP_DIR).chcp && export APPX_UUID=`cat $$($1.MANIFEST_FILE) | sed -n -e 's/.*Name="\([0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*\)".*/\1/p'` && \
         export APPX_FULL_NAME=`PowerShell "get-appxpackage $$$$APPX_UUID" | sed -n -e 's/.*PackageFullName *: *\(.*\)/\1/p'` && \
         if [ -n "$$$$APPX_FULL_NAME" ]; then PowerShell "remove-appxpackage $$$$APPX_FULL_NAME"; fi
	@chcp.com 850 > $$($1.TMP_DIR)/chcp.log && PowerShell "add-appxpackage $$($1.PACKAGE_FILE)"
	@touch $$@

  $$($1.CER_INSTALLATION_FLAG): $$($1.CER_FILE)
	@echo Adding certificate $$(notdir $$($1.CER_FILE))...
	@chcp.com 850 > $$($1.TMP_DIR)/chcp.log && "${SYSTEMROOT}/system32/Certutil" -addStore TrustedPeople $$($1.CER_FILE)
	@touch $$@

  UNINSTALL_APPX.$1:
	@rm -f $$($1.APPX_INSTALLATION_FLAG) $$($1.CER_INSTALLATION_FLAG) $(INSTALLATION_FLAGS) $(INSTALLATION_FLAG)
	@chcp.com 850 > $$($1.TMP_DIR)/chcp.log && export APPX_UUID=`cat $$($1.MANIFEST_FILE) | sed -n -e 's/.*Name="\([0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*\)".*/\1/p'` && \
         export APPX_FULL_NAME=`PowerShell "get-appxpackage $$$$APPX_UUID" | sed -n -e 's/.*PackageFullName *: *\(.*\)/\1/p'` && \
         if [ -n "$$$$APPX_FULL_NAME" ]; then PowerShell "remove-appxpackage $$$$APPX_FULL_NAME"; fi
endef
