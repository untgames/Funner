###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.DUMMY_RENDERER_DRIVER.SOURCES RENDER.DUMMY_RENDERER_DRIVER.TESTS

#Цель №1 - Dummy render system sources
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.TYPE             := static-lib
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.NAME             := render.mid_level.dummy_renderer_driver
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.INCLUDE_DIRS     := sources
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.SOURCE_DIRS      :=
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.LIB_DIRS         :=  
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.LIBS             := 
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.COMPILER_DEFINES :=
RENDER.DUMMY_RENDERER_DRIVER.SOURCES.IMPORTS          := ../mid_level/compile.static.mak ../../common/compile.static.mak

#Цель №2 - Dummy render system tests
RENDER.DUMMY_RENDERER_DRIVER.TESTS.TYPE             := test-suite
RENDER.DUMMY_RENDERER_DRIVER.TESTS.INCLUDE_DIRS     :=
RENDER.DUMMY_RENDERER_DRIVER.TESTS.SOURCE_DIRS      :=
RENDER.DUMMY_RENDERER_DRIVER.TESTS.LIB_DIRS         :=
RENDER.DUMMY_RENDERER_DRIVER.TESTS.LIBS             :=
RENDER.DUMMY_RENDERER_DRIVER.TESTS.COMPILER_CFLAGS  :=
RENDER.DUMMY_RENDERER_DRIVER.TESTS.COMPILER_DEFINES := 
RENDER.DUMMY_RENDERER_DRIVER.TESTS.IMPORTS          := link.static.mak ../mid_level/compile.static.mak
