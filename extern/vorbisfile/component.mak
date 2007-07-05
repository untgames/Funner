###################################################################################################
#Список целей
###################################################################################################
TARGETS := VORBISFILE_STATIC

#Цель №1 - VORBISFILE_STATIC
VORBISFILE_STATIC.TYPE             := static-lib
VORBISFILE_STATIC.NAME             := vorbisfile_static
VORBISFILE_STATIC.INCLUDE_DIRS     := include ../vorbis/include ../ogg/include
VORBISFILE_STATIC.SOURCE_DIRS      := sources
VORBISFILE_STATIC.LIB_DIRS         :=
VORBISFILE_STATIC.LIBS             :=
VORBISFILE_STATIC.COMPILER_DEFINES :=
VORBISFILE_STATIC.COMPILER_CFLAGS  := /wd4244
