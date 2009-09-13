###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIBS              := funner.render.low_level.opengl_driver 
export.win32.LIBS        := gdi32
export.macosx.LINK_FLAGS := -framework IOKit -framework AGL 
export.iphone.LINK_FLAGS := -framework OpenGLES
export.LINK_INCLUDES     := OpenGLDriver
export.IMPORTS           := ../low_level/link.static.mak ../../common/wxf.link.static.mak
export.egl.IMPORTS       := ../../../extern/gles_win32/link.mak
