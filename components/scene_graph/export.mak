# Compile exports
export.compile.scene_graph.core.INCLUDE_DIRS := include
export.compile.scene_graph.core.IMPORTS      := compile.math.bound_volumes compile.xtl

# Link exports
export.link.scene_graph.core.LIBS     := funner.scene_graph.core
export.link.scene_graph.core.IMPORTS  := link.common link.math.bound_volumes link.media.rms link.media.animation link.media.font

# XScene Link exports
export.link.scene_graph.xscene.LIBS          := funner.scene_graph.xscene
export.link.scene_graph.xscene.IMPORTS       := link.scene_graph.core
export.link.scene_graph.xscene.LINK_INCLUDES := XSceneLoader

export.info.scene_graph.core.CHMS := funner.scene_graph.core
