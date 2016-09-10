###################################################################################################
#Build for iPhone Device armv7s (iPhone 5/5C, iPad 4)
###################################################################################################

PROFILES += iosarm32

COMMON_CFLAGS     += -arch armv7s
COMMON_LINK_FLAGS += -arch armv7s
FAT_LIB_ARCH_TYPE := armv7s

include $(TOOLSETS_DIR)/common/iphone-device.mak
