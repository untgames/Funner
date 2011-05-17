###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := BOUND_VOLUMES.SOURCES BOUND_VOLUMES.TESTS BOUND_VOLUMES.INFO

BOUND_VOLUMES.SOURCES.TYPE        := static-lib
BOUND_VOLUMES.SOURCES.NAME        := funner.math.bound_volumes
BOUND_VOLUMES.SOURCES.SOURCE_DIRS := sources
BOUND_VOLUMES.SOURCES.IMPORTS     := compile.math.bound_volumes compile.xtl

#Цель - Bound volumes tests
BOUND_VOLUMES.TESTS.TYPE        := test-suite
BOUND_VOLUMES.TESTS.SOURCE_DIRS := tests
BOUND_VOLUMES.TESTS.IMPORTS     := compile.math.bound_volumes link.math.bound_volumes

#Цель - сборка документации
BOUND_VOLUMES.INFO.TYPE        := doxygen-info
BOUND_VOLUMES.INFO.CHM_NAME    := funner.math.bound_volumes
BOUND_VOLUMES.INFO.SOURCE_DIRS := include
BOUND_VOLUMES.INFO.IMPORTS     := compile.math.bound_volumes
