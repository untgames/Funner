###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := WINDOW_DRIVER_SOURCES WINDOW_DRIVER_TESTS

#Цель №1 - Window driver sources
WINDOW_DRIVER_SOURCES.TYPE             := static-lib
WINDOW_DRIVER_SOURCES.NAME             := window_driver
WINDOW_DRIVER_SOURCES.INCLUDE_DIRS     := include ../low_level/include ../../commonlib/include ../../xtl/include ../../systemlib/include
WINDOW_DRIVER_SOURCES.SOURCE_DIRS      := sources
WINDOW_DRIVER_SOURCES.LIB_DIRS         :=  
WINDOW_DRIVER_SOURCES.LIBS             := 
WINDOW_DRIVER_SOURCES.COMPILER_CFLAGS  :=
WINDOW_DRIVER_SOURCES.COMPILER_DEFINES := 

#Цель №2 - Window driver tests
WINDOW_DRIVER_TESTS.TYPE             := test-suite
WINDOW_DRIVER_TESTS.INCLUDE_DIRS     := include ../low_level/include ../../commonlib/include ../../xtl/include ../../systemlib/include
WINDOW_DRIVER_TESTS.SOURCE_DIRS      := tests
WINDOW_DRIVER_TESTS.LIB_DIRS         :=
WINDOW_DRIVER_TESTS.LIBS             := input_ll window_driver commonlib system user32
WINDOW_DRIVER_TESTS.DLLS             :=
WINDOW_DRIVER_TESTS.DLL_DIRS         :=
WINDOW_DRIVER_TESTS.COMPILER_CFLAGS  :=
WINDOW_DRIVER_TESTS.COMPILER_DEFINES :=
