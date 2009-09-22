###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.MANAGER.SOURCES SOUND.MANAGER.TESTS

#Цель №1 - Sound system sources
SOUND.MANAGER.SOURCES.TYPE             := static-lib
SOUND.MANAGER.SOURCES.NAME             := funner.sound.manager
SOUND.MANAGER.SOURCES.SOURCE_DIRS      := sources/manager
SOUND.MANAGER.SOURCES.IMPORTS          := compile.sound.manager compile.media.sound compile.common

#Цель №2 - Sound system tests
SOUND.MANAGER.TESTS.TYPE             := test-suite
SOUND.MANAGER.TESTS.SOURCE_DIRS      := tests/manager
SOUND.MANAGER.TESTS.IMPORTS          := compile.sound.manager link.sound.manager link.sound.low_level.openal_driver compile.system \
                                        link.media.sound.snddecl link.media.sound.default_decoders run.sound.low_level.openal_driver
