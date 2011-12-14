SCRIPTS.RUNTIME.SOURCES.TYPE        := lua-module
SCRIPTS.RUNTIME.SOURCES.NAME        := com.untgames.runtime
SCRIPTS.RUNTIME.SOURCES.SOURCE_DIRS := sources
TARGETS                             += SCRIPTS.RUNTIME.SOURCES

SCRIPTS.RUNTIME.TESTS.TYPE         := test-suite
SCRIPTS.RUNTIME.TESTS.SOURCE_DIRS  := tests
TARGETS                            += SCRIPTS.RUNTIME.TESTS
