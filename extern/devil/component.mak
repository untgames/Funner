###################################################################################################
#Определения и константы
###################################################################################################
#TARGETS := DEVIL
TARGETS := JPEGLIB TIFFLIB DEVILDLL ILUDLL

#Цель №1 - DevIL sources
#DEVIL.TYPE             := static-lib
#DEVIL.NAME             := devil
#DEVIL.INCLUDE_DIRS     := include include/il components/jpeg/include components/tiff/include components/il/include components/ilu/include ../zlib/include
#DEVIL.SOURCE_DIRS      := components/jpeg/sources components/tiff/sources components/il/sources components/ilu/sources
#DEVIL.LIB_DIRS         :=  
#DEVIL.LIBS             := 
#DEVIL.COMPILER_CFLAGS  :=
#DEVIL.COMPILER_DEFINES := IL_STATIC_LIB

#Цель №1 - JPEGLIB sources
JPEGLIB.TYPE             := static-lib
JPEGLIB.NAME             := jpeglib
JPEGLIB.INCLUDE_DIRS     := components/jpeg/include
JPEGLIB.SOURCE_DIRS      := components/jpeg/sources
JPEGLIB.LIB_DIRS         :=  
JPEGLIB.LIBS             := 
JPEGLIB.COMPILER_CFLAGS  :=
JPEGLIB.COMPILER_DEFINES :=

#Цель №2 - TIFFLIB sources
TIFFLIB.TYPE             := static-lib
TIFFLIB.NAME             := tifflib
TIFFLIB.INCLUDE_DIRS     := components/tiff/include components/jpeg/include ../zlib/include
TIFFLIB.SOURCE_DIRS      := components/tiff/sources
TIFFLIB.LIB_DIRS         :=  
TIFFLIB.LIBS             := zlib
TIFFLIB.COMPILER_CFLAGS  :=
TIFFLIB.COMPILER_DEFINES :=

#Цель №3 - DevILDLL sources
DEVILDLL.TYPE             := dynamic-lib
DEVILDLL.NAME             := devil
DEVILDLL.INCLUDE_DIRS     := include include/il components/jpeg/include components/tiff/include components/il/include ../zlib/include
DEVILDLL.SOURCE_DIRS      := components/il/sources
DEVILDLL.LIB_DIRS         :=  
DEVILDLL.LIBS             := jpeglib tifflib zlib
DEVILDLL.COMPILER_CFLAGS  :=
DEVILDLL.COMPILER_DEFINES :=

#Цель №4 - ILUDLL sources
ILUDLL.TYPE             := dynamic-lib
ILUDLL.NAME             := ilu
ILUDLL.INCLUDE_DIRS     := include include/il components/jpeg/include components/tiff/include components/il/include components/ilu/include ../zlib/include
ILUDLL.SOURCE_DIRS      := components/ilu/sources
ILUDLL.LIB_DIRS         :=  
ILUDLL.LIBS             := devil
ILUDLL.COMPILER_CFLAGS  :=
ILUDLL.COMPILER_DEFINES :=
