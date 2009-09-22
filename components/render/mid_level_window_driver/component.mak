###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS

#PROFILES += egl gles

#Цель №1 - Low level render system sources
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.TYPE             := static-lib
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.NAME             := funner.render.mid_level.window_driver
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.INCLUDE_DIRS     := sources
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.SOURCE_DIRS      := sources/basic_renderer sources/renderer2d sources/low_level_renderer
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.IMPORTS          := compile.render.mid_level.window_driver compile.common compile.media.image compile.render.low_level

#Цель №2 - Low level render system tests
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.TYPE             := test-suite
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.INCLUDE_DIRS     := ../../xtl/include
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.SOURCE_DIRS      := tests
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.IMPORTS          := link.render.mid_level.window_driver compile.render.mid_level.window_driver \
                                                         compile.render.low_level compile.system link.system  \
                                                         link.render.low_level.opengl_driver compile.media.image \
                                                         link.common.xml run.render.low_level.opengl_driver
