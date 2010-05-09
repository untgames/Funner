# Compile exports
export.compile.media.animation.INCLUDE_DIRS := include
export.compile.media.animation.IMPORTS      := compile.common

# Link exports
export.link.media.animation.LIBS    := funner.media.animation
export.link.media.animation.IMPORTS := link.common

# XML animation link exports
export.link.media.animation.xanim.LIBS          := funner.media.animation.xanim
export.link.media.animation.xanim.LINK_INCLUDES := XAnimLoader XAnimSaver
export.link.media.animation.xanim.IMPORTS       := link.media.animation link.common.xml

# Binary animation link exports
export.link.media.animation.binanim.LIBS          := funner.media.animation.binanim
export.link.media.animation.binanim.LINK_INCLUDES := BinAnimLoader BinAnimSaver
export.link.media.animation.binanim.IMPORTS       := link.media.animation
