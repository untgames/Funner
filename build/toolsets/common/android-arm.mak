###################################################################################################
#Constants
###################################################################################################
PROFILES                  += arm android-arm
ANDROID_TOOLCHAIN         := arm-linux-androideabi
ANDROID_TOOLS_PREFIX      := arm-linux-androideabi
ANDROID_TOOLCHAIN_VERSION := 4.9
ANDROID_ARCH              := arm
BUSYBOX_FILE              := $(BUILD_DIR)platforms/android/arm/busybox

include $(TOOLSETS_DIR)/common/android.mak
