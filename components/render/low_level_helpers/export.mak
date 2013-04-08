# Compile exports
export.compile.render.low_level.helpers.INCLUDE_DIRS  := include ../../xtl/include
export.compile.render.low_level.helpers.IMPORTS       := compile.render.low_level

# Link exports
export.link.render.low_level.helpers.LIBS    := funner.render.low_level.helpers
export.link.render.low_level.helpers.IMPORTS := link.common

