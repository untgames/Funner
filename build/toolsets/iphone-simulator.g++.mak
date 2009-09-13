###################################################################################################
#—борка под iPhone Simulator 3.0 g++
###################################################################################################

COMPILER_GCC := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC   := /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/g++-4.2

IPHONEOS_VERSION := 30000
IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator3.0.sdk

include $(TOOLSETS_DIR)/iphone.g++.mak
