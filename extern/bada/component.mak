TARGETS := EXTERN.BADA.SOURCES EXTERN.BADA.TESTS

EXTERN.BADA.SOURCES.TYPE        := static-lib
EXTERN.BADA.SOURCES.NAME        := funner.extern.bada
EXTERN.BADA.SOURCES.SOURCE_DIRS := sources
EXTERN.BADA.SOURCES.bada_simulator.SOURCE_DIRS := sources/simulator

EXTERN.BADA.TESTS.TYPE        := test-suite
EXTERN.BADA.TESTS.SOURCE_DIRS := tests
EXTERN.BADA.TESTS.LIBS        := funner.extern.bada