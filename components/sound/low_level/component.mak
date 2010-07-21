###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.LOW_LEVEL.SOURCES SOUND.LOW_LEVEL.TESTS SOUND.LOW_LEVEL.INFO

#Цель №1 - Sound system sources
SOUND.LOW_LEVEL.SOURCES.TYPE             := static-lib
SOUND.LOW_LEVEL.SOURCES.NAME             := funner.sound.low_level
SOUND.LOW_LEVEL.SOURCES.INCLUDE_DIRS     := 
SOUND.LOW_LEVEL.SOURCES.SOURCE_DIRS      := sources/device
SOUND.LOW_LEVEL.SOURCES.LIB_DIRS         :=  
SOUND.LOW_LEVEL.SOURCES.LIBS             := 
SOUND.LOW_LEVEL.SOURCES.COMPILER_CFLAGS  :=
SOUND.LOW_LEVEL.SOURCES.COMPILER_DEFINES := 
SOUND.LOW_LEVEL.SOURCES.IMPORTS          := compile.sound.low_level compile.common

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
SOUND.LOW_LEVEL.TESTS.IMPORTS          := compile.sound.low_level link.sound.low_level compile.common

#Цель - сборка документации
SOUND.LOW_LEVEL.INFO.TYPE        := doxygen-info
SOUND.LOW_LEVEL.INFO.CHM_NAME    := funner.sound.low_level
SOUND.LOW_LEVEL.INFO.SOURCE_DIRS := include
SOUND.LOW_LEVEL.INFO.IMPORTS     := compile.sound.low_level
