###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.CORE.SOURCES
#TARGETS += RENDER.SCENE_RENDER.CORE.TESTS
#TARGETS += RENDER.SCENE_RENDER.RENDER3D.SOURCES
#TARGETS += RENDER.SCENE_RENDER.RENDER3D.TESTS

#Цель - Scene render sources
RENDER.SCENE_RENDER.CORE.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER.CORE.SOURCES.NAME        := funner.render.scene_render
RENDER.SCENE_RENDER.CORE.SOURCES.SOURCE_DIRS := sources/core
RENDER.SCENE_RENDER.CORE.SOURCES.IMPORTS     := compile.render.scene_render compile.common compile.scene_graph.core \
                                                compile.render.manager
                                             
#Цель - Scene render tests
RENDER.SCENE_RENDER.CORE.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER.CORE.TESTS.SOURCE_DIRS := tests/core
RENDER.SCENE_RENDER.CORE.TESTS.IMPORTS     := compile.render.scene_render link.render.scene_render \
                                              compile.scene_graph.core compile.common compile.system link.system \
                                              link.render.low_level.opengl_driver run.render.low_level.opengl_driver                                              

#Цель - Scene render3d sources
RENDER.SCENE_RENDER.RENDER3D.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER.RENDER3D.SOURCES.NAME        := funner.render.scene_render.render3d
RENDER.SCENE_RENDER.RENDER3D.SOURCES.SOURCE_DIRS := sources/scene_render3d sources/scene_render3d/techniques
RENDER.SCENE_RENDER.RENDER3D.SOURCES.IMPORTS     := compile.render.scene_render compile.common compile.scene_graph.core compile.render.manager

#Цель - Scene render tests
RENDER.SCENE_RENDER.RENDER3D.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER.RENDER3D.TESTS.SOURCE_DIRS := tests/scene_render3d
RENDER.SCENE_RENDER.RENDER3D.TESTS.IMPORTS     := compile.render.scene_render link.render.scene_render \
                                                  compile.scene_graph.core compile.common compile.system link.system \
                                                  link.render.low_level.opengl_driver run.render.low_level.opengl_driver \
                                                  link.render.scene_render.render3d \
                                                  link.media.image.dds link.media.rfx link.media.rfx.default_serializers \
                                                  link.media.geometry.binmesh link.media.geometry.xmesh
