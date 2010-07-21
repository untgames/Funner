# Compile exports
export.compile.sound.manager.INCLUDE_DIRS := include
export.compile.sound.manager.IMPORTS      := compile.sound.low_level

# Link exports
export.link.sound.manager.LIBS    := funner.sound.manager
export.link.sound.manager.IMPORTS := link.sound.low_level

export.info.sound.manager.CHMS := funner.sound.manager
