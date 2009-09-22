# Compile exports
export.compile.media.geometry.INCLUDE_DIRS := include
export.compile.media.geometry.IMPORTS      := compile.common

# Link exports
export.link.media.geometry.LIBS    := funner.media.geometry 
export.link.media.geometry.IMPORTS := link.common

# Binary mesh link exports
export.link.media.geometry.binmesh.LIBS          := funner.media.geometry.binmesh
export.link.media.geometry.binmesh.LINK_INCLUDES := BinMeshLoader BinMeshSaver
export.link.media.geometry.binmesh.IMPORTS       := link.media.geometry

# XML mesh link exports
export.link.media.geometry.xmesh.LIBS          := funner.media.geometry.xmesh
export.link.media.geometry.xmesh.LINK_INCLUDES := XMeshLoader XMeshSaver
export.link.media.geometry.xmesh.IMPORTS       := link.media.geometry link.common.xml


