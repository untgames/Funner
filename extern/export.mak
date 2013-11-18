COMPONENT_DIRS := zzip pcre tiff devil libpsd gles_win32 gles_wince bullet ogg vorbis \
                  vorbisfile theora lua luajit firebreath mongoose beagleboard openalsdk shiny meego \
                  wcecompat libffi pthreads_wince qualcomm_texture_converter lib64 dxsdk angle \
                  libiconv win32_load_dll win8_compat flac

ifeq (,$(filter beagleboard,$(PROFILES))$(filter tabletos,$(PROFILES)))
  COMPONENT_DIRS += curl
endif

ifeq (,$(filter tabletos,$(PROFILES)))
  COMPONENT_DIRS += zlib jpeg libpng freetype openssl
else
  COMPONENT_DIRS += tabletos
endif

ifneq (,$(filter bada,$(PROFILES)))
  COMPONENT_DIRS += bada
endif

ifneq (,$(filter android-arm,$(PROFILES)))
  COMPONENT_DIRS += android_ndk_profiler
endif

ifneq (,$(filter android,$(PROFILES)))
  COMPONENT_DIRS += gcm google_play_billing facebook_android_sdk android_support_v4 flurry
endif

ifneq (,$(filter iphone,$(PROFILES)))
  COMPONENT_DIRS += flurry
endif

ifeq (,$(filter android,$(PROFILES)))
ifneq (,$(filter x86,$(PROFILES)))
  COMPONENT_DIRS += geekinfo
endif
endif