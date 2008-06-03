###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.VORBIS_STATIC

#Цель №1 - VORBIS_STATIC
EXTERN.VORBIS_STATIC.TYPE                 := static-lib
EXTERN.VORBIS_STATIC.NAME                 := extern.vorbis_static
EXTERN.VORBIS_STATIC.INCLUDE_DIRS         := include ../ogg/include
EXTERN.VORBIS_STATIC.SOURCE_DIRS          := sources
EXTERN.VORBIS_STATIC.LIB_DIRS             :=
EXTERN.VORBIS_STATIC.LIBS                 :=
EXTERN.VORBIS_STATIC.COMPILER_DEFINES     :=
EXTERN.VORBIS_STATIC.msvc.COMPILER_CFLAGS := -wd4244 -wd4305
EXTERN.VORBIS_STATIC.g++.COMPILER_CFLAGS  := -Wno-unused -Wno-uninitialized
