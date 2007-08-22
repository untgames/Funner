###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIALIB_COMPONENTS MEDIALIB_UTILS MEDIALIB_COLLADA MEDIALIB_TESTS MEDIALIB_COLLADA_TESTS

#Цель №1 - MediaLib sources
MEDIALIB_COMPONENTS.TYPE             := static-lib
MEDIALIB_COMPONENTS.NAME             := medialib
MEDIALIB_COMPONENTS.INCLUDE_DIRS     := include ../../extern/devil/include ../commonlib/include ../mathlib/include ../xtl/include ../../extern/vorbisfile/include ../../extern/ogg/include
MEDIALIB_COMPONENTS.SOURCE_DIRS      := sources/imagelib sources/soundlib sources/fontlib sources/geometry
MEDIALIB_COMPONENTS.LIB_DIRS         :=  
MEDIALIB_COMPONENTS.LIBS             := 
MEDIALIB_COMPONENTS.COMPILER_CFLAGS  :=
MEDIALIB_COMPONENTS.COMPILER_DEFINES := IL_STATIC_LIB

#Цель №2 - MediaLib utils
MEDIALIB_UTILS.TYPE             := application
MEDIALIB_UTILS.NAME             := image2bfs
MEDIALIB_UTILS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include
MEDIALIB_UTILS.SOURCE_DIRS      := utils
MEDIALIB_UTILS.LIB_DIRS         :=  
MEDIALIB_UTILS.LIBS             := medialib devil ilu commonlib zzip zlib
MEDIALIB_UTILS.COMPILER_CFLAGS  :=
MEDIALIB_UTILS.COMPILER_DEFINES :=

#Цель №3 - MediaLib tests
MEDIALIB_TESTS.TYPE             := test-suite
MEDIALIB_TESTS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include ../mathlib/include
MEDIALIB_TESTS.SOURCE_DIRS      := tests/imagelib tests/soundlib tests/fontlib tests/geometry
MEDIALIB_TESTS.LIB_DIRS         :=  
MEDIALIB_TESTS.LIBS             := commonlib zzip zlib vorbisfile_static vorbis_static ogg medialib devil ilu
MEDIALIB_TESTS.COMPILER_CFLAGS  :=
MEDIALIB_TESTS.COMPILER_DEFINES :=

#Цель №4 - MediaLib COLLADA
MEDIALIB_COLLADA.TYPE             := static-lib
MEDIALIB_COLLADA.NAME             := collada
MEDIALIB_COLLADA.INCLUDE_DIRS     := include ../commonlib/include ../mathlib/include ../xtl/include
MEDIALIB_COLLADA.SOURCE_DIRS      := sources/collada-new/model sources/collada-new/dae_loader
MEDIALIB_COLLADA.LIB_DIRS         :=  
MEDIALIB_COLLADA.LIBS             := 
MEDIALIB_COLLADA.COMPILER_CFLAGS  :=
MEDIALIB_COLLADA.COMPILER_DEFINES :=

#Цель №5 - MediaLib COLLADA tests
MEDIALIB_COLLADA_TESTS.TYPE             := test-suite
MEDIALIB_COLLADA_TESTS.INCLUDE_DIRS     := include ../mathlib/include ../xtl/include ../commonlib/include
MEDIALIB_COLLADA_TESTS.SOURCE_DIRS      := tests/collada-new
MEDIALIB_COLLADA_TESTS.LIB_DIRS         :=
MEDIALIB_COLLADA_TESTS.LIBS             := commonlib zzip zlib collada
MEDIALIB_COLLADA_TESTS.COMPILER_CFLAGS  :=
MEDIALIB_COLLADA_TESTS.COMPILER_DEFINES :=
