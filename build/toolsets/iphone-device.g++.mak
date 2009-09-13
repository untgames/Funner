###################################################################################################
#—борка под iPhone Device 2.0 g++
###################################################################################################

COMMON_CFLAGS     += -arch armv6 -miphoneos-version-min=2.0 #-fobjc-call-cxx-cdtors  #флаг fobjc-call-cxx-cdtors необходим дл€ использовани€ с++ классов в objective-c классах в gcc версий до 4,2
COMMON_LINK_FLAGS += -arch armv6 -miphoneos-version-min=2.0

COMPILER_GCC := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC   := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++-4.2

IPHONEOS_DEPLOYMENT_TARGET := 2.0
MACOSX_DEPLOYMENT_TARGET   := 10.5

export IPHONEOS_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET

IPHONEOS_VERSION := 20000
IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS2.0.sdk

PROFILES += iphone-device

include $(TOOLSETS_DIR)/iphone.g++.mak
