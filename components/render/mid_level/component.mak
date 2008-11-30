###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.MID_LEVEL.SOURCES RENDER.MID_LEVEL.TESTS

#Цель №1 - RENDER system sources
RENDER.MID_LEVEL.SOURCES.TYPE             := static-lib
RENDER.MID_LEVEL.SOURCES.NAME             := funner.render.mid_level
RENDER.MID_LEVEL.SOURCES.INCLUDE_DIRS     :=
RENDER.MID_LEVEL.SOURCES.SOURCE_DIRS      := sources
RENDER.MID_LEVEL.SOURCES.LIB_DIRS         :=  
RENDER.MID_LEVEL.SOURCES.LIBS             := 
RENDER.MID_LEVEL.SOURCES.COMPILER_CFLAGS  :=
RENDER.MID_LEVEL.SOURCES.COMPILER_DEFINES := 
RENDER.MID_LEVEL.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak ../../media/image/compile.static.mak

#Цель №2 - RENDER system tests
RENDER.MID_LEVEL.TESTS.TYPE             := test-suite
RENDER.MID_LEVEL.TESTS.INCLUDE_DIRS     := ../../xtl/include
RENDER.MID_LEVEL.TESTS.SOURCE_DIRS      := tests
RENDER.MID_LEVEL.TESTS.LIB_DIRS         :=
RENDER.MID_LEVEL.TESTS.LIBS             :=
RENDER.MID_LEVEL.TESTS.DLLS             :=
RENDER.MID_LEVEL.TESTS.DLL_DIRS         :=
RENDER.MID_LEVEL.TESTS.COMPILER_CFLAGS  :=
RENDER.MID_LEVEL.TESTS.IMPORTS          := compile.static.mak link.static.mak
