###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := NETWORK.SOURCES NETWORK.TESTS

#Цель №1 - System library sources
NETWORK.SOURCES.TYPE               := static-lib
NETWORK.SOURCES.NAME               := funner.network
NETWORK.SOURCES.INCLUDE_DIRS       := sources
NETWORK.SOURCES.SOURCE_DIRS        := sources/wrappers
NETWORK.SOURCES.unistd.SOURCE_DIRS := sources/platform/unistd
NETWORK.SOURCES.win32.SOURCE_DIRS  := sources/platform/win32 sources/platform/curl
NETWORK.SOURCES.IMPORTS            := compile.network compile.system compile.common compile.extern.curl

#Цель №2 - System library tests
NETWORK.TESTS.TYPE        := test-suite
NETWORK.TESTS.SOURCE_DIRS := tests/wrappers
NETWORK.TESTS.IMPORTS     := compile.network link.network compile.common
