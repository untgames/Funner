###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCENE_GRAPH_PLAYER_SOURCES SCENE_GRAPH_PLAYER_TESTS

#Цель №1 - SceneGraphPlayer sources
SCENE_GRAPH_PLAYER_SOURCES.TYPE             := static-lib
SCENE_GRAPH_PLAYER_SOURCES.NAME             := sound.scene_graph_player
SCENE_GRAPH_PLAYER_SOURCES.INCLUDE_DIRS     := 
SCENE_GRAPH_PLAYER_SOURCES.SOURCE_DIRS      := sources
SCENE_GRAPH_PLAYER_SOURCES.LIB_DIRS         :=  
SCENE_GRAPH_PLAYER_SOURCES.LIBS             := 
SCENE_GRAPH_PLAYER_SOURCES.COMPILER_CFLAGS  :=
SCENE_GRAPH_PLAYER_SOURCES.COMPILER_DEFINES :=
SCENE_GRAPH_PLAYER_SOURCES.IMPORTS          := compile.static.mak ../low_level/compile.static.mak

#Цель №2 - SceneGraphPlayer tests
SCENE_GRAPH_PLAYER_TESTS.TYPE             := test-suite
SCENE_GRAPH_PLAYER_TESTS.INCLUDE_DIRS     :=
SCENE_GRAPH_PLAYER_TESTS.SOURCE_DIRS      := tests
SCENE_GRAPH_PLAYER_TESTS.LIBS             :=
SCENE_GRAPH_PLAYER_TESTS.DLLS             :=
SCENE_GRAPH_PLAYER_TESTS.DLL_DIRS         :=
SCENE_GRAPH_PLAYER_TESTS.COMPILER_CFLAGS  :=
SCENE_GRAPH_PLAYER_TESTS.COMPILER_DEFINES :=
SCENE_GRAPH_PLAYER_TESTS.IMPORTS          := link.static.mak compile.static.mak ../../system/exports.static.mak ../openal_device/link.static.mak ../../media/sound/snddecl.link.static.mak \
														                 ../../media/sound/default_decoders.link.static.mak ../openal_device/run.static.mak
