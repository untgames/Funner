# Compile exports
export.compile.media.physics.INCLUDE_DIRS := include
export.compile.media.physics.IMPORTS      := compile.common compile.math.vecmath

# Link exports
export.link.media.physics.LIBS    := funner.media.physics
export.link.media.physics.IMPORTS := link.common

# XML physics link exports
export.link.media.physics.xphys.LIBS          := funner.media.physics.xphys
export.link.media.physics.xphys.LINK_INCLUDES := XPhysLoader XPhysSaver
export.link.media.physics.xphys.IMPORTS       := link.media.physics link.common.xml

export.info.media.sound.CHMS := funner.media.physics