###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.OPENAL_DRIVER.SOURCES SOUND.OPENAL_DRIVER.TESTS

#Цель №1 - OPENALDEVICE sources
SOUND.OPENAL_DRIVER.SOURCES.TYPE                   := static-lib
SOUND.OPENAL_DRIVER.SOURCES.NAME                   := funner.sound.low_level.openal_driver
SOUND.OPENAL_DRIVER.SOURCES.SOURCE_DIRS            := sources
SOUND.OPENAL_DRIVER.SOURCES.IMPORTS                := compile.sound.low_level compile.media.sound compile.system compile.extern.openal

#Цель №2 - OPENALDEVICE tests
SOUND.OPENAL_DRIVER.TESTS.TYPE         := test-suite
SOUND.OPENAL_DRIVER.TESTS.INCLUDE_DIRS := ../../xtl/include
SOUND.OPENAL_DRIVER.TESTS.SOURCE_DIRS  := tests
SOUND.OPENAL_DRIVER.TESTS.LIB_DIRS     :=
SOUND.OPENAL_DRIVER.TESTS.IMPORTS      := link.sound.low_level.openal_driver run.sound.low_level.openal_driver compile.common compile.system compile.sound.low_level \
                                          link.media.sound.default_decoders
