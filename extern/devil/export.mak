# Compile exports
export.compile.extern.devil.INCLUDE_DIRS            := include
export.compile.extern.devil.iphone.COMPILER_DEFINES := IL_STATIC_LIB

# Link exports
export.link.extern.devil.LIBS    := funner.extern.devil funner.extern.ilu
export.link.extern.devil.IMPORTS := link.extern.jpeg link.extern.tiff link.extern.libpng link.extern.zlib

# Run exports
ifeq (,$(filter iphone,$(PROFILES)))
export.run.extern.devil.DLLS := funner.extern.devil funner.extern.ilu
endif

