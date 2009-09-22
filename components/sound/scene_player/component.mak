###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.SCENE_PLAYER.SOURCES SOUND.SCENE_PLAYER.TESTS

#Цель №1 - ScenePlayer sources
SOUND.SCENE_PLAYER.SOURCES.TYPE             := static-lib
SOUND.SCENE_PLAYER.SOURCES.NAME             := funner.sound.scene_player
SOUND.SCENE_PLAYER.SOURCES.SOURCE_DIRS      := sources
SOUND.SCENE_PLAYER.SOURCES.IMPORTS          := compile.sound.scene_player compile.sound.low_level compile.system

#Цель №2 - ScenePlayer tests
SOUND.SCENE_PLAYER.TESTS.TYPE             := test-suite
SOUND.SCENE_PLAYER.TESTS.SOURCE_DIRS      := tests
SOUND.SCENE_PLAYER.TESTS.IMPORTS          := link.sound.scene_player compile.sound.scene_player compile.system link.sound.low_level.openal_driver \
                                             link.media.sound.snddecl link.media.sound.default_decoders run.sound.low_level.openal_driver
