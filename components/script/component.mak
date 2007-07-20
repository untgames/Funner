###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPTLIB_COMPONENTS SCRIPTLIB_TESTS

#Цель №1 - SCRIPTLIB sources
SCRIPTLIB_COMPONENTS.TYPE             := static-lib
SCRIPTLIB_COMPONENTS.NAME             := scriptlib
SCRIPTLIB_COMPONENTS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include ../../extern/lua/include
SCRIPTLIB_COMPONENTS.SOURCE_DIRS      := sources
SCRIPTLIB_COMPONENTS.LIB_DIRS         :=  
SCRIPTLIB_COMPONENTS.LIBS             := lua
SCRIPTLIB_COMPONENTS.COMPILER_CFLAGS  :=
SCRIPTLIB_COMPONENTS.COMPILER_DEFINES := 

#Цель №2 - Scriptlib tests
SCRIPTLIB_TESTS.TYPE             := test-suite
SCRIPTLIB_TESTS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include ../../extern/lua/include
SCRIPTLIB_TESTS.SOURCE_DIRS      := tests
SCRIPTLIB_TESTS.LIB_DIRS         :=  
SCRIPTLIB_TESTS.LIBS             := commonlib scriptlib lua
SCRIPTLIB_TESTS.COMPILER_CFLAGS  :=
SCRIPTLIB_TESTS.COMPILER_DEFINES :=
