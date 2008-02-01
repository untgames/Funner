###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := OPENGLDRIVER_SOURCES OPENGLDRIVER_TESTS

#Цель №1 - OpenGL render system sources
OPENGLDRIVER_SOURCES.TYPE             := static-lib
OPENGLDRIVER_SOURCES.NAME             := opengl_driver
OPENGLDRIVER_SOURCES.INCLUDE_DIRS     := include sources ../lowlevel/include ../../commonlib/include ../../xtl/include ../../../extern/glew/include
OPENGLDRIVER_SOURCES.SOURCE_DIRS      := sources/common sources/platform/win32 sources/driver sources/input_stage sources/output_stage sources/texture_manager sources/texture_manager/nv_dxt
OPENGLDRIVER_SOURCES.LIB_DIRS         :=  
OPENGLDRIVER_SOURCES.LIBS             := 
OPENGLDRIVER_SOURCES.COMPILER_CFLAGS  := /wd4355
OPENGLDRIVER_SOURCES.COMPILER_DEFINES := GLEW_STATIC GLEW_MX

#Цель №2 - OpenGL render system tests
OPENGLDRIVER_TESTS.TYPE             := test-suite
OPENGLDRIVER_TESTS.INCLUDE_DIRS     := include sources ../lowlevel/include ../../commonlib/include ../../xtl/include ../../systemlib/include
OPENGLDRIVER_TESTS.SOURCE_DIRS      := tests/driver tests/device
OPENGLDRIVER_TESTS.LIB_DIRS         :=
OPENGLDRIVER_TESTS.LIBS             := commonlib zzip zlib system glew_static render_ll opengl_driver user32 gdi32 opengl32
OPENGLDRIVER_TESTS.COMPILER_CFLAGS  :=
OPENGLDRIVER_TESTS.COMPILER_DEFINES := 
