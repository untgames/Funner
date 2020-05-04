###################################################################################################
#Constants
###################################################################################################
ANDROID_TOOLCHAIN    := aarch64-linux-android
ANDROID_TOOLS_PREFIX := aarch64-linux-android
ANDROID_CLANG_TARGET := aarch64-none-linux-android
ANDROID_ABI          := arm64-v8a
ANDROID_ARCH         := arm64
ANDROID_NDK_PLATFORM := 21
PROFILES             += android-arm64

include $(TOOLSETS_DIR)/common/android-arm.mak
