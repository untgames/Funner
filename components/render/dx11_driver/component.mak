###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.DX11_DRIVER.SOURCES RENDER.DX11_DRIVER.TESTS

#DX11 render system sources
RENDER.DX11_DRIVER.SOURCES.TYPE         := static-lib
RENDER.DX11_DRIVER.SOURCES.NAME         := funner.render.low_level.dx11_driver
RENDER.DX11_DRIVER.SOURCES.INCLUDE_DIRS += sources
RENDER.DX11_DRIVER.SOURCES.SOURCE_DIRS  += sources/common
RENDER.DX11_DRIVER.SOURCES.SOURCE_DIRS  += sources/driver
RENDER.DX11_DRIVER.SOURCES.SOURCE_DIRS  += sources/render_target_manager
RENDER.DX11_DRIVER.SOURCES.SOURCE_DIRS  += sources/texture_manager
RENDER.DX11_DRIVER.SOURCES.SOURCE_DIRS  += sources/shader_manager
RENDER.DX11_DRIVER.SOURCES.SOURCE_DIRS  += sources/input_manager
RENDER.DX11_DRIVER.SOURCES.SOURCE_DIRS  += sources/output_manager
RENDER.DX11_DRIVER.SOURCES.IMPORTS      := compile.render.low_level compile.render.low_level.helpers compile.common compile.system compile.extern.dxsdk compile.math.vecmath

#DX11 render system tests
RENDER.DX11_DRIVER.TESTS.TYPE          := test-suite
RENDER.DX11_DRIVER.TESTS.SOURCE_DIRS   += tests/driver
RENDER.DX11_DRIVER.TESTS.SOURCE_DIRS   += tests/device
RENDER.DX11_DRIVER.TESTS.IMPORTS       := link.render.low_level.dx11_driver run.render.low_level.dx11_driver compile.render.low_level \
                                          compile.system link.system link.common compile.media.image compile.media.geometry \
                                          link.media.image link.media.image.dds link.media.geometry.xmesh
