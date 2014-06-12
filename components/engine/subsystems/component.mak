###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SUBSYSTEMS.SOURCES ENGINE.SUBSYSTEMS.TESTS

#Цель - sources
ENGINE.SUBSYSTEMS.SOURCES.TYPE                    := static-lib
ENGINE.SUBSYSTEMS.SOURCES.NAME                    := funner.engine.subsystems
ENGINE.SUBSYSTEMS.SOURCES.SOURCE_DIRS             := sources/common
ENGINE.SUBSYSTEMS.SOURCES.IMPORTS                 := compile.engine.core \
                                                     compile.input.manager compile.input.low_level \
                                                     compile.render.scene_render.client compile.sound.scene_player compile.system \
                                                     compile.script.core compile.media.rms \
                                                     compile.physics.manager
ENGINE.SUBSYSTEMS.SOURCES.has_windows.IMPORTS     := compile.input.window_driver compile.render.scene_render.server                                                
ENGINE.SUBSYSTEMS.SOURCES.has_windows.SOURCE_DIRS := sources/window_manager
ENGINE.SUBSYSTEMS.SOURCES.iphone.SOURCE_DIRS      := sources/iphone

#Цель - tests
ENGINE.SUBSYSTEMS.TESTS.TYPE                := test-suite
ENGINE.SUBSYSTEMS.TESTS.SOURCE_DIRS         := tests
ENGINE.SUBSYSTEMS.TESTS.INSTALLATION_FILES  := tests/config_window_manager_shell.xml tests/config_window_manager.xml tests/config_shell.xml tests/config_cursor.xml
ENGINE.SUBSYSTEMS.TESTS.IMPORTS             := compile.engine.core compile.common compile.system compile.input.manager \
                                               link.engine.subsystems.scene_render link.engine.subsystems.scene_player \
                                               link.engine.subsystems.input_manager link.engine.subsystems.shell link.engine.subsystems.log \
                                               link.engine.subsystems.file_system link.engine.subsystems.resource_system \
                                               link.engine.subsystems.physics_manager link.engine.subsystems.scene_manager \
                                               link.input.manager.xkeymap link.input.manager.xkeyreg link.sound.low_level.openal_driver run.sound.low_level.openal_driver \
                                               link.script.lua link.engine.script_binds.math link.engine.script_binds.sg \
                                               link.engine.script_binds.engine \
                                               link.engine.script_binds.input link.engine.script_binds.system \
                                               link.engine.script_binds.lua_override link.common.default_console_handler \
                                               link.engine.script_binds.physics link.physics.low_level.bullet_driver link.media.animation.xanim \
                                               link.scene_graph.xscene compile.scene_graph.core
ENGINE.SUBSYSTEMS.TESTS.has_windows.IMPORTS := link.render.low_level.opengl_driver link.engine.subsystems.window_input_driver \
                                               link.engine.subsystems.window_manager
ENGINE.SUBSYSTEMS.TESTS.win32.IMPORTS       := link.input.direct_input_driver
