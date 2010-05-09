###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.ANIMATION.SOURCES MEDIA.ANIMATION.XANIM.SOURCES MEDIA.ANIMATION.TESTS MEDIA.ANIMATION.XANIM.TESTS

#Цель - sources
MEDIA.ANIMATION.SOURCES.TYPE        := static-lib
MEDIA.ANIMATION.SOURCES.NAME        := funner.media.animation
MEDIA.ANIMATION.SOURCES.SOURCE_DIRS := sources/core
MEDIA.ANIMATION.SOURCES.IMPORTS     := compile.media.animation compile.media.shared

#Цель - xanim serializer sources
MEDIA.ANIMATION.XANIM.SOURCES.TYPE        := static-lib
MEDIA.ANIMATION.XANIM.SOURCES.NAME        := funner.media.animation.xanim
MEDIA.ANIMATION.XANIM.SOURCES.SOURCE_DIRS := sources/xanim
MEDIA.ANIMATION.XANIM.SOURCES.IMPORTS     := compile.media.animation compile.math.curves 

#Цель - tests
MEDIA.ANIMATION.TESTS.TYPE        := test-suite
MEDIA.ANIMATION.TESTS.SOURCE_DIRS := tests/core
MEDIA.ANIMATION.TESTS.IMPORTS     := compile.media.animation link.media.animation

#Цель - xanim serializer tests
MEDIA.ANIMATION.XANIM.TESTS.TYPE        := test-suite
MEDIA.ANIMATION.XANIM.TESTS.SOURCE_DIRS := tests/xanim
MEDIA.ANIMATION.XANIM.TESTS.IMPORTS     := compile.media.animation compile.math.curves link.media.animation.xanim
