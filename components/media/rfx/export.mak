# Compile exports
export.compile.media.rfx.INCLUDE_DIRS := include
export.compile.media.rfx.IMPORTS      := compile.common

# Link exports
export.link.media.rfx.LIBS    := funner.media.rfx
export.link.media.rfx.IMPORTS := link.common

# XMtl link exports
export.link.media.rfx.xmtl.LIBS          := funner.media.rfx.xmtl
export.link.media.rfx.xmtl.LINK_INCLUDES := XMtlLoader XMtlSaver
export.link.media.rfx.xmtl.IMPORTS       := link.media.rfx link.common.xml
