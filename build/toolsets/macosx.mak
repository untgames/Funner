###################################################################################################
#Сборка под MacOS X g++
###################################################################################################

ifndef MACOSX_VERSION
  $(error "Environment variable 'MACOSX_VERSION' not found (use macosx-version.g++ toolset)")
endif

###################################################################################################
#Константы
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += macosx unistd carbon cocoa cocoa_desktop has_windows haswchar pthread_static_library x86 clang g++x86
DLL_PATH := DYLD_LIBRARY_PATH

MACOSX_DEPLOYMENT_TARGET := $(MACOSX_VERSION)

export MACOSX_DEPLOYMENT_TARGET

MACOSX_SDK_PATH := /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk

COMMON_CFLAGS               += -msse3 -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=$(MACOSX_VERSION) -arch i386 -fvisibility=hidden -Wno-unused-function
COMMON_LINK_FLAGS           += -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=$(MACOSX_VERSION) -arch i386
MAP_FILE_LINK_OPTION_PREFIX := -Wl,-map,
COMPILER_GCC                := /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang
LINKER_GCC                  := /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++

include $(TOOLSETS_DIR)/g++.mak

PROFILES := $(filter-out g++,$(PROFILES))

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
