###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPTBINDS_COMPONENTS SCRIPTBINDS_TESTS

#Цель №1 - SCRIPTBINDS sources
SCRIPTBINDS_COMPONENTS.TYPE             := static-lib
SCRIPTBINDS_COMPONENTS.NAME             := scriptbinds
SCRIPTBINDS_COMPONENTS.INCLUDE_DIRS     := include ../script/include ../xtl/include ../commonlib/include ../mathlib/include ../scenegraph/include ../boundvolumes/include
SCRIPTBINDS_COMPONENTS.SOURCE_DIRS      := sources
SCRIPTBINDS_COMPONENTS.LIB_DIRS         :=  
SCRIPTBINDS_COMPONENTS.LIBS             := sg
SCRIPTBINDS_COMPONENTS.COMPILER_CFLAGS  := 
SCRIPTBINDS_COMPONENTS.COMPILER_DEFINES := 

#Цель №2 - SCRIPTBINDS tests
SCRIPTBINDS_TESTS.TYPE             := test-suite
SCRIPTBINDS_TESTS.INCLUDE_DIRS     := include ../script/include ../commonlib/include ../xtl/include ../mathlib/include
SCRIPTBINDS_TESTS.SOURCE_DIRS      := tests/lua
SCRIPTBINDS_TESTS.LIB_DIRS         :=  
SCRIPTBINDS_TESTS.LIBS             := commonlib scriptbinds lua zzip zlib scriptlib scriptbinds sg
SCRIPTBINDS_TESTS.COMPILER_CFLAGS  := 
SCRIPTBINDS_TESTS.COMPILER_DEFINES :=
