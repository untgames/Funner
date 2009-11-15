###################################################################################################
#Определения и константы
###################################################################################################
TARGETS        := ENGINE.FUNNER_SHARED_LIBRARY ENGINE.FUNNER_LIBRARY ENGINE.LAUNCHER.SOURCES ENGINE.LAUNCHER.SDK
TARGETS.win32  := ENGINE.CLAUNCHER.SOURCES
TARGETS.iphone := ENGINE.FUNNER_LIBRARY_LIPO

#PROFILES += gles_win32 egl

LAUNCHER_VERSION_STRING := "Launcher version 1.0"

ENGINE.FUNNER_SHARED_LIBRARY.TYPE              := static-lib
ENGINE.FUNNER_SHARED_LIBRARY.NAME              := funner.engine.shared
ENGINE.FUNNER_SHARED_LIBRARY.SOURCE_DIRS       := sources/shared
ENGINE.FUNNER_SHARED_LIBRARY.INCLUDE_DIRS      := include
ENGINE.FUNNER_SHARED_LIBRARY.COMPILER_DEFINES  := LAUNCHER_VERSION='$(LAUNCHER_VERSION_STRING)' FUNNER_BUILD=1
ENGINE.FUNNER_SHARED_LIBRARY.IMPORTS           := compile.engine.core compile.common compile.system \
                                             link.common.default_console_handler link.common.zip_file_system link.common.aes \
                                             link.media.rms link.media.rfx.xmtl link.media.sound.snddecl link.media.sound.default_decoders \
                                             link.media.font.xfont link.media.players.null \
                                             link.input.manager.xkeymap link.input.manager.xkeyreg link.input.window_driver \
                                             link.render.scene_render.render2d link.script.lua \
                                             link.engine.script_binds.math link.engine.script_binds.sg link.engine.script_binds.bv \
                                             link.engine.script_binds.render link.engine.script_binds.engine \
                                             link.engine.script_binds.input link.engine.script_binds.lua_override \
                                             link.engine.script_binds.system link.engine.script_binds.common \
                                             link.engine.script_binds.rms link.engine.script_binds.media.players \
                                             link.sound.low_level.openal_driver run.sound.low_level.openal_driver \
                                             link.engine.subsystems.scene_render link.engine.subsystems.scene_player \
                                             link.engine.subsystems.input_manager link.engine.subsystems.shell \
                                             link.engine.subsystems.log link.engine.subsystems.file_system \
                                             link.engine.subsystems.resource_system \
                                             link.media.image.pvr
ENGINE.FUNNER_SHARED_LIBRARY.has_windows.IMPORTS := link.render.low_level.opengl_driver link.engine.subsystems.window_input_driver \
                                             link.engine.subsystems.window_manager link.engine.subsystems.window_renderer
ENGINE.FUNNER_SHARED_LIBRARY.win32.IMPORTS       := link.input.direct_input_driver
ENGINE.FUNNER_SHARED_LIBRARY.iphone.IMPORTS      := link.media.image.pvr link.input.iphone_driver link.media.players.iphone \
                                             link.engine.subsystems.iphone_audio_session

#Цель - сборка движка
ifneq (,$(filter iphone,$(PROFILES)))
ENGINE.FUNNER_LIBRARY.TYPE                := fat-static-lib
else
ENGINE.FUNNER_LIBRARY.TYPE                := dynamic-lib
endif
ENGINE.FUNNER_LIBRARY.NAME                := funner
ENGINE.FUNNER_LIBRARY.LIBS                := funner.engine.shared
ENGINE.FUNNER_LIBRARY.win32.LINK_INCLUDES := FunnerInit
#ENGINE.FUNNER_LIBRARY.win32.LINK_FLAGS    := -noentry
ENGINE.FUNNER_LIBRARY.IMPORTS             := $(ENGINE.FUNNER_SHARED_LIBRARY.IMPORTS)
ENGINE.FUNNER_LIBRARY.has_windows.IMPORTS := $(ENGINE.FUNNER_SHARED_LIBRARY.has_windows.IMPORTS)
ENGINE.FUNNER_LIBRARY.win32.IMPORTS       := $(ENGINE.FUNNER_SHARED_LIBRARY.win32.IMPORTS)
ENGINE.FUNNER_LIBRARY.iphone.IMPORTS      := $(ENGINE.FUNNER_SHARED_LIBRARY.iphone.IMPORTS)

#Цель - объединение библиотек для iPhone
ENGINE.FUNNER_LIBRARY_LIPO.NAME     := funner
ENGINE.FUNNER_LIBRARY_LIPO.TYPE     := lipo-lib
ENGINE.FUNNER_LIBRARY_LIPO.LIBS     := funner
ENGINE.FUNNER_LIBRARY_LIPO.PROFILES := iphone-device.g++ iphone-simulator.g++

#Цель - application
ENGINE.LAUNCHER.SOURCES.TYPE                   := application
ENGINE.LAUNCHER.SOURCES.NAME                   := launcher
ENGINE.LAUNCHER.SOURCES.INCLUDE_DIRS           := include
ENGINE.LAUNCHER.SOURCES.EXECUTION_DIR          := sources
ENGINE.LAUNCHER.SOURCES.macosx.COMPILER_CFLAGS := -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/CoreFoundation.framework/Headers/
ENGINE.LAUNCHER.SOURCES.iphone.COMPILER_CFLAGS := -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/CoreFoundation.framework/Headers/
ENGINE.LAUNCHER.SOURCES.macosx.LINK_FLAGS      := -framework CoreFoundation
ENGINE.LAUNCHER.SOURCES.iphone.LINK_FLAGS      := -framework CoreFoundation
ENGINE.LAUNCHER.SOURCES.msvc.LINK_FLAGS        := -subsystem:windows -entry:mainCRTStartup
ENGINE.LAUNCHER.SOURCES.win32.SOURCE_DIRS      := sources/platform/win32
ENGINE.LAUNCHER.SOURCES.macosx.SOURCE_DIRS     := sources/platform/macosx
ENGINE.LAUNCHER.SOURCES.iphone.SOURCE_DIRS     := sources/platform/iphone

#Цель - console application
ENGINE.CLAUNCHER.SOURCES.TYPE                := application
ENGINE.CLAUNCHER.SOURCES.NAME                := clauncher
ENGINE.CLAUNCHER.SOURCES.INCLUDE_DIRS        := include
ENGINE.CLAUNCHER.SOURCES.EXECUTION_DIR       := sources
ENGINE.CLAUNCHER.SOURCES.IMPORTS             := $(ENGINE.LAUNCHER.SOURCES.IMPORTS)
ENGINE.CLAUNCHER.SOURCES.has_windows.IMPORTS := $(ENGINE.LAUNCHER.SOURCES.has_windows.IMPORTS)
ENGINE.CLAUNCHER.SOURCES.win32.IMPORTS       := $(ENGINE.LAUNCHER.SOURCES.win32.IMPORTS)
ENGINE.CLAUNCHER.SOURCES.win32.SOURCE_DIRS   := sources/platform/win32

#Цель - SDK
ENGINE.LAUNCHER.SDK.TYPE             := installation
ENGINE.LAUNCHER.SDK.INSTALLATION_DIR := sdk/bin
ENGINE.LAUNCHER.SDK.TARGETS          := ENGINE.LAUNCHER.SOURCES
