###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.win32.LIB_DIRS    := ../../../extern/openalsdk/lib/win32
export.LIBS              := funner.sound.low_level.openal_driver
export.win32.LIBS        := openal32
export.LINK_INCLUDES     := OpenAL
export.macosx.LINK_FLAGS := -framework OpenAL
export.IMPORTS           := ../low_level/link.static.mak ../../media/sound/link.static.mak ../../system/link.static.mak
