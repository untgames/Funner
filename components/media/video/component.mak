###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.VIDEO.CORE MEDIA.VIDEO.THEORA.SOURCES MEDIA.VIDEO.GVD.SOURCES MEDIA.VIDEO.TESTS

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

#GVD sources
MEDIA.VIDEO.GVD.SOURCES.TYPE          := static-lib
MEDIA.VIDEO.GVD.SOURCES.NAME          := funner.media.video.gvd
MEDIA.VIDEO.GVD.SOURCES.SOURCE_DIRS   := sources/gvd
MEDIA.VIDEO.GVD.SOURCES.IMPORTS       := compile.media.video compile.media.image

#Video tests
MEDIA.VIDEO.TESTS.TYPE        := test-suite
MEDIA.VIDEO.TESTS.SOURCE_DIRS := tests/core tests/theora tests/gvd
MEDIA.VIDEO.TESTS.IMPORTS     := compile.media.video link.media.video compile.media.image link.media.video.theora link.media.video.gvd
