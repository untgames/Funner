###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.SCENE_PLAYER.SOURCES SOUND.SCENE_PLAYER.TESTS

#Цель №1 - ScenePlayer sources
SOUND.SCENE_PLAYER.SOURCES.TYPE             := static-lib
SOUND.SCENE_PLAYER.SOURCES.NAME             := sound.scene_player
SOUND.SCENE_PLAYER.SOURCES.INCLUDE_DIRS     := 
SOUND.SCENE_PLAYER.SOURCES.SOURCE_DIRS      := sources
SOUND.SCENE_PLAYER.SOURCES.LIB_DIRS         :=  
SOUND.SCENE_PLAYER.SOURCES.LIBS             := 
SOUND.SCENE_PLAYER.SOURCES.COMPILER_CFLAGS  :=
SOUND.SCENE_PLAYER.SOURCES.COMPILER_DEFINES :=
SOUND.SCENE_PLAYER.SOURCES.IMPORTS          := compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak

#Цель №2 - ScenePlayer tests
SOUND.SCENE_PLAYER.TESTS.TYPE             := test-suite
SOUND.SCENE_PLAYER.TESTS.INCLUDE_DIRS     :=
SOUND.SCENE_PLAYER.TESTS.SOURCE_DIRS      := tests
SOUND.SCENE_PLAYER.TESTS.LIBS             :=
SOUND.SCENE_PLAYER.TESTS.DLLS             :=
SOUND.SCENE_PLAYER.TESTS.DLL_DIRS         :=
SOUND.SCENE_PLAYER.TESTS.COMPILER_CFLAGS  :=
SOUND.SCENE_PLAYER.TESTS.COMPILER_DEFINES :=
SOUND.SCENE_PLAYER.TESTS.LINK_INCLUDES    :=
SOUND.SCENE_PLAYER.TESTS.IMPORTS          := link.static.mak compile.static.mak ../../system/compile.static.mak ../openal_device/link.static.mak ../../media/sound/snddecl.link.static.mak \
						  				                 ../../media/sound/default_decoders.link.static.mak ../openal_device/run.static.mak
