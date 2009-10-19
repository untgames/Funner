TARGETS := MEDIA.PLAYERS.CORE MEDIA.PLAYERS.TESTS

#MediaPlayer core
MEDIA.PLAYERS.CORE.TYPE        := static-lib
MEDIA.PLAYERS.CORE.NAME        := funner.media.players.core
MEDIA.PLAYERS.CORE.SOURCE_DIRS := sources/core
MEDIA.PLAYERS.CORE.IMPORTS     := compile.media.players compile.common
MEDIA.PLAYERS.CORE.msvc.COMPILER_CFLAGS  := -wd4355

#MediaPlayer tests
MEDIA.PLAYERS.TESTS.TYPE        := test-suite
MEDIA.PLAYERS.TESTS.SOURCE_DIRS := tests/core
MEDIA.PLAYERS.TESTS.IMPORTS     := compile.media.players link.media.players.core compile.common
