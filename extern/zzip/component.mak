###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.ZZIP

#Цель №1 - ZZIP
ifneq (,$(filter no_dll,$(PROFILES)))
EXTERN.ZZIP.TYPE                       := static-lib
else
EXTERN.ZZIP.TYPE                       := dynamic-lib
endif
EXTERN.ZZIP.NAME                       := funner.extern.zzip
EXTERN.ZZIP.INCLUDE_DIRS               := include
EXTERN.ZZIP.SOURCE_DIRS                := sources
EXTERN.ZZIP.IMPORTS                    := compile.extern.zzip compile.extern.zlib link.extern.zlib
EXTERN.ZZIP.msvc.COMPILER_CFLAGS       := -wd4244
EXTERN.ZZIP.msvc.COMPILER_DEFINES      := _CRT_SECURE_NO_DEPRECATE
EXTERN.ZZIP.vcx86-64.COMPILER_CFLAGS   := -wd4267 -wd4028
EXTERN.ZZIP.g++.COMPILER_CFLAGS        := --no-warn
EXTERN.ZZIP.wince.IMPORTS              := link.extern.wcecompat
EXTERN.ZZIP.wince.COMPILER_CFLAGS      := -FIdirect.h
