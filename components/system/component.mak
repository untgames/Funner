###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SYSTEMLIB_SOURCES SYSTEMLIB_TESTS

#Цель №1 - System library sources
SYSTEMLIB_SOURCES.TYPE             := static-lib
SYSTEMLIB_SOURCES.NAME             := system
SYSTEMLIB_SOURCES.INCLUDE_DIRS     := sources
SYSTEMLIB_SOURCES.SOURCE_DIRS      := sources/wrappers
SYSTEMLIB_SOURCES.LIB_DIRS         :=
SYSTEMLIB_SOURCES.EXECTUTION_DIR   :=
SYSTEMLIB_SOURCES.LIBS             :=
SYSTEMLIB_SOURCES.COMPILER_CFLAGS  :=
SYSTEMLIB_SOURCES.COMPILER_DEFINES :=
SYSTEMLIB_SOURCES.IMPORTS          := compile.static.mak
SYSTEMLIB_SOURCES.win32.SOURCE_DIRS := sources/platform/win32
SYSTEMLIB_SOURCES.unistd.SOURCE_DIRS := sources/platform/unistd

#Цель №2 - System library tests
SYSTEMLIB_TESTS.TYPE             := test-suite
SYSTEMLIB_TESTS.INCLUDE_DIRS     :=
SYSTEMLIB_TESTS.SOURCE_DIRS      := tests/wrappers
SYSTEMLIB_TESTS.LIB_DIRS         :=
SYSTEMLIB_TESTS.EXECTUTION_DIR   :=
SYSTEMLIB_TESTS.LIBS             :=
SYSTEMLIB_TESTS.COMPILER_CFLAGS  :=
SYSTEMLIB_TESTS.COMPILER_DEFINES :=
SYSTEMLIB_TESTS.IMPORTS          := compile.static.mak link.static.mak
