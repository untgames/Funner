COMPONENT_DIRS := zlib zzip jpeg tiff libpng devil libpsd gles_win32 gles_wince bullet ogg vorbis \
                  vorbisfile theora lua luajit firebreath mongoose beagleboard openalsdk shiny meego \
                  wcecompat libffi libiconv pthreads_wince openssl qualcomm_texture_converter lib64 dxsdk angle

ifeq (,$(filter beagleboard,$(PROFILES)))
  COMPONENT_DIRS += curl
endif

ifneq (,$(filter bada,$(PROFILES)))
  COMPONENT_DIRS += bada
endif

ifneq (,$(filter android-arm,$(PROFILES)))
  COMPONENT_DIRS += android_ndk_profiler
endif

ifeq (,$(filter android,$(PROFILES)))
ifneq (,$(filter x86,$(PROFILES)))
  COMPONENT_DIRS += geekinfo
endif
endif