# Compile exports
export.compile.extern.openssl.COMPILER_CFLAGS  :=
export.compile.extern.zlib.COMPILER_CFLAGS     :=
export.compile.extern.libpng.COMPILER_CFLAGS   := -Ilibpng14
export.compile.extern.freetype.COMPILER_CFLAGS := -Ifreetype2
export.compile.extern.jpeg.COMPILER_CFLAGS     := -Ijpeg

# Link exports
export.link.extern.libpng.LINK_FLAGS   := -lpng
export.link.extern.jpeg.LINK_FLAGS     := -ljpeg
export.link.extern.freetype.LINK_FLAGS := -lfreetype
export.link.extern.zlib.LINK_FLAGS     := -lz
export.link.extern.openssl.LINK_FLAGS  := -lssl
