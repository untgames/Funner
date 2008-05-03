###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER_LOW_LEVEL_SOURCES RENDER_LOW_LEVEL_UTILS RENDER_LOW_LEVEL_TESTS

#Цель №1 - RENDER system sources
RENDER_LOW_LEVEL_SOURCES.TYPE             := static-lib
RENDER_LOW_LEVEL_SOURCES.NAME             := render.low_level
RENDER_LOW_LEVEL_SOURCES.INCLUDE_DIRS     := include
RENDER_LOW_LEVEL_SOURCES.SOURCE_DIRS      := sources
RENDER_LOW_LEVEL_SOURCES.LIB_DIRS         :=  
RENDER_LOW_LEVEL_SOURCES.LIBS             := 
RENDER_LOW_LEVEL_SOURCES.COMPILER_CFLAGS  :=
RENDER_LOW_LEVEL_SOURCES.COMPILER_DEFINES := 
RENDER_LOW_LEVEL_SOURCES.IMPORTS          := ../../common/exports.static.mak

#Цель №2 - RENDER system tests
RENDER_LOW_LEVEL_TESTS.TYPE             := test-suite
RENDER_LOW_LEVEL_TESTS.INCLUDE_DIRS     :=
RENDER_LOW_LEVEL_TESTS.SOURCE_DIRS      := tests
RENDER_LOW_LEVEL_TESTS.LIB_DIRS         :=
RENDER_LOW_LEVEL_TESTS.LIBS             :=
RENDER_LOW_LEVEL_TESTS.DLLS             :=
RENDER_LOW_LEVEL_TESTS.DLL_DIRS         :=
RENDER_LOW_LEVEL_TESTS.COMPILER_CFLAGS  :=
RENDER_LOW_LEVEL_TESTS.IMPORTS          := exports.static.mak ../../common/exports.static.mak

#Цель №2 - RENDER system utils
RENDER_LOW_LEVEL_UTILS.TYPE             := test-suite
RENDER_LOW_LEVEL_UTILS.INCLUDE_DIRS     := include ../../xtl/include
RENDER_LOW_LEVEL_UTILS.SOURCE_DIRS      := utils
RENDER_LOW_LEVEL_UTILS.LIB_DIRS         :=
RENDER_LOW_LEVEL_UTILS.LIBS             := common
RENDER_LOW_LEVEL_UTILS.DLLS             :=
RENDER_LOW_LEVEL_UTILS.DLL_DIRS         :=
RENDER_LOW_LEVEL_UTILS.COMPILER_CFLAGS  :=
RENDER_LOW_LEVEL_UTILS.COMPILER_DEFINES := __MYSTL_STANDALONE__
