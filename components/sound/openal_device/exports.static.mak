###################################################################################################
#Настройки, экспортируемые для библиотек использующих Openaldevice
###################################################################################################
export.INCLUDE_DIRS     := include
export.LIB_DIRS         := ../../../extern/openalsdk/lib/win32
export.LIBS             := sound.low_level.openal_driver sound.low_level common openal32
export.DLLS             := OpenAL32 wrap_oal
export.DLL_DIRS         := ../../../extern/openalsdk/bin
export.LINK_INCLUDES    := OpenAL
export.IMPORTS          := ../../media/sound/link.static.mak ../../media/sound/compile.static.mak
