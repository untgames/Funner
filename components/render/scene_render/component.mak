###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.UTILS.SERIALIZER_GENERATOR
#TARGETS += RENDER.SCENE_RENDER.CORE.SOURCES

#Цель - Scene render sources
RENDER.SCENE_RENDER.CORE.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER.CORE.SOURCES.NAME        := funner.render.scene_render
RENDER.SCENE_RENDER.CORE.SOURCES.SOURCE_DIRS := sources/core
RENDER.SCENE_RENDER.CORE.SOURCES.IMPORTS     := compile.render.scene_render compile.common compile.scene_graph.core \
                                                compile.render.manager

#Цель - Scene render serializer generator
RENDER.SCENE_RENDER.UTILS.SERIALIZER_GENERATOR.TYPE        := test-suite
RENDER.SCENE_RENDER.UTILS.SERIALIZER_GENERATOR.SOURCE_DIRS := utils/serializer_generator
RENDER.SCENE_RENDER.UTILS.SERIALIZER_GENERATOR.IMPORTS     := compile.common link.common
