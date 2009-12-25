# Compile exports
export.compile.media.adobe.xfl.INCLUDE_DIRS := include
export.compile.media.adobe.xfl.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.media.adobe.xfl.LIBS    := funner.media.adobe.xfl
export.link.media.adobe.xfl.IMPORTS := link.common

# Link XFL loader
export.link.media.adobe.xfl.loader.LIBS          := funner.media.adobe.xfl.loader
export.link.media.adobe.xfl.loader.LINK_INCLUDES := XflLoader
export.link.media.adobe.xfl.loader.IMPORTS       := link.media.adobe.xfl link.common.xml