###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.PNG

#Цель - PNG sources
EXTERN.PNG.TYPE         := static-lib
EXTERN.PNG.NAME         := funner.extern.libpng
EXTERN.PNG.INCLUDE_DIRS := include ../zlib/include
EXTERN.PNG.SOURCE_DIRS  := sources
