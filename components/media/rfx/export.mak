# Compile exports
export.compile.media.rfx.obsolete.INCLUDE_DIRS := include/obsolete
export.compile.media.rfx.obsolete.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.media.rfx.obsolete.LIBS    := funner.media.rfx.obsolete
export.link.media.rfx.obsolete.IMPORTS := link.common

# XMtl link exports
export.link.media.rfx.xmtl.obsolete.LIBS          := funner.media.rfx.xmtl.obsolete
export.link.media.rfx.xmtl.obsolete.LINK_INCLUDES := XMtlLoaderObsolete XMtlSaverObsolete
export.link.media.rfx.xmtl.obsolete.IMPORTS       := link.media.rfx.obsolete link.common.xml
