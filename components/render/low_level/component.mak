###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.LOW_LEVEL.SOURCES RENDER.LOW_LEVEL.UTILS RENDER.LOW_LEVEL.TESTS

#Цель №1 - RENDER system sources
RENDER.LOW_LEVEL.SOURCES.TYPE             := static-lib
RENDER.LOW_LEVEL.SOURCES.NAME             := render.low_level
RENDER.LOW_LEVEL.SOURCES.INCLUDE_DIRS     :=
RENDER.LOW_LEVEL.SOURCES.SOURCE_DIRS      := sources
RENDER.LOW_LEVEL.SOURCES.LIB_DIRS         :=  
RENDER.LOW_LEVEL.SOURCES.LIBS             := 
RENDER.LOW_LEVEL.SOURCES.COMPILER_CFLAGS  :=
RENDER.LOW_LEVEL.SOURCES.COMPILER_DEFINES := 
RENDER.LOW_LEVEL.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak

#Цель №2 - RENDER system tests
RENDER.LOW_LEVEL.TESTS.TYPE             := test-suite
RENDER.LOW_LEVEL.TESTS.INCLUDE_DIRS     :=
RENDER.LOW_LEVEL.TESTS.SOURCE_DIRS      := tests
RENDER.LOW_LEVEL.TESTS.LIB_DIRS         :=
RENDER.LOW_LEVEL.TESTS.LIBS             :=
RENDER.LOW_LEVEL.TESTS.DLLS             :=
RENDER.LOW_LEVEL.TESTS.DLL_DIRS         :=
RENDER.LOW_LEVEL.TESTS.COMPILER_CFLAGS  :=
RENDER.LOW_LEVEL.TESTS.IMPORTS          := compile.static.mak link.static.mak

#Цель №2 - RENDER system utils
RENDER.LOW_LEVEL.UTILS.TYPE             := test-suite
RENDER.LOW_LEVEL.UTILS.INCLUDE_DIRS     :=
RENDER.LOW_LEVEL.UTILS.SOURCE_DIRS      := utils
RENDER.LOW_LEVEL.UTILS.LIB_DIRS         :=
RENDER.LOW_LEVEL.UTILS.LIBS             :=
RENDER.LOW_LEVEL.UTILS.DLLS             :=
RENDER.LOW_LEVEL.UTILS.DLL_DIRS         :=
RENDER.LOW_LEVEL.UTILS.COMPILER_CFLAGS  :=
RENDER.LOW_LEVEL.UTILS.COMPILER_DEFINES := __MYSTL_STANDALONE__
RENDER.LOW_LEVEL.UTILS.IMPORTS          := compile.static.mak

