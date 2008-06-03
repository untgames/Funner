###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT_MANAGER_SOURCES INPUT_MANAGER_TESTS

#Цель №1 - INPUT manager sources
INPUT_MANAGER_SOURCES.TYPE             := static-lib
INPUT_MANAGER_SOURCES.NAME             := input.manager
INPUT_MANAGER_SOURCES.INCLUDE_DIRS     := 
INPUT_MANAGER_SOURCES.SOURCE_DIRS      := sources
INPUT_MANAGER_SOURCES.LIB_DIRS         :=  
INPUT_MANAGER_SOURCES.LIBS             := 
INPUT_MANAGER_SOURCES.COMPILER_CFLAGS  :=
INPUT_MANAGER_SOURCES.COMPILER_DEFINES := 
INPUT_MANAGER_SOURCES.IMPORTS          := compile.static.mak ../low_level/compile.static.mak ../../common/compile.static.mak

#Цель №2 - INPUT manager tests
INPUT_MANAGER_TESTS.TYPE             := test-suite
INPUT_MANAGER_TESTS.INCLUDE_DIRS     :=
INPUT_MANAGER_TESTS.SOURCE_DIRS      := tests
INPUT_MANAGER_TESTS.LIB_DIRS         :=
INPUT_MANAGER_TESTS.LIBS             :=
INPUT_MANAGER_TESTS.DLLS             :=
INPUT_MANAGER_TESTS.DLL_DIRS         :=
INPUT_MANAGER_TESTS.COMPILER_CFLAGS  :=
INPUT_MANAGER_TESTS.COMPILER_DEFINES :=
INPUT_MANAGER_TESTS.IMPORTS          := compile.static.mak link.static.mak keymap.link.static.mak keydet.link.static.mak \
                                        ../low_level/compile.static.mak ../../common/compile.static.mak ../../common/link.static.mak
