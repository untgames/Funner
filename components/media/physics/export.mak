# Compile exports
export.compile.media.physics.INCLUDE_DIRS := include
export.compile.media.physics.IMPORTS      := compile.common compile.math.vecmath

# Link exports
export.link.media.physics.LIBS    := funner.media.physics
export.link.media.physics.IMPORTS := link.common

export.info.media.sound.CHMS := funner.media.physics