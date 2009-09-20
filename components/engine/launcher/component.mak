###################################################################################################
#Определения и константы
###################################################################################################
TARGETS       := ENGINE.FUNNER_LIBRARY ENGINE.LAUNCHER.SOURCES ENGINE.LAUNCHER.SDK
TARGETS.win32 := ENGINE.CLAUNCHER.SOURCES

LAUNCHER_VERSION_STRING := "Launcher version 1.0"

#Цель - сборка движка
ENGINE.FUNNER_LIBRARY.TYPE             := dynamic-lib
ENGINE.FUNNER_LIBRARY.NAME             := funner
ENGINE.FUNNER_LIBRARY.SOURCE_DIRS      := sources/shared
ENGINE.FUNNER_LIBRARY.COMPILER_DEFINES := LAUNCHER_VERSION='$(LAUNCHER_VERSION_STRING)' FUNNER_BUILD=1
ENGINE.FUNNER_LIBRARY.INCLUDE_DIRS     := include
ENGINE.FUNNER_LIBRARY.IMPORTS          := ../core/compile.static.mak \
                                            ../../common/compile.static.mak ../../common/default_console_handler.link.static.mak \
                                            ../../common/zip_file_system.link.static.mak ../../common/aes.link.static.mak \
                                            ../../media/rms/link.static.mak ../../media/rfx/xmtl.link.static.mak \
                                            ../../media/sound/snddecl.link.static.mak ../../media/sound/default_decoders.link.static.mak \
                                            ../../media/font/xfont.link.static.mak \
                                            ../../input/manager/xkeymap.link.static.mak ../../input/manager/xkeyreg.link.static.mak \
                                            ../../input/manager/compile.static.mak ../../input/window_driver/link.static.mak \
                                            ../../render/scene_render/compile.static.mak ../../render/scene_render/render2d.link.static.mak \
                                            ../../script/lua/link.static.mak \
                                            ../script_binds/math.link.static.mak   ../script_binds/sg.link.static.mak ../script_binds/bv.link.static.mak \
                                            ../script_binds/render.link.static.mak ../script_binds/engine.link.static.mak \
                                            ../script_binds/input.link.static.mak  ../script_binds/lua_override.link.static.mak \
                                            ../script_binds/system.link.static.mak ../script_binds/common.link.static.mak \
                                            ../script_binds/rms.link.static.mak \
                                            ../../sound/openal_device/link.static.mak ../../sound/openal_device/run.static.mak \
                                            ../subsystems/scene_render.link.static.mak ../subsystems/scene_player.link.static.mak \
                                            ../subsystems/input_manager.link.static.mak ../subsystems/shell.link.static.mak \
                                            ../subsystems/log.link.static.mak ../subsystems/file_system.link.static.mak \
                                            ../subsystems/resource_system.link.static.mak \
                                            ../../system/compile.static.mak
ENGINE.FUNNER_LIBRARY.has_windows.IMPORTS := ../../render/opengl_driver/link.static.mak ../subsystems/window_input_driver.link.static.mak \
                                             ../subsystems/window_manager.link.static.mak ../subsystems/window_renderer.link.static.mak
ENGINE.FUNNER_LIBRARY.win32.IMPORTS       := ../../input/direct_input_driver/link.static.mak

#Цель - win32 application
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
