###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT_LOW_LEVEL_SOURCES INPUT_LOW_LEVEL_TESTS

#Цель №1 - INPUT system sources
INPUT_LOW_LEVEL_SOURCES.TYPE             := static-lib
INPUT_LOW_LEVEL_SOURCES.NAME             := input.low_level
INPUT_LOW_LEVEL_SOURCES.INCLUDE_DIRS     := include
INPUT_LOW_LEVEL_SOURCES.SOURCE_DIRS      := sources
INPUT_LOW_LEVEL_SOURCES.LIB_DIRS         :=  
INPUT_LOW_LEVEL_SOURCES.LIBS             := 
INPUT_LOW_LEVEL_SOURCES.COMPILER_CFLAGS  :=
INPUT_LOW_LEVEL_SOURCES.COMPILER_DEFINES := 
INPUT_LOW_LEVEL_SOURCES.IMPORTS          := ../../common/exports.static.mak

#Цель №2 - INPUT system tests
INPUT_LOW_LEVEL_TESTS.TYPE             := test-suite
INPUT_LOW_LEVEL_TESTS.INCLUDE_DIRS     :=
INPUT_LOW_LEVEL_TESTS.SOURCE_DIRS      := tests
INPUT_LOW_LEVEL_TESTS.LIB_DIRS         :=
INPUT_LOW_LEVEL_TESTS.LIBS             :=
INPUT_LOW_LEVEL_TESTS.DLLS             :=
INPUT_LOW_LEVEL_TESTS.DLL_DIRS         :=
INPUT_LOW_LEVEL_TESTS.COMPILER_CFLAGS  :=
INPUT_LOW_LEVEL_TESTS.COMPILER_DEFINES :=
INPUT_LOW_LEVEL_TESTS.IMPORTS          := exports.static.mak
