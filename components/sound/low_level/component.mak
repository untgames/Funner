###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.LOW_LEVEL.SOURCES SOUND.LOW_LEVEL.TESTS

#Цель №1 - Sound system sources
SOUND.LOW_LEVEL.SOURCES.TYPE             := static-lib
SOUND.LOW_LEVEL.SOURCES.NAME             := funner.sound.low_level
SOUND.LOW_LEVEL.SOURCES.INCLUDE_DIRS     := 
SOUND.LOW_LEVEL.SOURCES.SOURCE_DIRS      := sources/device
SOUND.LOW_LEVEL.SOURCES.LIB_DIRS         :=  
SOUND.LOW_LEVEL.SOURCES.LIBS             := 
SOUND.LOW_LEVEL.SOURCES.COMPILER_CFLAGS  :=
SOUND.LOW_LEVEL.SOURCES.COMPILER_DEFINES := 
SOUND.LOW_LEVEL.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak

#Цель №2 - Sound system tests
SOUND.LOW_LEVEL.TESTS.TYPE             := test-suite
SOUND.LOW_LEVEL.TESTS.INCLUDE_DIRS     :=
SOUND.LOW_LEVEL.TESTS.SOURCE_DIRS      := tests/device
SOUND.LOW_LEVEL.TESTS.LIB_DIRS         := 
SOUND.LOW_LEVEL.TESTS.LIBS             :=
SOUND.LOW_LEVEL.TESTS.DLLS             :=
SOUND.LOW_LEVEL.TESTS.DLL_DIRS         :=
SOUND.LOW_LEVEL.TESTS.COMPILER_CFLAGS  :=
SOUND.LOW_LEVEL.TESTS.COMPILER_DEFINES :=
SOUND.LOW_LEVEL.TESTS.IMPORTS          := compile.static.mak link.static.mak ../../common/compile.static.mak
