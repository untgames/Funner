###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.SCENE_RENDER_2D.SOURCES RENDER.SCENE_RENDER_2D.TESTS

#Цель - Scene render sources
RENDER.SCENE_RENDER_2D.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER_2D.SOURCES.NAME        := funner.render.scene_render2d
RENDER.SCENE_RENDER_2D.SOURCES.SOURCE_DIRS := sources/screen sources/core sources/render2d
RENDER.SCENE_RENDER_2D.SOURCES.IMPORTS     := compile.render.scene_render2d compile.common compile.scene_graph.core \
                                              compile.render.low_level compile.render.mid_level compile.media.image \
                                              compile.media.rfx.obsolete compile.media.image compile.media.font compile.media.video
RENDER.SCENE_RENDER_2D.SOURCES.msvc.COMPILER_CFLAGS := -wd4355                                                
                                             
#Цель - Scene render tests
RENDER.SCENE_RENDER_2D.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER_2D.TESTS.SOURCE_DIRS := tests/core tests/render2d
RENDER.SCENE_RENDER_2D.TESTS.IMPORTS     := compile.render.scene_render2d link.render.scene_render2d link.render.mid_level.debug_renderer_driver \
                                            compile.scene_graph.core compile.common compile.render.mid_level \
                                            link.media.rfx.xmtl.obsolete link.media.font.xfont
