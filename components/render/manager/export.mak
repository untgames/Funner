# Compile exports
export.compile.render.manager.INCLUDE_DIRS := include
export.compile.render.manager.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.render.manager.LIBS    := funner.render.manager
export.link.render.manager.IMPORTS := link.common link.media.rfx link.media.image link.media.geometry link.system link.render.low_level
