# Link subsystems exports
export.link.engine.subsystems.LIBS    := funner.engine.subsystems
export.link.engine.subsystems.IMPORTS := link.engine.core
export.link.engine.subsystems.MULTI_IMPORTS := 1

# Link direct input exports
export.link.engine.subsystems.direct_input_driver.LINK_INCLUDES := DirectInputDriverSubsystem
export.link.engine.subsystems.direct_input_driver.IMPORTS       := link.engine.subsystems link.input.direct_input_driver

# Link file system exports
export.link.engine.subsystems.file_system.LINK_INCLUDES := FileSystem
export.link.engine.subsystems.file_system.IMPORTS       := link.engine.subsystems

# Link input manager exports
export.link.engine.subsystems.input_manager.LINK_INCLUDES := InputManagerSubsystem
export.link.engine.subsystems.input_manager.IMPORTS       := link.engine.subsystems link.input.manager

# Link log exports
export.link.engine.subsystems.log.LINK_INCLUDES := LogSubsystem
export.link.engine.subsystems.log.IMPORTS       := link.engine.subsystems

# Link resource system exports
export.link.engine.subsystems.resource_system.LINK_INCLUDES := ResourceSystem
export.link.engine.subsystems.resource_system.IMPORTS       := link.engine.subsystems link.media.rms

# Link scene player exports
export.link.engine.subsystems.scene_player.LINK_INCLUDES := ScenePlayerSubsystem
export.link.engine.subsystems.scene_player.IMPORTS       := link.engine.subsystems link.sound.scene_player link.media.sound

# Link render exports
export.link.engine.subsystems.scene_render.LINK_INCLUDES := SceneRenderServerSubsystem SceneRenderClientSubsystem
export.link.engine.subsystems.scene_render.IMPORTS       := link.engine.subsystems link.render.scene_render.server link.render.scene_render.client

# Link scene manager exports
export.link.engine.subsystems.scene_manager.LINK_INCLUDES := SceneManagerSubsystem
export.link.engine.subsystems.scene_manager.IMPORTS       := link.engine.subsystems link.scene_graph.core link.media.font

# Link shell exports
export.link.engine.subsystems.shell.LINK_INCLUDES := ShellSubsystem
export.link.engine.subsystems.shell.IMPORTS       := link.engine.subsystems link.script.core

# Link iPhone audio session exports
export.link.engine.subsystems.iphone_audio_session.LINK_INCLUDES := IPhoneAudioSessionSubsystem
export.link.engine.subsystems.iphone_audio_session.IMPORTS       := link.engine.subsystems link.script.core
export.link.engine.subsystems.iphone_audio_session.LINK_FLAGS    := -framework AudioToolbox -framework MediaPlayer

# Link iPhone configuration exports
export.link.engine.subsystems.iphone_configuration.LINK_INCLUDES := IPhoneConfigurationSubsystem
export.link.engine.subsystems.iphone_configuration.IMPORTS       := link.engine.subsystems

# Link window input driver exports
export.link.engine.subsystems.window_input_driver.LINK_INCLUDES := WindowInputDriverSubsystem
export.link.engine.subsystems.window_input_driver.IMPORTS       := link.engine.subsystems link.input.window_driver

# Link window manager exports
export.link.engine.subsystems.window_manager.LINK_INCLUDES := WindowManagerSubsystem
export.link.engine.subsystems.window_manager.IMPORTS       := link.engine.subsystems link.system link.media.rms

# Link physics manager exports
export.link.engine.subsystems.physics_manager.LINK_INCLUDES := PhysicsManagerSubsystem
export.link.engine.subsystems.physics_manager.IMPORTS       := link.engine.subsystems link.physics.manager
