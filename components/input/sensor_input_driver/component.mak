###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.SENSOR_INPUT_DRIVER.SOURCES INPUT.SENSOR_INPUT_DRIVER.TESTS

#Цель №1 - Window driver sources
INPUT.SENSOR_INPUT_DRIVER.SOURCES.TYPE                 := static-lib
INPUT.SENSOR_INPUT_DRIVER.SOURCES.NAME                 := funner.input.sensor_input_driver
INPUT.SENSOR_INPUT_DRIVER.SOURCES.INCLUDE_DIRS         :=
INPUT.SENSOR_INPUT_DRIVER.SOURCES.SOURCE_DIRS          := sources
INPUT.SENSOR_INPUT_DRIVER.SOURCES.LIB_DIRS             :=  
INPUT.SENSOR_INPUT_DRIVER.SOURCES.LIBS                 := 
INPUT.SENSOR_INPUT_DRIVER.SOURCES.COMPILER_CFLAGS      :=
INPUT.SENSOR_INPUT_DRIVER.SOURCES.COMPILER_DEFINES     :=
INPUT.SENSOR_INPUT_DRIVER.SOURCES.IMPORTS              := compile.input.manager compile.system compile.input.low_level
INPUT.SENSOR_INPUT_DRIVER.SOURCES.msvc.COMPILER_CFLAGS := -wd4503

#Цель №2 - Window driver tests
INPUT.SENSOR_INPUT_DRIVER.TESTS.TYPE             := test-suite
INPUT.SENSOR_INPUT_DRIVER.TESTS.INCLUDE_DIRS     :=
INPUT.SENSOR_INPUT_DRIVER.TESTS.SOURCE_DIRS      := tests
INPUT.SENSOR_INPUT_DRIVER.TESTS.LIB_DIRS         := 
INPUT.SENSOR_INPUT_DRIVER.TESTS.LIBS             :=
INPUT.SENSOR_INPUT_DRIVER.TESTS.DLLS             :=
INPUT.SENSOR_INPUT_DRIVER.TESTS.DLL_DIRS         :=
INPUT.SENSOR_INPUT_DRIVER.TESTS.COMPILER_CFLAGS  :=
INPUT.SENSOR_INPUT_DRIVER.TESTS.COMPILER_DEFINES :=
INPUT.SENSOR_INPUT_DRIVER.TESTS.IMPORTS          := link.input.sensor_input_driver compile.input.low_level compile.system
