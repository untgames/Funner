###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.DEVIL EXTERN.ILU

#Цель - DevILDLL sources
ifeq (,$(filter no_dll,$(PROFILES)))
EXTERN.DEVIL.TYPE                       := dynamic-lib
else
EXTERN.DEVIL.TYPE                       := static-lib
endif

EXTERN.DEVIL.NAME                            := funner.extern.devil
EXTERN.DEVIL.INCLUDE_DIRS                    := components/il/include/IL components/il/include
EXTERN.DEVIL.IMPORTS                         := compile.extern.devil link.extern.libpng link.extern.jpeg link.extern.tiff link.extern.zlib compile.extern.jpeg compile.extern.libpng compile.extern.tiff compile.extern.zlib
EXTERN.DEVIL.SOURCE_DIRS                     := components/il/sources
EXTERN.DEVIL.COMPILER_DEFINES                := HAVE_CONFIG_H
EXTERN.DEVIL.linux.COMPILER_DEFINES          := MM_MALLOC
EXTERN.DEVIL.msvc.COMPILER_CFLAGS            := -wd4101
EXTERN.DEVIL.g++.COMPILER_CFLAGS             := --no-warn
EXTERN.DEVIL.clang.COMPILER_CFLAGS           := -w
EXTERN.DEVIL.mingw.COMPILER_DEFINES          := XMD_H
EXTERN.DEVIL.bada_simulator.COMPILER_DEFINES := XMD_H
EXTERN.DEVIL.bada_simulator.IMPORTS          := link.extern.bada_addons
EXTERN.DEVIL.wince.IMPORTS                   := link.extern.wcecompat

#Цель - ILUDLL sources
ifeq (,$(filter no_dll,$(PROFILES)))
EXTERN.ILU.TYPE                  := dynamic-lib
else
EXTERN.ILU.TYPE                  := static-lib
endif
EXTERN.ILU.NAME                  := funner.extern.ilu
EXTERN.ILU.INCLUDE_DIRS          := components/il/include/IL components/il/include components/ilu/include ../zlib/include
EXTERN.ILU.IMPORTS               := compile.extern.devil
EXTERN.ILU.SOURCE_DIRS           := components/ilu/sources
EXTERN.ILU.LIBS                  := funner.extern.devil
EXTERN.ILU.COMPILER_DEFINES      := HAVE_CONFIG_H
EXTERN.ILU.g++.COMPILER_CFLAGS   := --no-warn
EXTERN.ILU.clang.COMPILER_CFLAGS := -w
