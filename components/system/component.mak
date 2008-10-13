###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SYSTEMLIB.SOURCES SYSTEMLIB.TESTS

#Цель №1 - System library sources
SYSTEMLIB.SOURCES.TYPE                   := static-lib
SYSTEMLIB.SOURCES.NAME                   := system
SYSTEMLIB.SOURCES.INCLUDE_DIRS           := sources
SYSTEMLIB.SOURCES.SOURCE_DIRS            := sources/wrappers
SYSTEMLIB.SOURCES.LIB_DIRS               :=
SYSTEMLIB.SOURCES.EXECTUTION_DIR         :=
SYSTEMLIB.SOURCES.LIBS                   :=
SYSTEMLIB.SOURCES.COMPILER_CFLAGS        :=
SYSTEMLIB.SOURCES.COMPILER_DEFINES       :=
SYSTEMLIB.SOURCES.IMPORTS                := compile.static.mak
SYSTEMLIB.SOURCES.win32.SOURCE_DIRS      := sources/platform/win32
SYSTEMLIB.SOURCES.win32.COMPILER_CFLAGS  := -wd4355
SYSTEMLIB.SOURCES.no_windows.SOURCE_DIRS := sources/platform/no_windows
SYSTEMLIB.SOURCES.carbon.SOURCE_DIRS     := sources/platform/carbon

#Цель №2 - System library tests
SYSTEMLIB.TESTS.TYPE             := test-suite
SYSTEMLIB.TESTS.INCLUDE_DIRS     :=
SYSTEMLIB.TESTS.SOURCE_DIRS      := tests/wrappers
SYSTEMLIB.TESTS.LIB_DIRS         :=
SYSTEMLIB.TESTS.EXECTUTION_DIR   :=
SYSTEMLIB.TESTS.LIBS             :=
SYSTEMLIB.TESTS.COMPILER_CFLAGS  :=
SYSTEMLIB.TESTS.COMPILER_DEFINES :=
SYSTEMLIB.TESTS.IMPORTS          := compile.static.mak link.static.mak
