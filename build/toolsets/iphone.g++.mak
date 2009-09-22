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

PROFILES += iphone unistd has_windows haswchar gles
DLL_PATH := DYLD_LIBRARY_PATH

COMMON_CFLAGS     += -Os -isysroot $(IPHONE_SDK_PATH)
COMMON_LINK_FLAGS += -isysroot $(IPHONE_SDK_PATH) -mmacosx-version-min=10.5

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
