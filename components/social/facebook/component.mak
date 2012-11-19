###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOCIAL.FACEBOOK.SOURCES SOCIAL.FACEBOOK.TESTS

#Цель - sources
SOCIAL.FACEBOOK.SOURCES.TYPE               := static-lib
SOCIAL.FACEBOOK.SOURCES.NAME               := funner.social.facebook
SOCIAL.FACEBOOK.SOURCES.INCLUDE_DIRS       += sources/platform
SOCIAL.FACEBOOK.SOURCES.SOURCE_DIRS        := sources/shared
SOCIAL.FACEBOOK.SOURCES.IMPORTS            := compile.social.core compile.system
SOCIAL.FACEBOOK.SOURCES.iphone.SOURCE_DIRS := sources/platform/ios

#Цель - tests
SOCIAL.FACEBOOK.TESTS.TYPE        := test-suite
SOCIAL.FACEBOOK.TESTS.SOURCE_DIRS := tests
SOCIAL.FACEBOOK.TESTS.IMPORTS			:= compile.social.core compile.common compile.system link.social.facebook
