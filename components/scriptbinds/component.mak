###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SCRIPTBINDS_COMPONENTS SCRIPTBINDS_TESTS

#���� �1 - SCRIPTBINDS sources
SCRIPTBINDS_COMPONENTS.TYPE             := static-lib
SCRIPTBINDS_COMPONENTS.NAME             := scriptbinds
SCRIPTBINDS_COMPONENTS.INCLUDE_DIRS     := include
SCRIPTBINDS_COMPONENTS.SOURCE_DIRS      := sources
SCRIPTBINDS_COMPONENTS.LIB_DIRS         :=  
SCRIPTBINDS_COMPONENTS.LIBS             := 
SCRIPTBINDS_COMPONENTS.COMPILER_CFLAGS  := 
SCRIPTBINDS_COMPONENTS.COMPILER_DEFINES := 
SCRIPTBINDS_COMPONENTS.IMPORTS				  := ../script/exports.mak ../scenegraph/exports.mak

#���� �2 - SCRIPTBINDS tests
SCRIPTBINDS_TESTS.TYPE             := test-suite
SCRIPTBINDS_TESTS.INCLUDE_DIRS     := include
SCRIPTBINDS_TESTS.SOURCE_DIRS      := tests/lua
SCRIPTBINDS_TESTS.LIB_DIRS         :=  
SCRIPTBINDS_TESTS.LIBS             := scriptbinds
SCRIPTBINDS_TESTS.COMPILER_CFLAGS  := 
SCRIPTBINDS_TESTS.COMPILER_DEFINES :=
SCRIPTBINDS_TESTS.IMPORTS	  		   := ../script/exports.mak ../scenegraph/exports.mak
