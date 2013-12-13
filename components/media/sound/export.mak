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
export.link.media.sound.default_decoders.LIBS          := funner.media.sound.default_decoders
export.link.media.sound.default_decoders.LINK_INCLUDES := WavLoader OggLoader
export.link.media.sound.default_decoders.IMPORTS       := link.media.sound link.extern.vorbisfile

# Flac decoder link exports
export.link.media.sound.flac_decoder.LIBS          := funner.media.sound.flac_decoder
export.link.media.sound.flac_decoder.LINK_INCLUDES := FlacLoader
export.link.media.sound.flac_decoder.IMPORTS       := link.media.sound link.extern.flac

export.info.media.sound.CHMS := funner.media.sound