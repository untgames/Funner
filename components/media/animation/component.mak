###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.ANIMATION.SOURCES MEDIA.ANIMATION.TESTS

#Цель - sources
MEDIA.ANIMATION.SOURCES.TYPE        := static-lib
MEDIA.ANIMATION.SOURCES.NAME        := funner.media.animation
MEDIA.ANIMATION.SOURCES.SOURCE_DIRS := sources/core
MEDIA.ANIMATION.SOURCES.IMPORTS     := compile.media.animation compile.media.shared

#Цель - tests
MEDIA.ANIMATION.TESTS.TYPE        := test-suite
MEDIA.ANIMATION.TESTS.SOURCE_DIRS := tests/core
MEDIA.ANIMATION.TESTS.IMPORTS     := compile.media.animation link.media.animation

                                                                                       