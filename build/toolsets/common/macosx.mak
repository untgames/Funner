###################################################################################################
#Build for MacOS X g++
###################################################################################################

ifndef MACOSX_VERSION
  $(error "Environment variable 'MACOSX_VERSION' not found (use macosx-version.g++ toolset)")
endif

###################################################################################################
#Constants
###################################################################################################
EXE_SUFFIX     :=
DLL_PREFIX     := lib
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += macosx unistd carbon cocoa cocoa_desktop has_windows haswchar pthread_static_library clang
DLL_PATH := DYLD_LIBRARY_PATH

MACOSX_DEPLOYMENT_TARGET := $(MACOSX_VERSION)

export MACOSX_DEPLOYMENT_TARGET

ADDITIONAL_PATHS=$$(pwd)/$(BUILD_DIR)tools/macos

XCODE_PATH ?= /Applications/Xcode.app

MACOSX_SDK_PATH := $(XCODE_PATH)/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk

COMMON_CFLAGS               += -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=$(MACOSX_VERSION) -fvisibility=hidden -Wno-unused-function -stdlib=libc++
COMMON_LINK_FLAGS           += -isysroot $(MACOSX_SDK_PATH) -mmacosx-version-min=$(MACOSX_VERSION) -dead_strip -stdlib=libc++
MAP_FILE_LINK_OPTION_PREFIX := -Wl,-map,
COMPILER_GCC                := $(XCODE_PATH)/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang
LINKER_GCC                  := $(XCODE_PATH)/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++

include $(TOOLSETS_DIR)/g++.mak

PROFILES := $(filter-out g++,$(PROFILES))

SOURCE_FILES_SUFFIXES += mm         #Source files extension

###################################################################################################
#Shared-library linking (output file name)
###################################################################################################
define tools.link.dll
-dynamiclib -Wl,-undefined -Wl,error -install_name @loader_path/$(notdir $(basename $1))$(DLL_SUFFIX)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR) && mv $(DIST_LIB_DIR)/$(notdir $1) $(DIST_LIB_DIR)/$(notdir $1))
endef
