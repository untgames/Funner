# Compile exports
export.compile.extern.flac.INCLUDE_DIRS          := include
export.compile.extern.flac.msvc.COMPILER_DEFINES := FLAC__NO_DLL

# Link exports
export.link.extern.flac.LIBS := funner.extern.flac_static
