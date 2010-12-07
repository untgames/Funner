# Compile exports
export.compile.media.image.INCLUDE_DIRS := include
export.compile.media.image.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.media.image.LIBS             := funner.media.image
export.link.media.image.LINK_INCLUDES    := DevILLoaders DevILSavers
export.link.media.image.IMPORTS          := link.common link.extern.devil

# XAtlas link exports
export.link.media.image.xatlas.LIBS          := funner.media.image.xatlas
export.link.media.image.xatlas.LINK_INCLUDES := XAtlasLoader XAtlasSaver
export.link.media.image.xatlas.IMPORTS       := link.media.image link.common.xml

# Left-bottom packer link exports
export.link.media.image.left_bottom_packer.LIBS          := funner.media.image.left_bottom_packer
export.link.media.image.left_bottom_packer.LINK_INCLUDES := LeftBottomPacker
export.link.media.image.left_bottom_packer.IMPORTS       := link.media.image

# PVRTC loader exports
export.link.media.image.pvr.LIBS          := funner.media.image.pvr_loader
export.link.media.image.pvr.LINK_INCLUDES := PvrLoader
export.link.media.image.pvr.IMPORTS       := link.media.image

# DDS loader exports
export.link.media.image.dds.LIBS          := funner.media.image.dds_loader
export.link.media.image.dds.LINK_INCLUDES := DdsLoader
export.link.media.image.dds.IMPORTS       := link.media.image

# ANI cursors exports
export.link.media.image.ani_cursor_loader.LIBS          := funner.media.image.ani_cursors
export.link.media.image.ani_cursor_loader.LINK_INCLUDES := AniCursorLoader
export.link.media.image.ani_cursor_loader.IMPORTS       := link.media.image

export.info.media.image.CHMS := funner.media.image
