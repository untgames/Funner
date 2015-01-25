PROFILES                += win32 x86 vcx86 x86_win32
COMMON_LINK_FLAGS       += /MACHINE:X86

include $(TOOLSETS_DIR)/common/msvc.mak

LIB := $(MSVC_PATH)/lib;$(MSVC_PATH)/atlmfc/lib;$(PLATFORM_SDK_PATH)/lib;$(LIB)

export LIB
