###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SOURCES ENGINE.TESTS

#Цель - CLIENTLib sources
ENGINE.SOURCES.TYPE        := static-lib
ENGINE.SOURCES.NAME        := funner.engine.core
ENGINE.SOURCES.SOURCE_DIRS := sources
ENGINE.SOURCES.IMPORTS     := compile.engine.core

#Цель - CLIENTLib tests
ENGINE.TESTS.TYPE        := test-suite
ENGINE.TESTS.SOURCE_DIRS := tests
ENGINE.TESTS.IMPORTS     := compile.engine.core link.engine.core link.common.xml
