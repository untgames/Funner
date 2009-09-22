# Compile exports
export.compile.render.low_level.INCLUDE_DIRS  := include ../../xtl/include

# Link exports
export.link.render.low_level.LIBS    := funner.render.low_level
export.link.render.low_level.IMPORTS := link.common
