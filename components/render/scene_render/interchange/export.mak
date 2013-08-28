# Compile exports
export.compile.render.scene_render.interchange.INCLUDE_DIRS := include ../../../xtl/include
export.compile.render.scene_render.interchange.IMPORTS      := compile.common compile.math.vecmath

# Link exports
export.link.render.scene_render.interchange.LIBS     := funner.render.scene_render.interchange
export.link.render.scene_render.interchange.IMPORTS  := link.common link.system
