###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.WINDOW_DRIVER.SOURCES INPUT.WINDOW_DRIVER.TESTS

#Цель №1 - Window driver sources
INPUT.WINDOW_DRIVER.SOURCES.TYPE               := static-lib
INPUT.WINDOW_DRIVER.SOURCES.NAME               := funner.input.window_driver
INPUT.WINDOW_DRIVER.SOURCES.INCLUDE_DIRS       := sources
INPUT.WINDOW_DRIVER.SOURCES.SOURCE_DIRS        := sources/shared
INPUT.WINDOW_DRIVER.SOURCES.iphone.SOURCE_DIRS := sources/iphone
INPUT.WINDOW_DRIVER.SOURCES.IMPORTS            := compile.static.mak ../manager/compile.static.mak ../../system/compile.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak

#Цель №2 - Window driver tests
INPUT.WINDOW_DRIVER.TESTS.TYPE        := test-suite
INPUT.WINDOW_DRIVER.TESTS.SOURCE_DIRS := tests
INPUT.WINDOW_DRIVER.TESTS.IMPORTS     := compile.static.mak link.static.mak ../../common/compile.static.mak ../low_level/compile.static.mak ../../system/compile.static.mak

ifeq (,$(filter iphone,$(PROFILES)))
INPUT.WINDOW_DRIVER.SOURCES.SOURCE_DIRS += sources/default
endif