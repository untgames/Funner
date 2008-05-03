###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := SG_COMPONENTS SG_TESTS

#���� �1 - SG components sources
SG_COMPONENTS.TYPE             := static-lib
SG_COMPONENTS.NAME             := scene_graph
SG_COMPONENTS.INCLUDE_DIRS     := include ../bound_volumes/include
SG_COMPONENTS.SOURCE_DIRS      := sources/core sources/helpers
SG_COMPONENTS.LIB_DIRS         :=  
SG_COMPONENTS.LIBS             :=
SG_COMPONENTS.COMPILER_CFLAGS  := -wd4355
SG_COMPONENTS.COMPILER_DEFINES :=  
SG_COMPONENTS.IMPORTS          := ../common/exports.static.mak  

#���� �2 - SG tests
SG_TESTS.TYPE             := test-suite
SG_TESTS.INCLUDE_DIRS     :=
SG_TESTS.SOURCE_DIRS      := tests/core
SG_TESTS.LIB_DIRS         :=
SG_TESTS.LIBS             :=
SG_TESTS.COMPILER_CFLAGS  :=
SG_TESTS.COMPILER_DEFINES := 
SG_TESTS.IMPORTS          := exports.static.mak
