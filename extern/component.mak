###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#Цель - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := zlib zzip pcre jpeg tiff libpng devil ogg vorbis vorbisfile lua freetype libpsd bullet theora mongoose

ifeq (,$(filter iphone,$(PROFILES))$(filter beagleboard,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += curl
endif

ifneq (,$(filter win32,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += luajit
endif

ifneq (,$(filter linux,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif
