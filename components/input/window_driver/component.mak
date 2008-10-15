###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.WINDOW_DRIVER.SOURCES INPUT.WINDOW_DRIVER.TESTS

#Цель №1 - Window driver sources
INPUT.WINDOW_DRIVER.SOURCES.TYPE             := static-lib
INPUT.WINDOW_DRIVER.SOURCES.NAME             := funner.input.window_driver
INPUT.WINDOW_DRIVER.SOURCES.INCLUDE_DIRS     := 
INPUT.WINDOW_DRIVER.SOURCES.SOURCE_DIRS      := sources
INPUT.WINDOW_DRIVER.SOURCES.LIB_DIRS         :=  
INPUT.WINDOW_DRIVER.SOURCES.LIBS             := 
INPUT.WINDOW_DRIVER.SOURCES.COMPILER_CFLAGS  :=
INPUT.WINDOW_DRIVER.SOURCES.COMPILER_DEFINES := 
INPUT.WINDOW_DRIVER.SOURCES.IMPORTS          := compile.static.mak ../manager/compile.static.mak ../../system/compile.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak

#Цель №2 - Window driver tests
INPUT.WINDOW_DRIVER.TESTS.TYPE             := test-suite
INPUT.WINDOW_DRIVER.TESTS.INCLUDE_DIRS     :=
INPUT.WINDOW_DRIVER.TESTS.SOURCE_DIRS      := tests
INPUT.WINDOW_DRIVER.TESTS.LIB_DIRS         :=
INPUT.WINDOW_DRIVER.TESTS.LIBS             :=
INPUT.WINDOW_DRIVER.TESTS.DLLS             :=
INPUT.WINDOW_DRIVER.TESTS.DLL_DIRS         :=
INPUT.WINDOW_DRIVER.TESTS.COMPILER_CFLAGS  :=
INPUT.WINDOW_DRIVER.TESTS.COMPILER_DEFINES :=
INPUT.WINDOW_DRIVER.TESTS.IMPORTS          := compile.static.mak link.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak
