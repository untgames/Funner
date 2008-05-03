###################################################################################################
#Настройки, экспортируемые для библиотек использующих Openaldevice
###################################################################################################
export.INCLUDE_DIRS     := include
export.LIB_DIRS         := ../../../extern/openalsdk/lib/win32
export.LIBS             := openaldevice openal32
export.DLLS             := OpenAL32 wrap_oal
export.DLL_DIRS         := ../../../extern/openalsdk/bin
export.IMPORTS          := ../lowlevel/exports.mak
