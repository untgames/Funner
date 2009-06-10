###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.DEVIL EXTERN.ILU

#Цель - DevILDLL sources
EXTERN.DEVIL.TYPE                   := static-lib
EXTERN.DEVIL.NAME                   := funner.extern.devil
EXTERN.DEVIL.INCLUDE_DIRS           := include components/il/include/IL ../jpeg/include ../tiff/include ../libpng/include components/il/include ../zlib/include
EXTERN.DEVIL.SOURCE_DIRS            := components/il/sources
EXTERN.DEVIL.COMPILER_DEFINES       := IL_STATIC_LIB HAVE_CONFIG_H
EXTERN.DEVIL.linux.COMPILER_DEFINES := MM_MALLOC
EXTERN.DEVIL.msvc.COMPILER_CFLAGS   := -wd4101
EXTERN.DEVIL.g++.COMPILER_CFLAGS    := --no-warn

#Цель - ILUDLL sources
EXTERN.ILU.TYPE                 := static-lib
EXTERN.ILU.NAME                 := funner.extern.ilu
EXTERN.ILU.INCLUDE_DIRS         := include components/il/include/IL ../jpeg/include ../tiff/include components/il/include components/ilu/include ../zlib/include
EXTERN.ILU.SOURCE_DIRS          := components/ilu/sources
EXTERN.ILU.COMPILER_DEFINES     := IL_STATIC_LIB HAVE_CONFIG_H
EXTERN.ILU.g++.COMPILER_CFLAGS  := --no-warn
