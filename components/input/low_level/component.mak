###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.LOW_LEVEL.SOURCES INPUT.LOW_LEVEL.TESTS INPUT.LOW_LEVEL.INFO

#Цель №1 - INPUT system sources
INPUT.LOW_LEVEL.SOURCES.TYPE        := static-lib
INPUT.LOW_LEVEL.SOURCES.NAME        := funner.input.low_level
INPUT.LOW_LEVEL.SOURCES.SOURCE_DIRS := sources
INPUT.LOW_LEVEL.SOURCES.IMPORTS     := compile.input.low_level

#Цель №2 - INPUT system tests
INPUT.LOW_LEVEL.TESTS.TYPE        := test-suite
INPUT.LOW_LEVEL.TESTS.SOURCE_DIRS := tests
INPUT.LOW_LEVEL.TESTS.IMPORTS     := compile.input.low_level link.input.low_level

#Цель - сборка документации
INPUT.LOW_LEVEL.INFO.TYPE        := doxygen-info
INPUT.LOW_LEVEL.INFO.CHM_NAME    := funner.input.low_level
INPUT.LOW_LEVEL.INFO.SOURCE_DIRS := include
INPUT.LOW_LEVEL.INFO.IMPORTS     := compile.input.low_level
