###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := STORE.CORE.SOURCES STORE.CORE.TESTS STORE.CORE.INFO

#Target - sources
STORE.CORE.SOURCES.TYPE        := static-lib
STORE.CORE.SOURCES.NAME        := funner.store.core
STORE.CORE.SOURCES.SOURCE_DIRS := sources
STORE.CORE.SOURCES.IMPORTS     := compile.store.core compile.common

#Target - tests
STORE.CORE.TESTS.TYPE        := test-suite
STORE.CORE.TESTS.SOURCE_DIRS := tests
STORE.CORE.TESTS.IMPORTS     := compile.store.core compile.common link.store.core

#Target - build documentation
STORE.CORE.INFO.TYPE        := doxygen-info
STORE.CORE.INFO.CHM_NAME    := funner.store.core
STORE.CORE.INFO.SOURCE_DIRS := include
STORE.CORE.INFO.IMPORTS     := compile.store.core
