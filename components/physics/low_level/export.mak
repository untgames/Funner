# Compile exports
export.compile.physics.low_level.INCLUDE_DIRS := include
export.compile.physics.low_level.IMPORTS      := compile.xtl compile.math.vecmath

# Link exports
export.link.physics.low_level.LIBS    := funner.physics.low_level
export.link.physics.low_level.IMPORTS := link.common

export.info.physics.low_level.CHMS := funner.physics.low_level
