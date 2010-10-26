###################################################################################################
#Сборка под MSVC
###################################################################################################

###################################################################################################
#Выбор конфигурации MSVC
###################################################################################################
ifneq (,$(VS100COMNTOOLS))
  MSVC_PATH         ?= $(VS100COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS100COMNTOOLS)../../Common7/Ide
  PROFILES          += vc10 haswchar
endif

ifneq (,$(VS90COMNTOOLS))
  MSVC_PATH         ?= $(VS90COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS90COMNTOOLS)../../Common7/Ide
  PROFILES          += vc9 haswchar
endif

ifneq (,$(VS80COMNTOOLS))
  MSVC_PATH         ?= $(VS80COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS80COMNTOOLS)../../Common7/Ide
  PLATFORM_SDK_PATH ?= $(MSVC_PATH)/platformsdk
  PROFILES          += vc8 haswchar
endif

ifneq (,$(VS71COMNTOOLS))
  MSVC_PATH         ?= $(VS71COMNTOOLS)../../vc7
  MSVS_COMMON_PATH  ?= $(VS71COMNTOOLS)../../Common7/Ide
  PLATFORM_SDK_PATH ?= $(MSVC_PATH)/platformsdk
  PROFILES          += vc7
endif

ifneq (,$(VCTOOLKITINSTALLDIR))
  MSVC_PATH         ?= $(VCTOOLKITINSTALLDIR).
  MSVS_COMMON_PATH  ?= $(VCTOOLKITINSTALLDIR)bin
  PROFILES          += vc7
endif

ifneq (,$(filter vc7,$(PROFILES)))
  COMMON_CFLAGS += -wd4675 -GR
  PROFILES      += nowchar
endif

ifeq (,$(MSVS_COMMON_PATH))
  $(error 'Microsoft Visual Studio not detected (empty MSVC_PATH)')
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

MSVC_PATH          := $(call convert_path,$(MSVC_PATH))
MSVC_BIN_PATH      := $(MSVC_PATH)/bin
MSVS_COMMON_PATH   := $(call convert_path,$(MSVS_COMMON_PATH))
COMMON_CFLAGS      += -W3 -Ox -wd4996 $(if $(analyze),-analyze) -nologo -FC
FRAMEWORK_DIR      := ${SYSTEMROOT}/Microsoft.NET/Framework/v2.0.50727
VALID_TARGET_TYPES += cs-dynamic-lib cs-application

###################################################################################################
#Константы
###################################################################################################
LIB_SUFFIX     := .lib
OBJ_SUFFIX     := .obj
EXE_SUFFIX     := .exe
DLL_SUFFIX     := .dll
DLL_LIB_SUFFIX := .lib
DLL_PREFIX     :=
PROFILES   += msvc win32 has_windows x86 vcx86
COMMON_LINK_FLAGS += -stack:128000 /MACHINE:X86
SOURCE_PROCESS_MACROSES += process_idl process_rc

###################################################################################################
#Конфигурация переменных расположения библиотек
###################################################################################################
INCLUDE := $(MSVC_PATH)/include;$(MSVC_PATH)/atlmfc/include;$(INCLUDE)
LIB     := $(MSVC_PATH)/lib;$(MSVC_PATH)/atlmfc/lib;$(LIB)

ifeq (,$(PLATFORM_SDK_PATH))
  $(error 'Microsoft SDKs not detected (empty PLATFORM_SDK_PATH)')  
endif

INCLUDE := $(PLATFORM_SDK_PATH)/include;$(INCLUDE)
LIB     := $(PLATFORM_SDK_PATH)/lib;$(LIB)

export INCLUDE
export LIB

###################################################################################################
#Компиляция исходников (список исходников, список подключаемых каталогов, список подключаемых файлов, каталог с объектными файлами,
#список дефайнов, флаги компиляции, pch файл, список каталогов с dll)
###################################################################################################
define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" \
&& "$(MSVC_BIN_PATH)/cl" -c -Fo"$4\\" $(patsubst %,-I"%",$2) $(patsubst %,-FI"%",$3) $(COMMON_CFLAGS) $6 $(if $(filter -clr,$6),$(foreach dir,$8 $(DIST_BIN_DIR),-AI $(dir)) -MD,-EHsc -MT) $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.c,$1) $(filter %.cpp,$1) $(if $7,-FI"$7" -Yc"$7" -Fp"$4\\")
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
define tools.lib
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/lib" -nologo -out:$1 $2
endef

###################################################################################################
#Выполнение команды (команда, каталог запуска, базовый каталог, дополнительные пути поиска библиотек и приложений)
###################################################################################################
define tools.run
$(call prepare_to_execute,$2,$4) && chmod u+x "$(CURDIR)/$(firstword $1)" && "$(CURDIR)/$(firstword $1)"
endef

###################################################################################################
#Обработка IDL-файлов (цель, имя модуля)
###################################################################################################
define process_idl
  $2.IDL_SOURCE_LIST := $$(wildcard $$($2.SOURCE_DIR)/*.idl)
  $2.TLB_LIST        := $$($2.IDL_SOURCE_LIST:$$($2.SOURCE_DIR)/%.idl=$$($2.TMP_DIR)/%.tlb)
  $1.FLAG_FILES      := $$($2.TLB_LIST) $$($1.FLAG_FILES)
  $1.TLB_LIST        := $$($1.TLB_LIST) $$($2.TLB_LIST)

  $$($2.TMP_DIR)/%.tlb: $$($2.SOURCE_DIR)/%.idl
		@echo Compile $$(notdir $$<)...
		@export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" && "$(PLATFORM_SDK_PATH)/bin/midl" -nologo $$(patsubst %,-I"%",$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS)) $$(foreach def,$$($2.COMPILER_DEFINES),-D$$(subst %,$(SPACE),$$(def))) -tlb "$$@" -iid $$($2.SOURCE_DIR)/$$(notdir $$(basename $$@))_i.c  -h $$($2.SOURCE_DIR)/$$(notdir $$(basename $$@))_i.h $$<
endef

###################################################################################################
#Обработка RC-файлов (цель, имя модуля)
###################################################################################################
define process_rc
  $2.RC_SOURCE_LIST := $$(wildcard $$($2.SOURCE_DIR)/*.rc)
  $1.OBJECT_FILES   := $$($1.OBJECT_FILES) $$($2.RC_SOURCE_LIST:$$($2.SOURCE_DIR)/%.rc=$$($2.TMP_DIR)/%.res)

  $$($2.TMP_DIR)/%.res: $$($2.SOURCE_DIR)/%.rc $$($1.TLB_LIST)
		@echo Compile $$(notdir $$<)...
		@export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" && "$(PLATFORM_SDK_PATH)/bin/rc" $$(patsubst %,-i"%",$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS) $$($2.TMP_DIR)) $$(foreach def,$$($2.COMPILER_DEFINES),-d$$(subst %,$(SPACE),$$(def))) -fo "$$@" $$<
endef

###################################################################################################
#Компиляция исходников C# (имя выходного файла, список исходников, список необходимых длл, список подключаемых dll каталогов,
#список дефайнов, флаги компиляции)
###################################################################################################
define tools.cscompile
export PATH="$(FRAMEWORK_DIR);$$PATH" && "$(FRAMEWORK_DIR)/csc" -nologo $6 -out:"$1" $(if $(filter %.dll,$1),-t:library,-t:exe) $(patsubst %,-lib:"%",$4 $(DIST_BIN_DIR)) $(patsubst %,-reference:"%.dll",$3) $(if $(strip $5),-define:"$(strip $5)") $(subst /,\\,$2)
endef

#Обработка каталога с C# исходниками (имя цели, имя каталога)
define process_cs_source_dir 
  ifneq (,$$(wildcard $2/sources.mak))
    SOURCE_FILES :=
  
    include $2/sources.mak    

    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $$(SOURCE_FILES:%=$2/%))
  else
    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $2/*.cs)
  endif
endef

#Обработка цели cs-assembly (имя цели, расширение целевого файла)
define process_target_csassembly
  $1.SOURCE_DIRS := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_cs_source_dir,$1,$$(dir))))

  $1.TARGET_DLLS   := $$($1.DLLS:%=$(DIST_BIN_DIR)/%$(DLL_SUFFIX))    
  $1.TARGET        := $(DIST_BIN_DIR)/$$($1.NAME)$2
  $1.DLL_DIRS      := $$(call specialize_paths,$$($1.DLL_DIRS)) $(DIST_BIN_DIR)
  $1.EXECUTION_DIR := $$(strip $$($1.EXECUTION_DIR))
  $1.EXECUTION_DIR := $$(if $$($1.EXECUTION_DIR),$(COMPONENT_DIR)$$($1.EXECUTION_DIR))
  DIST_DIRS        := $$(DIST_DIRS) $$(dir $$($1.TARGET))  

  $$($1.TARGET): $$($1.SOURCE_FILES) $$($1.TARGET_DLLS)
		@echo Compile $$(notdir $$@)...
		@$$(call tools.cscompile,$$@,$$($1.SOURCE_FILES),$$($1.DLLS),$$($1.DLL_DIRS),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS))

  BUILD.$1: $$($1.TARGET_DLLS) $$($1.TARGET)

  .PHONY: BUILD.$1

  build: BUILD.$1
  
  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))
endef

#Обработка цели cs-dynamic-lib (имя цели)
define process_target.cs-dynamic-lib
  $$(eval $$(call process_target_csassembly,$1,$(DLL_SUFFIX)))
endef

#Обработка цели cs-application (имя цели)
define process_target.cs-application
  $$(eval $$(call process_target_csassembly,$1,$(EXE_SUFFIX)))
   
  ifeq (,$$($1.EXECUTION_DIR))
    $1.EXECUTION_DIR := $(DIST_BIN_DIR)
  endif

  RUN.$1: $$($1.TARGET)

  .PHONY: RUN.$1

  RUN.$1: $$($1.TARGET)
		@echo Running $$(notdir $$<)...
		@$$(call prepare_to_execute,$$($1.EXECUTION_DIR),$$($1.DLL_DIRS)) && && $$(patsubst %,"$(CURDIR)/%",$$<) $(args)

  ifneq (,$$(filter $$(files:%=$(DIST_BIN_DIR)/%$(EXE_SUFFIX)),$$($1.TARGET)))
    run: RUN.$1
  endif  
endef
