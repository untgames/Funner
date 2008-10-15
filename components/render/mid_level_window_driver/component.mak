###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS

#Цель №1 - Low level render system sources
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.TYPE             := static-lib
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.NAME             := funner.render.mid_level.window_driver
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.INCLUDE_DIRS     := sources
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.SOURCE_DIRS      := sources/basic_renderer sources/renderer2d sources/low_level_renderer
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.LIB_DIRS         :=  
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.LIBS             := 
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.COMPILER_DEFINES :=
RENDER.MID_LEVEL_WINDOW_DRIVER.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak ../../media/image/compile.static.mak ../low_level/compile.static.mak

#Цель №2 - Low level render system tests
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.TYPE             := test-suite
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.INCLUDE_DIRS     := ../../xtl/include
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.SOURCE_DIRS      := tests
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.LIB_DIRS         :=
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.LIBS             :=
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.COMPILER_CFLAGS  :=
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.COMPILER_DEFINES := 
RENDER.MID_LEVEL_WINDOW_DRIVER.TESTS.IMPORTS          := link.static.mak compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak ../../system/link.static.mak  \
                                                         ../opengl_driver/link.static.mak ../../media/image/compile.static.mak \
                                                         ../../common/xml.link.static.mak
