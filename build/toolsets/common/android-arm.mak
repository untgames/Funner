###################################################################################################
#Константы
###################################################################################################
PROFILES                  += arm android-arm
ANDROID_TOOLCHAIN         := arm-linux-androideabi
ANDROID_TOOLS_PREFIX      := arm-linux-androideabi
ANDROID_TOOLCHAIN_VERSION := 4.8
ANDROID_ARCH              := arm
ANDROID_ABI               := armeabi
BUSYBOX_FILE              := $(BUILD_DIR)platforms/android/arm/busybox
COMMON_CFLAGS             += -mthumb
COMMON_CFLAGS             += -DARM

include $(TOOLSETS_DIR)/common/android.mak
