# Compile exports
export.compile.extern.qualcomm_texture_converter.INCLUDE_DIRS := include

# Link exports
export.link.extern.qualcomm_texture_converter.LIBS             := 
export.link.extern.qualcomm_texture_converter.win32.LIBS       := TextureConverter
export.link.extern.qualcomm_texture_converter.win32.LIB_DIRS   := lib/win32
export.link.extern.qualcomm_texture_converter.macosx.LIBS      := TextureConverterOSX
export.link.extern.qualcomm_texture_converter.macosx.LIB_DIRS  := lib/osx/Release
export.link.extern.qualcomm_texture_converter.android.LIBS     := TextureConverter
export.link.extern.qualcomm_texture_converter.android.LIB_DIRS := lib/android
