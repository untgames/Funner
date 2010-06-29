# Compile exports
export.compile.media.video.INCLUDE_DIRS := include
export.compile.media.video.IMPORTS      := compile.common

# Link exports
export.link.media.video.LIBS    := funner.media.video
export.link.media.video.IMPORTS := link.common link.media.image

# Theora link exports
export.link.media.video.theora.LIBS          := funner.media.video.theora
export.link.media.video.theora.LINK_INCLUDES := TheoraDecoder
export.link.media.video.theora.IMPORTS       := link.media.video link.extern.theora
