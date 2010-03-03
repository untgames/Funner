PROFILES              += bada_device
BADA_TOOLCHAIN        := ARM
BADA_TOOLCHAIN_PREFIX := arm-samsung-nucleuseabi-
COMMON_CFLAGS         += -DBADA_DEVICE
DLL_LIB_SUFFIX        := .so
DLL_PREFIX            := lib
EXE_SUFFIX            :=

include $(TOOLSETS_DIR)/bada.mak
