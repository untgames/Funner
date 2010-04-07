###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SUBSYSTEMS.SOURCES ENGINE.SUBSYSTEMS.TESTS

#Цель - sources
ENGINE.SUBSYSTEMS.SOURCES.TYPE                    := static-lib
ENGINE.SUBSYSTEMS.SOURCES.NAME                    := funner.engine.subsystems
ENGINE.SUBSYSTEMS.SOURCES.SOURCE_DIRS             := sources/common
ENGINE.SUBSYSTEMS.SOURCES.IMPORTS                 := compile.engine.core compile.render.scene_render \
                                                     compile.input.manager compile.input.low_level \
                                                     compile.sound.scene_player compile.system compile.script.core compile.media.rms
ENGINE.SUBSYSTEMS.SOURCES.has_windows.IMPORTS     := compile.input.window_driver compile.render.mid_level.window_driver
ENGINE.SUBSYSTEMS.SOURCES.has_windows.SOURCE_DIRS := sources/window_manager
ENGINE.SUBSYSTEMS.SOURCES.iphone.SOURCE_DIRS      := sources/iphone
ENGINE.SUBSYSTEMS.SOURCES.iphone.COMPILER_CFLAGS  := -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/AudioToolbox.framework/Headers \
                                                     -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/MediaPlayer.framework/Headers/

#Цель - tests
ENGINE.SUBSYSTEMS.TESTS.TYPE                := test-suite
ENGINE.SUBSYSTEMS.TESTS.SOURCE_DIRS         := tests
ENGINE.SUBSYSTEMS.TESTS.IMPORTS             := compile.engine.core compile.common compile.system compile.render.scene_render compile.input.manager \
                                               link.engine.subsystems.scene_render link.engine.subsystems.scene_player \
                                               link.engine.subsystems.input_manager link.engine.subsystems.shell link.engine.subsystems.log \
                                               link.engine.subsystems.file_system link.engine.subsystems.resource_system \
                                               link.input.manager.xkeymap link.input.manager.xkeyreg link.sound.low_level.openal_driver run.sound.low_level.openal_driver \
                                               link.script.lua link.engine.script_binds.math link.engine.script_binds.sg \
                                               link.engine.script_binds.render link.engine.script_binds.engine \
                                               link.engine.script_binds.input link.engine.script_binds.system \
                                               link.engine.script_binds.lua_override link.common.default_console_handler
ENGINE.SUBSYSTEMS.TESTS.has_windows.IMPORTS := link.render.low_level.opengl_driver link.engine.subsystems.window_input_driver \
                                               link.engine.subsystems.window_manager link.engine.subsystems.window_renderer
ENGINE.SUBSYSTEMS.TESTS.win32.IMPORTS       := link.input.direct_input_driver
