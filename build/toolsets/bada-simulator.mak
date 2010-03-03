PROFILES              += bada_simulator
BADA_TOOLCHAIN        := Win32
BADA_TOOLCHAIN_PREFIX := i686-mingw32-
COMMON_CFLAGS         += -DBADA_SIMULATOR
DLL_LIB_SUFFIX        := .a
DLL_PREFIX            :=

include $(TOOLSETS_DIR)/bada.mak
