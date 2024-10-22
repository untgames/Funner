###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.VIDEO.CORE MEDIA.VIDEO.THEORA.SOURCES MEDIA.VIDEO.TESTS MEDIA.VIDEO.INFO

#Video sources
MEDIA.VIDEO.CORE.TYPE        := static-lib
MEDIA.VIDEO.CORE.NAME        := funner.media.video
MEDIA.VIDEO.CORE.SOURCE_DIRS := sources/core
MEDIA.VIDEO.CORE.IMPORTS     := compile.media.video compile.media.image

#Theora sources
MEDIA.VIDEO.THEORA.SOURCES.TYPE          := static-lib
MEDIA.VIDEO.THEORA.SOURCES.NAME          := funner.media.video.theora
MEDIA.VIDEO.THEORA.SOURCES.SOURCE_DIRS   := sources/theora
MEDIA.VIDEO.THEORA.SOURCES.IMPORTS       := compile.extern.theora compile.media.video compile.media.image

#Video tests
MEDIA.VIDEO.TESTS.TYPE        := test-suite
MEDIA.VIDEO.TESTS.SOURCE_DIRS := tests/core tests/theora
MEDIA.VIDEO.TESTS.IMPORTS     := compile.media.video link.media.video compile.media.image link.media.video.theora

#Цель - сборка документации
MEDIA.VIDEO.INFO.TYPE        := doxygen-info
MEDIA.VIDEO.INFO.CHM_NAME    := funner.media.video
MEDIA.VIDEO.INFO.SOURCE_DIRS := include
MEDIA.VIDEO.INFO.IMPORTS     := compile.media.video
