# Compile exports
export.compile.media.sound.INCLUDE_DIRS := include
export.compile.media.sound.IMPORTS      := compile.common

# Link exports
export.link.media.sound.LIBS          := funner.media.sound
export.link.media.sound.LINK_INCLUDES := 
export.link.media.sound.IMPORTS       := link.common

# Snddecl link exports
export.link.media.sound.snddecl.LIBS          := funner.media.sound.snddecl
export.link.media.sound.snddecl.LINK_INCLUDES := SnddeclLoader SnddeclSaver
export.link.media.sound.snddecl.IMPORTS       := link.media.sound link.common.xml

# Default decoders link exports
export.link.media.sound.default_decoders.LIBS          := funner.media.sound.default_decoders funner.extern.vorbisfile_static funner.extern.vorbis_static funner.extern.ogg
export.link.media.sound.default_decoders.LINK_INCLUDES := WavLoader OggLoader
export.link.media.sound.default_decoders.IMPORTS       := link.media.sound
