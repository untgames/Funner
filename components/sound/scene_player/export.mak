# Compile exports
export.compile.sound.scene_player.INCLUDE_DIRS := include
export.compile.sound.scene_player.IMPORTS      := compile.scene_graph.core compile.sound.manager

# Link exports
export.link.sound.scene_player.LIBS    := funner.sound.scene_player
export.link.sound.scene_player.IMPORTS := link.sound.manager link.system link.scene_graph.core

export.info.sound.scene_player.CHMS := funner.sound.scene_player
