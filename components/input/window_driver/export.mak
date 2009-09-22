# Compile exports
export.compile.input.window_driver.INCLUDE_DIRS := include

# Link exports
export.link.input.window_driver.LIBS    := funner.input.window_driver
export.link.input.window_driver.IMPORTS := link.input.low_level link.system
