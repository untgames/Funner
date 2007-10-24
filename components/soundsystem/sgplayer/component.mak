###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SGPLAYER_SOURCES SGPLAYER_TESTS

#Цель №1 - SGPlayer sources
SGPLAYER_SOURCES.TYPE             := static-lib
SGPLAYER_SOURCES.NAME             := sgplayer
SGPLAYER_SOURCES.INCLUDE_DIRS     := include ../../scenegraph/include ../../mathlib/include ../../xtl/include ../../boundvolumes/include ../manager/include ../../systemlib/include ../../commonlib/include
SGPLAYER_SOURCES.SOURCE_DIRS      := sources
SGPLAYER_SOURCES.LIB_DIRS         :=  
SGPLAYER_SOURCES.LIBS             := 
SGPLAYER_SOURCES.COMPILER_CFLAGS  :=
SGPLAYER_SOURCES.COMPILER_DEFINES := 

#Цель №2 - SGPlayer tests
SGPLAYER_TESTS.TYPE             := test-suite
SGPLAYER_TESTS.INCLUDE_DIRS     := include ../../scenegraph/include ../../mathlib/include ../../xtl/include ../../boundvolumes/include ../manager/include ../openaldevice/include ../../systemlib/include
SGPLAYER_TESTS.SOURCE_DIRS      := tests
SGPLAYER_TESTS.LIB_DIRS         := ../../../extern/openalsdk/lib/win32
SGPLAYER_TESTS.LIBS             := system user32 commonlib sg openaldevice openal32 medialib zlib zzip soundsystem sgplayer vorbisfile_static vorbis_static ogg
SGPLAYER_TESTS.DLLS             :=
SGPLAYER_TESTS.DLL_DIRS         := ../../../extern/openalsdk/bin
SGPLAYER_TESTS.COMPILER_CFLAGS  :=
SGPLAYER_TESTS.COMPILER_DEFINES :=
