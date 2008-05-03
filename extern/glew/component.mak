###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := GLEW_SOURCES_STATIC

#Цель №1 - GLEW sources
GLEW_SOURCES_STATIC.TYPE             := static-lib
GLEW_SOURCES_STATIC.NAME             := extern.glew_static
GLEW_SOURCES_STATIC.INCLUDE_DIRS     := include
GLEW_SOURCES_STATIC.SOURCE_DIRS      := sources
GLEW_SOURCES_STATIC.LIB_DIRS         := 
GLEW_SOURCES_STATIC.LIBS             :=
GLEW_SOURCES_STATIC.COMPILER_CFLAGS  :=
GLEW_SOURCES_STATIC.COMPILER_DEFINES := GLEW_STATIC GLEW_MX
