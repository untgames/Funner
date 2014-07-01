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
DEMOS.RENDER2D.SHARED.IMPORTS          := compile.system compile.render.scene_render.server compile.render.scene_render.client \
                                          compile.scene_graph.core

#Цель №2 - Тесты 2D рендера
DEMOS.RENDER2D.TESTS.TYPE             := test-suite
DEMOS.RENDER2D.TESTS.INCLUDE_DIRS     := shared
DEMOS.RENDER2D.TESTS.SOURCE_DIRS      := sources
DEMOS.RENDER2D.TESTS.LIB_DIRS         :=  
DEMOS.RENDER2D.TESTS.LIBS             := funner.demos.render2d.shared
DEMOS.RENDER2D.TESTS.LINK_INCLUDES    :=
DEMOS.RENDER2D.TESTS.INSTALLATION_FILES := media
DEMOS.RENDER2D.TESTS.IMPORTS          := compile.common compile.system compile.scene_graph.core link.render.scene_render.client link.render.scene_render.server \
                                         link.render.low_level.opengl_driver link.system compile.render.scene_render.client \
                                         link.common.wxf link.media.rfx.default_serializers \
                                         link.media.image.pvr run.render.low_level.opengl_driver link.media.video.theora
