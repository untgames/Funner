PROFILES += x86 g++x86 macx86

COMMON_CFLAGS     += -msse3 -arch i386
COMMON_LINK_FLAGS += -arch i386

include $(TOOLSETS_DIR)/common/macosx.mak
