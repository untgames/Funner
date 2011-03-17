export.compile.engine.launcher.INCLUDE_DIRS := include

export.link.engine.launcher_shared.LIBS                := funner.engine.shared
export.link.engine.launcher_shared.has_windows.IMPORTS := link.render.low_level.opengl_driver link.engine.subsystems.window_input_driver \
                                             link.engine.subsystems.window_manager link.engine.subsystems.window_renderer link.media.image.dds
export.link.engine.launcher_shared.win32.IMPORTS       := link.input.direct_input_driver
export.link.engine.launcher_shared.iphone.IMPORTS      := link.media.image.pvr link.input.iphone_driver link.media.players.iphone \
                                             link.engine.subsystems.iphone_audio_session link.engine.subsystems.iphone_configuration
export.link.engine.launcher_shared.beagleboard.IMPORTS := link.media.image.pvr
export.link.engine.launcher_shared.IMPORTS := link.common.default_console_handler link.common.zip_file_system link.common.aes \
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
                                link.media.video.theora link.network

export.link.engine.launcher.IMPORTS := link.engine.launcher_shared
export.link.engine.launcher.LINK_INCLUDES := FunnerInit FunnerMain
