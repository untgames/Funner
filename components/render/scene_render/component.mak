###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.SCENE_RENDER.SOURCES RENDER.SCENE_RENDER.TESTS

#Цель №1 - Scene render sources
RENDER.SCENE_RENDER.SOURCES.TYPE             := static-lib
RENDER.SCENE_RENDER.SOURCES.NAME             := render.scene_render
RENDER.SCENE_RENDER.SOURCES.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.SOURCES.SOURCE_DIRS      := sources
RENDER.SCENE_RENDER.SOURCES.LIB_DIRS         :=  
RENDER.SCENE_RENDER.SOURCES.LIBS             := 
RENDER.SCENE_RENDER.SOURCES.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.SOURCES.COMPILER_DEFINES := 
RENDER.SCENE_RENDER.SOURCES.IMPORTS          := compile.static.mak

#Цель №2 - Scene render tests
RENDER.SCENE_RENDER.TESTS.TYPE             := test-suite
RENDER.SCENE_RENDER.TESTS.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.TESTS.SOURCE_DIRS      := tests
RENDER.SCENE_RENDER.TESTS.LIB_DIRS         :=
RENDER.SCENE_RENDER.TESTS.LIBS             :=
RENDER.SCENE_RENDER.TESTS.DLLS             :=
RENDER.SCENE_RENDER.TESTS.DLL_DIRS         :=
RENDER.SCENE_RENDER.TESTS.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.TESTS.IMPORTS          := compile.static.mak link.static.mak
