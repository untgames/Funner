###################################################################################################
#—борка под iPhone Device 4.0 g++ armv6 (iPhone 1G/3G, iPod Touch 1G/2G)
###################################################################################################

COMMON_CFLAGS     += -DARM6 -arch armv6
COMMON_LINK_FLAGS += -arch armv6

include $(TOOLSETS_DIR)/iphone-device.mak
