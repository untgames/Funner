###################################################################################################
#—борка под iPhone Simulator 3.0 g++
###################################################################################################

COMMON_CFLAGS += -mmacosx-version-min=10.5 -D__IPHONE_OS_VERSION_MIN_REQUIRED=30000

COMPILER_GCC := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC   := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/g++-4.2

IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator3.0.sdk

include $(TOOLSETS_DIR)/iphone.g++.mak
