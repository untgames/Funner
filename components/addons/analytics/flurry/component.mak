###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ANALYTICS.FLURRY.SOURCES ANALYTICS.FLURRY.TESTS  

#Цель sources
ANALYTICS.FLURRY.SOURCES.TYPE               := static-lib
ANALYTICS.FLURRY.SOURCES.NAME               := funner.analytics.flurry
ANALYTICS.FLURRY.SOURCES.INCLUDE_DIRS       := include sources/platform
ANALYTICS.FLURRY.SOURCES.SOURCE_DIRS        := sources/shared sources/platform/default
ANALYTICS.FLURRY.SOURCES.iphone.SOURCE_DIRS := sources/platform/ios
ANALYTICS.FLURRY.SOURCES.IMPORTS            := compile.common
ANALYTICS.FLURRY.SOURCES.iphone.IMPORTS     := compile.extern.flurry compile.system

#Цель - CommonLib tests
ANALYTICS.FLURRY.TESTS.TYPE           := test-suite
ANALYTICS.FLURRY.TESTS.SOURCE_DIRS    := tests
ANALYTICS.FLURRY.TESTS.IMPORTS        := compile.analytics.flurry link.analytics.flurry
ANALYTICS.FLURRY.TESTS.iphone.IMPORTS := link.extern.flurry
