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
INPUT.WINDOW_DRIVER.SOURCES.IMPORTS            := compile.input.window_driver compile.input.manager compile.system compile.common compile.input.low_level

#Цель №2 - Window driver tests
INPUT.WINDOW_DRIVER.TESTS.TYPE        := test-suite
INPUT.WINDOW_DRIVER.TESTS.SOURCE_DIRS := tests
INPUT.WINDOW_DRIVER.TESTS.IMPORTS     := compile.input.window_driver link.input.window_driver compile.common compile.input.low_level compile.system

ifeq (,$(filter iphone,$(PROFILES)))
INPUT.WINDOW_DRIVER.SOURCES.SOURCE_DIRS += sources/default
endif