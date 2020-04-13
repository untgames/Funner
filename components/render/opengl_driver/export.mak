# Link exports
export.link.render.low_level.opengl_driver.LIBS                := funner.render.low_level.opengl_driver
export.link.render.low_level.opengl_driver.LINK_INCLUDES       := OpenGLDriver
export.link.render.low_level.opengl_driver.IMPORTS             := link.render.low_level link.render.low_level.helpers link.common.wxf link.system
export.link.render.low_level.opengl_driver.x86_win32.LIBS      := gdi32
export.link.render.low_level.opengl_driver.win32.IMPORTS       := link.system
export.link.render.low_level.opengl_driver.macosx.LINK_FLAGS   := -framework AGL
export.link.render.low_level.opengl_driver.iphone.LINK_FLAGS   := -framework OpenGLES
export.link.render.low_level.opengl_driver.linux.IMPORTS       := link.system
export.link.render.low_level.opengl_driver.meego_ia32.IMPORTS  := link.extern.meego_ia32
export.link.render.low_level.opengl_driver.android.LIBS        := EGL GLESv2
export.link.render.low_level.opengl_driver.tabletos.LIBS       := EGL GLESv1_CM

#Linux egl flags
ifneq (,$(filter egl,$(PROFILES)))
export.link.render.low_level.opengl_driver.linux.LIBS := EGL GLESv2
endif

ifneq (meego-ia32,$(TOOLSET))
#export.link.render.low_level.opengl_driver.linux.LIBS := Xrandr Xxf86vm
endif

#Run exports (don't remove - used for common imports at run target)
export.run.render.low_level.opengl_driver.IMPORTS :=

# Win32 egl flags
ifneq (,$(filter gles,$(PROFILES)))
export.link.render.low_level.opengl_driver.win32.IMPORTS := link.extern.gles_win32
export.run.render.low_level.opengl_driver.win32.IMPORTS  := run.extern.gles_win32
export.link.render.low_level.opengl_driver.wince.IMPORTS := link.extern.gles_wince
endif

ifneq (,$(filter gles2,$(PROFILES)))
export.link.render.low_level.opengl_driver.win32.IMPORTS := link.extern.angle
export.run.render.low_level.opengl_driver.win32.IMPORTS  := run.extern.angle
endif
