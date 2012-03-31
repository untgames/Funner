COMPONENT_DIRS := zlib zzip jpeg tiff libpng devil libpsd gles_win32 gles_wince bullet ogg vorbis vorbisfile theora lua luajit firebreath mongoose beagleboard openalsdk shiny meego wcecompat libffi libiconv pthreads_wince openssl

ifeq (,$(filter android,$(PROFILES))$(filter beagleboard,$(PROFILES))$(filter android,$(PROFILES)))
  COMPONENT_DIRS += curl
endif

ifneq (,$(filter bada,$(PROFILES)))
  COMPONENT_DIRS += bada
endif

ifneq (,$(filter android,$(PROFILES)))
  COMPONENT_DIRS += android_ndk_profiler
endif

ifneq (,$(filter x86,$(PROFILES)))
  COMPONENT_DIRS += geekinfo
endif
