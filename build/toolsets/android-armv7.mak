###################################################################################################
#Constants
###################################################################################################
COMMON_CFLAGS        += -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mfpu=neon -mthumb
COMMON_CFLAGS        += -D__ARM_ARCH_7__ -D__ARM_ARCH_7A__
COMMON_LINK_FLAGS    += -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mfpu=neon -mthumb
ANDROID_CLANG_TARGET := armv7-none-linux-androideabi
ANDROID_ABI          := armeabi-v7a

include $(TOOLSETS_DIR)/common/android-arm.mak
