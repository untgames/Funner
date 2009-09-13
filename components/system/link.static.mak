###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIBS              := funner.common funner.system
export.IMPORTS           := ../common/link.static.mak
export.win32.LIBS        := user32
export.macosx.LINK_FLAGS := -framework CoreFoundation
export.iphone.LINK_FLAGS := -framework CoreFoundation -framework Foundation -framework UIKit -framework QuartzCore
export.linux.LIBS        := dl pthread
