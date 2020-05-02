PROFILES          += win32 x86-64 vcx86-64 x86-64_win32
COMMON_LINK_FLAGS += /MACHINE:X64
ML_NAME           := ml64

include $(TOOLSETS_DIR)/common/msvc.mak

ifneq (,$(UCRT_PATH))
  LIB := $(UCRT_PATH)/Lib/$(UCRT_VERSION)/um/x64/;$(UCRT_PATH)/Lib/$(UCRT_VERSION)/ucrt/x64/;$(LIB)
endif

ifeq (,$(filter vc14_1,$(PROFILES)))
#For VS 2013/2015
MSVC_BIN_PATH := $(MSVC_BIN_PATH)/amd64
LIB           := $(MSVC_PATH)/lib/amd64;$(MSVC_PATH)/atlmfc/lib/amd64;$(PLATFORM_SDK_PATH)/lib/x64;$(LIB)
else
#For VS 2017/2019/...
MSVC_BIN_PATH := $(MSVC_PATH)bin/Hostx86/x64
LIB           := $(MSVC_PATH)/lib/x64;$(MSVC_PATH)/atlmfc/lib/x64;$(PLATFORM_SDK_PATH)/lib/x64;$(LIB)
endif

export LIB

ifneq (,$(filter vc14,$(PROFILES)))
PROFILES += vc14-x86-64
endif
