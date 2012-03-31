###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.ANDROID_DRIVER.SOURCES INPUT.ANDROID_DRIVER.TESTS

#Цель №1
INPUT.ANDROID_DRIVER.SOURCES.TYPE        := static-lib
INPUT.ANDROID_DRIVER.SOURCES.NAME        := funner.input.android_driver
INPUT.ANDROID_DRIVER.SOURCES.SOURCE_DIRS := sources
INPUT.ANDROID_DRIVER.SOURCES.IMPORTS     := compile.input.low_level compile.system

#Цель №2
INPUT.ANDROID_DRIVER.TESTS.TYPE        := test-suite
INPUT.ANDROID_DRIVER.TESTS.SOURCE_DIRS := tests
INPUT.ANDROID_DRIVER.TESTS.IMPORTS     := link.input.android_driver compile.input.low_level
