###################################################################################################
#Определения и константы
###################################################################################################
#TARGETS := RENDER.SCENE_RENDER.CORE.SOURCES RENDER.SCENE_RENDER.CORE.TESTS
TARGETS := RENDER.SCENE_RENDER.CORE.SOURCES

#Цель №1 - Scene render sources
RENDER.SCENE_RENDER.CORE.SOURCES.TYPE             := static-lib
RENDER.SCENE_RENDER.CORE.SOURCES.NAME             := render.scene_render
RENDER.SCENE_RENDER.CORE.SOURCES.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.CORE.SOURCES.SOURCE_DIRS      := sources/core
RENDER.SCENE_RENDER.CORE.SOURCES.LIB_DIRS         :=  
RENDER.SCENE_RENDER.CORE.SOURCES.LIBS             := 
RENDER.SCENE_RENDER.CORE.SOURCES.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.CORE.SOURCES.COMPILER_DEFINES :=
RENDER.SCENE_RENDER.CORE.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak ../../scene_graph/compile.static.mak \
                                             ../mid_level/compile.static.mak

#Цель №2 - Scene render tests
RENDER.SCENE_RENDER.CORE.TESTS.TYPE             := test-suite
RENDER.SCENE_RENDER.CORE.TESTS.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.CORE.TESTS.SOURCE_DIRS      := tests/core
RENDER.SCENE_RENDER.CORE.TESTS.LIB_DIRS         :=
RENDER.SCENE_RENDER.CORE.TESTS.LIBS             :=
RENDER.SCENE_RENDER.CORE.TESTS.DLLS             :=
RENDER.SCENE_RENDER.CORE.TESTS.DLL_DIRS         :=
RENDER.SCENE_RENDER.CORE.TESTS.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.CORE.TESTS.IMPORTS          := compile.static.mak link.static.mak ../mid_level/compile.static.mak
