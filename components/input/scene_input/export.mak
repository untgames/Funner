# Manager compile exports
export.compile.input.scene_input.INCLUDE_DIRS := include
export.compile.input.scene_input.IMPORTS      := compile.scene_graph.core

# Manager link exports
export.link.input.scene_input.LIBS     := funner.input.scene_input
export.link.input.scene_input.IMPORTS  := link.scene_graph.core

export.info.input.scene_input.CHMS := funner.input.scene_input
