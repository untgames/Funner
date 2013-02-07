###################################################################################################
#—борка под iPhone Simulator 3.2 g++
###################################################################################################

PROFILES += x86

COMMON_MMFLAGS    += -fobjc-abi-version=2 -fobjc-legacy-dispatch
COMMON_CFLAGS     += -mmacosx-version-min=10.6 -D__IPHONE_OS_VERSION_MIN_REQUIRED=30200 -arch i386
COMMON_LINK_FLAGS += -arch i386 -Xlinker -objc_abi_version -Xlinker 2 -mmacosx-version-min=10.6

COMPILER_GCC := /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/llvm-gcc-4.2
LINKER_GCC   := /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/llvm-g++-4.2

IPHONE_SDK_PATH  := /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator6.1.sdk

include $(TOOLSETS_DIR)/iphone.mak
