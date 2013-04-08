###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.LOW_LEVEL_HELPERS.SOURCES RENDER.LOW_LEVEL_HELPERS.TESTS

#Цель - RENDER system sources
RENDER.LOW_LEVEL_HELPERS.SOURCES.TYPE               := static-lib
RENDER.LOW_LEVEL_HELPERS.SOURCES.NAME               := funner.render.low_level.helpers
RENDER.LOW_LEVEL_HELPERS.SOURCES.SOURCE_DIRS        := sources
RENDER.LOW_LEVEL_HELPERS.SOURCES.IMPORTS            := compile.render.low_level.helpers compile.common

#Цель - RENDER system tests
RENDER.LOW_LEVEL_HELPERS.TESTS.TYPE             := test-suite
RENDER.LOW_LEVEL_HELPERS.TESTS.SOURCE_DIRS      := tests
RENDER.LOW_LEVEL_HELPERS.TESTS.IMPORTS          := compile.render.low_level.helpers link.render.low_level.helpers
