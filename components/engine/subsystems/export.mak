# Link subsystems exports
export.link.engine.subsystems.LIBS    := funner.engine.subsystems
export.link.engine.subsystems.IMPORTS := link.engine.core

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
export.link.engine.subsystems.scene_render.LINK_INCLUDES := SceneRenderSubsystem
export.link.engine.subsystems.scene_render.IMPORTS       := link.engine.subsystems link.render.scene_render

# Link shell exports
export.link.engine.subsystems.shell.LINK_INCLUDES := ShellSubsystem
export.link.engine.subsystems.shell.IMPORTS       := link.engine.subsystems link.script.core

# Link window input driver exports
export.link.engine.subsystems.window_input_driver.LINK_INCLUDES := WindowInputDriverSubsystem
export.link.engine.subsystems.window_input_driver.IMPORTS       := link.engine.subsystems link.input.window_driver

# Link window manager exports
export.link.engine.subsystems.window_manager.LINK_INCLUDES := WindowManagerSubsystem
export.link.engine.subsystems.window_manager.IMPORTS       := link.engine.subsystems link.system

# Link window renderer exports
export.link.engine.subsystems.window_renderer.LINK_INCLUDES := WindowRendererSubsystem
export.link.engine.subsystems.window_renderer.IMPORTS       := link.engine.subsystems link.render.mid_level.window_driver
