###################################################################################################
#—борка под iPhone Simulator 4.3 g++
###################################################################################################

PROFILES += x86 clang

COMMON_MMFLAGS    += -fobjc-abi-version=2 -fobjc-legacy-dispatch
COMMON_CFLAGS     += -mmacosx-version-min=10.6 -D__IPHONE_OS_VERSION_MIN_REQUIRED=40300 -arch i386 -Wno-unused-function
COMMON_LINK_FLAGS += -arch i386 -Xlinker -objc_abi_version -Xlinker 2 -mmacosx-version-min=10.6

COMPILER_GCC := /Applications/Xcode5-DP5.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang
LINKER_GCC   := /Applications/Xcode5-DP5.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++

IPHONE_SDK_PATH  := /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator6.1.sdk

include $(TOOLSETS_DIR)/iphone.mak
