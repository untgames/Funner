###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIBS                 := funner.common funner.system
export.IMPORTS              := ../common/link.static.mak
export.EXCLUDE_DEFAULT_LIBS := funner.common.single_threaded_lockable
export.win32.LIBS           := user32
export.macosx.LINK_FLAGS    := -mmacosx-version-min=10.4
export.linux.LIBS           := dl pthread
