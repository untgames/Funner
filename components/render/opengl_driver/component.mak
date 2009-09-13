###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.OPENGL_DRIVER.UTILS RENDER.OPENGL_DRIVER.SOURCES RENDER.OPENGL_DRIVER.TESTS

#PROFILES += egl gles

#OpenGL render system utilities
RENDER.OPENGL_DRIVER.UTILS.TYPE             := test-suite
RENDER.OPENGL_DRIVER.UTILS.INCLUDE_DIRS     :=
RENDER.OPENGL_DRIVER.UTILS.SOURCE_DIRS      := utils
RENDER.OPENGL_DRIVER.UTILS.LIB_DIRS         :=
RENDER.OPENGL_DRIVER.UTILS.LIBS             :=
RENDER.OPENGL_DRIVER.UTILS.COMPILER_CFLAGS  :=
RENDER.OPENGL_DRIVER.UTILS.COMPILER_DEFINES := 
RENDER.OPENGL_DRIVER.UTILS.IMPORTS          :=  ../../common/compile.static.mak ../../common/link.static.mak

#OpenGL render system sources
RENDER.OPENGL_DRIVER.SOURCES.TYPE                   := static-lib
RENDER.OPENGL_DRIVER.SOURCES.NAME                   := funner.render.low_level.opengl_driver
RENDER.OPENGL_DRIVER.SOURCES.INCLUDE_DIRS           := sources
RENDER.OPENGL_DRIVER.SOURCES.macosx.COMPILER_CFLAGS := -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/ApplicationServices.framework/Frameworks/CoreGraphics.framework/Headers/ \
                                                       -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/IOKit.framework/Headers/graphics/ \
                                                       -I$(MACOSX_SDK_PATH)/System/Library/Frameworks/AGL.framework/Headers/
RENDER.OPENGL_DRIVER.SOURCES.iphone.COMPILER_CFLAGS := -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/OpenGLES.framework/Headers/ \
                                                       -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/ApplicationServices.framework/Frameworks/CoreGraphics.framework/Headers/ \
                                                       -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/UIKit.framework/Headers/ \
                                                       -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/QuartzCore.framework/Headers/
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            := sources/common \
                                                       sources/driver \
                                                       sources/render_target_manager \
                                                       sources/render_target_manager/fbo_manager \
                                                       sources/input_stage \
                                                       sources/texture_manager \
                                                       sources/texture_manager/nv_dxt \
                                                       sources/shader_stage \
                                                       sources/shader_stage/fpp \
                                                       sources/output_stage \
                                                       sources/query_manager \
                                                       sources/render_target_manager/swap_chain_manager
RENDER.OPENGL_DRIVER.SOURCES.LIB_DIRS               :=  
RENDER.OPENGL_DRIVER.SOURCES.LIBS                   := 
RENDER.OPENGL_DRIVER.SOURCES.COMPILER_DEFINES       :=
RENDER.OPENGL_DRIVER.SOURCES.IMPORTS                := ../low_level/compile.static.mak ../../common/compile.static.mak ../../system/compile.static.mak
RENDER.OPENGL_DRIVER.SOURCES.msvc.COMPILER_CFLAGS   := -wd4355

RENDER.OPENGL_DRIVER.SOURCES.gles.COMPILER_DEFINES := OPENGL_ES_SUPPORT
RENDER.OPENGL_DRIVER.SOURCES.iphone.SOURCE_DIRS    := sources/platform/iphone
RENDER.OPENGL_DRIVER.SOURCES.egl.SOURCE_DIRS       := sources/platform/egl
RENDER.OPENGL_DRIVER.SOURCES.egl.IMPORTS           := ../../../extern/gles_win32/compile.mak

ifeq (,$(filter gles,$(PROFILES)))

RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS        +=  \
                                                   sources/shader_stage/glsl
RENDER.OPENGL_DRIVER.SOURCES.win32.SOURCE_DIRS  := sources/platform/win32
RENDER.OPENGL_DRIVER.SOURCES.macosx.SOURCE_DIRS := sources/platform/macosx

endif

#OpenGL render system tests
RENDER.OPENGL_DRIVER.TESTS.TYPE             := test-suite
RENDER.OPENGL_DRIVER.TESTS.INCLUDE_DIRS     :=
RENDER.OPENGL_DRIVER.TESTS.SOURCE_DIRS      := tests/driver tests/device tests/complex tests/unit
RENDER.OPENGL_DRIVER.TESTS.LIB_DIRS         :=
RENDER.OPENGL_DRIVER.TESTS.LIBS             :=
RENDER.OPENGL_DRIVER.TESTS.COMPILER_CFLAGS  :=
RENDER.OPENGL_DRIVER.TESTS.COMPILER_DEFINES := 
RENDER.OPENGL_DRIVER.TESTS.IMPORTS          := link.static.mak run.static.mak ../low_level/compile.static.mak \
                                               ../../system/compile.static.mak ../../system/link.static.mak ../../common/link.static.mak \
                                               ../../media/image/compile.static.mak ../../media/geometry/compile.static.mak \
                                               ../../media/geometry/compile.static.mak  ../../media/image/link.static.mak \
                                               ../../media/geometry/xmesh.link.static.mak
