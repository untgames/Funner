###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS

#Цель №1 - SG components sources
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.TYPE             := static-lib
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.NAME             := funner.scene_graph.controllers.box2d_physics
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.INCLUDE_DIRS     :=
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.SOURCE_DIRS      := sources
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.LIB_DIRS         :=  
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.LIBS             :=
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.COMPILER_DEFINES := 
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.IMPORTS          := compile.common compile.scene_graph.core compile.extern.box2d
SCENE_GRAPH.CONTROLLERS.BOX2D.SOURCES.g++.COMPILER_CFLAGS := -Wno-unused -Wno-switch

#Цель №2 - SG tests
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.TYPE             := test-suite
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.INCLUDE_DIRS     :=
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.SOURCE_DIRS      := tests
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.LIB_DIRS         :=
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.LIBS             :=
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.COMPILER_CFLAGS  :=
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.COMPILER_DEFINES := 
SCENE_GRAPH.CONTROLLERS.BOX2D.TESTS.IMPORTS          := link.scene_graph.controllers.box2d_physics compile.scene_graph.core compile.common link.common.xml
