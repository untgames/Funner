###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.JPEGLIB EXTERN.TIFFLIB EXTERN.DEVIL EXTERN.ILU

#Цель №1 - JPEGLIB sources
EXTERN.JPEGLIB.TYPE                 := static-lib
EXTERN.JPEGLIB.NAME                 := funner.extern.jpeglib
EXTERN.JPEGLIB.INCLUDE_DIRS         := components/jpeg/include
EXTERN.JPEGLIB.SOURCE_DIRS          := components/jpeg/sources
EXTERN.JPEGLIB.LIB_DIRS             :=
EXTERN.JPEGLIB.LIBS                 :=
EXTERN.JPEGLIB.COMPILER_CFLAGS      :=
EXTERN.JPEGLIB.g++.COMPILER_CFLAGS  := --no-warn

#Цель №2 - TIFFLIB sources
EXTERN.TIFFLIB.TYPE             := static-lib
EXTERN.TIFFLIB.NAME             := funner.extern.tifflib
EXTERN.TIFFLIB.INCLUDE_DIRS     := components/tiff/include components/jpeg/include ../zlib/include
EXTERN.TIFFLIB.SOURCE_DIRS      := components/tiff/sources
EXTERN.TIFFLIB.LIB_DIRS         :=  
EXTERN.TIFFLIB.LIBS             :=
EXTERN.TIFFLIB.msvc.COMPILER_CFLAGS := -wd4013
EXTERN.TIFFLIB.COMPILER_DEFINES :=
EXTERN.TIFFLIB.g++.COMPILER_CFLAGS := --no-warn

#Цель №3 - DevILDLL sources
EXTERN.DEVIL.TYPE                 := static-lib
EXTERN.DEVIL.NAME                 := funner.extern.devil
EXTERN.DEVIL.INCLUDE_DIRS         := include components/il/include/il components/jpeg/include components/tiff/include components/il/include ../zlib/include
EXTERN.DEVIL.SOURCE_DIRS          := components/il/sources
EXTERN.DEVIL.COMPILER_DEFINES     := IL_NO_LCMS IL_NO_MNG IL_NO_PNG IL_STATIC_LIB XMD_H HAVE_CONFIG_H
EXTERN.DEVIL.msvc.COMPILER_CFLAGS := -wd4142 -wd4005 -wd4018
EXTERN.DEVIL.g++.COMPILER_CFLAGS  := --no-warn

#Цель №4 - ILUDLL sources
EXTERN.ILU.TYPE                 := static-lib
EXTERN.ILU.NAME                 := funner.extern.ilu
EXTERN.ILU.INCLUDE_DIRS         := include components/il/include/il components/jpeg/include components/tiff/include components/il/include components/ilu/include ../zlib/include
EXTERN.ILU.SOURCE_DIRS          := components/ilu/sources
EXTERN.ILU.COMPILER_DEFINES     := IL_STATIC_LIB HAVE_CONFIG_H
EXTERN.ILU.msvc.COMPILER_CFLAGS := -wd4090
EXTERN.ILU.g++.COMPILER_CFLAGS  := --no-warn
