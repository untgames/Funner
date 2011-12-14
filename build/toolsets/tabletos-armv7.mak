TABLETOS_ARCHITECTURE    := armle-v7
TABLETOS_NDK_TOOL_PREFIX := arm-unknown-nto-qnx6.5.0eabi-
PROFILES                 += arm
COMMON_CFLAGS            += -DARM

include $(TOOLSETS_DIR)/common/tabletos.mak
