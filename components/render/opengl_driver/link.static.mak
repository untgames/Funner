###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIBS          := funner.render.low_level.opengl_driver 
export.win32.LIBS    := gdi32
export.LINK_INCLUDES := OpenGLDriver
export.IMPORTS       := ../low_level/link.static.mak ../../common/wxf.link.static.mak
