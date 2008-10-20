###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIBS              := funner.system
export.win32.LIBS        := user32
export.macosx.LINK_FLAGS := -arch i386 -isysroot /Developer/SDKs/MacOSX10.4u.sdk  -mmacosx-version-min=10.4
export.IMPORTS           := ../common/link.static.mak
