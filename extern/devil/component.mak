###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEVIL_EXTERN_LIBS DEVIL_COMPONENTS

#Цель №1 - DevIL external libraries
DEVIL_EXTERN_LIBS.TYPE       := package
DEVIL_EXTERN_LIBS.COMPONENTS := extern/jpeg extern/tiff

#Цель №2 - DevIL sources
DEVIL_COMPONENTS.TYPE             := static-lib
DEVIL_COMPONENTS.NAME             := devil
DEVIL_COMPONENTS.INCLUDE_DIRS     := include include/IL extern/jpeg/include extern/tiff/include
DEVIL_COMPONENTS.SOURCE_DIRS      := sources
DEVIL_COMPONENTS.LIB_DIRS         :=  
DEVIL_COMPONENTS.LIBS             := 
DEVIL_COMPONENTS.COMPILER_CFLAGS  :=
DEVIL_COMPONENTS.COMPILER_DEFINES :=
