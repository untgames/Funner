###################################################################################################
#Build for iPhone Simulator x86-64
###################################################################################################

PROFILES += x86-64 g++x86-64 iosx86-64

COMMON_CFLAGS     += -arch x86_64
COMMON_LINK_FLAGS += -arch x86_64
FAT_LIB_ARCH_TYPE := x86_64

include $(TOOLSETS_DIR)/common/iphone-simulator.mak
