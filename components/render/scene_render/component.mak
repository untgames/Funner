###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.CORE.SOURCES

#Цель - Scene render sources
RENDER.SCENE_RENDER.CORE.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER.CORE.SOURCES.NAME        := funner.render.scene_render
RENDER.SCENE_RENDER.CORE.SOURCES.SOURCE_DIRS := sources/core
RENDER.SCENE_RENDER.CORE.SOURCES.IMPORTS     := compile.render.scene_render compile.common compile.scene_graph.core \
                                                compile.render.manager
