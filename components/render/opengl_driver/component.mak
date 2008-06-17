###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := OPENGLDRIVER_SOURCES OPENGLDRIVER_TESTS

#Цель №1 - OpenGL render system sources
OPENGLDRIVER_SOURCES.TYPE             := static-lib
OPENGLDRIVER_SOURCES.NAME             := render.low_level.opengl_driver
OPENGLDRIVER_SOURCES.INCLUDE_DIRS     := sources ../../../extern/glew/include
OPENGLDRIVER_SOURCES.SOURCE_DIRS      := sources/common sources/platform/win32 sources/driver sources/input_stage \
                                         sources/output_stage sources/output_stage/swap_chain_manager sources/output_stage/fbo_manager \
                                         sources/texture_manager sources/texture_manager/nv_dxt sources/rasterizer_stage sources/shader_stage \
                                         sources/shader_stage/glsl sources/shader_stage/fpp sources/query_manager
OPENGLDRIVER_SOURCES.LIB_DIRS         :=  
OPENGLDRIVER_SOURCES.LIBS             := 
OPENGLDRIVER_SOURCES.COMPILER_DEFINES := GLEW_STATIC GLEW_MX
OPENGLDRIVER_SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak
OPENGLDRIVER_SOURCES.msvc.COMPILER_CFLAGS  := -wd4355

#Цель №2 - OpenGL render system tests
OPENGLDRIVER_TESTS.TYPE             := test-suite
OPENGLDRIVER_TESTS.INCLUDE_DIRS     :=
OPENGLDRIVER_TESTS.SOURCE_DIRS      := tests/driver tests/device tests/complex
OPENGLDRIVER_TESTS.LIB_DIRS         :=
OPENGLDRIVER_TESTS.LIBS             :=
OPENGLDRIVER_TESTS.COMPILER_CFLAGS  :=
OPENGLDRIVER_TESTS.COMPILER_DEFINES := 
OPENGLDRIVER_TESTS.IMPORTS          := link.static.mak compile.static.mak \
                                       ../../system/compile.static.mak ../../system/link.static.mak ../../common/link.static.mak \
                                       ../../media/image/compile.static.mak ../../media/geometry/compile.static.mak \
                                       ../../media/geometry/compile.static.mak  ../../media/image/link.static.mak \
                                       ../../media/geometry/xmesh.link.static.mak
