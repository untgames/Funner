# Compile exports
export.compile.scene_graph.core.INCLUDE_DIRS := include ../math/bound_volumes/include ../../xtl/include

# Link exports
export.link.scene_graph.core.LIBS     := funner.scene_graph.core
export.link.scene_graph.core.IMPORTS  := link.common
