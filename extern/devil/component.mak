###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.JPEGLIB EXTERN.TIFFLIB EXTERN.PNGLIB EXTERN.DEVIL EXTERN.ILU

#Цель - JPEGLIB sources
EXTERN.JPEGLIB.TYPE                 := static-lib
EXTERN.JPEGLIB.NAME                 := funner.extern.jpeglib
EXTERN.JPEGLIB.INCLUDE_DIRS         := components/jpeg/include
EXTERN.JPEGLIB.SOURCE_DIRS          := components/jpeg/sources
EXTERN.JPEGLIB.LIB_DIRS             :=
EXTERN.JPEGLIB.LIBS                 :=
EXTERN.JPEGLIB.COMPILER_CFLAGS      :=
EXTERN.JPEGLIB.g++.COMPILER_CFLAGS  := --no-warn

#Цель - TIFFLIB sources
EXTERN.TIFFLIB.TYPE             := static-lib
EXTERN.TIFFLIB.NAME             := funner.extern.tifflib
EXTERN.TIFFLIB.INCLUDE_DIRS     := components/tiff/include components/jpeg/include ../zlib/include
EXTERN.TIFFLIB.SOURCE_DIRS      := components/tiff/sources
EXTERN.TIFFLIB.LIB_DIRS         :=  
EXTERN.TIFFLIB.LIBS             :=
EXTERN.TIFFLIB.msvc.COMPILER_CFLAGS := -wd4013
EXTERN.TIFFLIB.COMPILER_DEFINES :=
EXTERN.TIFFLIB.g++.COMPILER_CFLAGS := --no-warn

#Цель - PNGLIB sources
EXTERN.PNGLIB.TYPE             := static-lib
EXTERN.PNGLIB.NAME             := funner.extern.pnglib
EXTERN.PNGLIB.INCLUDE_DIRS     := components/png/include ../zlib/include
EXTERN.PNGLIB.SOURCE_DIRS      := components/png/sources

#Цель - DevILDLL sources
EXTERN.DEVIL.TYPE                    := static-lib
EXTERN.DEVIL.NAME                    := funner.extern.devil
EXTERN.DEVIL.INCLUDE_DIRS            := include components/il/include/IL components/jpeg/include components/tiff/include components/png/include components/il/include ../zlib/include
EXTERN.DEVIL.SOURCE_DIRS             := components/il/sources
EXTERN.DEVIL.COMPILER_DEFINES        := IL_STATIC_LIB HAVE_CONFIG_H
EXTERN.DEVIL.ubuntu.COMPILER_DEFINES := MM_MALLOC
EXTERN.DEVIL.msvc.COMPILER_CFLAGS    := -wd4101
EXTERN.DEVIL.g++.COMPILER_CFLAGS     := --no-warn

#Цель - ILUDLL sources
EXTERN.ILU.TYPE                 := static-lib
EXTERN.ILU.NAME                 := funner.extern.ilu
EXTERN.ILU.INCLUDE_DIRS         := include components/il/include/IL components/jpeg/include components/tiff/include components/il/include components/ilu/include ../zlib/include
EXTERN.ILU.SOURCE_DIRS          := components/ilu/sources
EXTERN.ILU.COMPILER_DEFINES     := IL_STATIC_LIB HAVE_CONFIG_H
EXTERN.ILU.g++.COMPILER_CFLAGS  := --no-warn
