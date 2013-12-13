###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.PCRE

#Цель - PCRE
EXTERN.PCRE.TYPE                  := static-lib
EXTERN.PCRE.NAME                  := funner.extern.pcre
EXTERN.PCRE.INCLUDE_DIRS          := include
EXTERN.PCRE.SOURCE_DIRS           := sources
EXTERN.PCRE.COMPILER_DEFINES      := PCRE_STATIC
EXTERN.PCRE.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE
EXTERN.PCRE.g++.COMPILER_CFLAGS   := --no-warn
EXTERN.PCRE.clang.COMPILER_CFLAGS := -w
