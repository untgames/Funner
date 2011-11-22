###################################################################################################
#Список целей
###################################################################################################
TARGETS += EXTERN.GEEKINFO.SOURCES
TARGETS += EXTERN.GEEKINFO.TESTS

#Цель - GeekInfo
EXTERN.GEEKINFO.SOURCES.TYPE                  := static-lib
EXTERN.GEEKINFO.SOURCES.NAME                  := funner.extern.geekinfo
EXTERN.GEEKINFO.SOURCES.SOURCE_DIRS           := sources/common
EXTERN.GEEKINFO.SOURCES.INCLUDE_DIRS          := sources
EXTERN.GEEKINFO.SOURCES.x86_win32.SOURCE_DIRS := sources/win32
EXTERN.GEEKINFO.SOURCES.macosx.SOURCE_DIRS    := sources/macosx
EXTERN.GEEKINFO.SOURCES.linux.SOURCE_DIRS     := sources/linux
EXTERN.GEEKINFO.SOURCES.x86.SOURCE_DIRS       := sources/x86 sources/intel
EXTERN.GEEKINFO.SOURCES.msvc.COMPILER_CFLAGS  := -W0
EXTERN.GEEKINFO.SOURCES.msvc.SOURCE_DIRS      := sources/intel/msvc
EXTERN.GEEKINFO.SOURCES.IMPORTS               := compile.extern.geekinfo

#Цель - GeekInfo tests
EXTERN.GEEKINFO.TESTS.TYPE         := test-suite
EXTERN.GEEKINFO.TESTS.SOURCE_DIRS  := tests
EXTERN.GEEKINFO.TESTS.IMPORTS      := compile.extern.geekinfo link.extern.geekinfo
