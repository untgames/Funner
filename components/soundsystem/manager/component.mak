###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND_SYSTEM_SOURCES SOUND_SYSTEM_TESTS

#Цель №1 - Sound system sources
SOUND_SYSTEM_SOURCES.TYPE             := static-lib
SOUND_SYSTEM_SOURCES.NAME             := soundmanager
SOUND_SYSTEM_SOURCES.INCLUDE_DIRS     := include ../../medialib/include
SOUND_SYSTEM_SOURCES.SOURCE_DIRS      := sources/manager
SOUND_SYSTEM_SOURCES.LIB_DIRS         :=  
SOUND_SYSTEM_SOURCES.LIBS             := 
SOUND_SYSTEM_SOURCES.COMPILER_CFLAGS  :=
SOUND_SYSTEM_SOURCES.COMPILER_DEFINES := 
SOUND_SYSTEM_SOURCES.IMPORTS          := ../lowlevel/exports.mak ../../systemlib/exports.mak

#Цель №2 - Sound system tests
SOUND_SYSTEM_TESTS.TYPE             := test-suite
SOUND_SYSTEM_TESTS.INCLUDE_DIRS     := include ../../systemlib/include ../openaldevice/include
SOUND_SYSTEM_TESTS.SOURCE_DIRS      := tests/manager
SOUND_SYSTEM_TESTS.LIB_DIRS         := ../../../extern/openalsdk/lib/win32
SOUND_SYSTEM_TESTS.LIBS             := soundmanager openaldevice medialib vorbisfile_static vorbis_static ogg openal32
SOUND_SYSTEM_TESTS.DLLS             :=
SOUND_SYSTEM_TESTS.DLL_DIRS         :=
SOUND_SYSTEM_TESTS.COMPILER_CFLAGS  :=
SOUND_SYSTEM_TESTS.COMPILER_DEFINES :=
SOUND_SYSTEM_TESTS.IMPORTS          := ../lowlevel/exports.mak ../../systemlib/exports.mak
