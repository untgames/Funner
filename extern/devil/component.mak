###################################################################################################
#Определения и константы
###################################################################################################
#TARGETS := DEVIL
TARGETS := JPEGLIB TIFFLIB DEVIL ILU

#Цель №1 - JPEGLIB sources
JPEGLIB.TYPE                 := static-lib
JPEGLIB.NAME                 := extern.jpeglib
JPEGLIB.INCLUDE_DIRS         := components/jpeg/include
JPEGLIB.SOURCE_DIRS          := components/jpeg/sources
JPEGLIB.LIB_DIRS             :=
JPEGLIB.LIBS                 :=
JPEGLIB.COMPILER_CFLAGS      :=
JPEGLIB.g++.COMPILER_CFLAGS  := --no-warn

#Цель №2 - TIFFLIB sources
TIFFLIB.TYPE             := static-lib
TIFFLIB.NAME             := extern.tifflib
TIFFLIB.INCLUDE_DIRS     := components/tiff/include components/jpeg/include ../zlib/include
TIFFLIB.SOURCE_DIRS      := components/tiff/sources
TIFFLIB.LIB_DIRS         :=  
TIFFLIB.LIBS             :=
TIFFLIB.COMPILER_CFLAGS  :=
TIFFLIB.COMPILER_DEFINES :=

#Цель №3 - DevILDLL sources
DEVIL.TYPE                 := dynamic-lib
DEVIL.NAME                 := extern.devil
DEVIL.INCLUDE_DIRS         := include include/il components/jpeg/include components/tiff/include components/il/include ../zlib/include
DEVIL.SOURCE_DIRS          := components/il/sources
DEVIL.LIB_DIRS             :=
DEVIL.LIBS                 := extern.jpeglib extern.tifflib extern.zlib kernel32
DEVIL.COMPILER_DEFINES     :=
DEVIL.msvc.COMPILER_CFLAGS := -wd4142 -wd4005 -wd4018
DEVIL.g++.COMPILER_CFLAGS  := --no-warn
DEVIL.g++.COMPILER_DEFINES := XMD_H

#Цель №4 - ILUDLL sources
ILU.TYPE                 := dynamic-lib
ILU.NAME                 := extern.ilu
ILU.INCLUDE_DIRS         := include include/il components/jpeg/include components/tiff/include components/il/include components/ilu/include ../zlib/include
ILU.SOURCE_DIRS          := components/ilu/sources
ILU.LIB_DIRS             :=
ILU.LIBS                 := extern.devil
ILU.msvc.COMPILER_CFLAGS := -wd4090
ILU.g++.COMPILER_CFLAGS  := --no-warn
ILU.COMPILER_DEFINES     :=

#Цель №5 - DevIL sources
#DEVIL.TYPE             := static-lib
#DEVIL.NAME             := devil
#DEVIL.INCLUDE_DIRS     := include include/il components/jpeg/include components/tiff/include components/il/include components/ilu/include ../zlib/include
#DEVIL.SOURCE_DIRS      := components/jpeg/sources components/tiff/sources components/il/sources components/ilu/sources
#DEVIL.LIB_DIRS         :=  
#DEVIL.LIBS             := 
#DEVIL.COMPILER_CFLAGS  :=
#DEVIL.COMPILER_DEFINES := IL_STATIC_LIB
