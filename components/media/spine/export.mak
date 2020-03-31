# Compile exports
export.compile.media.spine.INCLUDE_DIRS := include
export.compile.media.spine.IMPORTS      := compile.math.vecmath compile.xtl

# Link exports
export.link.media.spine.LIBS    := funner.media.spine.core
export.link.media.spine.IMPORTS := link.media.geometry

# Spine link exports
export.link.media.spine.loader_3.6.LIBS          := funner.media.spine.loader_3.6
export.link.media.spine.loader_3.6.LINK_INCLUDES := SpineLoader_36
export.link.media.spine.loader_3.6.IMPORTS       := link.media.spine link.extern.spine_3.6

# Spine link exports
export.link.media.spine.loader_3.8.LIBS          := funner.media.spine.loader_3.8
export.link.media.spine.loader_3.8.LINK_INCLUDES := SpineLoader_38
export.link.media.spine.loader_3.8.IMPORTS       := link.media.spine link.extern.spine_3.8
