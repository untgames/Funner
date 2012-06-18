###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PLARIUM.LAUNCHER.SOURCES

#Цель - application
PLARIUM.LAUNCHER.SOURCES.NAME               := plarium-launcher
PLARIUM.LAUNCHER.SOURCES.TYPE               := application
PLARIUM.LAUNCHER.SOURCES.INCLUDE_DIRS       := include
PLARIUM.LAUNCHER.SOURCES.SOURCE_DIRS        := sources/shared
PLARIUM.LAUNCHER.SOURCES.macosx.LINK_FLAGS := -framework Foundation -framework AppKit
PLARIUM.LAUNCHER.SOURCES.msvc.LINK_FLAGS    := -subsystem:windows -entry:mainCRTStartup
PLARIUM.LAUNCHER.SOURCES.win32.SOURCE_DIRS  := sources/platform/win32
PLARIUM.LAUNCHER.SOURCES.macosx.SOURCE_DIRS := sources/platform/macosx
PLARIUM.LAUNCHER.SOURCES.IMPORTS            := compile.engine.launcher compile.plarium.hsserver compile.plarium.system link.plarium.hsserver
