###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.RENDER2D.SHARED DEMOS.RENDER2D.TESTS

#PROFILES += egl gles

#Цель №1 - Общий код 2D рендера
DEMOS.RENDER2D.SHARED.TYPE             := static-lib
DEMOS.RENDER2D.SHARED.NAME             := funner.demos.render2d.shared
DEMOS.RENDER2D.SHARED.INCLUDE_DIRS     :=
DEMOS.RENDER2D.SHARED.SOURCE_DIRS      := shared
DEMOS.RENDER2D.SHARED.IMPORTS          := ../../system/compile.static.mak ../../render/scene_render/compile.static.mak \
                                         ../../render/mid_level_window_driver/compile.static.mak ../../render/low_level/compile.static.mak \
                                         ../../scene_graph/core/compile.static.mak

#Цель №2 - Тесты 2D рендера
DEMOS.RENDER2D.TESTS.TYPE             := test-suite
DEMOS.RENDER2D.TESTS.INCLUDE_DIRS     := shared
DEMOS.RENDER2D.TESTS.SOURCE_DIRS      := sources
DEMOS.RENDER2D.TESTS.LIB_DIRS         :=  
DEMOS.RENDER2D.TESTS.LIBS             := funner.demos.render2d.shared
DEMOS.RENDER2D.TESTS.LINK_INCLUDES    :=
DEMOS.RENDER2D.TESTS.IMPORTS          := ../../system/link.static.mak ../../scene_graph/core/compile.static.mak \
                                         ../../render/scene_render/compile.static.mak ../../render/scene_render/render2d.link.static.mak \
                                         ../../render/mid_level_window_driver/link.static.mak ../../render/opengl_driver/link.static.mak \
                                         ../../media/rfx/xmtl.link.static.mak ../../render/debug_renderer_driver/link.static.mak \
                                         ../../common/compile.static.mak ../../common/configurator.link.static.mak ../../common/wxf.link.static.mak \
                                         ../../render/opengl_driver/run.static.mak
