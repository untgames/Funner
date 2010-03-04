PROFILES              += bada_simulator
BADA_TOOLCHAIN        := Win32
BADA_TOOLCHAIN_PREFIX := i686-mingw32-
COMMON_CFLAGS         += -DBADA_SIMULATOR
COMMON_LINK_FLAGS     += -L$(BADA_SDK_HOME)/Lib/WinSgpp
DLL_LIB_SUFFIX        := .a
DLL_PREFIX            :=
EXE_SUFFIX            := .exe

include $(TOOLSETS_DIR)/bada.mak
