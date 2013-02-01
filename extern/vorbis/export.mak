# Compile exports
export.compile.extern.vorbis.INCLUDE_DIRS := include
export.compile.extern.vorbis.IMPORTS      := compile.extern.ogg

# Link exports
export.link.extern.vorbis.LIBS    := funner.extern.vorbis_static
export.link.extern.vorbis.IMPORTS := link.extern.ogg
