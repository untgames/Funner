###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.OPENGL_DRIVER.UTILS RENDER.OPENGL_DRIVER.SOURCES RENDER.OPENGL_DRIVER.TESTS

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
RENDER.OPENGL_DRIVER.SOURCES.TYPE             := static-lib
RENDER.OPENGL_DRIVER.SOURCES.NAME             := render.low_level.opengl_driver
RENDER.OPENGL_DRIVER.SOURCES.INCLUDE_DIRS     := sources sources/platform/win32/ddk
RENDER.OPENGL_DRIVER.SOURCES.SOURCE_DIRS      := sources/common sources/driver sources/input_stage \
                                                 sources/render_target_manager sources/render_target_manager/swap_chain_manager \
                                                 sources/render_target_manager/fbo_manager \
                                                 sources/texture_manager sources/texture_manager/nv_dxt sources/shader_stage \
                                                 sources/shader_stage/fpp sources/shader_stage/glsl sources/query_manager sources/output_stage
RENDER.OPENGL_DRIVER.SOURCES.LIB_DIRS         :=  
RENDER.OPENGL_DRIVER.SOURCES.LIBS             := 
RENDER.OPENGL_DRIVER.SOURCES.COMPILER_DEFINES := GLEW_STATIC GLEW_MX
RENDER.OPENGL_DRIVER.SOURCES.IMPORTS          := ../low_level/compile.static.mak ../../common/compile.static.mak ../../system/compile.static.mak
RENDER.OPENGL_DRIVER.SOURCES.msvc.COMPILER_CFLAGS  := -wd4355
RENDER.OPENGL_DRIVER.SOURCES.win32.SOURCE_DIRS := sources/platform/win32

#OpenGL render system tests
RENDER.OPENGL_DRIVER.TESTS.TYPE             := test-suite
RENDER.OPENGL_DRIVER.TESTS.INCLUDE_DIRS     :=
RENDER.OPENGL_DRIVER.TESTS.SOURCE_DIRS      := tests/driver tests/device tests/complex
RENDER.OPENGL_DRIVER.TESTS.LIB_DIRS         :=
RENDER.OPENGL_DRIVER.TESTS.LIBS             :=
RENDER.OPENGL_DRIVER.TESTS.COMPILER_CFLAGS  :=
RENDER.OPENGL_DRIVER.TESTS.COMPILER_DEFINES := 
RENDER.OPENGL_DRIVER.TESTS.IMPORTS          := link.static.mak ../low_level/compile.static.mak \
                                       ../../system/compile.static.mak ../../system/link.static.mak ../../common/link.static.mak \
                                       ../../media/image/compile.static.mak ../../media/geometry/compile.static.mak \
                                       ../../media/geometry/compile.static.mak  ../../media/image/link.static.mak \
                                       ../../media/geometry/xmesh.link.static.mak
