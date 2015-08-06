###################################################################################################
#Constants
###################################################################################################
COMMON_CFLAGS     += -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16
COMMON_CFLAGS     += -D__ARM_ARCH_7__ -D__ARM_ARCH_7A__
COMMON_LINK_FLAGS += -Wl,--fix-cortex-a8

include $(TOOLSETS_DIR)/common/android-arm.mak
