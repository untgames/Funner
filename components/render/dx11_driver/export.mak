# Link exports
export.link.render.low_level.dx11_driver.LIBS           := funner.render.low_level.dx11_driver
export.link.render.low_level.dx11_driver.LINK_INCLUDES  := DX11Driver
export.link.render.low_level.dx11_driver.IMPORTS        := link.render.low_level link.system

#Run exports (don't remove - used for common imports at run target)
export.run.render.low_level.dx11_driver.IMPORTS :=
