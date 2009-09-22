###################################################################################################
#Определени¤ и константы
###################################################################################################
TARGETS := SCRIPT.CORE.SOURCES SCRIPT.CORE.TESTS

#Цель - SCRIPT_CORE sources
SCRIPT.CORE.SOURCES.TYPE        := static-lib
SCRIPT.CORE.SOURCES.NAME        := funner.script.core
SCRIPT.CORE.SOURCES.SOURCE_DIRS := sources
SCRIPT.CORE.SOURCES.IMPORTS			:= compile.script.core compile.common

#Цель - SCRIPT_CORE tests
SCRIPT.CORE.TESTS.TYPE        := test-suite
SCRIPT.CORE.TESTS.SOURCE_DIRS := tests
SCRIPT.CORE.TESTS.IMPORTS			:= compile.script.core link.script.core compile.common
