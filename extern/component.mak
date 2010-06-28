###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#Цель - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := zlib zzip pcre jpeg tiff libpng devil ogg vorbis vorbisfile lua freetype libpsd bullet theora

ifeq (,$(filter iphone,$(PROFILES)))
  COMPONENT_DIRS += curl
endif

ifneq (,$(filter x86,$(PROFILES)))
  COMPONENT_DIRS += luajit
endif
