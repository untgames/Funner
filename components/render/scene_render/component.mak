###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.SCENE_RENDER.CORE

#Цель №1 - Scene render sources
RENDER.SCENE_RENDER.CORE.TYPE             := static-lib
RENDER.SCENE_RENDER.CORE.NAME             := render.scene_render
RENDER.SCENE_RENDER.CORE.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.CORE.SOURCE_DIRS      := sources/core
RENDER.SCENE_RENDER.CORE.LIB_DIRS         :=  
RENDER.SCENE_RENDER.CORE.LIBS             := 
RENDER.SCENE_RENDER.CORE.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.CORE.COMPILER_DEFINES :=
RENDER.SCENE_RENDER.CORE.IMPORTS          := compile.static.mak ../../common/compile.static.mak ../../scene_graph/compile.static.mak \
                                             ../mid_level/compile.static.mak

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
