# Compile exports
export.compile.extern.gles2_win32.INCLUDE_DIRS := include

# Link exports
export.link.extern.gles2_win32.LIB_DIRS := lib
export.link.extern.gles2_win32.LIBS     := libEGL libGLESv2

# Run exports
export.run.extern.gles2_win32.DLLS     := libEGL libGLESv2 TextureConverter
export.run.extern.gles2_win32.DLL_DIRS := bin
