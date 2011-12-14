# Compile exports
export.compile.media.rfx.INCLUDE_DIRS := include
export.compile.media.rfx.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.media.rfx.LIBS    := funner.media.rfx
export.link.media.rfx.IMPORTS := link.common

# Default serializers link exports
export.link.media.rfx.default_serializers.LIBS          := funner.media.rfx.default_serializers
export.link.media.rfx.default_serializers.LINK_INCLUDES := XmtlLoader XmtlSaver DefaultShaderLoader
export.link.media.rfx.default_serializers.IMPORTS       := link.media.rfx link.common.xml

# Compile exports (obsolete)
export.compile.media.rfx.obsolete.INCLUDE_DIRS := include/obsolete
export.compile.media.rfx.obsolete.IMPORTS      := compile.math.vecmath compile.common

# Link exports (obsolete)
export.link.media.rfx.obsolete.LIBS    := funner.media.rfx.obsolete
export.link.media.rfx.obsolete.IMPORTS := link.common

# XMtl link exports
export.link.media.rfx.xmtl.obsolete.LIBS          := funner.media.rfx.xmtl.obsolete
export.link.media.rfx.xmtl.obsolete.LINK_INCLUDES := XMtlLoaderObsolete XMtlSaverObsolete
export.link.media.rfx.xmtl.obsolete.IMPORTS       := link.media.rfx.obsolete link.common.xml

# Documentation
export.info.media.rfx.CHMS := funner.media.rfx

