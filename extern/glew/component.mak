###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.GLEW_SOURCES_STATIC

#Цель №1 - GLEW sources
EXTERN.GLEW_SOURCES_STATIC.TYPE             := static-lib
EXTERN.GLEW_SOURCES_STATIC.NAME             := extern.glew_static
EXTERN.GLEW_SOURCES_STATIC.INCLUDE_DIRS     := include
EXTERN.GLEW_SOURCES_STATIC.SOURCE_DIRS      := sources
EXTERN.GLEW_SOURCES_STATIC.LIB_DIRS         := 
EXTERN.GLEW_SOURCES_STATIC.LIBS             :=
EXTERN.GLEW_SOURCES_STATIC.COMPILER_CFLAGS  :=
EXTERN.GLEW_SOURCES_STATIC.COMPILER_DEFINES := GLEW_STATIC GLEW_MX
