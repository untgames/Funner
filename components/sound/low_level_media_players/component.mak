TARGETS := SOUND.LOW_LEVEL_MEDIA_PLAYERS.SOURCES SOUND.LOW_LEVEL_MEDIA_PLAYERS.TESTS

SOUND.LOW_LEVEL_MEDIA_PLAYERS.SOURCES.TYPE        := static-lib
SOUND.LOW_LEVEL_MEDIA_PLAYERS.SOURCES.NAME        := funner.sound.low_level_media_players
SOUND.LOW_LEVEL_MEDIA_PLAYERS.SOURCES.SOURCE_DIRS := sources
SOUND.LOW_LEVEL_MEDIA_PLAYERS.SOURCES.IMPORTS     := compile.sound.low_level compile.media.players compile.common

SOUND.LOW_LEVEL_MEDIA_PLAYERS.TESTS.TYPE        := test-suite
SOUND.LOW_LEVEL_MEDIA_PLAYERS.TESTS.SOURCE_DIRS := tests
SOUND.LOW_LEVEL_MEDIA_PLAYERS.TESTS.IMPORTS     := compile.media.players link.sound.low_level_media_players compile.system \
                                                   run.sound.low_level_media_players
