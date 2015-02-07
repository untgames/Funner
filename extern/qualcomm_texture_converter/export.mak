# Compile exports
export.compile.extern.qualcomm_texture_converter.INCLUDE_DIRS := include

# Link exports
export.link.extern.qualcomm_texture_converter.LIBS            := 
export.link.extern.qualcomm_texture_converter.win32.LIBS      := TextureConverter
export.link.extern.qualcomm_texture_converter.win32.LIB_DIRS  := lib/win32
export.link.extern.qualcomm_texture_converter.macosx.LIBS     := TextureConverter
export.link.extern.qualcomm_texture_converter.macosx.LIB_DIRS := lib/osx
export.run.extern.qualcomm_texture_converter.win32.DLLS       := TextureConverter
export.run.extern.qualcomm_texture_converter.win32.DLL_DIRS   := lib/win32
export.run.extern.qualcomm_texture_converter.macosx.DLLS      := TextureConverter
export.run.extern.qualcomm_texture_converter.macosx.DLL_DIRS  := lib/osx
