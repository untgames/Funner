###################################################################################################
#Сборка под MacOS X g++
###################################################################################################

###################################################################################################
#Константы
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += macosx macosx_desktop unistd carbon has_windows haswchar
DLL_PATH := DYLD_LIBRARY_PATH

MACOSX_SDK_PATH := /Developer/SDKs/MacOSX10.4u.sdk

COMMON_CFLAGS     += -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=10.4
COMMON_LINK_FLAGS += -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=10.4

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Линковка shared-library (имя выходного файла)
###################################################################################################
define tools.link.dll
-dynamiclib -Wl,-undefined -Wl,error -install_name $(notdir $(basename $1))$(DLL_SUFFIX)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR) && mv $(DIST_LIB_DIR)/$(notdir $1) $(DIST_LIB_DIR)/$(LIB_PREFIX)$(notdir $1))
endef
