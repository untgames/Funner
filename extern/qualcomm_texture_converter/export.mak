# Compile exports
export.compile.extern.qualcomm_texture_converter.INCLUDE_DIRS := include

# Link exports
export.link.extern.qualcomm_texture_converter.LIBS              := TextureConverter
export.link.extern.qualcomm_texture_converter.vcx86.LIB_DIRS    := lib/win32
export.link.extern.qualcomm_texture_converter.vcx86-64.LIB_DIRS := lib/win64
export.link.extern.qualcomm_texture_converter.macosx.LIB_DIRS   := lib/osx
export.run.extern.qualcomm_texture_converter.vcx86.DLLS         := TextureConverter
export.run.extern.qualcomm_texture_converter.vcx86.DLL_DIRS     := lib/win32
export.run.extern.qualcomm_texture_converter.vcx86-64.DLLS      := TextureConverter
export.run.extern.qualcomm_texture_converter.vcx86-64.DLL_DIRS  := lib/win64
export.run.extern.qualcomm_texture_converter.macosx.DLLS        := TextureConverter
export.run.extern.qualcomm_texture_converter.macosx.DLL_DIRS    := lib/osx
