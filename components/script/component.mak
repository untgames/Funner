###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPTLIB_SOURCES SCRIPTLIB_TESTS

#Цель №1 - SCRIPTLIB sources
SCRIPTLIB_SOURCES.TYPE             := static-lib
SCRIPTLIB_SOURCES.NAME             := script
SCRIPTLIB_SOURCES.INCLUDE_DIRS     := include ../../extern/lua/include
SCRIPTLIB_SOURCES.SOURCE_DIRS      := sources/core sources/lua
SCRIPTLIB_SOURCES.LIB_DIRS         :=  
SCRIPTLIB_SOURCES.LIBS             :=
SCRIPTLIB_SOURCES.COMPILER_CFLAGS  := 
SCRIPTLIB_SOURCES.COMPILER_DEFINES := 
SCRIPTLIB_SOURCES.IMPORTS				   := ../common/exports.static.mak

#Цель №2 - Scriptlib tests
SCRIPTLIB_TESTS.TYPE             := test-suite
SCRIPTLIB_TESTS.INCLUDE_DIRS     :=
SCRIPTLIB_TESTS.SOURCE_DIRS      := tests/core tests/lua
SCRIPTLIB_TESTS.LIB_DIRS         :=  
SCRIPTLIB_TESTS.LIBS             :=
SCRIPTLIB_TESTS.COMPILER_CFLAGS  := 
SCRIPTLIB_TESTS.COMPILER_DEFINES :=
SCRIPTLIB_TESTS.IMPORTS				   := exports.static.mak
