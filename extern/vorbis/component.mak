TARGETS := EXTERN.VORBIS_STATIC

EXTERN.VORBIS_STATIC.TYPE                     := static-lib
EXTERN.VORBIS_STATIC.NAME                     := funner.extern.vorbis_static
EXTERN.VORBIS_STATIC.IMPORTS                  := compile.extern.vorbis
EXTERN.VORBIS_STATIC.SOURCE_DIRS              := sources
EXTERN.VORBIS_STATIC.msvc.COMPILER_CFLAGS     := -wd4244 -wd4305
EXTERN.VORBIS_STATIC.vcx86-64.COMPILER_CFLAGS := -wd4267
EXTERN.VORBIS_STATIC.g++.COMPILER_CFLAGS      := --no-warn
EXTERN.VORBIS_STATIC.clang.COMPILER_CFLAGS    := -w
