###################################################################################################
#Сборка под iPhone Simulator 3.0 g++
###################################################################################################

###################################################################################################
#Константы
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += iphone_simulator macosx unistd has_windows haswchar
DLL_PATH := DYLD_LIBRARY_PATH

COMPILER_GCC := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC   := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/g++-4.2

IPHONE_SDK_PATH := /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator3.0.sdk

COMMON_CFLAGS     += -isysroot $(IPHONE_SDK_PATH)
COMMON_LINK_FLAGS += -isysroot $(IPHONE_SDK_PATH)

include $(TOOLSETS_DIR)/g++.mak

SOURCE_FILES_SUFFIXES += mm         #Расширения исходных файлов

###################################################################################################
#Линковка shared-library (имя выходного файла)
###################################################################################################
define tools.link.dll
-dynamiclib -Wl,-undefined -Wl,error -install_name $(notdir $(basename $1))$(DLL_SUFFIX)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR) && mv $(DIST_LIB_DIR)/$(notdir $1) $(DIST_LIB_DIR)/$(LIB_PREFIX)$(notdir $1))
endef
