###################################################################################################
#Constants
###################################################################################################
PROFILES                  += arm android-arm
ANDROID_TOOLCHAIN_VERSION := 4.9
BUSYBOX_FILE              := $(BUILD_DIR)platforms/android/arm/busybox

include $(TOOLSETS_DIR)/common/android.mak
