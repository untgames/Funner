###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.SERVER.SOURCES
TARGETS += RENDER.SCENE_RENDER.SERVER.TESTS

#Цель - scene render server sources
RENDER.SCENE_RENDER.SERVER.SOURCES.TYPE         := static-lib
RENDER.SCENE_RENDER.SERVER.SOURCES.NAME         := funner.render.scene_render.server
RENDER.SCENE_RENDER.SERVER.SOURCES.INCLUDE_DIRS := sources
RENDER.SCENE_RENDER.SERVER.SOURCES.SOURCE_DIRS  := sources/communication sources/renderer sources/scene sources/techniques
RENDER.SCENE_RENDER.SERVER.SOURCES.IMPORTS      := compile.render.scene_render.server compile.render.scene_render.interchange compile.common compile.system compile.render.manager \
                                                   compile.math.bound_volumes compile.media.image

#Цель - scene render server tests
RENDER.SCENE_RENDER.SERVER.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER.SERVER.TESTS.SOURCE_DIRS := tests/communication tests/renderer
RENDER.SCENE_RENDER.SERVER.TESTS.IMPORTS     := compile.render.scene_render.server link.render.scene_render.server compile.render.scene_render.client \
                                                link.render.scene_render.client compile.common compile.system link.render.low_level.opengl_driver \
                                                run.render.low_level.opengl_driver link.media.image.dds link.media.rfx.default_serializers \
                                                link.media.geometry.binmesh link.media.geometry.xmesh
