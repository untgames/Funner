###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SG_COMPONENTS SG_TESTS

#Цель №1 - SG components sources
SG_COMPONENTS.TYPE             := static-lib
SG_COMPONENTS.NAME             := sg
SG_COMPONENTS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include ../mathlib/include ../boundvolumes/include
SG_COMPONENTS.SOURCE_DIRS      := sources/core
SG_COMPONENTS.LIB_DIRS         :=  
SG_COMPONENTS.LIBS             :=
SG_COMPONENTS.COMPILER_CFLAGS  :=
SG_COMPONENTS.COMPILER_DEFINES := 

#Цель №2 - SG tests
SG_TESTS.TYPE             := test-suite
SG_TESTS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include ../mathlib/include ../boundvolumes/include
SG_TESTS.SOURCE_DIRS      := tests/core
SG_TESTS.LIB_DIRS         :=
SG_TESTS.LIBS             := sg commonlib
SG_TESTS.COMPILER_CFLAGS  :=
SG_TESTS.COMPILER_DEFINES := 
