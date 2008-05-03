###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPTLIB_COMPONENTS SCRIPTLIB_TESTS

#Цель №1 - SCRIPTLIB sources
SCRIPTLIB_COMPONENTS.TYPE             := static-lib
SCRIPTLIB_COMPONENTS.NAME             := scriptlib
SCRIPTLIB_COMPONENTS.INCLUDE_DIRS     := include ../../extern/lua/include
SCRIPTLIB_COMPONENTS.SOURCE_DIRS      := sources/core sources/lua
SCRIPTLIB_COMPONENTS.LIB_DIRS         :=  
SCRIPTLIB_COMPONENTS.LIBS             := lua
SCRIPTLIB_COMPONENTS.COMPILER_CFLAGS  := 
SCRIPTLIB_COMPONENTS.COMPILER_DEFINES := 
SCRIPTLIB_COMPONENTS.IMPORTS				  := ../common.static.mak

#Цель №2 - Scriptlib tests
SCRIPTLIB_TESTS.TYPE             := test-suite
SCRIPTLIB_TESTS.INCLUDE_DIRS     := include ../../extern/lua/include
SCRIPTLIB_TESTS.SOURCE_DIRS      := tests/core tests/lua
SCRIPTLIB_TESTS.LIB_DIRS         :=  
SCRIPTLIB_TESTS.LIBS             := scriptlib lua
SCRIPTLIB_TESTS.COMPILER_CFLAGS  := 
SCRIPTLIB_TESTS.COMPILER_DEFINES :=
SCRIPTLIB_TESTS.IMPORTS				   := ../common.static.mak
