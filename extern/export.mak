COMPONENT_DIRS := zlib zzip jpeg tiff libpng devil libpsd gles_win32

ifeq (,$(filter iphone,$(PROFILES)))
  COMPONENT_DIRS += curl
endif
