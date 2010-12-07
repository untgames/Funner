###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.SOUND.SOURCES MEDIA.SOUND.DEFAULT_DECODERS MEDIA.SOUND.SNDDECL MEDIA.SOUND.TESTS MEDIA.SOUND.INFO

#Цель №1 - Sound sources
MEDIA.SOUND.SOURCES.TYPE        := static-lib
MEDIA.SOUND.SOURCES.NAME        := funner.media.sound
MEDIA.SOUND.SOURCES.SOURCE_DIRS := sources/core
MEDIA.SOUND.SOURCES.IMPORTS     := compile.media.sound compile.media.shared

#Цель №2 - Sound sources
MEDIA.SOUND.DEFAULT_DECODERS.TYPE             := static-lib
MEDIA.SOUND.DEFAULT_DECODERS.NAME             := funner.media.sound.default_decoders
MEDIA.SOUND.DEFAULT_DECODERS.SOURCE_DIRS      := sources/default_decoders
MEDIA.SOUND.DEFAULT_DECODERS.IMPORTS          := compile.media.sound compile.extern.vorbisfile
MEDIA.SOUND.DEFAULT_DECODERS.msvc.COMPILER_CFLAGS  := -wd4244

#Цель №3 - Sound sources
MEDIA.SOUND.SNDDECL.TYPE        := static-lib
MEDIA.SOUND.SNDDECL.NAME        := funner.media.sound.snddecl
MEDIA.SOUND.SNDDECL.SOURCE_DIRS := sources/snddecl
MEDIA.SOUND.SNDDECL.IMPORTS     := compile.media.sound

#Цель №4 - Sound tests
MEDIA.SOUND.TESTS.TYPE        := test-suite
MEDIA.SOUND.TESTS.SOURCE_DIRS := tests
MEDIA.SOUND.TESTS.IMPORTS     := compile.media.sound link.media.sound link.media.sound.default_decoders link.media.sound.snddecl

#Цель - сборка документации
MEDIA.SOUND.INFO.TYPE        := doxygen-info
MEDIA.SOUND.INFO.CHM_NAME    := funner.media.sound
MEDIA.SOUND.INFO.SOURCE_DIRS := include
MEDIA.SOUND.INFO.IMPORTS     := compile.media.sound
