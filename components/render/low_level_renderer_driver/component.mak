###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS

#Цель №1 - Low level render system sources
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.TYPE             := static-lib
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.NAME             := render.mid_level.low_level_renderer_driver
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.INCLUDE_DIRS     := 
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.SOURCE_DIRS      := sources
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.LIB_DIRS         :=  
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.LIBS             := 
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.COMPILER_DEFINES :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak ../../media/image/compile.static.mak ../low_level/compile.static.mak

#Цель №2 - Low level render system tests
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.TYPE             := test-suite
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.INCLUDE_DIRS     := ../../xtl/include
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.SOURCE_DIRS      := tests
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.LIB_DIRS         :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.LIBS             :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.COMPILER_CFLAGS  :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.COMPILER_DEFINES := 
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.IMPORTS          := link.static.mak compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak ../../system/link.static.mak ../opengl_driver/compile.static.mak ../opengl_driver/link.static.mak
