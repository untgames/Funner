###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.OPENGL_DRIVER.UTILS RENDER.OPENGL_DRIVER.SOURCES RENDER.OPENGL_DRIVER.TESTS

#PROFILES += egl gles

#OpenGL render system utilities
RENDER.OPENGL_DRIVER.UTILS.TYPE        := test-suite
RENDER.OPENGL_DRIVER.UTILS.SOURCE_DIRS := utils
RENDER.OPENGL_DRIVER.UTILS.IMPORTS     := compile.common link.common

#OpenGL render system sources
RENDER.OPENGL_DRIVER.SOURCES.TYPE                   := static-lib
RENDER.OPENGL_DRIVER.SOURCES.NAME                   := funner.render.low_level.opengl_driver
RENDER.OPENGL_DRIVER.SOURCES.INCLUDE_DIRS           := sources
RENDER.OPENGL_DRIVER.SOURCES.macosx.COMPILER_CFLAGS := -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/ApplicationServices.framework/Frameworks/CoreGraphics.framework/Headers/ \
                                                       -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/IOKit.framework/Headers/graphics/ \
                                                       -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/AGL.framework/Headers/
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            := sources/common \
                                                       sources/driver \
                                                       sources/render_target_manager \
                                                       sources/render_target_manager/fbo_manager \
                                                       sources/input_stage \
                                                       sources/texture_manager \
                                                       sources/texture_manager/nv_dxt \
                                                       sources/shader_stage \
                                                       sources/shader_stage/ffp \
                                                       sources/output_stage \
                                                       sources/query_manager \
                                                       sources/render_target_manager/swap_chain_manager
RENDER.OPENGL_DRIVER.SOURCES.IMPORTS                := compile.render.low_level compile.common compile.system
RENDER.OPENGL_DRIVER.SOURCES.msvc.COMPILER_CFLAGS   := -wd4355
RENDER.OPENGL_DRIVER.SOURCES.gles.COMPILER_DEFINES  := OPENGL_ES_SUPPORT
RENDER.OPENGL_DRIVER.SOURCES.iphone.SOURCE_DIRS     := sources/platform/iphone
RENDER.OPENGL_DRIVER.SOURCES.egl.SOURCE_DIRS        := sources/platform/egl

ifeq (,$(filter gles,$(PROFILES)))

RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS        += sources/shader_stage/glsl
RENDER.OPENGL_DRIVER.SOURCES.win32.SOURCE_DIRS  := sources/platform/win32
RENDER.OPENGL_DRIVER.SOURCES.macosx.SOURCE_DIRS := sources/platform/macosx

else

RENDER.OPENGL_DRIVER.SOURCES.win32.IMPORTS  := compile.extern.gles_win32

endif

#OpenGL render system tests
RENDER.OPENGL_DRIVER.TESTS.TYPE             := test-suite
RENDER.OPENGL_DRIVER.TESTS.SOURCE_DIRS      := tests/driver tests/device tests/complex tests/unit
RENDER.OPENGL_DRIVER.TESTS.IMPORTS          := link.render.low_level.opengl_driver run.render.low_level.opengl_driver compile.render.low_level \
                                               compile.system link.system link.common compile.media.image compile.media.geometry \
                                               link.media.image link.media.geometry.xmesh link.media.image.pvr
#RENDER.OPENGL_DRIVER.TESTS.INSTALLATION_FILES :=   
