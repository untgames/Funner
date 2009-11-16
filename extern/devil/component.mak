###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.DEVIL EXTERN.ILU

#Цель - DevILDLL sources
ifeq (,$(filter iphone,$(PROFILES)))
EXTERN.DEVIL.TYPE                       := dynamic-lib
else
EXTERN.DEVIL.TYPE                       := static-lib
endif

EXTERN.DEVIL.NAME                       := funner.extern.devil
EXTERN.DEVIL.INCLUDE_DIRS               := components/il/include/IL ../jpeg/include ../tiff/include ../libpng/include components/il/include ../zlib/include
EXTERN.DEVIL.IMPORTS                    := compile.extern.devil link.extern.libpng link.extern.jpeg link.extern.tiff link.extern.zlib
EXTERN.DEVIL.SOURCE_DIRS                := components/il/sources
EXTERN.DEVIL.COMPILER_DEFINES           := HAVE_CONFIG_H
EXTERN.DEVIL.linux.COMPILER_DEFINES     := MM_MALLOC
EXTERN.DEVIL.msvc.COMPILER_CFLAGS       := -wd4101
EXTERN.DEVIL.g++.COMPILER_CFLAGS        := --no-warn
EXTERN.DEVIL.mingw.g++.COMPILER_DEFINES := XMD_H

#Цель - ILUDLL sources
ifeq (,$(filter iphone,$(PROFILES)))
EXTERN.ILU.TYPE                := dynamic-lib
else
EXTERN.ILU.TYPE                := static-lib
endif
EXTERN.ILU.NAME                := funner.extern.ilu
EXTERN.ILU.INCLUDE_DIRS        := components/il/include/IL ../jpeg/include ../tiff/include components/il/include components/ilu/include ../zlib/include
EXTERN.ILU.IMPORTS             := compile.extern.devil
EXTERN.ILU.SOURCE_DIRS         := components/ilu/sources
EXTERN.ILU.LIBS                := funner.extern.devil
EXTERN.ILU.COMPILER_DEFINES    := HAVE_CONFIG_H
EXTERN.ILU.g++.COMPILER_CFLAGS := --no-warn
