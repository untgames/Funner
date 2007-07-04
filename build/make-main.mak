###################################################################################################
#Константы сборки
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := component.mak #Базовое имя файла конфигурации компонента
TMP_DIR_SHORT_NAME                      := tmp           #Базовое имя каталога с временными файлами
SOURCE_FILES_SUFFIXES                   := c cpp         #Расширения исходных файлов
BUILD_DIR_SHORT_NAME                    := build         #Базовое имя каталога со скриптами сборки
TMP_DIR_SHORT_NAME                      := tmp           #Базовое имя каталога с временными файлами сборки
OUT_DIR_SHORT_NAME                      := out           #Базовое имя каталога с результатами сборки
PCH_SHORT_NAME                          := pch.h         #Базовое имя PCH файла
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := batch-flag    #Базовое имя файла-флага пакетной компиляции
VALID_TARGET_TYPES                      := static-lib dynamic-lib application test-suite package #Допустимые типы целей

###################################################################################################
#Производные пути и переменные
###################################################################################################
COMPONENT_CONFIGURATION_FILE_SHORT_NAME := $(strip $(COMPONENT_CONFIGURATION_FILE_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
SOURCE_FILES_SUFFIXES                   := $(strip $(SOURCE_FILES_SUFFIXES))
BUILD_DIR_SHORT_NAME                    := $(strip $(BUILD_DIR_SHORT_NAME))
TMP_DIR_SHORT_NAME                      := $(strip $(TMP_DIR_SHORT_NAME))
OUT_DIR_SHORT_NAME                      := $(strip $(OUT_DIR_SHORT_NAME))
OUT_DIR                                 := $(ROOT)/$(OUT_DIR_SHORT_NAME)
OUT_LIB_DIR                             := $(OUT_DIR)/lib
OUT_BIN_DIR                             := $(OUT_DIR)/bin
BUILD_DIR                               := $(ROOT)/$(BUILD_DIR_SHORT_NAME)
TMP_DIRS                                := $(ROOT)/$(TMP_DIR_SHORT_NAME)
PCH_SHORT_NAME                          := $(strip $(PCH_SHORT_NAME))
BATCH_COMPILE_FLAG_FILE_SHORT_NAME      := $(strip $(BATCH_COMPILE_FLAG_FILE_SHORT_NAME))
DIRS                                     = $(TMP_DIRS) $(OUT_DIR) $(OUT_LIB_DIR) $(OUT_BIN_DIR)

###################################################################################################
#Если не указан список обрабатываемых файлов - обрабатываем все доступные файлы (для тестирования)
###################################################################################################
files ?= *

###################################################################################################
#Цель сборки "по умолчанию"
###################################################################################################
default: build

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
define specialize_path
$(wildcard $(foreach dir,$($1),$(COMPONENT_DIR)$(dir)))
endef

###################################################################################################
#Обработка цели компонента
###################################################################################################

#Создание зависимости объектного файла от флаг-файла пакетной компиляции (имя исходного файла, временный каталог, флаг-файл)
define create_object_file_dependency
$2/$$(notdir $$(basename $1)).obj: $1 $3
	@
endef

#Правила пакетной компиляции (имя цели, имя модуля)
define batch-compile
  $2.FLAG_FILE  := $$($2.TMP_DIR)/$$(BATCH_COMPILE_FLAG_FILE_SHORT_NAME)
  $1.FLAG_FILES := $$($1.FLAG_FILES) $$($2.FLAG_FILE)
  
  $$($2.FLAG_FILE): $$($2.SOURCE_FILES)
		@cl $$(sort $$(filter-out force,$$?) $$($2.NEW_SOURCE_FILES)) /nologo /c /Fo"$$($2.TMP_DIR)\\" $$($1.INCLUDE_DIRS:%=/I"%") $$($1.BUILD_CFLAGS) $$($1.BUILD_DEFINES:%=/D"%")
		@echo batch-flag-file > $$@
		
  ifneq (,$$($2.NEW_SOURCE_FILES))
    $$($2.FLAG_FILE): force
  endif
  
  $$(foreach source,$$($2.SOURCE_FILES),$$(eval $$(call create_object_file_dependency,$$(source),$$($2.TMP_DIR),$$(FLAG_FILE))))
endef

#Обработка каталога с исходными файлами (имя цели, путь к каталогу с исходными файлами, список применяемых макросов)
define process_source_dir
  MODULE_PATH                      := $$(subst ./,,$$(subst ../,,$2))
  MODULE_NAME                      := $1.$$(subst /,-,$$(MODULE_PATH))
  $$(MODULE_NAME).SOURCE_DIR       := $2
  $$(MODULE_NAME).TMP_DIR          := $(ROOT)/$(TMP_DIR_SHORT_NAME)/$1/$$(MODULE_PATH)
  $$(MODULE_NAME).SOURCE_FILES     := $$(wildcard $$(SOURCE_FILES_SUFFIXES:%=$2/*.%))
  $$(MODULE_NAME).OBJECT_FILES     := $$(patsubst %,$$($$(MODULE_NAME).TMP_DIR)/%.obj,$$(notdir $$(basename $$($$(MODULE_NAME).SOURCE_FILES))))
  $$(MODULE_NAME).NEW_SOURCE_FILES := $$(strip $$(foreach file,$$($$(MODULE_NAME).SOURCE_FILES),$$(if $$(wildcard $$(patsubst %,$$($$(MODULE_NAME).TMP_DIR)/%.obj,$$(notdir $$(basename $$(file))))),,$$(file))))
  $$(MODULE_NAME).PCH              := $$(wildcard $1/$$(PCH_SHORT_NAME))
  $1.TMP_DIRS                      := $$($$(MODULE_NAME).TMP_DIR) $$($1.TMP_DIRS)
  $1.OBJECT_FILES                  := $$($1.OBJECT_FILES) $$($$(MODULE_NAME).OBJECT_FILES)

  $$(foreach macros,batch-compile $3,$$(eval $$(call $$(macros),$1,$$(MODULE_NAME))))
endef

#Создание зависимости для копирования внешних файлов (имя внешнего файла, каталоги поиска)
define create_extern_file_dependency
  $1: $$(firstword $$(wildcard $$(patsubst %,%/$$(notdir $1),$2)) $$(notdir $1))
		@cp -fv $$< $$@
endef

#Общее для целей с исходными файлами (имя цели, список макросов применяемых для обработки каталогов с исходными файлами)
define process_target_with_sources
  $1.INCLUDE_DIRS := $$(call specialize_path,$1.INCLUDE_DIRS)
  $1.SOURCE_DIRS  := $$(call specialize_path,$1.SOURCE_DIRS)
  $1.LIB_DIRS     := $$(call specialize_path,$1.LIB_DIRS) $(OUT_LIB_DIR)
  $1.DLL_DIRS     := $$(call specialize_path,$1.DLL_DIRS)
  $1.LIBS         := $$($1.LIBS:%=%.lib)
  $1.TARGET_DLLS  := $$($1.DLLS:%=$(OUT_BIN_DIR)/%.dll)
  
  $$(foreach dir,$$($1.SOURCE_DIRS),$$(eval $$(call process_source_dir,$1,$$(dir),$2)))

  TMP_DIRS := $$($1.TMP_DIRS) $$(TMP_DIRS)

  build: $$($1.TARGET_DLLS)
  
  $$(foreach file,$$($1.TARGET_DLLS),$$(eval $$(call create_extern_file_dependency,$$(file),$$($1.DLL_DIRS))))  
endef

#Обработка цели static-lib (имя цели)
define process_target.static-lib
  $1.LIB_FILE  := $(OUT_LIB_DIR)/$$(strip $$($1.NAME)).lib
  TARGET_FILES := $$(TARGET_FILES) $$($1.LIB_FILE)

  build: $$($1.LIB_FILE)

  $$(eval $$(call process_target_with_sources,$1))  

  $$($1.LIB_FILE): $$($1.FLAG_FILES)
		@lib /nologo /out:$$@ $$($1.OBJECT_FILES)      
endef

#Обработка каталога с исходными файлами тестов (имя цели, имя модуля)
define process_tests_source_dir
  $2.TEST_EXE_FILES    := $$($2.OBJECT_FILES:%.obj=%.exe)
  $2.TEST_RESULT_FILES := $$($2.OBJECT_FILES:%.obj=%.result)

  build: $$($2.TEST_EXE_FILES)
  test: TEST_MODULE.$2
  check: CHECK_MODULE.$2
  .PHONY: TEST_MODULE.$2 CHECK_MODULE.$2  
  
#Правило сборки теста
  $$($2.TMP_DIR)/%.exe: $$($2.TMP_DIR)/%.obj $$($1.FLAG_FILES)
		@echo Linking $(notdir $$@)...
		@link $$(filter %.obj,$$<) $$($1.LIBS)  /nologo /out:"$$@" $$($1.LIB_DIRS:%=/libpath:"%") $$($1.LINK_FLAGS)

#Правило получения файла-результата тестирования
  $$($2.TMP_DIR)/%.result: $$($2.TMP_DIR)/%.exe
		@$$< > $$@

#Правило запуска тестов
  TEST_MODULE.$2: $$($2.TEST_EXE_FILES)
		@for file in $$(wildcard $$(files:%=$$($2.TMP_DIR)/%.exe)); do $$$$file; done		

#Правило проверки результатов тестирования
  CHECK_MODULE.$2: $$($2.TEST_EXE_FILES:%.exe=%.result)
		@for file in $$(notdir $$(wildcard $$(files:%=$$($2.TMP_DIR)/%.result))); do diff --context=1 $$($2.SOURCE_DIR)/$$$$file $$($2.TMP_DIR)/$$$$file; done		
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
		@$$(foreach component,$$($1.COMPONENTS),$(MAKE) -C $$(subst :, ,$$(component)) $2 && ) true
endef

#Обработка цели package (имя цели)
define process_target.package
$$(foreach command,build clean fullyclean test check run,$$(eval $$(call process_package_components,$1,$$(command))))  
endef

#Проверка корректности типа цели (тип цели)
define test_target_type
  ifeq (,$$(findstring $1,$(VALID_TARGET_TYPES)))
    $$(error Wrong target type '$1')
  endif
endef

###################################################################################################
#Правила сборки
###################################################################################################
all: build check
build: create_dirs
rebuild: clean build
test: build
check: build
force:

.PHONY: build rebuild clean fullyclean run test check help create_dirs force

#Обработка целей компонента
$(foreach target,$(TARGETS),$(eval $(call test_target_type,$(strip $($(target).TYPE)))))
$(foreach target,$(TARGETS),$(eval $(call process_target.$(strip $($(target).TYPE)),$(target))))

#Создание каталогов
create_dirs: $(DIRS)

$(DIRS):
	@mkdir -p $@

#Очистка
clean: single_component_clean
fullyclean: single_component_fullyclean
.PHONY: single_component_fullyclean single_component_clean

single_component_clean:
	@$(if $(TMP_DIRS),$(RM) -r $(addsuffix /*,$(TMP_DIRS)))
	
single_component_fullyclean: single_component_clean
	@$(if $(TARGET_FILES),$(RM) $(TARGET_FILES))
