###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.ADOBE.XFL.SOURCES MEDIA.ADOBE.XFL.LOADER.SOURCES MEDIA.ADOBE.XFL.TESTS

#Цель sources
MEDIA.ADOBE.XFL.SOURCES.TYPE        := static-lib
MEDIA.ADOBE.XFL.SOURCES.NAME        := funner.media.adobe.xfl
MEDIA.ADOBE.XFL.SOURCES.SOURCE_DIRS := sources/core
MEDIA.ADOBE.XFL.SOURCES.IMPORTS     := compile.media.adobe.xfl compile.media.shared

#Цель loader sources
MEDIA.ADOBE.XFL.LOADER.SOURCES.TYPE        := static-lib
MEDIA.ADOBE.XFL.LOADER.SOURCES.NAME        := funner.media.adobe.xfl.loader
MEDIA.ADOBE.XFL.LOADER.SOURCES.SOURCE_DIRS := sources/xfl_loader
MEDIA.ADOBE.XFL.LOADER.SOURCES.IMPORTS     := compile.media.adobe.xfl

#Цель XFL loader tests
MEDIA.ADOBE.XFL.TESTS.TYPE        := test-suite
MEDIA.ADOBE.XFL.TESTS.SOURCE_DIRS := tests
MEDIA.ADOBE.XFL.TESTS.IMPORTS     := compile.media.adobe.xfl link.media.adobe.xfl.loader
