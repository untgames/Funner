# Compile exports
export.compile.media.players.INCLUDE_DIRS := include ../../xtl/include

# Link exports
export.link.media.players.core.LIBS    := funner.media.players.core
export.link.media.players.core.IMPORTS := link.common

# Link exports null
export.link.media.players.null.LIBS          := funner.media.players.null
export.link.media.players.null.IMPORTS       := link.media.players.core
export.link.media.players.null.LINK_INCLUDES := NullPlayer

# Link exports iphone
export.link.media.players.iphone.LIBS          := funner.media.players.iphone
export.link.media.players.iphone.IMPORTS       := link.media.players.core
export.link.media.players.iphone.LINK_FLAGS    := -framework Foundation -framework AVFoundation -framework MediaPlayer -framework UIKit
export.link.media.players.iphone.LINK_INCLUDES := IPhonePlayers
