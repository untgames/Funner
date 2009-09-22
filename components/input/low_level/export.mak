# Compile exports
export.compile.input.low_level.INCLUDE_DIRS := include
export.compile.input.low_level.IMPORTS	    := compile.common

# Link exports
export.link.input.low_level.LIBS    := funner.input.low_level
export.link.input.low_level.IMPORTS := link.common
