###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := OPENGLDRIVER_SOURCES

#Цель №1 - RENDER system sources
OPENGLDRIVER_SOURCES.TYPE             := static-lib
OPENGLDRIVER_SOURCES.NAME             := opengl_driver
OPENGLDRIVER_SOURCES.INCLUDE_DIRS     := include sources/shared ../lowlevel/include ../../commonlib/include ../../xtl/include ../../../extern/glew/include
OPENGLDRIVER_SOURCES.SOURCE_DIRS      := sources/driver sources/platform/win32
OPENGLDRIVER_SOURCES.LIB_DIRS         :=  
OPENGLDRIVER_SOURCES.LIBS             := 
OPENGLDRIVER_SOURCES.COMPILER_CFLAGS  :=
OPENGLDRIVER_SOURCES.COMPILER_DEFINES := 
