# Link exports
export.link.render.low_level.opengl_driver.LIBS              := funner.render.low_level.opengl_driver
export.link.render.low_level.opengl_driver.win32.LIBS        := gdi32
export.link.render.low_level.opengl_driver.macosx.LINK_FLAGS := -framework IOKit -framework AGL
export.link.render.low_level.opengl_driver.iphone.LINK_FLAGS := -framework OpenGLES
export.link.render.low_level.opengl_driver.LINK_INCLUDES     := OpenGLDriver
export.link.render.low_level.opengl_driver.IMPORTS           := link.render.low_level link.common.wxf
export.link.render.low_level.opengl_driver.egl.IMPORTS       := link.extern.gles_win32

# Run exports
export.run.render.low_level.opengl_driver.egl.IMPORTS := run.extern.gles_win32
