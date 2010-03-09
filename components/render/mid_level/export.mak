# Compile exports
export.compile.render.mid_level.INCLUDE_DIRS := include
export.compile.render.mid_level.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.render.mid_level.LIBS    := funner.render.mid_level
export.link.render.mid_level.IMPORTS := link.common link.media.rfx link.media.geometry link.system link.render.low_level
