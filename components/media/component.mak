###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIALIB_COMPONENTS MEDIALIB_UTILS MEDIALIB_COLLADA MEDIALIB_TESTS MEDIALIB_COLLADA_TESTS MEDIALIB_COLLADA_CONVERT_TESTS

#Цель №1 - MediaLib sources
MEDIALIB_COMPONENTS.TYPE             := static-lib
MEDIALIB_COMPONENTS.NAME             := media
MEDIALIB_COMPONENTS.INCLUDE_DIRS     := include sources ../../extern/devil/include ../../extern/vorbisfile/include ../../extern/ogg/include
MEDIALIB_COMPONENTS.SOURCE_DIRS      := sources/imagelib sources/soundlib sources/fontlib sources/geometry/core sources/rfx
MEDIALIB_COMPONENTS.LIB_DIRS         :=  
MEDIALIB_COMPONENTS.LIBS             := 
MEDIALIB_COMPONENTS.COMPILER_CFLAGS  := -wd4244
MEDIALIB_COMPONENTS.COMPILER_DEFINES := IL_STATIC_LIB
MEDIALIB_COMPONENTS.IMPORTS          := ../common/exports.static.mak

#Цель №2 - MediaLib utils
MEDIALIB_UTILS.TYPE             := application
MEDIALIB_UTILS.NAME             := image2bfs
MEDIALIB_UTILS.INCLUDE_DIRS     :=
MEDIALIB_UTILS.SOURCE_DIRS      := utils
MEDIALIB_UTILS.LIB_DIRS         :=  
MEDIALIB_UTILS.LIBS             :=
MEDIALIB_UTILS.COMPILER_CFLAGS  :=
MEDIALIB_UTILS.COMPILER_DEFINES :=
MEDIALIB_UTILS.IMPORTS          := exports.static.mak ../common/exports.static.mak

#Цель №3 - MediaLib tests
MEDIALIB_TESTS.TYPE             := test-suite
MEDIALIB_TESTS.INCLUDE_DIRS     :=
MEDIALIB_TESTS.SOURCE_DIRS      := tests/imagelib tests/soundlib tests/fontlib tests/geometry tests/rfx
MEDIALIB_TESTS.LIB_DIRS         :=  
MEDIALIB_TESTS.LIBS             :=
MEDIALIB_TESTS.COMPILER_CFLAGS  :=
MEDIALIB_TESTS.COMPILER_DEFINES :=
MEDIALIB_TESTS.IMPORTS          := exports.static.mak

#Цель №4 - MediaLib COLLADA
MEDIALIB_COLLADA.TYPE             := static-lib
MEDIALIB_COLLADA.NAME             := media.collada
MEDIALIB_COLLADA.INCLUDE_DIRS     := include sources
MEDIALIB_COLLADA.SOURCE_DIRS      := sources/collada/model sources/collada/dae_loader sources/collada/convert
MEDIALIB_COLLADA.LIB_DIRS         :=  
MEDIALIB_COLLADA.LIBS             := 
MEDIALIB_COLLADA.COMPILER_CFLAGS  :=
MEDIALIB_COLLADA.COMPILER_DEFINES :=
MEDIALIB_COLLADA.IMPORTS          := ../common/exports.static.mak

#Цель №5 - MediaLib COLLADA tests
MEDIALIB_COLLADA_TESTS.TYPE             := test-suite
MEDIALIB_COLLADA_TESTS.INCLUDE_DIRS     :=
MEDIALIB_COLLADA_TESTS.SOURCE_DIRS      := tests/collada
MEDIALIB_COLLADA_TESTS.LIB_DIRS         :=
MEDIALIB_COLLADA_TESTS.LIBS             :=
MEDIALIB_COLLADA_TESTS.COMPILER_CFLAGS  :=
MEDIALIB_COLLADA_TESTS.COMPILER_DEFINES :=
MEDIALIB_COLLADA_TESTS.IMPORTS          := exports.static.mak

#Цель №6 - MediaLib COLLADA convert tests
MEDIALIB_COLLADA_CONVERT_TESTS.TYPE             := test-suite
MEDIALIB_COLLADA_CONVERT_TESTS.INCLUDE_DIRS     :=
MEDIALIB_COLLADA_CONVERT_TESTS.SOURCE_DIRS      := tests/collada-convert
MEDIALIB_COLLADA_CONVERT_TESTS.LIB_DIRS         :=
MEDIALIB_COLLADA_CONVERT_TESTS.LIBS             :=
MEDIALIB_COLLADA_CONVERT_TESTS.COMPILER_CFLAGS  :=
MEDIALIB_COLLADA_CONVERT_TESTS.COMPILER_DEFINES :=
MEDIALIB_COLLADA_CONVERT_TESTS.IMPORTS          := exports.static.mak
