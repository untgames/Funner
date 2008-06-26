###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.MANAGER.SOURCES INPUT.MANAGER.TESTS

#Цель №1 - INPUT manager sources
INPUT.MANAGER.SOURCES.TYPE             := static-lib
INPUT.MANAGER.SOURCES.NAME             := input.manager
INPUT.MANAGER.SOURCES.INCLUDE_DIRS     := 
INPUT.MANAGER.SOURCES.SOURCE_DIRS      := sources
INPUT.MANAGER.SOURCES.LIB_DIRS         :=  
INPUT.MANAGER.SOURCES.LIBS             := 
INPUT.MANAGER.SOURCES.COMPILER_CFLAGS  :=
INPUT.MANAGER.SOURCES.COMPILER_DEFINES := 
INPUT.MANAGER.SOURCES.IMPORTS          := compile.static.mak ../low_level/compile.static.mak ../../common/compile.static.mak

#Цель №2 - INPUT manager tests
INPUT.MANAGER.TESTS.TYPE             := test-suite
INPUT.MANAGER.TESTS.INCLUDE_DIRS     :=
INPUT.MANAGER.TESTS.SOURCE_DIRS      := tests
INPUT.MANAGER.TESTS.LIB_DIRS         :=
INPUT.MANAGER.TESTS.LIBS             :=
INPUT.MANAGER.TESTS.DLLS             :=
INPUT.MANAGER.TESTS.DLL_DIRS         :=
INPUT.MANAGER.TESTS.COMPILER_CFLAGS  :=
INPUT.MANAGER.TESTS.COMPILER_DEFINES :=
INPUT.MANAGER.TESTS.IMPORTS          := compile.static.mak link.static.mak keymap.link.static.mak keydet.link.static.mak \
                                        ../low_level/compile.static.mak ../../common/compile.static.mak ../../common/link.static.mak
