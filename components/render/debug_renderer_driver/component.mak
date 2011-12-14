###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.DEBUG_RENDERER_DRIVER.SOURCES RENDER.DEBUG_RENDERER_DRIVER.TESTS

#Цель №1 - DEBUG render system sources
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.TYPE        := static-lib
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.NAME        := funner.render.mid_level.debug_renderer_driver
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.SOURCE_DIRS := sources
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.IMPORTS     := compile.render.mid_level compile.common compile.media.image

#Цель №2 - DEBUG render system tests
RENDER.DEBUG_RENDERER_DRIVER.TESTS.TYPE        := test-suite
RENDER.DEBUG_RENDERER_DRIVER.TESTS.SOURCE_DIRS := tests
RENDER.DEBUG_RENDERER_DRIVER.TESTS.IMPORTS     := link.render.mid_level.debug_renderer_driver compile.render.mid_level \
                                                  compile.common compile.media.image
