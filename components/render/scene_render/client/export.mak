# Compile exports
export.compile.render.scene_render.client.INCLUDE_DIRS := include ../../../xtl/include
export.compile.render.scene_render.client.IMPORTS      := compile.scene_graph.core

# Link exports
export.link.render.scene_render.client.LIBS     := funner.render.scene_render.client
export.link.render.scene_render.client.IMPORTS  := link.common link.system link.render.scene_render.interchange link.media.image
