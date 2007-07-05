###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS FUNNER_COMPONENTS

#Цель №1 - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := extern/zlib extern/zzip extern/pcre extern/devil extern/ogg extern/vorbis extern/vorbisfile

#Цель №2 - пакет
FUNNER_COMPONENTS.TYPE       := package
FUNNER_COMPONENTS.COMPONENTS := components/commonlib components/medialib components/xtl
