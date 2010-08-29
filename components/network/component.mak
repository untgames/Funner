###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := NETWORK.SOURCES NETWORK.TESTS

#Цель №1 - System library sources
NETWORK.SOURCES.TYPE          := static-lib
NETWORK.SOURCES.NAME          := funner.network
NETWORK.SOURCES.INCLUDE_DIRS  := sources
NETWORK.SOURCES.SOURCE_DIRS   := sources/wrappers
NETWORK.SOURCES.IMPORTS       := compile.network compile.system compile.common

#Цель №2 - System library tests
NETWORK.TESTS.TYPE             := test-suite
NETWORK.TESTS.SOURCE_DIRS      := tests
NETWORK.TESTS.IMPORTS          := compile.network link.network
