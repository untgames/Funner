###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIB_DIRS         := ../../../extern/openalsdk/lib/win32
export.LIBS             := sound.low_level.openal_driver openal32
export.LINK_INCLUDES    := OpenAL
export.IMPORTS          := ../low_level/link.static.mak ../../media/sound/link.static.mak ../../system/link.static.mak
