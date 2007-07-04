###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIALIB_EXTERN_LIBS MEDIALIB_COMPONENTS

#Цель №1 - MediaLib external libraries
MEDIALIB_EXTERN_LIBS.TYPE       := package
MEDIALIB_EXTERN_LIBS.COMPONENTS := extern/devil extern/devilu

#Цель №2 - MediaLib sources
MEDIALIB_COMPONENTS.TYPE             := static-lib
MEDIALIB_COMPONENTS.NAME             := medialib
MEDIALIB_COMPONENTS.INCLUDE_DIRS     := include extern/devil/include extern/devilu/include ../commonlib/include ../xtl/include
MEDIALIB_COMPONENTS.SOURCE_DIRS      := sources/imagelib sources/soundlib
MEDIALIB_COMPONENTS.LIB_DIRS         :=  
MEDIALIB_COMPONENTS.LIBS             := 
MEDIALIB_COMPONENTS.COMPILER_CFLAGS  :=
MEDIALIB_COMPONENTS.COMPILER_DEFINES :=
