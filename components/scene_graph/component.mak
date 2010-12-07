###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCENE_GRAPH.CORE.SOURCES SCENE_GRAPH.CORE.TESTS SCENE_GRAPH.CORE.INFO

#Цель №1 - SG components sources
SCENE_GRAPH.CORE.SOURCES.TYPE             := static-lib
SCENE_GRAPH.CORE.SOURCES.NAME             := funner.scene_graph.core
SCENE_GRAPH.CORE.SOURCES.INCLUDE_DIRS     := ../../math/bound_volumes/include
SCENE_GRAPH.CORE.SOURCES.SOURCE_DIRS      := sources/core sources/controllers
SCENE_GRAPH.CORE.SOURCES.LIB_DIRS         :=  
SCENE_GRAPH.CORE.SOURCES.LIBS             :=
SCENE_GRAPH.CORE.SOURCES.COMPILER_DEFINES :=  
SCENE_GRAPH.CORE.SOURCES.IMPORTS          := compile.scene_graph.core compile.common
SCENE_GRAPH.CORE.SOURCES.msvc.COMPILER_CFLAGS  := -wd4355

#Цель №2 - SG tests
SCENE_GRAPH.CORE.TESTS.TYPE             := test-suite
SCENE_GRAPH.CORE.TESTS.INCLUDE_DIRS     :=
SCENE_GRAPH.CORE.TESTS.SOURCE_DIRS      := tests/core tests/controllers
SCENE_GRAPH.CORE.TESTS.LIB_DIRS         :=
SCENE_GRAPH.CORE.TESTS.LIBS             :=
SCENE_GRAPH.CORE.TESTS.COMPILER_CFLAGS  :=
SCENE_GRAPH.CORE.TESTS.COMPILER_DEFINES := 
SCENE_GRAPH.CORE.TESTS.IMPORTS          := link.scene_graph.core compile.scene_graph.core compile.common

#Цель - сборка документации
SCENE_GRAPH.CORE.INFO.TYPE        := doxygen-info
SCENE_GRAPH.CORE.INFO.CHM_NAME    := funner.scene_graph.core
SCENE_GRAPH.CORE.INFO.SOURCE_DIRS := include
SCENE_GRAPH.CORE.INFO.IMPORTS     := compile.scene_graph.core
