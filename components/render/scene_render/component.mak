###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.CORE.SOURCES
TARGETS += RENDER.SCENE_RENDER.CORE.TESTS

#Цель №1 - Scene render sources
RENDER.SCENE_RENDER.CORE.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER.CORE.SOURCES.NAME        := funner.render.scene_render
RENDER.SCENE_RENDER.CORE.SOURCES.SOURCE_DIRS := sources/core
RENDER.SCENE_RENDER.CORE.SOURCES.IMPORTS     := compile.render.scene_render compile.common compile.scene_graph.core \
                                                compile.render.manager
                                             
#Цель №3 - Scene render tests
RENDER.SCENE_RENDER.CORE.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER.CORE.TESTS.SOURCE_DIRS := tests/core
RENDER.SCENE_RENDER.CORE.TESTS.IMPORTS     := compile.render.scene_render link.render.scene_render \
                                              compile.scene_graph.core compile.common compile.system link.system \
                                              link.render.low_level.opengl_driver run.render.low_level.opengl_driver                                              
