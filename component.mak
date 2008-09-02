###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS FUNNER_COMPONENTS FUNNER_SDK

#Цель №1 - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := extern/zlib extern/zzip extern/pcre extern/devil extern/ogg extern/vorbis extern/vorbisfile \
                                 extern/lua extern/freetype
FUNNER_EXTERN_LIBS.win32      := extern/glew

#Цель №2 - пакет
FUNNER_COMPONENTS.TYPE       := package
FUNNER_COMPONENTS.COMPONENTS := components/xtl components/common components/system components/media \
                                components/bound_volumes components/scene_graph components/script components/sound \
                                components/render components/input components/client components/tools components/demos

#Цель №3 - сборка SDK
FUNNER_SDK.TYPE        := sdk
FUNNER_SDK.EXPORT_DIRS := components/xtl/include components/common/include components/bound_volumes/include \
                          components/scene_graph/include components/script/core/include
