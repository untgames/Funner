###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.IPHONE_DRIVER.SOURCES INPUT.IPHONE_DRIVER.TESTS

#Цель №1
INPUT.IPHONE_DRIVER.SOURCES.TYPE        := static-lib
INPUT.IPHONE_DRIVER.SOURCES.NAME        := funner.input.iphone_driver
INPUT.IPHONE_DRIVER.SOURCES.SOURCE_DIRS := sources
INPUT.IPHONE_DRIVER.SOURCES.IMPORTS     := compile.input.low_level compile.system

#Цель №2
INPUT.IPHONE_DRIVER.TESTS.TYPE        := test-suite
INPUT.IPHONE_DRIVER.TESTS.SOURCE_DIRS := tests
INPUT.IPHONE_DRIVER.TESTS.IMPORTS     := link.input.iphone_driver compile.input.low_level
