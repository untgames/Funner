PROFILES            += win32 x86 vcx86 x86_win32 msvc_debug
COMMON_CFLAGS       += -Z7
COMMON_LINK_FLAGS   += /MACHINE:X86 /DEBUG:FULL
LINK_INCLUDE_PREFIX := _
ML_NAME             := ml

include $(TOOLSETS_DIR)/common/msvc.mak

ifneq (,$(UCRT_PATH))
  LIB := $(UCRT_PATH)/Lib/$(UCRT_VERSION)/um/x86/;$(UCRT_PATH)/Lib/$(UCRT_VERSION)/ucrt/x86/;$(LIB)
endif

LIB := $(MSVC_PATH)/lib;$(MSVC_PATH)/lib/x86;$(MSVC_PATH)/atlmfc/lib;$(PLATFORM_SDK_PATH)/lib;$(LIB)

export LIB
