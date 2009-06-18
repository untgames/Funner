###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SYSTEMLIB.SOURCES SYSTEMLIB.TESTS

#Цель №1 - System library sources
SYSTEMLIB.SOURCES.TYPE                   := static-lib
SYSTEMLIB.SOURCES.NAME                   := funner.system
SYSTEMLIB.SOURCES.INCLUDE_DIRS           := sources
SYSTEMLIB.SOURCES.SOURCE_DIRS            := sources/wrappers
SYSTEMLIB.SOURCES.IMPORTS                := compile.static.mak
SYSTEMLIB.SOURCES.win32.IMPORTS          := ../../extern/pthreads_win32/compile.static.mak
SYSTEMLIB.SOURCES.win32.SOURCE_DIRS      := sources/platform/win32
SYSTEMLIB.SOURCES.unistd.SOURCE_DIRS     := sources/platform/pthread sources/platform/unistd
SYSTEMLIB.SOURCES.msvc.COMPILER_CFLAGS   := -wd4355
SYSTEMLIB.SOURCES.no_windows.SOURCE_DIRS := sources/platform/no_windows
SYSTEMLIB.SOURCES.carbon.SOURCE_DIRS     := sources/platform/carbon
SYSTEMLIB.SOURCES.x11.SOURCE_DIRS        := sources/platform/x11

#Цель №2 - System library tests
SYSTEMLIB.TESTS.TYPE             := test-suite
SYSTEMLIB.TESTS.INCLUDE_DIRS     :=
SYSTEMLIB.TESTS.SOURCE_DIRS      := tests/wrappers tests/thread
SYSTEMLIB.TESTS.LIB_DIRS         :=
SYSTEMLIB.TESTS.EXECTUTION_DIR   :=
SYSTEMLIB.TESTS.LIBS             :=
SYSTEMLIB.TESTS.COMPILER_CFLAGS  :=
SYSTEMLIB.TESTS.COMPILER_DEFINES :=
SYSTEMLIB.TESTS.IMPORTS          := compile.static.mak link.static.mak
