###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUNDSYSTEM_COMPONENTS SOUNDSYSTEM_TESTS

#Цель №1 - Soundsystem sources
SOUNDSYSTEM_COMPONENTS.TYPE             := static-lib
SOUNDSYSTEM_COMPONENTS.NAME             := soundsystem
SOUNDSYSTEM_COMPONENTS.INCLUDE_DIRS     := include ../commonlib/include ../mathlib/include ../xtl/include ../medialib/include ../../extern/openalsdk/include
SOUNDSYSTEM_COMPONENTS.SOURCE_DIRS      := sources/openal
SOUNDSYSTEM_COMPONENTS.LIB_DIRS         :=  
SOUNDSYSTEM_COMPONENTS.LIBS             := 
SOUNDSYSTEM_COMPONENTS.COMPILER_CFLAGS  :=
SOUNDSYSTEM_COMPONENTS.COMPILER_DEFINES := 

#Цель №3 - Soundsystem tests
SOUNDSYSTEM_TESTS.TYPE             := test-suite
SOUNDSYSTEM_TESTS.INCLUDE_DIRS     := include ../commonlib/include ../mathlib/include ../xtl/include
SOUNDSYSTEM_TESTS.SOURCE_DIRS      := tests/openal
SOUNDSYSTEM_TESTS.LIB_DIRS         := ../../extern/openalsdk/lib/win32
SOUNDSYSTEM_TESTS.LIBS             := soundsystem openal32 commonlib medialib zzip zlib vorbisfile_static vorbis_static ogg
SOUNDSYSTEM_TESTS.DLLS             := OpenAL32
SOUNDSYSTEM_TESTS.DLL_DIRS         := ../../extern/openalsdk/bin
SOUNDSYSTEM_TESTS.COMPILER_CFLAGS  :=
SOUNDSYSTEM_TESTS.COMPILER_DEFINES :=
