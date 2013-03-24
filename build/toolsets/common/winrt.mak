###################################################################################################
#Сборка под WindowsRT
###################################################################################################

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
PROFILES       += msvc has_windows win8 winrt
FRAMEWORK_DIR  := ${SYSTEMROOT}/Microsoft.NET/Framework/v4.0.30319
SOURCE_FILES_SUFFIXES += asm
VALID_TARGET_TYPES += win8-appx

###################################################################################################
#Конфигурация переменных расположения библиотек
###################################################################################################
INCLUDE := $(WINRT_SDK)/include;$(WINRT_SDK)/include/shared;$(WINRT_SDK)/include/um;$(WINRT_SDK)/include/winrt;$(MSVC_PATH)/include;$(INCLUDE)
LIB     := $(WINRT_SDK)/lib/win8/um/$(CPU_ARCH);$(MSVC_PATH)/lib$(if $(filter x86,$(CPU_ARCH)),);$(LIB)

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
  $1.TMP_DIR        := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.OUT_DIR        := $$(if $$($1.OUT_DIR),$$(COMPONENT_DIR)/$$($1.OUT_DIR),$$(DIST_LIB_DIR))
  $1.EXE_FILE       := $$($1.TMP_DIR)/$$($1.NAME)$$(if $$(suffix $$($1.NAME)),,$(EXE_SUFFIX))
  $1.PACKAGE_FILE   := $$($1.OUT_DIR)/$$($1.NAME).appx
  $1.MANIFEST_FILE  := $(COMPONENT_DIR)$$($1.MANIFEST_FILE)
  $1.PFX_FILE       := $(COMPONENT_DIR)$$($1.PFX_FILE)
  TARGET_FILES      := $$(TARGET_FILES) $$($1.PACKAGE_FILE)
  $1.TARGET_DLLS    := $$($1.DLLS:%=$$($1.OUT_DIR)/$(DLL_PREFIX)%$(DLL_SUFFIX))

  $$(eval $$(call process_target_with_sources,$1))

  build: BUILD.$1

  .PHONY: BUILD.$1

  BUILD.$1: $$($1.PACKAGE_FILE)

  $$($1.EXE_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS) $$($1.OBJECT_FILES)
	@echo Linking $$(notdir $$@)...
	@$$(call $$(if $$($1.LINK_TOOL),$$($1.LINK_TOOL),$(LINK_TOOL)),$$@,$$($1.OBJECT_FILES) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))

  $$($1.PFX_FILE):
	@echo Create $$(notdir $$@)...
#	@$(WINRT_SDK)/
#rem MakeCert -n "CN=leny" -r -h 0 -eku "1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13" -e 03/24/2014 -sv MyKey.pvk MyKey.cer
#rem rem Pvk2Pfx -pvk MyKey.pvk -pi 123456 -spc MyKey.cer -pfx MyKey.pfx
#rem Pvk2Pfx -pvk MyKey.pvk -spc MyKey.cer -pfx MyKey.pfx


  $$($1.PACKAGE_FILE): $$($1.EXE_FILE) $$($1.MANIFEST_FILE) $$($1.PFX_FILE)
#	@echo Building $$(notdir $$($1.SOLUTION))...
#	@test -d "$$($1.TMP_DIR)" || mkdir -p "$$($1.TMP_DIR)"
#	@chcp.com 850 > $$($1.TMP_DIR)/chcp.stdout && $(call convert_path,$(FRAMEWORK_DIR))/msbuild.exe $$($1.SOLUTION) -p:VisualStudioVersion=11.0 -property:Configuration="Release" -property:Platform="$(TARGET_PLATFORM)" > $$($1.TMP_DIR).msbuild-out
endef
