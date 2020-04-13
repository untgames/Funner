###################################################################################################
#Build for MSVC
###################################################################################################

###################################################################################################
#Choose MSVC configuration
###################################################################################################

VSWHERE_PATH := C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe

ifneq (,$(wildcard $(subst $(SPACE),\$(SPACE),$(VSWHERE_PATH))))

ifeq (,$(UCRT_PATH))
  $(error 'Universal CRT path not defined (empty UCRT_PATH, default is c:\Program Files (x86)\Windows Kits\10)')
endif

ifeq (,$(UCRT_VERSION))
  $(error 'Universal CRT version not defined (empty UCRT_VERSION)')
endif

  MSVC_PATH         ?= $(shell "$(VSWHERE_PATH)" -latest -find VC/**/atlmfc)/../
  MSVC_BIN_PATH     ?= $(MSVC_PATH)bin/Hostx86/x86
  MSVS_COMMON_PATH  ?= $(shell "$(VSWHERE_PATH)" -latest -find Common7/IDE)
  PROFILES          += vc14 vc14_1 haswchar  
  COMMON_CFLAGS     += -wd4005
  INCLUDE           := $(UCRT_PATH)/Include/$(UCRT_VERSION)/ucrt/;$(INCLUDE)

else

ifneq (,$(VS140COMNTOOLS))

ifeq (,$(UCRT_PATH))
  $(error 'Universal CRT path not defined (empty UCRT_PATH, default is c:\Program Files (x86)\Windows Kits\10)')
endif

ifeq (,$(UCRT_VERSION))
  $(error 'Universal CRT version not defined (empty UCRT_VERSION)')
endif

  MSVC_PATH         ?= $(VS140COMNTOOLS)../../VC
  MSVS_COMMON_PATH  ?= $(VS140COMNTOOLS)../../Common7/Ide
  PROFILES          += vc14 haswchar  
  COMMON_CFLAGS     += -wd4005
  INCLUDE           := $(UCRT_PATH)/Include/$(UCRT_VERSION)/ucrt/;$(INCLUDE)
endif

endif

ifneq (,$(VS130COMNTOOLS))
  MSVC_PATH         ?= $(VS130COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS130COMNTOOLS)../../Common7/Ide
  PROFILES          += vc13 haswchar  
  COMMON_CFLAGS     += -wd4005
endif

ifneq (,$(VS120COMNTOOLS))
  MSVC_PATH         ?= $(VS120COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS120COMNTOOLS)../../Common7/Ide
  PROFILES          += vc12 haswchar  
  COMMON_CFLAGS     += -wd4005
endif

ifneq (,$(VS110COMNTOOLS))
  MSVC_PATH         ?= $(VS110COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS110COMNTOOLS)../../Common7/Ide
  PROFILES          += vc11 haswchar
  COMMON_CFLAGS     += -wd4005
endif

ifeq (,$(MSVS_COMMON_PATH))
  $(error 'Microsoft Visual Studio not detected (empty MSVC_PATH)')
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

ifeq (1,$(analyze))
  ifeq (,$(PVS_STUDIO_DIR))
    $(error 'PVS Studio not found (empty PVS_STUDIO_DIR)')
  endif
endif

MSVC_PATH          := $(call convert_path,$(MSVC_PATH))
PVS_STUDIO_DIR     := $(call convert_path,$(PVS_STUDIO_DIR))
MSVC_BIN_PATH      ?= $(MSVC_PATH)/bin
MSVS_COMMON_PATH   := $(call convert_path,$(MSVS_COMMON_PATH))
COMMON_CFLAGS      += -W3 -Ox -wd4996 $(if $(analyze),-analyze) -nologo -FC
FRAMEWORK_DIR      := ${SYSTEMROOT}/Microsoft.NET/Framework/v2.0.50727
VALID_TARGET_TYPES += cs-dynamic-lib cs-application

###################################################################################################
#Constants
###################################################################################################
LIB_SUFFIX     := .lib
OBJ_SUFFIX     := .obj
EXE_SUFFIX     := .exe
DLL_SUFFIX     := .dll
DLL_LIB_SUFFIX := .lib
DLL_PREFIX     :=
PROFILES                += msvc has_windows
SOURCE_PROCESS_MACROSES += process_idl process_rc
SOURCE_FILES_SUFFIXES   += asm
IGNORE_PVS_ERROR        += V126 V122 V201

###################################################################################################
#Configuration of libraries location variables
###################################################################################################
INCLUDE := $(MSVC_PATH)/include;$(MSVC_PATH)/atlmfc/include;$(INCLUDE)

ifeq (,$(PLATFORM_SDK_PATH))
#chedck default path
PLATFORM_SDK_DEFAULT_PATH := C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/

ifneq (,$(wildcard $(subst $(SPACE),\$(SPACE),$(PLATFORM_SDK_DEFAULT_PATH))))
  PLATFORM_SDK_PATH := $(PLATFORM_SDK_DEFAULT_PATH)
endif

endif

ifeq (,$(PLATFORM_SDK_PATH))
  $(error 'Microsoft SDKs not detected (empty PLATFORM_SDK_PATH, default is c:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\)')  
endif

INCLUDE := $(PLATFORM_SDK_PATH)/include;$(INCLUDE)

export INCLUDE

###################################################################################################
#Sources compilation (sources list, include directories list, include files list, object files directory,
#defines list, compilation flags, pch file, dlls directories list)
###################################################################################################
define tools.msvc-commandline
-c -Fo"$4\\" $(patsubst %,-I"%",$2) $(patsubst %,-FI"%",$3) $(if $(filter -W%,$6),$(filter-out -W%,$(COMMON_CFLAGS)),$(COMMON_CFLAGS)) $(strip $6) $(if $(filter -clr,$6),$(foreach dir,$8 $(DIST_BIN_DIR),-AI $(dir)) -MD,-EHsc -MT) $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.c,$1) $(filter %.cpp,$1) $(if $7,-FI"$7" -Yc"$7" -Fp"$4\\")
endef

define tools.pvs-studio
export PVS_EXCLUDE_PATH=`echo "$(subst /,\,$(MSVC_PATH))" | sed -e 's/\(.*\)\\Common7.*/\1/'` && \
$(call for_each_file,src,$1,"$(PVS_STUDIO_DIR)/x86/PVS-Studio" --vcinstalldir "$(MSVC_PATH)" --exclude-path "$$PVS_EXCLUDE_PATH"  --platform Win32 --cfg $(BUILD_DIR)pvs/pvs-studio.cfg --source-file $$src --cl-params $(call tools.msvc-commandline,$$src,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(strip $(IGNORE_PVS_ERROR)), > $4/$$src.pvs && sed $(foreach err,$(IGNORE_PVS_ERROR), -e '/.*error $(err):.*/{N;d;}' < $4/$$src.pvs)))
endef

define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" \
$(if $(filter %.c,$1)$(filter %.cpp,$1),&& $(if $(analyze),$(call tools.pvs-studio,$1,$2,$3,$4,$5,$6,$7,$8,$9) && ) "$(MSVC_BIN_PATH)/cl" $(call tools.msvc-commandline,$1,$2,$3,$4,$5,$6,$7,$8,$9)) \
$(if $(filter %.asm,$1),&& "$(MSVC_BIN_PATH)/$(ML_NAME)" -nologo -c -Fo"$4\\" $(patsubst %,-I"%",$2) $6 $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.asm,$1))
endef

###################################################################################################
#Linking files (output file name, files list, static libraries directories list,
#include link symbols list, link flags, def file)
###################################################################################################
define tools.link
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/link" -nologo -out:"$1" $(if $(filter %.dll,$1),-dll) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"$(LINK_INCLUDE_PREFIX)%",$4) $5 $2 $(COMMON_LINK_FLAGS) $(if $(map),-MAP:$(basename $1).map -MAPINFO:EXPORTS) $(if $6,-DEF:"$6")
endef

###################################################################################################
#Library building (output file name, files list)
###################################################################################################
define tools.lib
export PATH="$(MSVS_COMMON_PATH);$$PATH" && echo -nologo -out:$1 $2 > $1.commandline && "$(MSVC_BIN_PATH)/lib" @$1.commandline && $(dir $(SHELL))/$(RM) $1.commandline
endef

###################################################################################################
#Execute command (command, execution directory, libraries and executables additional search paths)
###################################################################################################
define tools.run
$(call prepare_to_execute,$2,$3) && chmod u+x "$(CURDIR)/$(firstword $1)" && "$(CURDIR)/$(firstword $1)"
endef

###################################################################################################
#Process IDL-files (target, module name)
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
#Process RC-files (target, module name)
###################################################################################################
define process_rc
  $2.RC_SOURCE_LIST := $$(wildcard $$($2.SOURCE_DIR)/*.rc)
  $1.OBJECT_FILES   := $$($1.OBJECT_FILES) $$($2.RC_SOURCE_LIST:$$($2.SOURCE_DIR)/%.rc=$$($2.TMP_DIR)/%.res)

  $$($2.TMP_DIR)/%.res: $$($2.SOURCE_DIR)/%.rc $$($1.TLB_LIST)
		@echo Compile $$(notdir $$<)...
		@export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" && "$(PLATFORM_SDK_PATH)/bin/rc" $$(patsubst %,-i"%",$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS) $$($2.TMP_DIR)) $$(foreach def,$$($2.COMPILER_DEFINES),-d$$(subst %,$(SPACE),$$(def))) -fo "$$@" $$<
endef

###################################################################################################
#C# sources compilation (output file name, sources list, needed dlls list, included dlls directories list,
#defines list, compilation flags)
###################################################################################################
define tools.cscompile
export PATH="$(FRAMEWORK_DIR);$$PATH" && "$(FRAMEWORK_DIR)/csc" -nologo $6 -out:"$1" $(if $(filter %.dll,$1),-t:library,-t:exe) $(patsubst %,-lib:"%",$4 $(DIST_BIN_DIR)) $(patsubst %,-reference:"%.dll",$3) $(if $(strip $5),-define:"$(strip $5)") $(subst /,\\,$2)
endef

#Process directory with C# sources (target name, directory name)
define process_cs_source_dir 
  ifneq (,$$(wildcard $2/sources.mak))
    SOURCE_FILES :=
  
    include $2/sources.mak    

    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $$(SOURCE_FILES:%=$2/%))
  else
    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $2/*.cs)
  endif
endef

#Process target cs-assembly (target name, output file extension)
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

#Process target cs-dynamic-lib (target name)
define process_target.cs-dynamic-lib
  $$(eval $$(call process_target_csassembly,$1,$(DLL_SUFFIX)))
endef

#Process target cs-application (target name)
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

include $(TOOLSETS_DIR)/common/mono.mak
