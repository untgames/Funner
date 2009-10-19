# Compile exports
export.compile.media.players.INCLUDE_DIRS := include ../../xtl/include

# Link exports
export.link.media.players.core.LIBS    := funner.media.players.core
export.link.media.players.core.IMPORTS := link.common
