###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.THEORA.SOURCES EXTERN.THEORA.TESTS

#Цель - THEORA sources
EXTERN.THEORA.SOURCES.TYPE                := static-lib
EXTERN.THEORA.SOURCES.NAME                := funner.extern.theora
EXTERN.THEORA.SOURCES.SOURCE_DIRS         := sources sources/x86
EXTERN.THEORA.SOURCES.IMPORTS             := compile.extern.theora
EXTERN.THEORA.SOURCES.g++.COMPILER_CFLAGS := --no-warn

#Цель - THEORA tests
EXTERN.THEORA.TESTS.TYPE                := test-suite
EXTERN.THEORA.TESTS.SOURCE_DIRS         := tests
EXTERN.THEORA.TESTS.IMPORTS             := compile.extern.theora link.extern.theora
EXTERN.THEORA.TESTS.g++.COMPILER_CFLAGS := --no-warn
