###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND_SYSTEM_TESTS

#Цель №1 - Sound system tests
SOUND_SYSTEM_TESTS.TYPE             := test-suite
SOUND_SYSTEM_TESTS.INCLUDE_DIRS     := ../include ../../../commonlib/include ../../../mathlib/include ../../../xtl/include ../../../systemlib/include ../../openaldevice/include
SOUND_SYSTEM_TESTS.SOURCE_DIRS      := device manager
SOUND_SYSTEM_TESTS.LIB_DIRS         := ../../../../extern/openalsdk/lib/win32
SOUND_SYSTEM_TESTS.LIBS             := commonlib soundsystem system user32 openaldevice medialib zzip zlib vorbisfile_static vorbis_static ogg openal32
SOUND_SYSTEM_TESTS.DLLS             :=
SOUND_SYSTEM_TESTS.DLL_DIRS         :=
SOUND_SYSTEM_TESTS.COMPILER_CFLAGS  :=
SOUND_SYSTEM_TESTS.COMPILER_DEFINES :=
