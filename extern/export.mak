COMPONENT_DIRS := zlib zzip jpeg tiff libpng devil libpsd gles_win32 bullet ogg vorbis vorbisfile theora

ifeq (,$(filter iphone,$(PROFILES))$(filter android,$(PROFILES)))
  COMPONENT_DIRS += curl
endif
