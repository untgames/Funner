###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := BOUND_VOLUMES_TESTS

#Цель №1 - Bound volumes tests
BOUND_VOLUMES_TESTS.TYPE             := test-suite
BOUND_VOLUMES_TESTS.SOURCE_DIRS      := tests
BOUND_VOLUMES_TESTS.IMPORTS          := compile.math.bound_volumes
