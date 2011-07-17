###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.LOW_LEVEL.SOURCES RENDER.LOW_LEVEL.UTILS RENDER.LOW_LEVEL.TESTS RENDER.LOW_LEVEL.INFO

#Цель - RENDER system sources
RENDER.LOW_LEVEL.SOURCES.TYPE               := static-lib
RENDER.LOW_LEVEL.SOURCES.NAME               := funner.render.low_level
RENDER.LOW_LEVEL.SOURCES.SOURCE_DIRS        := sources
RENDER.LOW_LEVEL.SOURCES.IMPORTS            := compile.render.low_level compile.common

#Цель - RENDER system tests
RENDER.LOW_LEVEL.TESTS.TYPE             := test-suite
RENDER.LOW_LEVEL.TESTS.SOURCE_DIRS      := tests
RENDER.LOW_LEVEL.TESTS.IMPORTS          := compile.render.low_level link.render.low_level

#Цель - RENDER system utils
RENDER.LOW_LEVEL.UTILS.TYPE             := test-suite
RENDER.LOW_LEVEL.UTILS.SOURCE_DIRS      := utils
RENDER.LOW_LEVEL.UTILS.COMPILER_DEFINES := __MYSTL_STANDALONE__
RENDER.LOW_LEVEL.UTILS.IMPORTS          := compile.render.low_level

#Цель - сборка документации
RENDER.LOW_LEVEL.INFO.TYPE        := doxygen-info
RENDER.LOW_LEVEL.INFO.CHM_NAME    := funner.render.low_level
RENDER.LOW_LEVEL.INFO.SOURCE_DIRS := include
RENDER.LOW_LEVEL.INFO.IMPORTS     := compile.render.low_level
