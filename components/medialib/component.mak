###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIALIB_COMPONENTS

#Цель №1 - MediaLib sources
MEDIALIB_COMPONENTS.TYPE             := static-lib
MEDIALIB_COMPONENTS.NAME             := medialib
MEDIALIB_COMPONENTS.INCLUDE_DIRS     := include ../../extern/devil/include ../commonlib/include ../xtl/include ../../extern/vorbisfile/include ../../extern/ogg/include
MEDIALIB_COMPONENTS.SOURCE_DIRS      := sources/imagelib sources/soundlib
MEDIALIB_COMPONENTS.LIB_DIRS         :=  
MEDIALIB_COMPONENTS.LIBS             := 
MEDIALIB_COMPONENTS.COMPILER_CFLAGS  :=
MEDIALIB_COMPONENTS.COMPILER_DEFINES := IL_STATIC_LIB
