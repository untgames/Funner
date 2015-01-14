###################################################################################################
#—борка под iPhone Device g++ armv64 (iPhone 5S/6/6+, iPad Air/Air2/Mini2/Mini3)
###################################################################################################

COMMON_CFLAGS     += -arch arm64
COMMON_LINK_FLAGS += -arch arm64
FAT_LIB_ARCH_TYPE := arm64

include $(TOOLSETS_DIR)/iphone-device.mak
