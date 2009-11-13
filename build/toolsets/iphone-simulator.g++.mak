###################################################################################################
#—борка под iPhone Simulator 3.0 g++
###################################################################################################

COMMON_CFLAGS += -mmacosx-version-min=10.5 -D__IPHONE_OS_VERSION_MIN_REQUIRED=22000

COMPILER_GCC := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc-4.0
LINKER_GCC   := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/g++-4.0

IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator2.2.sdk

include $(TOOLSETS_DIR)/iphone.g++.mak
