###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.VORBISFILE_STATIC

#Цель №1 - VORBISFILE_STATIC
EXTERN.VORBISFILE_STATIC.TYPE                  := static-lib
EXTERN.VORBISFILE_STATIC.NAME                  := funner.extern.vorbisfile_static
EXTERN.VORBISFILE_STATIC.INCLUDE_DIRS          := include ../vorbis/include ../ogg/include
EXTERN.VORBISFILE_STATIC.SOURCE_DIRS           := sources
EXTERN.VORBISFILE_STATIC.LIB_DIRS              :=
EXTERN.VORBISFILE_STATIC.LIBS                  :=
EXTERN.VORBISFILE_STATIC.COMPILER_DEFINES      :=
EXTERN.VORBISFILE_STATIC.msvc.COMPILER_CFLAGS  := -wd4244
EXTERN.VORBISFILE_STATIC.g++.COMPILER_CFLAGS   := -Wno-unused
