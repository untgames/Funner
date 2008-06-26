###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS

#Цель №1 - OpenGL render system sources
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.TYPE             := static-lib
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.NAME             := render.mid_level.low_level_renderer_driver
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.INCLUDE_DIRS     := sources
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.SOURCE_DIRS      :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.LIB_DIRS         :=  
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.LIBS             := 
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.COMPILER_DEFINES :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak

#Цель №2 - OpenGL render system tests
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.TYPE             := test-suite
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.INCLUDE_DIRS     :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.SOURCE_DIRS      :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.LIB_DIRS         :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.LIBS             :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.COMPILER_CFLAGS  :=
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.COMPILER_DEFINES := 
RENDER.LOW_LEVEL_RENDERER_DRIVER.TESTS.IMPORTS          := link.static.mak compile.static.mak
