###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PHYSICS.LOW_LEVEL.SOURCES PHYSICS.LOW_LEVEL.TESTS

#Цель Physics system sources
PHYSICS.LOW_LEVEL.SOURCES.TYPE        := static-lib
PHYSICS.LOW_LEVEL.SOURCES.NAME        := funner.physics.low_level
PHYSICS.LOW_LEVEL.SOURCES.SOURCE_DIRS := sources
PHYSICS.LOW_LEVEL.SOURCES.IMPORTS     := compile.physics.low_level compile.common

#Цель Physics system tests
PHYSICS.LOW_LEVEL.TESTS.TYPE        := test-suite
PHYSICS.LOW_LEVEL.TESTS.SOURCE_DIRS := tests
PHYSICS.LOW_LEVEL.TESTS.IMPORTS     := compile.physics.low_level link.physics.low_level
