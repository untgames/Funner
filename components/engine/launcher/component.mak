###################################################################################################
#Определения и константы
###################################################################################################
TARGETS        := ENGINE.FUNNER_SHARED_LIBRARY ENGINE.FUNNER_LIBRARY ENGINE.LAUNCHER.SOURCES
TARGETS.win32  := ENGINE.CLAUNCHER.SOURCES
TARGETS.macosx := ENGINE.CLAUNCHER.SOURCES
TARGETS.linux  := ENGINE.CLAUNCHER.SOURCES
TARGETS.win32  += ENGINE.CLAUNCHER.TESTS
TARGETS.iphone := ENGINE.FUNNER_LIBRARY_LIPO

#PROFILES += gles_win32 egl

LAUNCHER_VERSION_STRING := "Launcher version 1.0"

ENGINE.FUNNER_SHARED_LIBRARY.TYPE              := static-lib
ENGINE.FUNNER_SHARED_LIBRARY.NAME              := funner.engine.shared
ENGINE.FUNNER_SHARED_LIBRARY.SOURCE_DIRS       := sources/shared
ENGINE.FUNNER_SHARED_LIBRARY.INCLUDE_DIRS      := include
ENGINE.FUNNER_SHARED_LIBRARY.COMPILER_DEFINES  := LAUNCHER_VERSION='$(LAUNCHER_VERSION_STRING)' FUNNER_BUILD=1
ENGINE.FUNNER_SHARED_LIBRARY.IMPORTS           := compile.engine.core compile.common compile.system link.engine.launcher_shared
ENGINE.FUNNER_SHARED_LIBRARY.EXCLUDE_IMPORTS := link.common.auto_license_generator                                             
                                             
#Цель - сборка движка
ifeq (,$(filter no_dll,$(PROFILES)))
ENGINE.FUNNER_LIBRARY.TYPE                := dynamic-lib
endif
ifneq (,$(filter iphone,$(PROFILES)))
ENGINE.FUNNER_LIBRARY.TYPE                := fat-static-lib
endif
ifneq (,$(filter android,$(PROFILES)))
ENGINE.FUNNER_LIBRARY.TYPE                := ignore
endif
ifneq (,$(filter tabletos,$(PROFILES)))
ENGINE.FUNNER_LIBRARY.TYPE                := ignore
endif
ENGINE.FUNNER_LIBRARY.NAME                := funner
#ENGINE.FUNNER_LIBRARY.win32.LINK_FLAGS    := -noentry
ENGINE.FUNNER_LIBRARY.IMPORTS               := compile.engine.core compile.common compile.system link.engine.launcher
ENGINE.FUNNER_LIBRARY.EXCLUDE_IMPORTS       := link.common.auto_license_generator
ENGINE.FUNNER_LIBRARY.msvc_debug.LINK_FLAGS := -DEBUG
ENGINE.FUNNER_LIBRARY.LINK_INCLUDES         :=

#Цель - объединение библиотек для iPhone
ENGINE.FUNNER_LIBRARY_LIPO.NAME     := funner
ENGINE.FUNNER_LIBRARY_LIPO.TYPE     := lipo-lib
ENGINE.FUNNER_LIBRARY_LIPO.LIBS     := funner
ENGINE.FUNNER_LIBRARY_LIPO.PROFILES := iphone-device-armv6 iphone-device-armv7 iphone-simulator

#Цель - application
ifneq (,$(filter android,$(PROFILES)))
ENGINE.LAUNCHER.SOURCES.NAME                    := funner
ENGINE.LAUNCHER.SOURCES.TYPE                    := dynamic-lib
endif
ifneq (,$(filter iphone,$(PROFILES)))
ENGINE.LAUNCHER.SOURCES.TYPE                    := ignore
endif
ENGINE.LAUNCHER.SOURCES.NAME                    ?= launcher
ENGINE.LAUNCHER.SOURCES.TYPE                    ?= application
ENGINE.LAUNCHER.SOURCES.INCLUDE_DIRS            := include
ENGINE.LAUNCHER.SOURCES.EXECUTION_DIR           := sources
ENGINE.LAUNCHER.SOURCES.LINK_INCLUDES           :=
ENGINE.LAUNCHER.SOURCES.LIBS                    :=
ENGINE.LAUNCHER.SOURCES.macosx.LINK_FLAGS       := -framework Foundation -framework AppKit
ENGINE.LAUNCHER.SOURCES.iphone.LINK_FLAGS       := -framework OpenAL -framework UIKit -framework Foundation \
                                                   -framework OpenGLES -framework MediaPlayer -framework AVFoundation \
                                                   -framework AudioToolbox -framework QuartzCore -framework CoreGraphics \
                                                   -framework SystemConfiguration
ENGINE.LAUNCHER.SOURCES.msvc.LINK_FLAGS         := -subsystem:windows -entry:mainCRTStartup
ENGINE.LAUNCHER.SOURCES.win32.SOURCE_DIRS       := sources/platform/win32
ENGINE.LAUNCHER.SOURCES.macosx.SOURCE_DIRS      := sources/platform/macosx
ENGINE.LAUNCHER.SOURCES.iphone.SOURCE_DIRS      := sources/platform/iphone
ENGINE.LAUNCHER.SOURCES.iphone.LIBS             := funner
ENGINE.LAUNCHER.SOURCES.linux.SOURCE_DIRS       := sources/platform/linux
ENGINE.LAUNCHER.SOURCES.linux.LIBS              := dl
ifneq (,$(filter android,$(PROFILES)))
ENGINE.LAUNCHER.SOURCES.LIBS                    := funner.system #for duplicate JNI_OnLoad in OpenAL32 & system
endif
ENGINE.LAUNCHER.SOURCES.android.SOURCE_DIRS     := sources/platform/android
ENGINE.LAUNCHER.SOURCES.android.IMPORTS         := compile.engine.core link.engine.launcher compile.extern.android_ndk_profiler link.extern.android_ndk_profiler
ENGINE.LAUNCHER.SOURCES.android.EXCLUDE_IMPORTS := link.common.auto_license_generator
ENGINE.LAUNCHER.SOURCES.tabletos.SOURCE_DIRS     := sources/platform/tabletos
ENGINE.LAUNCHER.SOURCES.tabletos.IMPORTS         := compile.engine.core link.engine.launcher
ENGINE.LAUNCHER.SOURCES.tabletos.EXCLUDE_IMPORTS := link.common.auto_license_generator
ENGINE.LAUNCHER.SOURCES.tabletos.LINK_TOOL       := $(LINK_TOOL)
ifneq (,$(filter tabletos,$(PROFILES)))
ENGINE.LAUNCHER.SOURCES.LINK_TOOL                := $(SPACE)
endif

#Цель - console application
ENGINE.CLAUNCHER.SOURCES.TYPE                := application
ENGINE.CLAUNCHER.SOURCES.NAME                := clauncher
ENGINE.CLAUNCHER.SOURCES.INCLUDE_DIRS        := include
ENGINE.CLAUNCHER.SOURCES.EXECUTION_DIR       := sources
ENGINE.CLAUNCHER.SOURCES.IMPORTS             := $(ENGINE.LAUNCHER.SOURCES.IMPORTS)
ENGINE.CLAUNCHER.SOURCES.has_windows.IMPORTS := $(ENGINE.LAUNCHER.SOURCES.has_windows.IMPORTS)
ENGINE.CLAUNCHER.SOURCES.win32.IMPORTS       := $(ENGINE.LAUNCHER.SOURCES.win32.IMPORTS)
ENGINE.CLAUNCHER.SOURCES.win32.SOURCE_DIRS   := $(ENGINE.LAUNCHER.SOURCES.win32.SOURCE_DIRS)
ENGINE.CLAUNCHER.SOURCES.macosx.SOURCE_DIRS  := $(ENGINE.LAUNCHER.SOURCES.macosx.SOURCE_DIRS)
ENGINE.CLAUNCHER.SOURCES.linux.SOURCE_DIRS   := $(ENGINE.LAUNCHER.SOURCES.linux.SOURCE_DIRS)
ENGINE.CLAUNCHER.SOURCES.macosx.LINK_FLAGS   := $(ENGINE.LAUNCHER.SOURCES.macosx.LINK_FLAGS)
ENGINE.CLAUNCHER.SOURCES.linux.LIBS          := $(ENGINE.LAUNCHER.SOURCES.linux.LIBS)

#Цель - launcher tests
ENGINE.CLAUNCHER.TESTS.TYPE              := test-suite
ENGINE.CLAUNCHER.TESTS.SOURCE_DIRS       := tests
ENGINE.CLAUNCHER.TESTS.USED_APPLICATIONS := clauncher
