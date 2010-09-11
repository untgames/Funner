###################################################################################################
#Сборка под iPhone Device 3.0 g++ armv7 (iPhone 3GS, iPod Touch 3G, iPad)
###################################################################################################

IPHONEOS_DEPLOYMENT_TARGET := 3.0

export IPHONEOS_DEPLOYMENT_TARGET

COMMON_CFLAGS     += -arch armv7 -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET) #-gdwarf-2 -fobjc-call-cxx-cdtors  #флаг fobjc-call-cxx-cdtors необходим для использования с++ классов в objective-c классах в gcc версий до 4,2; gdwarf-2 - для профилирования
COMMON_LINK_FLAGS += -arch armv7 -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET)

COMPILER_GCC := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC   := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++-4.2

IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.1.sdk

include $(TOOLSETS_DIR)/iphone.mak
