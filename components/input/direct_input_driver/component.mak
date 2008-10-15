###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.DIRECT_INPUT_DRIVER.SOURCES INPUT.DIRECT_INPUT_DRIVER.TESTS

#Цель №1 - Window driver sources
INPUT.DIRECT_INPUT_DRIVER.SOURCES.TYPE                 := static-lib
INPUT.DIRECT_INPUT_DRIVER.SOURCES.NAME                 := funner.input.direct_input_driver
INPUT.DIRECT_INPUT_DRIVER.SOURCES.INCLUDE_DIRS         := ../../../extern/dinputsdk/include
INPUT.DIRECT_INPUT_DRIVER.SOURCES.SOURCE_DIRS          := sources
INPUT.DIRECT_INPUT_DRIVER.SOURCES.LIB_DIRS             :=  
INPUT.DIRECT_INPUT_DRIVER.SOURCES.LIBS                 := 
INPUT.DIRECT_INPUT_DRIVER.SOURCES.COMPILER_CFLAGS      :=
INPUT.DIRECT_INPUT_DRIVER.SOURCES.COMPILER_DEFINES     := DIRECT_INPUT_KEYBOARD_DISABLED
INPUT.DIRECT_INPUT_DRIVER.SOURCES.IMPORTS              := ../manager/compile.static.mak ../../system/compile.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak
INPUT.DIRECT_INPUT_DRIVER.SOURCES.msvc.COMPILER_CFLAGS := -wd4503

#Цель №2 - Window driver tests
INPUT.DIRECT_INPUT_DRIVER.TESTS.TYPE             := test-suite
INPUT.DIRECT_INPUT_DRIVER.TESTS.INCLUDE_DIRS     :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.SOURCE_DIRS      := tests
INPUT.DIRECT_INPUT_DRIVER.TESTS.LIB_DIRS         := 
INPUT.DIRECT_INPUT_DRIVER.TESTS.LIBS             :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.DLLS             :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.DLL_DIRS         :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.COMPILER_CFLAGS  :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.COMPILER_DEFINES :=
INPUT.DIRECT_INPUT_DRIVER.TESTS.IMPORTS          := link.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak
