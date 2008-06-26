###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER_MID_LEVEL_SOURCES RENDER_MID_LEVEL_TESTS

#Цель №1 - RENDER system sources
RENDER_MID_LEVEL_SOURCES.TYPE             := static-lib
RENDER_MID_LEVEL_SOURCES.NAME             := render.mid_level
RENDER_MID_LEVEL_SOURCES.INCLUDE_DIRS     :=
RENDER_MID_LEVEL_SOURCES.SOURCE_DIRS      := sources
RENDER_MID_LEVEL_SOURCES.LIB_DIRS         :=  
RENDER_MID_LEVEL_SOURCES.LIBS             := 
RENDER_MID_LEVEL_SOURCES.COMPILER_CFLAGS  :=
RENDER_MID_LEVEL_SOURCES.COMPILER_DEFINES := 
RENDER_MID_LEVEL_SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak

#Цель №2 - RENDER system tests
RENDER_MID_LEVEL_TESTS.TYPE             := test-suite
RENDER_MID_LEVEL_TESTS.INCLUDE_DIRS     :=
RENDER_MID_LEVEL_TESTS.SOURCE_DIRS      := tests
RENDER_MID_LEVEL_TESTS.LIB_DIRS         :=
RENDER_MID_LEVEL_TESTS.LIBS             :=
RENDER_MID_LEVEL_TESTS.DLLS             :=
RENDER_MID_LEVEL_TESTS.DLL_DIRS         :=
RENDER_MID_LEVEL_TESTS.COMPILER_CFLAGS  :=
RENDER_MID_LEVEL_TESTS.IMPORTS          := compile.static.mak link.static.mak
