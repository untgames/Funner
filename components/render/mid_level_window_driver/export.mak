# Compile exports
export.compile.render.mid_level.window_driver.INCLUDE_DIRS := include
export.compile.render.mid_level.window_driver.IMPORTS      := compile.render.mid_level compile.system

# Link exports
export.link.render.mid_level.window_driver.LIBS    := funner.render.mid_level.window_driver
export.link.render.mid_level.window_driver.IMPORTS := link.render.mid_level link.render.low_level link.media.image link.system
