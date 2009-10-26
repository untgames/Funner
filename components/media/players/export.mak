# Compile exports
export.compile.media.players.INCLUDE_DIRS := include ../../xtl/include

# Link exports
export.link.media.players.core.LIBS    := funner.media.players.core
export.link.media.players.core.IMPORTS := link.common

# Link exports iphone
export.link.media.players.iphone.LIBS          := funner.media.players.iphone
export.link.media.players.iphone.IMPORTS       := link.media.players.core
export.link.media.players.iphone.LINK_FLAGS    := -framework Foundation -framework AVFoundation
export.link.media.players.iphone.LINK_INCLUDES := IPhonePlayers
