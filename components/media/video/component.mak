###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.VIDEO.CORE MEDIA.VIDEO.TESTS

#Video sources
MEDIA.VIDEO.CORE.TYPE        := static-lib
MEDIA.VIDEO.CORE.NAME        := funner.media.video
MEDIA.VIDEO.CORE.SOURCE_DIRS := sources/core
MEDIA.VIDEO.CORE.IMPORTS     := compile.media.video compile.media.image

#Video tests
MEDIA.VIDEO.TESTS.TYPE        := test-suite
MEDIA.VIDEO.TESTS.SOURCE_DIRS := tests/core
MEDIA.VIDEO.TESTS.IMPORTS     := compile.media.video link.media.video compile.media.image link.media.image
