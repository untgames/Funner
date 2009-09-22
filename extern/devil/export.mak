# Compile exports
export.compile.extern.devil.INCLUDE_DIRS := include
#export.compile.extern.devil.COMPILER_DEFINES := IL_STATIC_LIB #for static library build

# Link exports
export.link.extern.devil.LIBS    := funner.extern.devil funner.extern.ilu
export.link.extern.devil.IMPORTS := link.extern.jpeg link.extern.tiff link.extern.libpng link.extern.zlib

# Run exports
export.run.extern.devil.DLLS := funner.extern.devil funner.extern.ilu
