# Compile exports
export.compile.extern.tiff.INCLUDE_DIRS := include

# Link exports
export.link.extern.tiff.LIBS           := funner.extern.tiff
export.link.extern.tiff.IMPORTS        := link.extern.jpeg
export.link.extern.tiff.x86_win32.LIBS := user32
