# Link exports
export.link.render.low_level.dx11_driver.LIBS           := funner.render.low_level.dx11_driver d3dx11 d3dcompiler
export.link.render.low_level.dx11_driver.LINK_INCLUDES  := DX11Driver
export.link.render.low_level.dx11_driver.IMPORTS        := link.render.low_level link.render.low_level.helpers link.system link.extern.dxsdk
export.link.render.low_level.dx11_driver.winrt.LIBS     := d3d11 dxgi

#Run exports (don't remove - used for common imports at run target)
export.run.render.low_level.dx11_driver.IMPORTS :=
