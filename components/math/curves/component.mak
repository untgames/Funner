TARGETS := MATH.CURVES.SOURCES MATH.CURVES.TESTS MATH.CURVES.INFO

MATH.CURVES.SOURCES.TYPE        := static-lib
MATH.CURVES.SOURCES.NAME        := funner.math.curves
MATH.CURVES.SOURCES.SOURCE_DIRS := sources
MATH.CURVES.SOURCES.IMPORTS     := compile.math.curves

MATH.CURVES.TESTS.TYPE        := test-suite
MATH.CURVES.TESTS.SOURCE_DIRS := tests
MATH.CURVES.TESTS.IMPORTS     := compile.math.curves

MATH.CURVES.INFO.TYPE        := doxygen-info
MATH.CURVES.INFO.CHM_NAME    := funner.math.curves
MATH.CURVES.INFO.SOURCE_DIRS := include
MATH.CURVES.INFO.IMPORTS     := compile.math.curves
