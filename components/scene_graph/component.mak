###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SG_COMPONENTS SG_TESTS

#Цель №1 - SG components sources
SG_COMPONENTS.TYPE             := static-lib
SG_COMPONENTS.NAME             := scene_graph
SG_COMPONENTS.INCLUDE_DIRS     := ../bound_volumes/include
SG_COMPONENTS.SOURCE_DIRS      := sources/core sources/helpers
SG_COMPONENTS.LIB_DIRS         :=  
SG_COMPONENTS.LIBS             :=
SG_COMPONENTS.COMPILER_DEFINES :=  
SG_COMPONENTS.IMPORTS          := compile.static.mak ../common/compile.static.mak  
SG_COMPONENTS.msvc.COMPILER_CFLAGS  := -wd4355

#Цель №2 - SG tests
SG_TESTS.TYPE             := test-suite
SG_TESTS.INCLUDE_DIRS     :=
SG_TESTS.SOURCE_DIRS      := tests/core
SG_TESTS.LIB_DIRS         :=
SG_TESTS.LIBS             :=
SG_TESTS.COMPILER_CFLAGS  :=
SG_TESTS.COMPILER_DEFINES := 
SG_TESTS.IMPORTS          := link.static.mak compile.static.mak
