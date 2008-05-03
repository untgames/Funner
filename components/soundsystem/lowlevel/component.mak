###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND_LOW_LEVEL_SOURCES SOUND_LOW_LEVEL_TESTS

#Цель №1 - Sound system sources
SOUND_LOW_LEVEL_SOURCES.TYPE             := static-lib
SOUND_LOW_LEVEL_SOURCES.NAME             := soundlowlevel
SOUND_LOW_LEVEL_SOURCES.INCLUDE_DIRS     := include ../../mathlib/include
SOUND_LOW_LEVEL_SOURCES.SOURCE_DIRS      := sources/device
SOUND_LOW_LEVEL_SOURCES.LIB_DIRS         :=  
SOUND_LOW_LEVEL_SOURCES.LIBS             := 
SOUND_LOW_LEVEL_SOURCES.COMPILER_CFLAGS  :=
SOUND_LOW_LEVEL_SOURCES.COMPILER_DEFINES := 
SOUND_LOW_LEVEL_SOURCES.IMPORTS          := ../../commonlib/exports.mak 

#Цель №2 - Sound system tests
SOUND_LOW_LEVEL_TESTS.TYPE             := test-suite
SOUND_LOW_LEVEL_TESTS.INCLUDE_DIRS     := include ../../mathlib/include
SOUND_LOW_LEVEL_TESTS.SOURCE_DIRS      := tests/device
SOUND_LOW_LEVEL_TESTS.LIB_DIRS         := 
SOUND_LOW_LEVEL_TESTS.LIBS             := soundlowlevel
SOUND_LOW_LEVEL_TESTS.DLLS             :=
SOUND_LOW_LEVEL_TESTS.DLL_DIRS         :=
SOUND_LOW_LEVEL_TESTS.COMPILER_CFLAGS  :=
SOUND_LOW_LEVEL_TESTS.COMPILER_DEFINES :=
SOUND_LOW_LEVEL_TESTS.IMPORTS          := ../../commonlib/exports.mak 
