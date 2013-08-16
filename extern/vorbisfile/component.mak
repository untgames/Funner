###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.VORBISFILE_STATIC

#Цель №1 - VORBISFILE_STATIC
EXTERN.VORBISFILE_STATIC.TYPE                  := static-lib
EXTERN.VORBISFILE_STATIC.NAME                  := funner.extern.vorbisfile_static
EXTERN.VORBISFILE_STATIC.IMPORTS               := compile.extern.vorbisfile
EXTERN.VORBISFILE_STATIC.SOURCE_DIRS           := sources
EXTERN.VORBISFILE_STATIC.msvc.COMPILER_CFLAGS  := -wd4244
EXTERN.VORBISFILE_STATIC.g++.COMPILER_CFLAGS   := -Wno-unused
EXTERN.VORBISFILE_STATIC.clang.COMPILER_CFLAGS := -w
