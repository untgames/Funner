###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEVIL

#Цель №1 - DevIL sources
DEVIL.TYPE             := static-lib
DEVIL.NAME             := devil
DEVIL.INCLUDE_DIRS     := include include/il components/jpeg/include components/tiff/include components/il/include components/ilu/include ../zlib/include
DEVIL.SOURCE_DIRS      := components/jpeg/sources components/tiff/sources components/il/sources components/ilu/sources
DEVIL.LIB_DIRS         :=  
DEVIL.LIBS             := 
DEVIL.COMPILER_CFLAGS  :=
DEVIL.COMPILER_DEFINES := IL_STATIC_LIB
