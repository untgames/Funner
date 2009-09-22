# Compile exports
export.compile.extern.gles_win32.INCLUDE_DIRS := include

# Link exports
export.link.extern.gles_win32.LIB_DIRS := lib
export.link.extern.gles_win32.LIBS     := libegl libgles_cm

# Run exports
export.run.extern.gles_win32.DLLS     := libegl libgles_cm
export.run.extern.gles_win32.DLL_DIRS := bin
