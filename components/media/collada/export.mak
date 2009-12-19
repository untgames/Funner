# Compile exports
export.compile.media.collada.INCLUDE_DIRS := include
export.compile.media.collada.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.media.collada.LIBS    := funner.media.collada
export.link.media.collada.IMPORTS := link.common

# Link converter
export.link.media.collada.convert.LIBS    := funner.media.collada.convert
export.link.media.collada.convert.IMPORTS := link.media.collada

# Link DAE loader
export.link.media.collada.dae.LIBS          := funner.media.collada.dae
export.link.media.collada.dae.LINK_INCLUDES := DaeLoader
export.link.media.collada.dae.IMPORTS       := link.media.collada link.common.xml
