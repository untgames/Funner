###################################################################################################
#Список целей
###################################################################################################
TARGETS += EXTERN.LIBICONV
TARGETS += EXTERN.ICONV.SOURCES
TARGETS += EXTERN.ICONV.TESTS

#Цель №1 - ICONV library
EXTERN.LIBICONV.TYPE             := static-lib
EXTERN.LIBICONV.NAME             := funner.extern.iconv
EXTERN.LIBICONV.INCLUDE_DIRS     := include sources sources/srclib
EXTERN.LIBICONV.SOURCE_DIRS      := sources/lib sources/srclib sources/srclib/uniwidth sources/libcharset
EXTERN.LIBICONV.LIB_DIRS         :=
EXTERN.LIBICONV.LIBS             :=
EXTERN.LIBICONV.COMPILER_DEFINES := DLL=0 NO_NLS=1 DEBUG=0 HAVE_CONFIG_H EXEEXT='"exe"' LIBDIR='"."'
EXTERN.LIBICONV.COMPILER_CFLAGS  :=
EXTERN.LIBICONV.msvc.COMPILER_CFLAGS  := -wd4116 -wd4018 -wd4090
EXTERN.LIBICONV.g++.COMPILER_CFLAGS   := --no-warn
EXTERN.LIBICONV.clang.COMPILER_CFLAGS := -w
EXTERN.LIBICONV.has_iconv.TYPE        := ignore

#Цель №2 - ICONV utility
EXTERN.ICONV.SOURCES.TYPE             := application
EXTERN.ICONV.SOURCES.NAME             := iconv
EXTERN.ICONV.SOURCES.INCLUDE_DIRS     := include sources sources/srclib sources/wrappers
EXTERN.ICONV.SOURCES.SOURCE_DIRS      := utils
EXTERN.ICONV.SOURCES.LIB_DIRS         :=
EXTERN.ICONV.SOURCES.LIBS             :=
EXTERN.ICONV.SOURCES.COMPILER_DEFINES := INSTALLPREFIX='"."' INSTALLDIR='"."'
EXTERN.ICONV.SOURCES.COMPILER_CFLAGS  :=
EXTERN.ICONV.SOURCES.IMPORTS          := link.extern.iconv
EXTERN.ICONV.SOURCES.msvc.COMPILER_CFLAGS  := -wd4013 -wd4018
EXTERN.ICONV.SOURCES.g++.COMPILER_CFLAGS   := --no-warn
EXTERN.ICONV.SOURCES.clang.COMPILER_CFLAGS := -w
EXTERN.ICONV.SOURCES.has_iconv.TYPE        := ignore
EXTERN.ICONV.SOURCES.LINK_FLAGS       := 

#Цель №3 - ICONV tests
EXTERN.ICONV.TESTS.TYPE              := test-suite
EXTERN.ICONV.TESTS.SOURCE_DIRS       := tests
EXTERN.ICONV.TESTS.USED_APPLICATIONS := $(if $(filter has_iconv,$(PROFILES)),,iconv)
