###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := EXTERN.PCRE

#Target - PCRE
EXTERN.PCRE.TYPE                     := static-lib
EXTERN.PCRE.NAME                     := funner.extern.pcre
EXTERN.PCRE.INCLUDE_DIRS             := include
EXTERN.PCRE.SOURCE_DIRS              := sources
EXTERN.PCRE.COMPILER_DEFINES         := PCRE_STATIC HAVE_CONFIG_H
EXTERN.PCRE.msvc.COMPILER_DEFINES    := _CRT_SECURE_NO_DEPRECATE
EXTERN.PCRE.vcx86-64.COMPILER_CFLAGS := -wd4244
EXTERN.PCRE.g++.COMPILER_CFLAGS      := --no-warn
EXTERN.PCRE.clang.COMPILER_CFLAGS    := -w
EXTERN.PCRE.emscripten.COMPILER_CFLAGS := -Wno-unused-const-variable -Wno-self-assign
