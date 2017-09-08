###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.CLIENT.SOURCES
TARGETS += RENDER.SCENE_RENDER.CLIENT.TESTS

#Цель - scene render server sources
RENDER.SCENE_RENDER.CLIENT.SOURCES.TYPE         := static-lib
RENDER.SCENE_RENDER.CLIENT.SOURCES.NAME         := funner.render.scene_render.client
RENDER.SCENE_RENDER.CLIENT.SOURCES.INCLUDE_DIRS := sources
RENDER.SCENE_RENDER.CLIENT.SOURCES.SOURCE_DIRS  := sources/core sources/rms sources/scene
RENDER.SCENE_RENDER.CLIENT.SOURCES.IMPORTS      := compile.render.scene_render.client compile.render.scene_render.interchange compile.common compile.system \
                                                   compile.media.image compile.media.font

#Цель - scene render server tests
RENDER.SCENE_RENDER.CLIENT.TESTS.TYPE         := test-suite
RENDER.SCENE_RENDER.CLIENT.TESTS.SOURCE_DIRS  := tests
RENDER.SCENE_RENDER.CLIENT.TESTS.INCLUDE_DIRS := sources
RENDER.SCENE_RENDER.CLIENT.TESTS.IMPORTS      := compile.render.scene_render.client link.render.scene_render.client compile.render.scene_render.interchange compile.common \
                                                 compile.media.image compile.system
