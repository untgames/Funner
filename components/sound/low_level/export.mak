# Compile exports
export.compile.sound.low_level.INCLUDE_DIRS := include
export.compile.sound.low_level.IMPORTS      := compile.xtl compile.math.vecmath

# Link exports
export.link.sound.low_level.LIBS    := funner.sound.low_level
export.link.sound.low_level.IMPORTS := link.common

export.info.sound.low_level.CHMS := funner.sound.low_level
