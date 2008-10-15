###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.LOW_LEVEL.SOURCES INPUT.LOW_LEVEL.TESTS

#Цель №1 - INPUT system sources
INPUT.LOW_LEVEL.SOURCES.TYPE             := static-lib
INPUT.LOW_LEVEL.SOURCES.NAME             := funner.input.low_level
INPUT.LOW_LEVEL.SOURCES.INCLUDE_DIRS     := 
INPUT.LOW_LEVEL.SOURCES.SOURCE_DIRS      := sources
INPUT.LOW_LEVEL.SOURCES.LIB_DIRS         :=  
INPUT.LOW_LEVEL.SOURCES.LIBS             := 
INPUT.LOW_LEVEL.SOURCES.COMPILER_CFLAGS  :=
INPUT.LOW_LEVEL.SOURCES.COMPILER_DEFINES := 
INPUT.LOW_LEVEL.SOURCES.IMPORTS          := compile.static.mak

#Цель №2 - INPUT system tests
INPUT.LOW_LEVEL.TESTS.TYPE             := test-suite
INPUT.LOW_LEVEL.TESTS.INCLUDE_DIRS     :=
INPUT.LOW_LEVEL.TESTS.SOURCE_DIRS      := tests
INPUT.LOW_LEVEL.TESTS.LIB_DIRS         :=
INPUT.LOW_LEVEL.TESTS.LIBS             :=
INPUT.LOW_LEVEL.TESTS.DLLS             :=
INPUT.LOW_LEVEL.TESTS.DLL_DIRS         :=
INPUT.LOW_LEVEL.TESTS.COMPILER_CFLAGS  :=
INPUT.LOW_LEVEL.TESTS.COMPILER_DEFINES :=
INPUT.LOW_LEVEL.TESTS.IMPORTS          := compile.static.mak link.static.mak
