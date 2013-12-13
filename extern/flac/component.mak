###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.FLAC_STATIC

#Цель №1 - FLAC_STATIC
EXTERN.FLAC_STATIC.TYPE                     := static-lib
EXTERN.FLAC_STATIC.NAME                     := funner.extern.flac_static
EXTERN.FLAC_STATIC.IMPORTS                  := compile.extern.flac
EXTERN.FLAC_STATIC.INCLUDE_DIRS             := sources/include
EXTERN.FLAC_STATIC.SOURCE_DIRS              := sources/common
EXTERN.FLAC_STATIC.win32.SOURCE_DIRS        := sources/win32
EXTERN.FLAC_STATIC.COMPILER_DEFINES         := VERSION='"1.3.0"'
EXTERN.FLAC_STATIC.macosx.COMPILER_DEFINES  := HAVE_SYS_PARAM_H HAVE_LROUND
EXTERN.FLAC_STATIC.iphone.COMPILER_DEFINES  := HAVE_SYS_PARAM_H HAVE_LROUND
EXTERN.FLAC_STATIC.android.COMPILER_DEFINES := HAVE_LROUND
EXTERN.FLAC_STATIC.x86.COMPILER_DEFINES     := FLAC__CPU_IA32
EXTERN.FLAC_STATIC.clang.COMPILER_CFLAGS    := -w
