###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := OPENALDEVICE_COMPONENTS OPENALDEVICE_TESTS

#Цель №1 - OPENALDEVICE sources
OPENALDEVICE_COMPONENTS.TYPE             := static-lib
OPENALDEVICE_COMPONENTS.NAME             := openaldevice
OPENALDEVICE_COMPONENTS.INCLUDE_DIRS     := include ../manager/include ../../commonlib/include ../../mathlib/include ../../xtl/include ../../medialib/include ../../../extern/openalsdk/include ../../systemlib/include
OPENALDEVICE_COMPONENTS.SOURCE_DIRS      := sources
OPENALDEVICE_COMPONENTS.LIB_DIRS         :=  
OPENALDEVICE_COMPONENTS.LIBS             := 
OPENALDEVICE_COMPONENTS.COMPILER_CFLAGS  :=
OPENALDEVICE_COMPONENTS.COMPILER_DEFINES := 

#Цель №3 - OPENALDEVICE tests
OPENALDEVICE_TESTS.TYPE             := test-suite
OPENALDEVICE_TESTS.INCLUDE_DIRS     := include ../manager/include ../../commonlib/include ../../mathlib/include ../../xtl/include ../../systemlib/include
OPENALDEVICE_TESTS.SOURCE_DIRS      := tests
OPENALDEVICE_TESTS.LIB_DIRS         := ../../../extern/openalsdk/lib/win32
OPENALDEVICE_TESTS.LIBS             := soundsystem openaldevice openal32 commonlib medialib zzip zlib vorbisfile_static vorbis_static ogg system user32
OPENALDEVICE_TESTS.DLLS             := OpenAL32 wrap_oal
OPENALDEVICE_TESTS.DLL_DIRS         := ../../../extern/openalsdk/bin
OPENALDEVICE_TESTS.COMPILER_CFLAGS  :=
OPENALDEVICE_TESTS.COMPILER_DEFINES :=
