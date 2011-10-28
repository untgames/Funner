# Compile exports
export.compile.physics.scene_physics.INCLUDE_DIRS := include
export.compile.physics.scene_physics.IMPORTS      := compile.physics.manager compile.scene_graph.core

# Link exports
export.link.physics.scene_physics.LIBS    := funner.physics.scene_physics
export.link.physics.scene_physics.IMPORTS := link.physics.manager link.scene_graph.core

export.info.physics.scene_physics.CHMS := funner.physics.scene_physics
