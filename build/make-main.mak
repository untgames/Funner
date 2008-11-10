###################################################################################################
#Константы сборки
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := component.mak #Базовое имя файла конфигурации компонента
TMP_DIR_SHORT_NAME                      := tmp           #Базовое имя каталога с временными файлами
SOURCE_FILES_SUFFIXES                   := c cpp         #Расширения исходных файлов
TMP_DIR_SHORT_NAME                      := tmp           #Базовое имя каталога с временными файлами сборки
DIST_DIR_SHORT_NAME                     := dist          #Базовое имя каталога с результатами сборки
TOOLSETS_DIR_SHORT_NAME                 := toolsets      #Базовое имя каталога с конфигурациями toolset-ов
PCH_SHORT_NAME                          := pch.h         #Базовое имя PCH файла
EXPORT_VAR_PREFIX                       := export        #Префикс имени переменной экспортирования настроек компонента
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := batch-flag    #Базовое имя файла-флага пакетной компиляции
VALID_TARGET_TYPES                      := static-lib dynamic-lib application test-suite package sdk cs-assembly #Допустимые типы целей
PACKAGE_COMMANDS                        := build clean test check run #Команды, делегируемые компонентам пакета
COMPILE_TOOL                            := tools.c++compile #Имя макроса утилиты компиляции C++ файлов
LINK_TOOL                               := tools.link       #Имя макроса утилиты редактора связей
LIB_TOOL                                := tools.lib        #Имя макроса утилиты архивирования объектных файлов
CSC_TOOL                                := tools.cscompile  #Имя макроса утилиты компиляции C# файлов
EXPORT_EXCLUDE_PATTERN                  := .svn            #Шаблон файлов, исключаемых из копирования при выполнении цели export-dirs
EXPORT_TAR_TMP_FILE_SHORT_NAME          := export-file.tar #Базовое имя файла архива, используемого при выполнении цели export-dirs

###################################################################################################
#Производные пути и переменные
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := $(strip $(COMPONENT_CONFIGURATION_FILE_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
SOURCE_FILES_SUFFIXES                   := $(strip $(SOURCE_FILES_SUFFIXES))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
DIST_DIR_SHORT_NAME                     := $(strip $(DIST_DIR_SHORT_NAME))
EXPORT_VAR_PREFIX                       := $(strip $(EXPORT_VAR_PREFIX))
EXPORT_TAR_TMP_FILE_SHORT_NAME          := $(strip $(EXPORT_TAR_TMP_FILE_SHORT_NAME))
EXPORT_EXCLUDE_PATTERN                  := $(strip $(EXPORT_EXCLUDE_PATTERN))
CURRENT_TOOLSET                         := $(TOOLSET)
BUILD_DIR                               := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
TOOLSETS_DIR                            := $(BUILD_DIR)$(strip $(TOOLSETS_DIR_SHORT_NAME))
TOOLSETS                                := $(patsubst $(TOOLSETS_DIR)/%.mak,%,$(wildcard $(TOOLSETS_DIR)/*.mak))
TOOLSET_FILE                            := $(TOOLSETS_DIR)/$(CURRENT_TOOLSET).mak
DIST_DIR                                := $(ROOT)/$(DIST_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)
DIST_LIB_DIR                            := $(DIST_DIR)/lib
DIST_BIN_DIR                            := $(DIST_DIR)/bin
PCH_SHORT_NAME                          := $(strip $(PCH_SHORT_NAME))
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := $(strip $(BATCH_COMPILE_FLAG_FILE_SHORT_NAME))
ROOT_TMP_DIR                            := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)
TMP_DIRS                                := $(ROOT_TMP_DIR)
DIRS                                     = $(TMP_DIRS) $(DIST_DIR) $(DIST_LIB_DIR) $(DIST_BIN_DIR)
COMPILE_TOOL                            := $(strip $(COMPILE_TOOL))
LINK_TOOL                               := $(strip $(LINK_TOOL))
LIB_TOOL                                := $(strip $(LIB_TOOL))
EMPTY                                   :=
SPACE                                   := $(EMPTY) $(EMPTY)

###################################################################################################
#Если не указан фильтры - обрабатываем все доступные
###################################################################################################
files ?= %
targets ?= %

###################################################################################################
#Цель сборки "по умолчанию"
###################################################################################################
default: build

###################################################################################################
#Преобразование Windows-путей
###################################################################################################
define convert_path
$(subst \,/,$1)
endef

###################################################################################################
#Формирование пути для выполнения программы (список путей к dll-файлам)
###################################################################################################
define build_execution_path
$(subst ;,:,$(call convert_path,$(CURDIR)/$(DIST_BIN_DIR);$(foreach dir,$1,$(dir);)$$PATH))
endef

###################################################################################################
#Перебор списка файлов (имя переменной с именем файла, список файлов, действие)
###################################################################################################
define for_each_file
$(if $2,for $1 in $2; do $3; if [ "$$?" -ne "0" ]; then exit 1; fi; done,true)
endef

###################################################################################################
#Подключение файла конфигурации утилит сборки
###################################################################################################
ifeq (,$(filter $(CURRENT_TOOLSET),$(TOOLSETS)))
  $(error Unknown toolset '$(CURRENT_TOOLSET)'. Use one of available toolsets '$(TOOLSETS)')
endif

include $(TOOLSET_FILE)

###################################################################################################
#Проверка наличия констант
###################################################################################################
define check_toolset_constant
  $(if $(filter $1, $(.VARIABLES)),,$(warning Build variable '$1' not defined (check file '$(TOOLSET_FILE)')))
endef

$(call check_toolset_constant,LIB_SUFFIX)
$(call check_toolset_constant,OBJ_SUFFIX)
$(call check_toolset_constant,DLL_SUFFIX)
$(call check_toolset_constant,EXE_SUFFIX)

###################################################################################################
#Подключение файла конфигурации компонента
###################################################################################################
COMPONENT_CONFIGURATION_FILE := $(firstword $(wildcard $(filter %$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME), $(MAKEFILE_LIST:makefile=$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME)))))
COMPONENT_DIR                := $(dir $(COMPONENT_CONFIGURATION_FILE))

ifeq (,$(COMPONENT_CONFIGURATION_FILE))
  $(error Configuration file '$(COMPONENT_CONFIGURATION_FILE_SHORT_NAME)' not found at project tree (root='$(ROOT)'))
endif

include $(COMPONENT_CONFIGURATION_FILE)

###################################################################################################
#Специализация пути
###################################################################################################
define specialize_paths
$(foreach dir,$1,$(COMPONENT_DIR)$(dir))
endef

###################################################################################################
#Обработка целей компонента
###################################################################################################

#Создание зависимости объектного файла от флаг-файла пакетной компиляции (имя исходного файла, временный каталог, флаг-файл)
define create_object_file_dependency
$2/$$(notdir $$(basename $1))$(OBJ_SUFFIX): $1 $3
	@
endef

#Тестирование времени изменения исходных файлов и соотв. им объектных (список исходных файлов, временный каталог)
define test_source_and_object_files
$(foreach src,$1,if [ $(src) -nt $2/$(notdir $(basename $(src)))$(OBJ_SUFFIX) ]; then echo $(src); fi &&) true 
endef

#Правила пакетной компиляции (имя цели, имя модуля)
define batch-compile
  $2.FLAG_FILE  := $$($2.TMP_DIR)/$$(BATCH_COMPILE_FLAG_FILE_SHORT_NAME)
  $1.FLAG_FILES := $$($1.FLAG_FILES) $$($2.FLAG_FILE)

  ifeq (,$$(wildcard $$($2.FLAG_FILE).incomplete-build))
  
    $$($2.FLAG_FILE): $$($2.SOURCE_FILES)
			@echo build-start > $$@.incomplete-build
			@$$(call $(COMPILE_TOOL),$$(sort $$(filter-out force,$$?)),$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS),$$($2.TMP_DIR),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS),$$($2.PCH),$$($1.DLL_DIRS))
			@echo batch-flag-file > $$@
			@$(RM) $$@.incomplete-build

  else  #В случае если сборка была не завершена
  
    $$($2.FLAG_FILE): $2.UPDATED_SOURCE_FILES := $$(shell $$(call test_source_and_object_files,$$($2.SOURCE_FILES),$$($2.TMP_DIR)))
  
    $$($2.FLAG_FILE): $$($2.SOURCE_FILES)
			@$$(if $$($2.UPDATED_SOURCE_FILES),$$(call $(COMPILE_TOOL),$$(sort $$($2.UPDATED_SOURCE_FILES)),$$($2.SOURCE_DIR) $$($1.INCLUDE_DIRS),$$($2.TMP_DIR),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS),$$($2.PCH),$$($1.DLL_DIRS)))
			@echo batch-flag-file > $$@
			@$(RM) $$@.incomplete-build

  endif

  ifneq (,$$($2.NEW_SOURCE_FILES))
    $$($2.FLAG_FILE): force
  endif
  
  $$(foreach source,$$($2.SOURCE_FILES),$$(eval $$(call create_object_file_dependency,$$(source),$$($2.TMP_DIR),$$($2.FLAG_FILE))))
endef

#Обработка каталога с исходными файлами (имя цели, путь к каталогу с исходными файлами, список применяемых макросов)
define process_source_dir
  ifeq (,$$(wildcard $2))
    $$(error Source dir '$2' not found at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  MODULE_PATH := $$(subst ./,,$$(subst ../,,$2))
  MODULE_NAME := $1.$$(subst /,-,$$(MODULE_PATH))
  
  ifneq (,$$(wildcard $2/sources.mak))
    SOURCE_FILES :=
  
    include $2/sources.mak    

    $$(MODULE_NAME).SOURCE_FILES := $$(wildcard $$(SOURCE_FILES:%=$2/%))
  else
    $$(MODULE_NAME).SOURCE_FILES := $$(wildcard $$(SOURCE_FILES_SUFFIXES:%=$2/*.%))
  endif

  $$(MODULE_NAME).SOURCE_DIR   := $2
  $$(MODULE_NAME).TMP_DIR      := $$($1.TMP_DIR)/$$(MODULE_PATH)
  $$(MODULE_NAME).OBJECT_FILES := $$(patsubst %,$$($$(MODULE_NAME).TMP_DIR)/%$(OBJ_SUFFIX),$$(notdir $$(basename $$($$(MODULE_NAME).SOURCE_FILES))))
  $$(MODULE_NAME).PCH          := $$(if $$(wildcard $2/$(PCH_SHORT_NAME)),$(PCH_SHORT_NAME))
  $1.TMP_DIRS                  := $$($$(MODULE_NAME).TMP_DIR) $$($1.TMP_DIRS)
  $1.OBJECT_FILES              := $$($1.OBJECT_FILES) $$($$(MODULE_NAME).OBJECT_FILES)

  $$(foreach macros,batch-compile $3,$$(eval $$(call $$(macros),$1,$$(MODULE_NAME))))
endef

#Создание зависимости для копирования внешних файлов (имя внешнего файла, каталоги поиска)
define create_extern_file_dependency
  $1: $$(firstword $$(wildcard $$(patsubst %,%/$$(notdir $1),$2)) $$(notdir $1))
		@cp -fv $$< $$@
		@chmod ug+rwx $$@
endef

#Общее для целей с исходными файлами (имя цели, список макросов применяемых для обработки каталогов с исходными файлами)
define process_target_with_sources
  $1.TMP_DIR        := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.TMP_DIRS       := $$($1.TMP_DIR)
  $1.INCLUDE_DIRS   := $$(call specialize_paths,$$($1.INCLUDE_DIRS))
  $1.SOURCE_DIRS    := $$(call specialize_paths,$$($1.SOURCE_DIRS))
  $1.LIB_DIRS       := $$(call specialize_paths,$$($1.LIB_DIRS)) $(DIST_LIB_DIR)
  $1.DLL_DIRS       := $$(call specialize_paths,$$($1.DLL_DIRS))
  $1.EXECUTION_DIR  := $$(strip $$($1.EXECUTION_DIR))
  $1.EXECUTION_DIR  := $$(if $$($1.EXECUTION_DIR),$(COMPONENT_DIR)$$($1.EXECUTION_DIR))
  $1.LIBS           := $$($1.LIBS:%=$(LIB_PREFIX)%$(LIB_SUFFIX))
  $1.TARGET_DLLS    := $$($1.DLLS:%=$(DIST_BIN_DIR)/%$(DLL_SUFFIX))
  $1.LIB_DEPS       := $$(filter $$(addprefix %/,$$($1.LIBS)),$$(wildcard $$($1.LIB_DIRS:%=%/*)))

  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_source_dir,$1,$$(dir),$2)))

  TMP_DIRS := $$($1.TMP_DIRS) $$(TMP_DIRS)
  TMP_CLEAN_DIRS := $(TMP_CLEAN_DIRS) $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1

  build: $$($1.TARGET_DLLS)  

  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))  
endef

#Обработка цели static-lib (имя цели)
define process_target.static-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty static name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.LIB_FILE  := $(DIST_LIB_DIR)/$(LIB_PREFIX)$$($1.NAME)$(LIB_SUFFIX)
  TARGET_FILES := $$(TARGET_FILES) $$($1.LIB_FILE)
  
  build: $$($1.LIB_FILE)

  $$(eval $$(call process_target_with_sources,$1))  

  $$($1.LIB_FILE): $$($1.FLAG_FILES)
		@echo Create library $$(notdir $$@)...
		@$$(call $(LIB_TOOL),$$@,$$($1.OBJECT_FILES))
endef

#Обработка цели dynamic-lib (имя цели)
define process_target.dynamic-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty dynamic library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.DLL_FILE  := $(DIST_BIN_DIR)/$$($1.NAME)$(DLL_SUFFIX)
  $1.LIB_FILE  := $$(dir $$($1.DLL_FILE))$(LIB_PREFIX)$$(notdir $$(basename $$($1.DLL_FILE)))$(LIB_SUFFIX)
  TARGET_FILES := $$(TARGET_FILES) $$($1.DLL_FILE) $(DIST_LIB_DIR)/$$(notdir $$(basename $$($1.DLL_FILE)))$(LIB_SUFFIX)

  build: $$($1.DLL_FILE)  

  $$(eval $$(call process_target_with_sources,$1))

  $$($1.DLL_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS)
		@echo Create dynamic library $$(notdir $$@)...
		@$$(call $(LINK_TOOL),$$@,$$($1.OBJECT_FILES) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))
		@$(RM) $$(basename $$@).exp
		@if [ -x $$($1.LIB_FILE) ]; then mv -f $$($1.LIB_FILE) $(DIST_LIB_DIR); fi
endef

#Обработка цели application (имя цели)
define process_target.application
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty application name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.EXE_FILE  := $(DIST_BIN_DIR)/$$($1.NAME)$$(if $$(suffix $$($1.NAME)),,$(EXE_SUFFIX))
  TARGET_FILES := $$(TARGET_FILES) $$($1.EXE_FILE)

  build: $$($1.EXE_FILE)

  $$(eval $$(call process_target_with_sources,$1))
  
  ifeq (,$$($1.EXECUTION_DIR))
    $1.EXECUTION_DIR := $(DIST_BIN_DIR)
  endif
  
  $$($1.EXE_FILE): $$($1.FLAG_FILES) $$($1.LIB_DEPS)
		@echo Linking $$(notdir $$@)...
		@$$(call $(LINK_TOOL),$$@,$$($1.OBJECT_FILES) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))

  RUN.$1: $$($1.EXE_FILE)
		@echo Running $$(notdir $$<)...
		@export PATH="$$(call build_execution_path,$$($1.DLL_DIRS))" && cd $$($1.EXECUTION_DIR) && $$(patsubst %,"$(CURDIR)/%",$$<) $(args)

  ifneq (,$$(filter $$(files:%=$(DIST_BIN_DIR)/%$(EXE_SUFFIX)),$$($1.EXE_FILE)))
    run: RUN.$1
  endif
endef

#Обработка каталога с исходными файлами тестов (имя цели, имя модуля)
define process_tests_source_dir
  $2.TEST_EXE_FILES    := $$(filter $$(files:%=$$($2.TMP_DIR)/%$(EXE_SUFFIX)),$$($2.OBJECT_FILES:%$(OBJ_SUFFIX)=%$(EXE_SUFFIX)))
  $2.TEST_RESULT_FILES := $$(patsubst $$($2.SOURCE_DIR)/%,$$($2.TMP_DIR)/%,$$(wildcard $$($2.SOURCE_DIR)/*.result))
  $2.EXECUTION_DIR     := $$(if $$($1.EXECUTION_DIR),$$($1.EXECUTION_DIR),$$($2.SOURCE_DIR))
  $1.TARGET_DLLS       := $$($1.TARGET_DLLS) $$($1.DLLS:%=$$($2.TMP_DIR)/%$(DLL_SUFFIX))

  build: $$($2.TEST_EXE_FILES)
  test: TEST_MODULE.$2
  check: CHECK_MODULE.$2
  .PHONY: TEST_MODULE.$2 CHECK_MODULE.$2
  
#Правило сборки теста
  $$($2.TMP_DIR)/%$(EXE_SUFFIX): $$($2.TMP_DIR)/%$(OBJ_SUFFIX) $$($1.LIB_DEPS)
		@echo Linking $$(notdir $$@)...
		@$$(call $(LINK_TOOL),$$@,$$(filter %$(OBJ_SUFFIX),$$<) $$($1.LIBS),$$($1.LIB_DIRS),$$($1.LINK_INCLUDES),$$($1.LINK_FLAGS))

#Правило получения файла-результата тестирования
  $$($2.TMP_DIR)/%.result: $$($2.TMP_DIR)/%$(EXE_SUFFIX)
		@echo Running $$(notdir $$<)...
		@export PATH="$$(call build_execution_path,$$($1.DLL_DIRS))" && cd $$($2.EXECUTION_DIR) && $$(patsubst %,"$(CURDIR)/%",$$<) > $$(patsubst %,"$(CURDIR)/%",$$@)

#Правило запуска тестов
  TEST_MODULE.$2: $$($2.TEST_EXE_FILES)
		@export PATH="$$(call build_execution_path,$$($1.DLL_DIRS))" && cd $$($2.EXECUTION_DIR) && $$(call for_each_file,file,$$(patsubst %,"$(CURDIR)/%",$$(filter $$(files:%=$$($2.TMP_DIR)/%$(EXE_SUFFIX)),$$^)),$$$$file)

#Правило проверки результатов тестирования
  CHECK_MODULE.$2: $$($2.TEST_RESULT_FILES)
		@echo Checking results of module '$2'...
		@$$(call for_each_file,file,$$(notdir $$(filter $$(files:%=$$($2.TMP_DIR)/%.result),$$^)),diff --strip-trailing-cr --context=1 $$($2.SOURCE_DIR)/$$$$file $$($2.TMP_DIR)/$$$$file)
endef

#Обработка цели test-suite (имя цели)
define process_target.test-suite
  $$(eval $$(call process_target_with_sources,$1,process_tests_source_dir))
  
  build: $$($1.FLAG_FILES)
endef

#Обработка компонентов пакета (имя пакета, команда make)
define process_package_components
  $2: PACKAGE_$2.$1

  .PHONY: PACKAGE_$2.$1

  PACKAGE_$2.$1:
		@$$(foreach component,$$($1.COMPONENTS),$(MAKE) -C $$(subst :, ,$(COMPONENT_DIR)/$$(component)) $2 && ) true
endef

#Обработка цели package (имя цели)
define process_target.package
  ifneq (,$$($1.COMPONENTS))
    $$(foreach command,$(PACKAGE_COMMANDS),$$(eval $$(call process_package_components,$1,$$(command))))
  else
    $$(warning Empty package at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif  
endef

#Обработка цели sdk (имя цели)
define process_target.sdk

ifneq (,$$($1.EXPORT_DIRS))
  $1.TMP_DIR        := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1
  $1.EXPORT_DIRS    := $$(sort $$(call specialize_paths,$$($1.EXPORT_DIRS)))
  $1.EXPORT_TMP_TAR := $$($1.TMP_DIR)/$(EXPORT_TAR_TMP_FILE_SHORT_NAME)

  TMP_DIRS := $$($1.TMP_DIR) $$(TMP_DIRS)
  TMP_CLEAN_DIRS := $(TMP_CLEAN_DIRS) $(ROOT)/$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/$1

    #Экспорт каталогов
  export-dirs: EXPORT_DIRS_TARGET.$1  
  .PHONY: EXPORT_DIRS_TARGET.$1
  
  EXPORT_DIRS_TARGET.$1:
		@echo Export dirs "$(DIST_DIR) <- $$($1.EXPORT_DIRS)"
		@tar --exclude=$(EXPORT_EXCLUDE_PATTERN) -cf $$($1.EXPORT_TMP_TAR) $$($1.EXPORT_DIRS)
		@tar --directory=$(DIST_DIR) --overwrite --overwrite-dir -xf $$($1.EXPORT_TMP_TAR)
endif

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

#Обработка цели cs-dll (имя цели)
define process_target.cs-assembly
  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_cs_source_dir,$1,$$(dir))))
  
  $1.SOURCE_FILES   := $$($1.SOURCE_FILES:%=$(COMPONENT_DIR)%)
  $1.NAME           := $(COMPONENT_DIR)$$($1.NAME)
  $1.DLL_DIRS       := $$(call specialize_paths,$$($1.DLL_DIRS))
  $1.EXECUTION_DIR  := $$(strip $$($1.EXECUTION_DIR))
  $1.EXECUTION_DIR  := $$(if $$($1.EXECUTION_DIR),$(COMPONENT_DIR)$$($1.EXECUTION_DIR))
  $1.TARGET_DLLS    := $$($1.DLLS:%=$(DIST_BIN_DIR)/%$(DLL_SUFFIX))

  build: $$($1.NAME)  

  $$($1.NAME): $$($1.SOURCE_FILES)
		@echo Compile $$@...
		@$$(call $(CSC_TOOL),$$($1.NAME),$$($1.SOURCE_FILES),$$($1.DLL_DIRS),$$($1.COMPILER_DEFINES),$$($1.COMPILER_CFLAGS))

  build: $$($1.TARGET_DLLS)

  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))
endef

#Импортирование переменных (префикс источника, префикс приёмника, относительный путь к используемому компоненту)
define import_variables
#  $$(warning src='$1' dst='$2' path='$3')  

  $2.INCLUDE_DIRS     := $$($2.INCLUDE_DIRS) $$($1.INCLUDE_DIRS:%=$3%)
  $2.LIB_DIRS         := $$($2.LIB_DIRS) $$($1.LIB_DIRS:%=$3%)
  $2.DLL_DIRS         := $$($2.DLL_DIRS) $$($1.DLL_DIRS:%=$3%)
  $2.DLLS             := $$($2.DLLS) $$($1.DLLS)
  $2.LIBS             := $$($2.LIBS) $$($1.LIBS)
  $2.COMPILER_CFLAGS  := $$($2.COMPILER_CFLAGS) $$($1.COMPILER_CFLAGS)
  $2.COMPILER_DEFINES := $$($2.COMPILER_DEFINES) $$($1.COMPILER_DEFINES)
  $2.LINK_INCLUDES    := $$($2.LINK_INCLUDES) $$($1.LINK_INCLUDES)
  $2.LINK_FLAGS       := $$($2.LINK_FLAGS) $$($1.LINK_FLAGS)
endef

#Импортирование настроек
define import_settings
#Получение относительного пути к используемому компоненту
  DEPENDENCY_EXPORT_FILE   := $$(COMPONENT_DIR)$1
  DEPENDENCY_COMPONENT_DIR := $$(dir $1)

#  $$(warning exporter='$$(DEPENDENCY_EXPORT_FILE)' dep-dir='$$(DEPENDENCY_COMPONENT_DIR)' importer=$2)

#Проверка наличия файла зависимости
  $$(if $$(wildcard $$(DEPENDENCY_EXPORT_FILE)),,$$(error Component export file '$$(DEPENDENCY_EXPORT_FILE)' not found))

#Очистка переменных $(EXPORT_VAR_PREFIX).*
  $$(foreach var,$$(filter $$(EXPORT_VAR_PREFIX).%,$$(.VARIABLES)),$$(eval $$(var) :=))

#Подключение файла зависимости
  include $$(DEPENDENCY_EXPORT_FILE)

#Изменение настроек
  $$(eval $$(call import_variables,$(EXPORT_VAR_PREFIX),$2,$$(DEPENDENCY_COMPONENT_DIR)))
  $$(foreach profile,$(PROFILES),$$(eval $$(call import_variables,$(EXPORT_VAR_PREFIX).$$(profile),$2,$$(DEPENDENCY_COMPONENT_DIR))))

#Импортирование вложенных зависимостей
  DEPENDENCY_IMPORTS  := $$($$(EXPORT_VAR_PREFIX).IMPORTS:%=$(dir $1)%)

  $$(foreach imp,$$(DEPENDENCY_IMPORTS),$$(eval $$(call import_settings,$$(imp),$2)))
endef

#Импорт значений toolset-настроек (имя цели, имя профиля)
define import_toolset_settings
$$(foreach var,$$(filter $1.$2.%,$$(.VARIABLES)),$$(eval $$(var:$1.$2.%=$1.%) = $$($$(var:$1.$2.%=$1.%)) $$($$(var))))
endef

#Обработка сборки цели
define process_target_common
    #Добавление toolset-настроек к общему списку настроек
  $$(foreach profile,$$(PROFILES),$$(eval $1.IMPORTS := $$($1.IMPORTS) $$($1.$$(profile).IMPORTS)))  

    #Импорт настроек
  $$(foreach imp,$$($1.IMPORTS),$$(eval $$(call import_settings,$$(imp),$1)))
  
    #Добавление toolset-настроек к общему списку настроек
  $$(foreach profile,$$(PROFILES),$$(eval $$(call import_toolset_settings,$1,$$(profile))))

  DUMP.$1:
		@echo Dump target \'$1\' settings
		@$$(foreach var,$$(sort $$(filter $1.%,$(.VARIABLES))),echo '    $$(var:$1.%=%) = $$($$(var))' && ) true

  dump: DUMP.$1

  .PHONY: DUMP.$1
  
  $$(eval $$(call process_target.$$(strip $$($1.TYPE)),$1))  
endef

#Проверка корректности типа цели (тип цели)
define test_target_type
  ifeq (,$$(findstring $$(strip $$($1.TYPE)),$(VALID_TARGET_TYPES)))
    $$(error Wrong target type '$$(strip $$($1.TYPE))' at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif
endef

###################################################################################################
#Правила сборки
###################################################################################################
all: build check
run: build
build: create-dirs
rebuild: clean build
#test: build
#check: build
export-dirs: create-dirs
#dist: check
dist : export-dirs
force:

.PHONY: build rebuild clean fullyclean run test check help create-dirs force dump export-dirs dist tar-dist

#Обработка целей компонента
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call test_target_type,$(target))))
$(foreach target,$(filter $(targets),$(TARGETS)),$(eval $(call process_target_common,$(target))))

#Создание каталогов
create-dirs: $(DIRS)

$(DIRS):
	@mkdir -p $@

#Очистка
clean:
	@cd $(ROOT_TMP_DIR) && $(RM) -r $(TMP_CLEAN_DIRS:$(ROOT_TMP_DIR)/%=%)

fullyclean: clean
	@$(RM) -r $(DIRS)

#Создание архива с дистрибутивом
tar-dist: dist
	@echo Create $(basename $(DIST_DIR)).tar...
	@tar -cf $(basename $(DIST_DIR)).tar $(DIST_DIR)
