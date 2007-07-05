###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEVIL

#Цель №1 - DevIL sources
DEVIL.TYPE             := static-lib
DEVIL.NAME             := devil
DEVIL.INCLUDE_DIRS     := include include/il sources/jpeg/include sources/tiff/include sources/il/include sources/ilu/include ../zlib/include
DEVIL.SOURCE_DIRS      := sources/jpeg/sources sources/tiff/sources sources/il/sources sources/ilu/sources
DEVIL.LIB_DIRS         :=  
DEVIL.LIBS             := 
DEVIL.COMPILER_CFLAGS  :=
DEVIL.COMPILER_DEFINES := IL_STATIC_LIB
