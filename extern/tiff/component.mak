###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.TIFF

#Цель - TIFF sources
EXTERN.TIFF.TYPE                 := static-lib
EXTERN.TIFF.NAME                 := funner.extern.tiff
EXTERN.TIFF.INCLUDE_DIRS         := include ../jpeg/include ../zlib/include
EXTERN.TIFF.SOURCE_DIRS          := sources
EXTERN.TIFF.unistd.SOURCE_DIRS   := sources/unix
EXTERN.TIFF.win32.SOURCE_DIRS    := sources/win32
EXTERN.TIFF.msvc.COMPILER_CFLAGS := -wd4013
EXTERN.TIFF.g++.COMPILER_CFLAGS  := --no-warn
