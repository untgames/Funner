# Compile exports
export.compile.media.image.INCLUDE_DIRS := include
export.compile.media.image.IMPORTS      := compile.common

# Link exports
export.link.media.image.LIBS             := funner.media.image
export.link.media.image.LINK_INCLUDES    := DevIL
export.link.media.image.IMPORTS          := link.common link.extern.devil

# XAtlas link exports
export.link.media.image.xatlas.LIBS          := funner.media.image.xatlas
export.link.media.image.xatlas.LINK_INCLUDES := XAtlasLoader XAtlasSaver
export.link.media.image.xatlas.IMPORTS       := link.media.image link.common.xml

# Left-bottom packer link exports
export.link.media.image.left_bottom_packer.LIBS          := funner.media.image.left_bottom_packer
export.link.media.image.left_bottom_packer.LINK_INCLUDES := LeftBottomPacker
export.link.media.image.left_bottom_packer.IMPORTS       := link.media.image
