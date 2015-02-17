###################################################################################################
#—борка под iPhone Simulator 4.3 g++
###################################################################################################

PROFILES += clang

COMMON_MMFLAGS    += -fobjc-abi-version=2 -fobjc-legacy-dispatch
COMMON_CFLAGS     += -mmacosx-version-min=10.6 -D__IPHONE_OS_VERSION_MIN_REQUIRED=40300 -Wno-unused-function
COMMON_LINK_FLAGS += -Xlinker -objc_abi_version -Xlinker 2 -mios-simulator-version-min=4.3

COMPILER_GCC := /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang
LINKER_GCC   := /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++

IPHONE_SDK_PATH  := /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk

include $(TOOLSETS_DIR)/common/iphone.mak
