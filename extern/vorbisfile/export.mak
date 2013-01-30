# Compile exports
export.compile.extern.vorbisfile.INCLUDE_DIRS := include
export.compile.extern.vorbisfile.IMPORTS      := compile.extern.ogg

# Link exports
export.link.extern.vorbisfile.LIBS   := funner.extern.vorbisfile_static
export.link.extern.vorbisfile.IMPORS := link.extern.vorbis
