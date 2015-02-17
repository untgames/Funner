###################################################################################################
#—борка под iPhone Device g++ armv7 (iPhone 3GS/4/4S, iPod Touch 3G/4G/5G, iPad 1/2/3/Mini)
###################################################################################################

COMMON_CFLAGS     += -arch armv7
COMMON_LINK_FLAGS += -arch armv7
FAT_LIB_ARCH_TYPE := armv7

include $(TOOLSETS_DIR)/common/iphone-device.mak
