PROFILES += x86-64 g++x86-64 macx86-64

COMMON_CFLAGS     += -msse3 -arch x86_64
COMMON_LINK_FLAGS += -arch x86_64

include $(TOOLSETS_DIR)/common/macosx.mak
