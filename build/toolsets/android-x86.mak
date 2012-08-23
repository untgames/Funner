###################################################################################################
#Константы
###################################################################################################
PROFILES                  += x86 g++x86
ANDROID_TOOLCHAIN         := x86
ANDROID_TOOLS_PREFIX      := i686-linux-android
ANDROID_TOOLCHAIN_VERSION := 4.4.3
ANDROID_ARCH              := x86
ANDROID_ABI               := x86
BUSYBOX_PATH              := //system/xbin/busybox
COMMON_CFLAGS             += -m32 -DX86

include $(TOOLSETS_DIR)/common/android.mak
