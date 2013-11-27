# Compile exports
export.compile.render.scene_render.server.INCLUDE_DIRS := include ../../../xtl/include
export.compile.render.scene_render.server.IMPORTS      :=

# Link exports
export.link.render.scene_render.server.LIBS         := funner.render.scene_render.server
export.link.render.scene_render.server.IMPORTS      := link.common link.system link.render.scene_render.interchange link.render.manager
export.link.render.scene_render.server.LINK_INCLUES := ForEachLightTechnique
