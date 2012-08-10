TARGETS += DEBUG.SOURCES
TARGETS += DEBUG.TESTS

DEBUG.SOURCES.TYPE        := static-lib
DEBUG.SOURCES.NAME        := funner.debug
DEBUG.SOURCES.SOURCE_DIRS := sources/map_file/core sources/map_file/parsers/gcc
DEBUG.SOURCES.IMPORTS     := compile.debug

DEBUG.TESTS.TYPE        := test-suite
DEBUG.TESTS.SOURCE_DIRS := tests/map_file
DEBUG.TESTS.IMPORTS     := compile.debug link.debug
