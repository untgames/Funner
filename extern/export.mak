COMPONENT_DIRS := zlib zzip jpeg tiff libpng devil libpsd gles_win32 bullet ogg vorbis vorbisfile theora lua luajit firebreath mongoose beagleboard openalsdk shiny meego wcecompat

ifeq (,$(filter iphone,$(PROFILES))$(filter android,$(PROFILES))$(filter beagleboard,$(PROFILES))$(filter android,$(PROFILES)))
  COMPONENT_DIRS += curl
endif

ifneq (,$(filter bada,$(PROFILES)))
  COMPONENT_DIRS += bada
endif
