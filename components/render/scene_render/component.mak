###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.SCENE_RENDER.CORE.SOURCES RENDER.SCENE_RENDER.CORE.TESTS RENDER.SCENE_RENDER.RENDER2D.SOURCES RENDER.SCENE_RENDER.RENDER2D.TESTS

#Цель №1 - Scene render sources
RENDER.SCENE_RENDER.CORE.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER.CORE.SOURCES.NAME        := funner.render.scene_render
RENDER.SCENE_RENDER.CORE.SOURCES.SOURCE_DIRS := sources/screen sources/core
RENDER.SCENE_RENDER.CORE.SOURCES.IMPORTS     := compile.render.scene_render compile.common compile.scene_graph.core \
                                                compile.render.mid_level compile.media.image
                                             
#Цель №2 - Scene render2d sources
RENDER.SCENE_RENDER.RENDER2D.SOURCES.TYPE                 := static-lib
RENDER.SCENE_RENDER.RENDER2D.SOURCES.NAME                 := funner.render.scene_render.render2d
RENDER.SCENE_RENDER.RENDER2D.SOURCES.SOURCE_DIRS          := sources/render2d
RENDER.SCENE_RENDER.RENDER2D.SOURCES.IMPORTS              := compile.render.scene_render compile.common compile.scene_graph.core \
                                                             compile.render.mid_level compile.media.rfx \
                                                             compile.media.image compile.media.font \
                                                             compile.media.video
RENDER.SCENE_RENDER.RENDER2D.SOURCES.msvc.COMPILER_CFLAGS := -wd4355

#Цель №3 - Scene render tests
RENDER.SCENE_RENDER.CORE.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER.CORE.TESTS.SOURCE_DIRS := tests/core
RENDER.SCENE_RENDER.CORE.TESTS.IMPORTS     := compile.render.scene_render link.render.scene_render link.render.mid_level.debug_renderer_driver \
                                              compile.scene_graph.core compile.common compile.render.mid_level

#Цель №4 - Scene render2d tests
RENDER.SCENE_RENDER.RENDER2D.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER.RENDER2D.TESTS.SOURCE_DIRS := tests/render2d
RENDER.SCENE_RENDER.RENDER2D.TESTS.IMPORTS     := compile.render.scene_render link.render.scene_render.render2d \
                                                  link.render.mid_level.debug_renderer_driver compile.scene_graph.core compile.common \
                                                  link.media.rfx.xmtl link.media.font.xfont
