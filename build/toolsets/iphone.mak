###################################################################################################
#Сборка под iPhone
###################################################################################################

ifndef IPHONE_SDK_PATH
  $(error "Environment variable 'IPHONE_SDK_PATH' not found (use iphone-version.g++ toolset)")
endif

###################################################################################################
#Константы
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += iphone cocoa unistd has_windows haswchar gles no_dll haswchar
DLL_PATH := DYLD_LIBRARY_PATH

COMMON_CFLAGS     += -Os -isysroot $(IPHONE_SDK_PATH) -DIPHONE
COMMON_LINK_FLAGS += -isysroot $(IPHONE_SDK_PATH)

include $(TOOLSETS_DIR)/g++.mak

SOURCE_FILES_SUFFIXES += mm         #Расширения исходных файлов

###################################################################################################
#Линковка shared-library (имя выходного файла)
###################################################################################################
define tools.link.dll
-dynamiclib -Wl,-undefined -Wl,error -install_name @loader_path/$(notdir $(basename $1))$(DLL_SUFFIX)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR) && mv $(DIST_LIB_DIR)/$(notdir $1) $(DIST_LIB_DIR)/$(LIB_PREFIX)$(notdir $1))
endef

###################################################################################################
#Сборка единой статической библиотеки
###################################################################################################
VALID_TARGET_TYPES += fat-static-lib lipo-lib
PACKAGE_COMMANDS   += lipo

#Получение пути к библиотеке
define get_full_library_path
$(strip $(firstword $(wildcard $(patsubst %,%/$(notdir $(strip $1)),$2))))
endef

#Поиск библиотеки (имя библиотеки, пути поиска)
define find_library
$(if $(call get_full_library_path,$1,$2),$(call get_full_library_path,$1,$2),$1)
endef

#Обработка цели объединения библиотек(имя цели)
define process_target.fat-static-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty fat static library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif  
  
  $1.LIB_FILE                      := $(DIST_LIB_DIR)/$(LIB_PREFIX)$$($1.NAME)$(LIB_SUFFIX)
  $1.LIB_DIRS                      := $$(strip $$($1.LIB_DIRS))
  $1.LIB_DIRS                      := $$(call specialize_paths,$$($1.LIB_DIRS)) $(DIST_LIB_DIR)
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.LIB_FILE)
  DIST_DIRS                        := $$(DIST_DIRS) $$(DIST_LIB_DIR)
  $1.SOURCE_INSTALLATION_LIB_FILES := $$($1.LIB_FILE)
  $1.LIBS                          := $$($1.LIBS:%=$(LIB_PREFIX)%$(LIB_SUFFIX))
  $1.LIBS                          := $$(foreach lib,$$($1.LIBS),$$(call find_library,$$(lib),$$($1.LIB_DIRS)))
  
  build: $$($1.LIB_FILE)

  $$($1.LIB_FILE): $$($1.LIBS)
		@echo Create fat static library $$(notdir $$($1.LIB_FILE))..
		libtool -c -o $$@ $$($1.LIBS)
endef

#Обработка цели объединения библиотек, собранных для разных архитектур (имя цели)
define process_target.lipo-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty lipo library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.LIB_FILE                      := $(DIST_LIB_DIR)/../$(LIB_PREFIX)$$($1.NAME)$(LIB_SUFFIX)
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.LIB_FILE)
  DIST_DIRS                        := $$(DIST_DIRS) $$(DIST_LIB_DIR)
  $1.SOURCE_INSTALLATION_LIB_FILES := $$($1.LIB_FILE)
  $1.LIBS                          := $$(foreach lib,$$($1.LIBS),$$(foreach profile,$$($1.PROFILES),$(DIST_LIB_DIR)/../../$$(profile)/lib/$(LIB_PREFIX)$$(lib)$(LIB_SUFFIX)))

  lipo: $$($1.LIB_FILE)

  $$($1.LIB_FILE): $$($1.LIBS)
		@echo Create lipo library $$(notdir $$@)..	
		@lipo $$($1.LIBS) -create -output $$@
endef

.PHONY: lipo
