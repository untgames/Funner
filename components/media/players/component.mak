TARGETS := MEDIA.PLAYERS.CORE

#MediaPlayer core
MEDIA.PLAYERS.CORE.TYPE        := static-lib
MEDIA.PLAYERS.CORE.NAME        := funner.media.players.core
MEDIA.PLAYERS.CORE.SOURCE_DIRS := sources/core
MEDIA.PLAYERS.CORE.IMPORTS     := compile.media.players compile.common
MEDIA.PLAYERS.CORE.msvc.COMPILER_CFLAGS  := -wd4355