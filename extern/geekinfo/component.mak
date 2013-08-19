###################################################################################################
#Список целей
###################################################################################################
TARGETS += EXTERN.GEEKINFO.SOURCES
TARGETS += EXTERN.GEEKINFO.TESTS

#Цель - GeekInfo
EXTERN.GEEKINFO.SOURCES.TYPE                   := static-lib
EXTERN.GEEKINFO.SOURCES.NAME                   := funner.extern.geekinfo
EXTERN.GEEKINFO.SOURCES.SOURCE_DIRS            := sources/common
EXTERN.GEEKINFO.SOURCES.INCLUDE_DIRS           := sources
EXTERN.GEEKINFO.SOURCES.x86_win32.SOURCE_DIRS  := sources/win32
EXTERN.GEEKINFO.SOURCES.x86_winrt.SOURCE_DIRS  := sources/win32
EXTERN.GEEKINFO.SOURCES.macosx.SOURCE_DIRS     := sources/macosx
EXTERN.GEEKINFO.SOURCES.linux.SOURCE_DIRS      := sources/linux
EXTERN.GEEKINFO.SOURCES.x86.SOURCE_DIRS        := sources/x86
EXTERN.GEEKINFO.SOURCES.msvc.COMPILER_CFLAGS   := -W0
EXTERN.GEEKINFO.SOURCES.msvc.SOURCE_DIRS       := sources/intel sources/intel/msvc sources/win32/msvc
EXTERN.GEEKINFO.SOURCES.g++.COMPILER_CFLAGS    := --no-warn
EXTERN.GEEKINFO.SOURCES.clang.COMPILER_CFLAGS  := -w
EXTERN.GEEKINFO.SOURCES.mingw.SOURCE_DIRS      := sources/win32/mingw sources/intel sources/intel/mingw-x86
EXTERN.GEEKINFO.SOURCES.mingw.INCLUDE_DIRS     := sources/intel/mingw-x86
EXTERN.GEEKINFO.SOURCES.mingw.COMPILER_DEFINES := _WIN32_WINNT=0x600
EXTERN.GEEKINFO.SOURCES.IMPORTS                := compile.extern.geekinfo

#Цель - GeekInfo tests
EXTERN.GEEKINFO.TESTS.TYPE         := test-suite
EXTERN.GEEKINFO.TESTS.SOURCE_DIRS  := tests
EXTERN.GEEKINFO.TESTS.IMPORTS      := compile.extern.geekinfo link.extern.geekinfo
