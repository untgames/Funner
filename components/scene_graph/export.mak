# Compile exports
export.compile.scene_graph.core.INCLUDE_DIRS := include ../../xtl/include
export.compile.scene_graph.core.IMPORTS      := compile.math.bound_volumes

# Link exports
export.link.scene_graph.core.LIBS     := funner.scene_graph.core
export.link.scene_graph.core.IMPORTS  := link.common
