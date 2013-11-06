###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ANALYTICS.FLURRY.SOURCES ANALYTICS.FLURRY.TESTS  

#Цель sources
ANALYTICS.FLURRY.SOURCES.TYPE         := static-lib
ANALYTICS.FLURRY.SOURCES.NAME         := funner.analytics.flurry
ANALYTICS.FLURRY.SOURCES.INCLUDE_DIRS := include sources/platform
ANALYTICS.FLURRY.SOURCES.SOURCE_DIRS  := sources/shared sources/platform/default
ANALYTICS.FLURRY.SOURCES.IMPORTS      := compile.common

#Цель - CommonLib tests
ANALYTICS.FLURRY.TESTS.TYPE        := test-suite
ANALYTICS.FLURRY.TESTS.SOURCE_DIRS := tests
ANALYTICS.FLURRY.TESTS.IMPORTS     := compile.analytics.flurry link.analytics.flurry
