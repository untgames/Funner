###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.ADOBE.XFL.SOURCES MEDIA.ADOBE.XFL.TESTS

#Цель sources
MEDIA.ADOBE.XFL.SOURCES.TYPE        := static-lib
MEDIA.ADOBE.XFL.SOURCES.NAME        := funner.adobe.xfl
MEDIA.ADOBE.XFL.SOURCES.SOURCE_DIRS := sources
MEDIA.ADOBE.XFL.SOURCES.IMPORTS     := compile.media.adobe.xfl compile.media.shared

#Цель DAE loader tests
MEDIA.ADOBE.XFL.TESTS.TYPE        := test-suite
MEDIA.ADOBE.XFL.TESTS.SOURCE_DIRS := tests
MEDIA.ADOBE.XFL.TESTS.IMPORTS     := compile.media.adobe.xfl link.media.adobe.xfl
