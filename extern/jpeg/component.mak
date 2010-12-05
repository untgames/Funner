###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.JPEG

#Цель - JPEG sources
EXTERN.JPEG.TYPE                 := static-lib
EXTERN.JPEG.NAME                 := funner.extern.jpeg
EXTERN.JPEG.INCLUDE_DIRS         := include
EXTERN.JPEG.SOURCE_DIRS          := sources
EXTERN.JPEG.g++.COMPILER_CFLAGS  := --no-warn
