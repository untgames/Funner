PROFILES          += win32 x86-64 vcx86-64 x86-64_win32
COMMON_LINK_FLAGS += /MACHINE:X64
ML_NAME           := ml64

include $(TOOLSETS_DIR)/common/msvc.mak

ifneq (,$(VS140COMNTOOLS))
  LIB := $(UCRT_PATH)/Lib/$(UCRT_VERSION)/ucrt/x64/;$(LIB)
endif

MSVC_BIN_PATH := $(MSVC_BIN_PATH)/amd64
LIB           := $(MSVC_PATH)/lib/amd64;$(MSVC_PATH)/atlmfc/lib/amd64;$(PLATFORM_SDK_PATH)/lib/x64;$(LIB)

export LIB
