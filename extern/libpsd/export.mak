# Compile exports
export.compile.extern.libpsd.INCLUDE_DIRS := include

# Link exports
export.link.extern.libpsd.LIBS    := funner.extern.libpsd
export.link.extern.libpsd.IMPORTS := link.extern.zlib link.extern.jpeg
