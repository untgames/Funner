###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT_MANAGER_SOURCES INPUT_MANAGER_TESTS

#Цель №1 - INPUT manager sources
INPUT_MANAGER_SOURCES.TYPE             := static-lib
INPUT_MANAGER_SOURCES.NAME             := input_manager
INPUT_MANAGER_SOURCES.INCLUDE_DIRS     := include  ../low_level/include ../../commonlib/include ../../xtl/include
INPUT_MANAGER_SOURCES.SOURCE_DIRS      := sources
INPUT_MANAGER_SOURCES.LIB_DIRS         :=  
INPUT_MANAGER_SOURCES.LIBS             := 
INPUT_MANAGER_SOURCES.COMPILER_CFLAGS  :=
INPUT_MANAGER_SOURCES.COMPILER_DEFINES := 

#Цель №2 - INPUT manager tests
INPUT_MANAGER_TESTS.TYPE             := test-suite
INPUT_MANAGER_TESTS.INCLUDE_DIRS     := include ../low_level/include ../../xtl/include ../../commonlib/include
INPUT_MANAGER_TESTS.SOURCE_DIRS      := tests
INPUT_MANAGER_TESTS.LIB_DIRS         :=
INPUT_MANAGER_TESTS.LIBS             := commonlib input_ll input_manager zzip zlib
INPUT_MANAGER_TESTS.DLLS             :=
INPUT_MANAGER_TESTS.DLL_DIRS         :=
INPUT_MANAGER_TESTS.COMPILER_CFLAGS  :=
INPUT_MANAGER_TESTS.COMPILER_DEFINES :=
