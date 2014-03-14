###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.OPENGL_DRIVER.UTILS RENDER.OPENGL_DRIVER.SOURCES RENDER.OPENGL_DRIVER.TESTS

#PROFILES += egl gles
PROFILES += egl gles2

#OpenGL render system utilities
RENDER.OPENGL_DRIVER.UTILS.TYPE        := test-suite
RENDER.OPENGL_DRIVER.UTILS.SOURCE_DIRS := utils
RENDER.OPENGL_DRIVER.UTILS.IMPORTS     := compile.common link.common

#OpenGL render system sources
RENDER.OPENGL_DRIVER.SOURCES.TYPE                   := static-lib
RENDER.OPENGL_DRIVER.SOURCES.NAME                   := funner.render.low_level.opengl_driver
RENDER.OPENGL_DRIVER.SOURCES.INCLUDE_DIRS           := sources
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/common
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/driver
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/render_target_manager
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/render_target_manager/fbo_manager
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/input_stage
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/texture_manager
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/texture_manager/nv_dxt
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/shader_stage
ifeq (,$(filter gles2,$(PROFILES)))
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/shader_stage/ffp
endif
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/output_stage
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/query_manager
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS            += sources/render_target_manager/swap_chain_manager
RENDER.OPENGL_DRIVER.SOURCES.IMPORTS                := compile.render.low_level compile.render.low_level.helpers compile.common compile.system
RENDER.OPENGL_DRIVER.SOURCES.msvc.COMPILER_CFLAGS   := -wd4355
RENDER.OPENGL_DRIVER.SOURCES.gles.COMPILER_DEFINES  := OPENGL_ES_SUPPORT
RENDER.OPENGL_DRIVER.SOURCES.gles2.COMPILER_DEFINES := OPENGL_ES2_SUPPORT
#RENDER.OPENGL_DRIVER.SOURCES.meego.COMPILER_DEFINES := NO_XRAND NO_X86VMODE
RENDER.OPENGL_DRIVER.SOURCES.linux.COMPILER_DEFINES := NO_XRAND NO_X86VMODE
RENDER.OPENGL_DRIVER.SOURCES.iphone.SOURCE_DIRS     := sources/platform/iphone
RENDER.OPENGL_DRIVER.SOURCES.beagleboard.IMPORTS    := compile.extern.beagleboard
RENDER.OPENGL_DRIVER.SOURCES.meego.IMPORTS          := compile.extern.meego
RENDER.OPENGL_DRIVER.SOURCES.egl.SOURCE_DIRS        := sources/platform/egl
RENDER.OPENGL_DRIVER.SOURCES.glx.SOURCE_DIRS        := sources/platform/glx
RENDER.OPENGL_DRIVER.SOURCES.android.SOURCE_DIRS    := sources/platform/egl/android
RENDER.OPENGL_DRIVER.SOURCES.tabletos.SOURCE_DIRS   := sources/platform/egl/tabletos

ifeq (,$(filter gles,$(PROFILES)))

#OpenGL & OpenGL ES 2

RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS  += sources/shader_stage/glsl

ifeq (,$(filter gles2,$(PROFILES)))

#OpenGL

RENDER.OPENGL_DRIVER.SOURCES.win32.SOURCE_DIRS  := sources/platform/win32
RENDER.OPENGL_DRIVER.SOURCES.macosx.SOURCE_DIRS := sources/platform/macosx

else

#OpenGL ES 2

RENDER.OPENGL_DRIVER.SOURCES.win32.IMPORTS  := compile.extern.gles2_win32

endif

else

#OpenGL ES

RENDER.OPENGL_DRIVER.SOURCES.win32.IMPORTS  := compile.extern.gles_win32
RENDER.OPENGL_DRIVER.SOURCES.wince.IMPORTS  := compile.extern.gles_wince

endif

#OpenGL render system tests
RENDER.OPENGL_DRIVER.TESTS.TYPE             := test-suite
RENDER.OPENGL_DRIVER.TESTS.SOURCE_DIRS      := tests/driver tests/device tests/complex tests/unit
RENDER.OPENGL_DRIVER.TESTS.LIBS             :=
RENDER.OPENGL_DRIVER.TESTS.IMPORTS          := link.render.low_level.opengl_driver run.render.low_level.opengl_driver compile.render.low_level \
                                               compile.system link.system link.common compile.media.image compile.media.geometry \
                                               link.media.image link.media.image.dds link.media.geometry.xmesh link.media.image.pvr
