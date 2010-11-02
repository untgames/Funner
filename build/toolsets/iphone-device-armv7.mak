###################################################################################################
#—борка под iPhone Device 3.0 g++ armv7 (iPhone 3GS, iPod Touch 3G, iPad)
###################################################################################################

COMMON_CFLAGS     += -DARM7 -arch armv7
COMMON_LINK_FLAGS += -arch armv7

include $(TOOLSETS_DIR)/iphone-device.mak
