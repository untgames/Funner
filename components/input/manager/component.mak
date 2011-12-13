###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.MANAGER.SOURCES INPUT.MANAGER.TESTS INPUT.MANAGER.INFO

#Цель №1 - INPUT manager sources
INPUT.MANAGER.SOURCES.TYPE        := static-lib
INPUT.MANAGER.SOURCES.NAME        := funner.input.manager
INPUT.MANAGER.SOURCES.SOURCE_DIRS := sources
INPUT.MANAGER.SOURCES.IMPORTS     := compile.input.manager compile.input.low_level compile.common

#Цель №2 - INPUT manager tests
INPUT.MANAGER.TESTS.TYPE        := test-suite
INPUT.MANAGER.TESTS.SOURCE_DIRS := tests
INPUT.MANAGER.TESTS.IMPORTS     := compile.input.manager link.input.manager link.input.manager.xkeymap link.input.manager.xkeydet \
                                   link.input.manager.xkeyreg compile.input.low_level compile.common link.common

#Цель - сборка документации
INPUT.MANAGER.INFO.TYPE        := doxygen-info
INPUT.MANAGER.INFO.CHM_NAME    := funner.input.manager
INPUT.MANAGER.INFO.SOURCE_DIRS := include
INPUT.MANAGER.INFO.IMPORTS     := compile.input.manager
