###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.VIDEO.CORE MEDIA.VIDEO.TESTS MEDIA.VIDEO.THEORA.SOURCES MEDIA.VIDEO.THEORA.TESTS

#Video sources
MEDIA.VIDEO.CORE.TYPE        := static-lib
MEDIA.VIDEO.CORE.NAME        := funner.media.video
MEDIA.VIDEO.CORE.SOURCE_DIRS := sources/core
MEDIA.VIDEO.CORE.IMPORTS     := compile.media.video compile.media.image

#Video tests
MEDIA.VIDEO.TESTS.TYPE        := test-suite
MEDIA.VIDEO.TESTS.SOURCE_DIRS := tests/core
MEDIA.VIDEO.TESTS.IMPORTS     := compile.media.video link.media.video compile.media.image

#Theora sources
MEDIA.VIDEO.THEORA.SOURCES.TYPE          := static-lib
MEDIA.VIDEO.THEORA.SOURCES.NAME          := funner.media.video.theora
MEDIA.VIDEO.THEORA.SOURCES.SOURCE_DIRS   := sources/theora
MEDIA.VIDEO.THEORA.SOURCES.IMPORTS       := compile.extern.theora compile.media.video compile.media.image

#Theora tests
MEDIA.VIDEO.THEORA.TESTS.TYPE        := test-suite
MEDIA.VIDEO.THEORA.TESTS.SOURCE_DIRS := tests/theora
MEDIA.VIDEO.THEORA.TESTS.IMPORTS     := compile.media.video compile.media.image link.media.video.theora
