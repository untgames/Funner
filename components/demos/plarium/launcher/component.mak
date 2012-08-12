###################################################################################################
#Определения и константы
###################################################################################################
TARGETS       := PLARIUM.LAUNCHER.SOURCES
TARGETS.win32 := PLARIUM.CLAUNCHER.SOURCES

#Цель - application
PLARIUM.LAUNCHER.SOURCES.NAME               := plarium-launcher
ifneq (,$(filter iphone,$(PROFILES)))
PLARIUM.LAUNCHER.SOURCES.TYPE               := static-lib
endif
ifneq (,$(filter android,$(PROFILES)))
PLARIUM.LAUNCHER.SOURCES.TYPE               := dynamic-lib
endif
ifneq (,$(filter win32,$(PROFILES)))
PLARIUM.LAUNCHER.SOURCES.TYPE               := application
endif
ifneq (,$(filter macosx,$(PROFILES)))
PLARIUM.LAUNCHER.SOURCES.TYPE               := application
endif
ifneq (,$(filter linux,$(PROFILES)))
PLARIUM.LAUNCHER.SOURCES.TYPE               := application
endif
PLARIUM.LAUNCHER.SOURCES.INCLUDE_DIRS       := include
PLARIUM.LAUNCHER.SOURCES.SOURCE_DIRS        := sources/shared
PLARIUM.LAUNCHER.SOURCES.macosx.LINK_FLAGS  := -framework Foundation -framework AppKit
PLARIUM.LAUNCHER.SOURCES.msvc.LINK_FLAGS    := -subsystem:windows -entry:mainCRTStartup
PLARIUM.LAUNCHER.SOURCES.win32.SOURCE_DIRS  := sources/platform/win32
PLARIUM.LAUNCHER.SOURCES.macosx.SOURCE_DIRS := sources/platform/macosx
PLARIUM.LAUNCHER.SOURCES.linux.LIBS         := dl
PLARIUM.LAUNCHER.SOURCES.linux.SOURCE_DIRS  := sources/platform/linux
PLARIUM.LAUNCHER.SOURCES.iphone.SOURCE_DIRS := sources/platform/iphone
PLARIUM.LAUNCHER.SOURCES.android.SOURCE_DIRS := sources/platform/android
PLARIUM.LAUNCHER.SOURCES.IMPORTS            := compile.engine.launcher compile.plarium.hsserver compile.plarium.system link.plarium.hsserver compile.plarium.sgi_stl
PLARIUM.LAUNCHER.SOURCES.android.IMPORTS    := link.engine.launcher
ifneq (,$(filter android,$(PROFILES)))
PLARIUM.LAUNCHER.SOURCES.LIBS               := funner.system #for duplicate JNI_OnLoad in OpenAL32 & system
endif

#Цель - console application
PLARIUM.CLAUNCHER.SOURCES.TYPE                := application
PLARIUM.CLAUNCHER.SOURCES.NAME                := plarium-clauncher
PLARIUM.CLAUNCHER.SOURCES.INCLUDE_DIRS        := include
PLARIUM.CLAUNCHER.SOURCES.IMPORTS             := $(PLARIUM.LAUNCHER.SOURCES.IMPORTS)
PLARIUM.CLAUNCHER.SOURCES.SOURCE_DIRS         := sources/shared
PLARIUM.CLAUNCHER.SOURCES.win32.SOURCE_DIRS   := sources/platform/win32
