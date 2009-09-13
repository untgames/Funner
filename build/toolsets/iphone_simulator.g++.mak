###################################################################################################
#Сборка под MacOS X g++
###################################################################################################

###################################################################################################
#Константы
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += iphone_simulator unistd has_windows haswchar
DLL_PATH := DYLD_LIBRARY_PATH

COMPILER_GCC := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc-4.0
LINKER_GCC   := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/g++-4.0

COMMON_CFLAGS     += -isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator2.2.sdk
COMMON_LINK_FLAGS += -isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator2.2.sdk

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
