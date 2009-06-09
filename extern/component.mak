###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#Цель - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := zlib zzip pcre devil ogg vorbis vorbisfile lua freetype box2d libpsd curl
FUNNER_EXTERN_LIBS.win32.COMPONENTS := pthreads_win32
