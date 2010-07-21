###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := BOUND_VOLUMES.TESTS BOUND_VOLUMES.INFO

#Цель №1 - Bound volumes tests
BOUND_VOLUMES.TESTS.TYPE             := test-suite
BOUND_VOLUMES.TESTS.SOURCE_DIRS      := tests
BOUND_VOLUMES.TESTS.IMPORTS          := compile.math.bound_volumes

#Цель - сборка документации
BOUND_VOLUMES.INFO.TYPE        := doxygen-info
BOUND_VOLUMES.INFO.CHM_NAME    := funner.math.bound_volumes
BOUND_VOLUMES.INFO.SOURCE_DIRS := include
BOUND_VOLUMES.INFO.IMPORTS     := compile.math.bound_volumes
