###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIALIB_COMPONENTS MEDIALIB_TESTS

#Цель №1 - MediaLib sources
MEDIALIB_COMPONENTS.TYPE             := static-lib
MEDIALIB_COMPONENTS.NAME             := medialib
MEDIALIB_COMPONENTS.INCLUDE_DIRS     := include ../../extern/devil/include ../commonlib/include ../xtl/include ../../extern/vorbisfile/include ../../extern/ogg/include
MEDIALIB_COMPONENTS.SOURCE_DIRS      := sources/imagelib sources/soundlib
MEDIALIB_COMPONENTS.LIB_DIRS         :=  
MEDIALIB_COMPONENTS.LIBS             := 
MEDIALIB_COMPONENTS.COMPILER_CFLAGS  :=
MEDIALIB_COMPONENTS.COMPILER_DEFINES := IL_STATIC_LIB

#Цель №2 - MediaLib tests
MEDIALIB_TESTS.TYPE             := test-suite
MEDIALIB_TESTS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include
MEDIALIB_TESTS.SOURCE_DIRS      := tests/soundlib tests/imagelib
MEDIALIB_TESTS.LIB_DIRS         :=  
MEDIALIB_TESTS.LIBS             := commonlib zzip zlib vorbisfile_static vorbis_static ogg medialib devil ilu
MEDIALIB_TESTS.COMPILER_CFLAGS  :=
MEDIALIB_TESTS.COMPILER_DEFINES :=
