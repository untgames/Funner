PROFILES          += win32 x86-64 vcx86-64 x86-64_win32
COMMON_LINK_FLAGS += /MACHINE:X64

include $(TOOLSETS_DIR)/common/msvc.mak

MSVC_BIN_PATH := $(MSVC_BIN_PATH)/amd64
LIB           := $(MSVC_PATH)/lib/amd64;$(MSVC_PATH)/atlmfc/lib/amd64;$(PLATFORM_SDK_PATH)/lib/x64;$(LIB)

export LIB
