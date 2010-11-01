###################################################################################################
#—борка под iPhone Device 3.0 g++ armv6 (iPhone, iPhone3G, iPod Touch, iPod Touch 2G)
###################################################################################################

COMMON_CFLAGS     += -arch armv6
COMMON_LINK_FLAGS += -arch armv6

include $(TOOLSETS_DIR)/iphone-device.mak
