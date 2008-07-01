###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.DEBUG_RENDERER_DRIVER.SOURCES RENDER.DEBUG_RENDERER_DRIVER.TESTS

#Цель №1 - DEBUG render system sources
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.TYPE             := static-lib
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.NAME             := render.mid_level.debug_renderer_driver
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.INCLUDE_DIRS     := sources
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.SOURCE_DIRS      :=
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.LIB_DIRS         :=  
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.LIBS             := 
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.COMPILER_DEFINES :=
RENDER.DEBUG_RENDERER_DRIVER.SOURCES.IMPORTS          := ../mid_level/compile.static.mak ../../common/compile.static.mak

#Цель №2 - DEBUG render system tests
RENDER.DEBUG_RENDERER_DRIVER.TESTS.TYPE             := test-suite
RENDER.DEBUG_RENDERER_DRIVER.TESTS.INCLUDE_DIRS     :=
RENDER.DEBUG_RENDERER_DRIVER.TESTS.SOURCE_DIRS      :=
RENDER.DEBUG_RENDERER_DRIVER.TESTS.LIB_DIRS         :=
RENDER.DEBUG_RENDERER_DRIVER.TESTS.LIBS             :=
RENDER.DEBUG_RENDERER_DRIVER.TESTS.COMPILER_CFLAGS  :=
RENDER.DEBUG_RENDERER_DRIVER.TESTS.COMPILER_DEFINES := 
RENDER.DEBUG_RENDERER_DRIVER.TESTS.IMPORTS          := link.static.mak ../mid_level/compile.static.mak
