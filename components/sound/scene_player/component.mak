###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCENE_PLAYER_SOURCES SCENE_PLAYER_TESTS

#Цель №1 - ScenePlayer sources
SCENE_PLAYER_SOURCES.TYPE             := static-lib
SCENE_PLAYER_SOURCES.NAME             := sound.scene_player
SCENE_PLAYER_SOURCES.INCLUDE_DIRS     := 
SCENE_PLAYER_SOURCES.SOURCE_DIRS      := sources
SCENE_PLAYER_SOURCES.LIB_DIRS         :=  
SCENE_PLAYER_SOURCES.LIBS             := 
SCENE_PLAYER_SOURCES.COMPILER_CFLAGS  :=
SCENE_PLAYER_SOURCES.COMPILER_DEFINES :=
SCENE_PLAYER_SOURCES.IMPORTS          := compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak

#Цель №2 - ScenePlayer tests
SCENE_PLAYER_TESTS.TYPE             := test-suite
SCENE_PLAYER_TESTS.INCLUDE_DIRS     :=
SCENE_PLAYER_TESTS.SOURCE_DIRS      := tests
SCENE_PLAYER_TESTS.LIBS             :=
SCENE_PLAYER_TESTS.DLLS             :=
SCENE_PLAYER_TESTS.DLL_DIRS         :=
SCENE_PLAYER_TESTS.COMPILER_CFLAGS  :=
SCENE_PLAYER_TESTS.COMPILER_DEFINES :=
SCENE_PLAYER_TESTS.IMPORTS          := link.static.mak compile.static.mak ../../system/compile.static.mak ../openal_device/link.static.mak ../../media/sound/snddecl.link.static.mak \
						  				                 ../../media/sound/default_decoders.link.static.mak ../openal_device/run.static.mak
