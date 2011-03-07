# Compile exports
export.compile.physics.manager.INCLUDE_DIRS := include
export.compile.physics.manager.IMPORTS      := compile.xtl compile.math.vecmath

# Link exports
export.link.physics.manager.LIBS    := funner.physics.manager
export.link.physics.manager.IMPORTS := link.physics.low_level

export.info.physics.manager.CHMS := funner.physics.manager
