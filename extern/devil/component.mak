###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.DEVIL EXTERN.ILU

#Цель - DevILDLL sources
EXTERN.DEVIL.TYPE                       := dynamic-lib
EXTERN.DEVIL.NAME                       := funner.extern.devil
EXTERN.DEVIL.INCLUDE_DIRS               := include components/il/include/IL ../jpeg/include ../tiff/include ../libpng/include components/il/include ../zlib/include
EXTERN.DEVIL.IMPORTS                    := ../libpng/link.static.mak ../jpeg/link.static.mak ../tiff/link.static.mak ../zlib/link.static.mak
EXTERN.DEVIL.SOURCE_DIRS                := components/il/sources
EXTERN.DEVIL.COMPILER_DEFINES           := HAVE_CONFIG_H
EXTERN.DEVIL.linux.COMPILER_DEFINES     := MM_MALLOC
EXTERN.DEVIL.msvc.COMPILER_CFLAGS       := -wd4101
EXTERN.DEVIL.g++.COMPILER_CFLAGS        := --no-warn
EXTERN.DEVIL.mingw.g++.COMPILER_DEFINES := XMD_H

#Цель - ILUDLL sources
EXTERN.ILU.TYPE                := dynamic-lib
EXTERN.ILU.NAME                := funner.extern.ilu
EXTERN.ILU.INCLUDE_DIRS        := include components/il/include/IL ../jpeg/include ../tiff/include components/il/include components/ilu/include ../zlib/include
EXTERN.ILU.SOURCE_DIRS         := components/ilu/sources
EXTERN.ILU.LIBS                := funner.extern.devil
EXTERN.ILU.COMPILER_DEFINES    := HAVE_CONFIG_H
EXTERN.ILU.g++.COMPILER_CFLAGS := --no-warn
