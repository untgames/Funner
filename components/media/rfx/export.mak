# Compile exports
export.compile.media.rfx.INCLUDE_DIRS := include
export.compile.media.rfx.IMPORTS      := compile.math.vecmath compile.common

# Link exports
export.link.media.rfx.LIBS    := funner.media.rfx
export.link.media.rfx.IMPORTS := link.common

# Default serializers link exports
export.link.media.rfx.default_serializers.LIBS          := funner.media.rfx.default_serializers
export.link.media.rfx.default_serializers.LINK_INCLUDES := XrfxLoader XmtlLoader XmtlSaver
export.link.media.rfx.default_serializers.IMPORTS       := link.media.rfx link.common.xml
