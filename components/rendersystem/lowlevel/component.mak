###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER_LOW_LEVEL_SOURCES RENDER_LOW_LEVEL_TESTS

#Цель №1 - RENDER system sources
RENDER_LOW_LEVEL_SOURCES.TYPE             := static-lib
RENDER_LOW_LEVEL_SOURCES.NAME             := render_ll
RENDER_LOW_LEVEL_SOURCES.INCLUDE_DIRS     := include ../../commonlib/include ../../xtl/include
RENDER_LOW_LEVEL_SOURCES.SOURCE_DIRS      := sources
RENDER_LOW_LEVEL_SOURCES.LIB_DIRS         :=  
RENDER_LOW_LEVEL_SOURCES.LIBS             := 
RENDER_LOW_LEVEL_SOURCES.COMPILER_CFLAGS  :=
RENDER_LOW_LEVEL_SOURCES.COMPILER_DEFINES := 

#Цель №2 - RENDER system tests
RENDER_LOW_LEVEL_TESTS.TYPE             := test-suite
RENDER_LOW_LEVEL_TESTS.INCLUDE_DIRS     := include ../../xtl/include ../../commonlib/include
RENDER_LOW_LEVEL_TESTS.SOURCE_DIRS      := tests
RENDER_LOW_LEVEL_TESTS.LIB_DIRS         :=
RENDER_LOW_LEVEL_TESTS.LIBS             := commonlib render-ll zzip zlib
RENDER_LOW_LEVEL_TESTS.DLLS             :=
RENDER_LOW_LEVEL_TESTS.DLL_DIRS         :=
RENDER_LOW_LEVEL_TESTS.COMPILER_CFLAGS  :=
RENDER_LOW_LEVEL_TESTS.COMPILER_DEFINES :=
