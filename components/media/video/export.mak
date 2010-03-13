# Compile exports
export.compile.media.video.INCLUDE_DIRS := include
export.compile.media.video.IMPORTS      := compile.common

# Link exports
export.link.media.video.LIBS    := funner.media.video
export.link.media.video.IMPORTS := link.common link.media.image
