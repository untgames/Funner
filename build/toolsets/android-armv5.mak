###################################################################################################
#Constants
###################################################################################################
COMMON_CFLAGS += -mtune=xscale -mtune=arm6 -march=armv5te -mfloat-abi=softfp -mfpu=vfp
COMMON_CFLAGS += -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__

include $(TOOLSETS_DIR)/common/android-arm.mak
