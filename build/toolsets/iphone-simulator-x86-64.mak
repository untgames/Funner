###################################################################################################
#—борка под iPhone Simulator 4.3 g++
###################################################################################################

PROFILES += x86-64

COMMON_CFLAGS     += -arch x86_64
COMMON_LINK_FLAGS += -arch x86_64
FAT_LIB_ARCH_TYPE := x86_64

include $(TOOLSETS_DIR)/common/iphone-simulator.mak
