###################################################################################################
#Build for iPhone Simulator 8.0 g++
###################################################################################################

PROFILES += clang

IPHONEOS_DEPLOYMENT_TARGET ?= 8.0

export IPHONEOS_DEPLOYMENT_TARGET

XCODE_PATH ?= /Applications/Xcode.app

COMMON_MMFLAGS    += -fobjc-abi-version=2 -fobjc-legacy-dispatch
COMMON_CFLAGS     += -mios-simulator-version-min=$(IPHONEOS_DEPLOYMENT_TARGET) -Wno-unused-function
COMMON_LINK_FLAGS += -Xlinker -objc_abi_version -Xlinker 2 -mios-simulator-version-min=$(IPHONEOS_DEPLOYMENT_TARGET)

COMPILER_GCC := $(XCODE_PATH)/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang
LINKER_GCC   := $(XCODE_PATH)/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++

IPHONE_SDK_PATH  := $(XCODE_PATH)/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk

include $(TOOLSETS_DIR)/common/iphone.mak
