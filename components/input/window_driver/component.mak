###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := WINDOW_DRIVER_SOURCES WINDOW_DRIVER_TESTS

#Цель №1 - Window driver sources
WINDOW_DRIVER_SOURCES.TYPE             := static-lib
WINDOW_DRIVER_SOURCES.NAME             := input.window_driver
WINDOW_DRIVER_SOURCES.INCLUDE_DIRS     := 
WINDOW_DRIVER_SOURCES.SOURCE_DIRS      := sources
WINDOW_DRIVER_SOURCES.LIB_DIRS         :=  
WINDOW_DRIVER_SOURCES.LIBS             := 
WINDOW_DRIVER_SOURCES.COMPILER_CFLAGS  :=
WINDOW_DRIVER_SOURCES.COMPILER_DEFINES := 
WINDOW_DRIVER_SOURCES.IMPORTS          := compile.static.mak ../manager/compile.static.mak ../../system/compile.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak

#Цель №2 - Window driver tests
WINDOW_DRIVER_TESTS.TYPE             := test-suite
WINDOW_DRIVER_TESTS.INCLUDE_DIRS     :=
WINDOW_DRIVER_TESTS.SOURCE_DIRS      := tests
WINDOW_DRIVER_TESTS.LIB_DIRS         :=
WINDOW_DRIVER_TESTS.LIBS             :=
WINDOW_DRIVER_TESTS.DLLS             :=
WINDOW_DRIVER_TESTS.DLL_DIRS         :=
WINDOW_DRIVER_TESTS.COMPILER_CFLAGS  :=
WINDOW_DRIVER_TESTS.COMPILER_DEFINES :=
WINDOW_DRIVER_TESTS.IMPORTS          := compile.static.mak link.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak
