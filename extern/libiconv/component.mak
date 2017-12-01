###################################################################################################
#Defines and constants
###################################################################################################
TARGETS += EXTERN.LIBICONV
TARGETS += EXTERN.ICONV.SOURCES
TARGETS += EXTERN.ICONV.TESTS

#Target - ICONV library
EXTERN.LIBICONV.TYPE                     := static-lib
EXTERN.LIBICONV.NAME                     := funner.extern.iconv
EXTERN.LIBICONV.INCLUDE_DIRS             := include sources sources/srclib
EXTERN.LIBICONV.SOURCE_DIRS              := sources/lib sources/srclib sources/srclib/uniwidth sources/libcharset
EXTERN.LIBICONV.COMPILER_DEFINES         := DLL=0 NO_NLS=1 DEBUG=0 HAVE_CONFIG_H EXEEXT='"exe"' LIBDIR='"."'
EXTERN.LIBICONV.msvc.COMPILER_CFLAGS     := -wd4116 -wd4018 -wd4090
EXTERN.LIBICONV.vcx86-64.COMPILER_CFLAGS := -wd4244 -wd4267
EXTERN.LIBICONV.g++.COMPILER_CFLAGS      := --no-warn
EXTERN.LIBICONV.clang.COMPILER_CFLAGS    := -w
EXTERN.LIBICONV.has_iconv.TYPE           := ignore
EXTERN.LIBICONV.emscripten.COMPILER_CFLAGS := -Wno-tautological-compare -Wno-parentheses-equality -Wno-static-in-inline -Wno-unused-variable -Wno-unused-function

#Target - ICONV utility
EXTERN.ICONV.SOURCES.TYPE                  := application
EXTERN.ICONV.SOURCES.NAME                  := iconv
EXTERN.ICONV.SOURCES.INCLUDE_DIRS          := include sources sources/srclib sources/wrappers
EXTERN.ICONV.SOURCES.SOURCE_DIRS           := utils
EXTERN.ICONV.SOURCES.COMPILER_DEFINES      := INSTALLPREFIX='"."' INSTALLDIR='"."'
EXTERN.ICONV.SOURCES.IMPORTS               := link.extern.iconv
EXTERN.ICONV.SOURCES.msvc.COMPILER_CFLAGS  := -wd4013 -wd4018
EXTERN.ICONV.SOURCES.g++.COMPILER_CFLAGS   := --no-warn
EXTERN.ICONV.SOURCES.clang.COMPILER_CFLAGS := -w
EXTERN.ICONV.SOURCES.has_iconv.TYPE        := ignore
EXTERN.ICONV.SOURCES.emscripten.COMPILER_CFLAGS := -Wno-incompatible-pointer-types-discards-qualifiers

#Target - ICONV tests
EXTERN.ICONV.TESTS.TYPE              := test-suite
EXTERN.ICONV.TESTS.SOURCE_DIRS       := tests
EXTERN.ICONV.TESTS.USED_APPLICATIONS := $(if $(filter has_iconv,$(PROFILES)),,iconv)
