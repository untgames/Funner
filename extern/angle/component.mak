TARGETS += EXTERN.ANGLE.GLES2
TARGETS += EXTERN.ANGLE.EGL
TARGETS += EXTERN.ANGLE.TESTS.COMMON
TARGETS += EXTERN.ANGLE.TESTS.SAMPLES

EXTERN.ANGLE.GLES2.TYPE              := dynamic-lib
EXTERN.ANGLE.GLES2.NAME              := libglesv2
EXTERN.ANGLE.GLES2.DEF_FILE          := sources/libGLESv2/libGLESv2.def
EXTERN.ANGLE.GLES2.INCLUDE_DIRS      += sources
EXTERN.ANGLE.GLES2.INCLUDE_DIRS      += dxsdk/include
EXTERN.ANGLE.GLES2.SOURCE_DIRS       += sources/libGLESv2
EXTERN.ANGLE.GLES2.SOURCE_DIRS       += sources/common
EXTERN.ANGLE.GLES2.SOURCE_DIRS       += sources/compiler
EXTERN.ANGLE.GLES2.SOURCE_DIRS       += sources/compiler/preprocessor
EXTERN.ANGLE.GLES2.SOURCE_DIRS       += sources/compiler/preprocessor/new
EXTERN.ANGLE.GLES2.SOURCE_DIRS       += sources/compiler/timing
EXTERN.ANGLE.GLES2.SOURCE_DIRS       += sources/compiler/depgraph
EXTERN.ANGLE.GLES2.win32.SOURCE_DIRS += sources/compiler/win
EXTERN.ANGLE.GLES2.COMPILER_DEFINES  += LIBGLESV2_EXPORTS
EXTERN.ANGLE.GLES2.COMPILER_DEFINES  += NOMINMAX
EXTERN.ANGLE.GLES2.LIBS              += d3d9 D3dx9 d3dcompiler
EXTERN.ANGLE.GLES2.IMPORTS           += compile.extern.angle compile.extern.dxsdk link.extern.dxsdk

EXTERN.ANGLE.EGL.TYPE             := dynamic-lib
EXTERN.ANGLE.EGL.NAME             := libegl
EXTERN.ANGLE.EGL.DEF_FILE         := sources/libEGL/libEGL.def
EXTERN.ANGLE.EGL.INCLUDE_DIRS     += sources
EXTERN.ANGLE.EGL.INCLUDE_DIRS     += dxsdk/include
EXTERN.ANGLE.EGL.SOURCE_DIRS      += sources/libEGL
EXTERN.ANGLE.EGL.SOURCE_DIRS      += sources/common
EXTERN.ANGLE.EGL.COMPILER_DEFINES += LIBEGL_EXPORTS
EXTERN.ANGLE.EGL.COMPILER_DEFINES += NOMINMAX
EXTERN.ANGLE.EGL.LIBS             += d3d9 dxguid user32 gdi32 dwmapi funner.extern.libglesv2
EXTERN.ANGLE.EGL.IMPORTS          += compile.extern.angle compile.extern.dxsdk link.extern.dxsdk

EXTERN.ANGLE.TESTS.COMMON.TYPE              := static-lib
EXTERN.ANGLE.TESTS.COMMON.NAME              := funner.extern.angle.tests-common
EXTERN.ANGLE.TESTS.COMMON.INCLUDE_DIRS      += tests/common
EXTERN.ANGLE.TESTS.COMMON.SOURCE_DIRS       += tests/common
EXTERN.ANGLE.TESTS.COMMON.win32.SOURCE_DIRS += tests/common/Win32
EXTERN.ANGLE.TESTS.COMMON.IMPORTS     	    += compile.extern.angle

EXTERN.ANGLE.TESTS.SAMPLES.TYPE         := test-suite
EXTERN.ANGLE.TESTS.SAMPLES.INCLUDE_DIRS += tests/common
EXTERN.ANGLE.TESTS.SAMPLES.SOURCE_DIRS  += tests/samples
EXTERN.ANGLE.TESTS.SAMPLES.LIBS         += funner.extern.angle.tests-common user32
EXTERN.ANGLE.TESTS.SAMPLES.win32.LIBS   += gdi32
EXTERN.ANGLE.TESTS.SAMPLES.IMPORTS      += compile.extern.angle link.extern.angle
