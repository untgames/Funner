###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SGPLAYER_SOURCES SGPLAYER_TESTS

#Цель №1 - SGPlayer sources
SGPLAYER_SOURCES.TYPE             := static-lib
SGPLAYER_SOURCES.NAME             := sgplayer
SGPLAYER_SOURCES.INCLUDE_DIRS     := include
SGPLAYER_SOURCES.SOURCE_DIRS      := sources
SGPLAYER_SOURCES.LIB_DIRS         :=  
SGPLAYER_SOURCES.LIBS             := 
SGPLAYER_SOURCES.COMPILER_CFLAGS  :=
SGPLAYER_SOURCES.COMPILER_DEFINES :=
SGPLAYER_SOURCES.IMPORTS          := ../manager/exports.mak ../../scenegraph/exports.mak ../../systemlib/exports.mak

#Цель №2 - SGPlayer tests
SGPLAYER_TESTS.TYPE             := test-suite
SGPLAYER_TESTS.INCLUDE_DIRS     := include
SGPLAYER_TESTS.SOURCE_DIRS      := tests
SGPLAYER_TESTS.LIBS             := medialib sgplayer vorbisfile_static vorbis_static ogg
SGPLAYER_TESTS.DLLS             :=
SGPLAYER_TESTS.DLL_DIRS         :=
SGPLAYER_TESTS.COMPILER_CFLAGS  :=
SGPLAYER_TESTS.COMPILER_DEFINES :=
SGPLAYER_TESTS.IMPORTS          := ../manager/exports.mak ../../scenegraph/exports.mak ../../systemlib/exports.mak ../openaldevice/exports.mak
