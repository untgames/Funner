MONO_CSC           := $(MONO)/bin/gmcs
VALID_TARGET_TYPES += mono-cs-dynamic-lib mono-cs-application

###################################################################################################
#Компиляция исходников C# (имя выходного файла, список исходников, список необходимых длл, список подключаемых dll каталогов,
#список дефайнов, флаги компиляции)
###################################################################################################
define tools.mono-cscompile
"$(MONO_CSC)" $6 -out:"$1" $(if $(filter %.dll,$1),-t:library,-t:exe) $(patsubst %,-lib:"%",$4 $(DIST_BIN_DIR)) $(patsubst %,-reference:"%.dll",$3) $(if $(strip $5),-define:"$(strip $5)") $(subst /,\\,$2)
endef

#Обработка каталога с C# исходниками (имя цели, имя каталога)
define mono_process_cs_source_dir 
  ifneq (,$$(wildcard $2/sources.mak))
    SOURCE_FILES :=
  
    include $2/sources.mak    

    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $$(SOURCE_FILES:%=$2/%))
  else
    $1.SOURCE_FILES := $$($1.SOURCE_FILES) $$(wildcard $2/*.cs)
  endif
endef

#Обработка цели cs-assembly (имя цели, расширение целевого файла)
define process_target_mono_csassembly
  ifeq (,$$(MONO))
    $$(error 'Mono not found (set "MONO" environment variable)')
  endif

  $1.SOURCE_DIRS := $$($1.SOURCE_DIRS:%=$(COMPONENT_DIR)%)

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call mono_process_cs_source_dir,$1,$$(dir))))

  $1.TARGET_DLLS   := $$($1.DLLS:%=$(DIST_BIN_DIR)/%.dll)
  $1.TARGET        := $(DIST_BIN_DIR)/$$($1.NAME)$2
  $1.DLL_DIRS      := $$(call specialize_paths,$$($1.DLL_DIRS)) $(DIST_BIN_DIR)
  $1.EXECUTION_DIR := $$(strip $$($1.EXECUTION_DIR))
  $1.EXECUTION_DIR := $$(if $$($1.EXECUTION_DIR),$(COMPONENT_DIR)$$($1.EXECUTION_DIR))
  DIST_DIRS        := $$(DIST_DIRS) $$(dir $$($1.TARGET))

  $$($1.TARGET): $$($1.SOURCE_FILES) $$($1.TARGET_DLLS)
		@echo Compile $$(notdir $$@)...
		@$$(call tools.mono-cscompile,$$@,$$($1.SOURCE_FILES),$$($1.DLLS),$$($1.DLL_DIRS),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS))

  BUILD.$1: $$($1.TARGET_DLLS) $$($1.TARGET)

  .PHONY: BUILD.$1

  build: BUILD.$1
  
  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))
endef

#Обработка цели cs-dynamic-lib (имя цели)
define process_target.mono-cs-dynamic-lib
  $$(eval $$(call process_target_mono_csassembly,$1,.dll))
endef

#Обработка цели cs-application (имя цели)
define process_target.mono-cs-application
  $$(eval $$(call process_target_mono_csassembly,$1,.exe))
   
  ifeq (,$$($1.EXECUTION_DIR))
    $1.EXECUTION_DIR := $(DIST_BIN_DIR)
  endif

  RUN.$1: $$($1.TARGET)

  .PHONY: RUN.$1

  RUN.$1: $$($1.TARGET)
		@echo Running $$(notdir $$<)...
		@$$(call prepare_to_execute,$$($1.EXECUTION_DIR),$$($1.DLL_DIRS)) && && $$(patsubst %,"$(CURDIR)/%",$$<) $(args)

  ifneq (,$$(filter $$(files:%=$(DIST_BIN_DIR)/%.exe),$$($1.TARGET)))
    run: RUN.$1
  endif  
endef
