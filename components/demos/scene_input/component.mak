###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.SCENE_INPUT.SHARED DEMOS.SCENE_INPUT.TESTS

#Цель №1 - Общий код 2D рендера
DEMOS.SCENE_INPUT.SHARED.TYPE             := static-lib
DEMOS.SCENE_INPUT.SHARED.NAME             := funner.demos.scene_input.shared
DEMOS.SCENE_INPUT.SHARED.INCLUDE_DIRS     :=
DEMOS.SCENE_INPUT.SHARED.SOURCE_DIRS      := shared
DEMOS.SCENE_INPUT.SHARED.IMPORTS          := compile.system compile.render.scene_render2d compile.render.mid_level.window_driver compile.render.low_level \
                                             compile.scene_graph.core compile.input.window_driver compile.input.scene_input compile.input.manager

#Цель №2 - Тесты 2D рендера
DEMOS.SCENE_INPUT.TESTS.TYPE             := test-suite
DEMOS.SCENE_INPUT.TESTS.INCLUDE_DIRS     := shared
DEMOS.SCENE_INPUT.TESTS.SOURCE_DIRS      := sources
DEMOS.SCENE_INPUT.TESTS.LIB_DIRS         :=  
DEMOS.SCENE_INPUT.TESTS.LIBS             := funner.demos.scene_input.shared
DEMOS.SCENE_INPUT.TESTS.LINK_INCLUDES    :=
DEMOS.SCENE_INPUT.TESTS.IMPORTS          := compile.common compile.system compile.scene_graph.core compile.render.scene_render2d link.render.scene_render2d \
                                         link.render.mid_level.window_driver link.render.low_level.opengl_driver link.system \
                                         link.media.rfx.xmtl.obsolete link.render.mid_level.debug_renderer_driver link.common.wxf \
                                         run.render.low_level.opengl_driver link.input.window_driver compile.input.scene_input link.input.scene_input \
                                         link.input.manager
