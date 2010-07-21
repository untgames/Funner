###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PHYSICS.LOW_LEVEL.SOURCES PHYSICS.LOW_LEVEL.TESTS PHYSICS.LOW_LEVEL.INFO

#Цель Physics system sources
PHYSICS.LOW_LEVEL.SOURCES.TYPE        := static-lib
PHYSICS.LOW_LEVEL.SOURCES.NAME        := funner.physics.low_level
PHYSICS.LOW_LEVEL.SOURCES.SOURCE_DIRS := sources
PHYSICS.LOW_LEVEL.SOURCES.IMPORTS     := compile.physics.low_level compile.common

#Цель Physics system tests
PHYSICS.LOW_LEVEL.TESTS.TYPE        := test-suite
PHYSICS.LOW_LEVEL.TESTS.SOURCE_DIRS := tests
PHYSICS.LOW_LEVEL.TESTS.IMPORTS     := compile.physics.low_level link.physics.low_level

#Цель - сборка документации
PHYSICS.LOW_LEVEL.INFO.TYPE        := doxygen-info
PHYSICS.LOW_LEVEL.INFO.CHM_NAME    := funner.physics.low_level
PHYSICS.LOW_LEVEL.INFO.SOURCE_DIRS := include
PHYSICS.LOW_LEVEL.INFO.IMPORTS     := compile.physics.low_level
