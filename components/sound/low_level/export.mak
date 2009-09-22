# Compile exports
export.compile.sound.low_level.INCLUDE_DIRS  := include ../../xtl/include ../../media/sound/include

# Link exports
export.link.sound.low_level.LIBS    := funner.sound.low_level
export.link.sound.low_level.IMPORTS := link.common
