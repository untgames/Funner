###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS FUNNER_COMPONENTS

#Цель №1 - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := extern/zlib extern/zzip extern/pcre extern/devil extern/ogg extern/vorbis extern/vorbisfile \
                                 extern/lua extern/glew

#Цель №2 - пакет
FUNNER_COMPONENTS.TYPE       := package
FUNNER_COMPONENTS.COMPONENTS := components/xtl components/common components/system components/media \
                                components/bound_volumes components/script components/scene_graph components/sound \
                                components/render components/input components/demos

