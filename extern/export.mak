COMPONENT_DIRS := zlib zzip jpeg tiff libpng devil libpsd gles_win32 bullet ogg vorbis vorbisfile theora lua luajit firebreath mongoose beagleboard openalsdk

ifeq (,$(filter iphone,$(PROFILES))$(filter android,$(PROFILES))$(filter beagleboard,$(PROFILES)))
  COMPONENT_DIRS += curl
endif
