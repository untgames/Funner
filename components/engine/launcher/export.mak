export.compile.engine.launcher.INCLUDE_DIRS := include

export.link.engine.launcher_shared.LIBS                := funner.engine.shared
export.link.engine.launcher_shared.has_windows.IMPORTS := link.render.low_level.opengl_driver link.engine.subsystems.window_input_driver \
                                             link.engine.subsystems.window_manager link.media.image.dds
export.link.engine.launcher_shared.win32.IMPORTS       := link.input.direct_input_driver
export.link.engine.launcher_shared.iphone.IMPORTS      := link.media.image.pvr link.media.players.iphone \
                                             link.engine.subsystems.iphone_audio_session link.engine.subsystems.iphone_configuration \
                                             link.social.game_kit link.store.ios_store link.push_notifications.ios_push_notifications
export.link.engine.launcher_shared.android.IMPORTS     := link.media.image.pvr link.store.android_store link.store.amazon_store \
                                                          link.push_notifications.google_push_notifications link.push_notifications.amazon_push_notifications
export.link.engine.launcher_shared.tabletos.IMPORTS    := link.media.image.pvr
export.link.engine.launcher_shared.beagleboard.IMPORTS := link.media.image.pvr
export.link.engine.launcher_shared.IMPORTS := link.common.default_console_handler link.common.zip_file_system link.common.aes \
                                link.media.rms link.media.sound.snddecl link.media.sound.default_decoders \
                                link.media.font.xfont link.media.players.null link.media.physics.xphys \
                                link.input.manager.xkeymap link.input.manager.xkeyreg link.input.window_driver link.input.sensor_input_driver \
                                link.script.lua \
                                link.engine.script_binds.math link.engine.script_binds.sg link.engine.script_binds.bv \
                                link.engine.script_binds.engine link.engine.script_binds.media.image \
                                link.engine.script_binds.input link.engine.script_binds.lua_override \
                                link.engine.script_binds.system link.engine.script_binds.common link.engine.script_binds.network \
                                link.engine.script_binds.rms link.engine.script_binds.media.players \
                                link.engine.script_binds.physics link.engine.script_binds.math.spline_loader \
                                link.engine.script_binds.social link.engine.script_binds.debug \
                                link.engine.script_binds.scene_input link.engine.script_binds.store \
                                link.engine.script_binds.analytics.flurry link.engine.script_binds.media.font \
                                link.engine.script_binds.push_notifications \
                                link.sound.low_level.openal_driver run.sound.low_level.openal_driver \
                                link.engine.subsystems.scene_render link.engine.subsystems.scene_player \
                                link.engine.subsystems.input_manager link.engine.subsystems.shell \
                                link.engine.subsystems.log link.engine.subsystems.file_system \
                                link.engine.subsystems.resource_system link.engine.subsystems.physics_manager \
                                link.engine.subsystems.scene_manager \
                                link.media.rfx.default_serializers link.media.geometry.binmesh link.media.geometry.xmesh \
                                link.media.video.theora link.network link.common.iconv \
                                link.physics.low_level.bullet_driver link.media.animation.xanim link.media.animation.binanim \
                                link.scene_graph.xscene link.social.facebook

export.link.engine.launcher.IMPORTS := link.engine.launcher_shared
export.link.engine.launcher.LINK_INCLUDES := FunnerInit FunnerMain
