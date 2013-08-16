###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.TIFF

#Цель - TIFF sources
EXTERN.TIFF.TYPE                   := static-lib
EXTERN.TIFF.NAME                   := funner.extern.tiff
EXTERN.TIFF.INCLUDE_DIRS           := include ../jpeg/include ../zlib/include
EXTERN.TIFF.SOURCE_DIRS            := sources
EXTERN.TIFF.unistd.SOURCE_DIRS     := sources/unix
EXTERN.TIFF.x86_win32.SOURCE_DIRS  := sources/win32
EXTERN.TIFF.wp8.SOURCE_DIRS        := sources/msdos
EXTERN.TIFF.winrt.SOURCE_DIRS      := sources/win32
EXTERN.TIFF.winrt.COMPILER_DEFINES := TIF_PLATFORM_CONSOLE
EXTERN.TIFF.wince.SOURCE_DIRS      := sources/msdos
EXTERN.TIFF.msvc.COMPILER_CFLAGS   := -wd4013 -wd4018
EXTERN.TIFF.g++.COMPILER_CFLAGS    := --no-warn
EXTERN.TIFF.clang.COMPILER_CFLAGS  := -w
