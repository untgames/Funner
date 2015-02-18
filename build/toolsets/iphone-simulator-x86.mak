###################################################################################################
#—борка под iPhone Simulator 4.3 g++
###################################################################################################

PROFILES += x86 iosx86

COMMON_CFLAGS     += -arch i386
COMMON_LINK_FLAGS += -arch i386
FAT_LIB_ARCH_TYPE := i386

include $(TOOLSETS_DIR)/common/iphone-simulator.mak
