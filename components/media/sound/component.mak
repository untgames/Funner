###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.SOUND.SOURCES MEDIA.SOUND.TESTS

#Цель №1 - MediaLib sources
MEDIA.SOUND.SOURCES.TYPE             := static-lib
MEDIA.SOUND.SOURCES.NAME             := media.sound
MEDIA.SOUND.SOURCES.INCLUDE_DIRS     := ../../extern/vorbisfile/include ../../extern/ogg/include
MEDIA.SOUND.SOURCES.SOURCE_DIRS      := sources
MEDIA.SOUND.SOURCES.LIB_DIRS         :=  
MEDIA.SOUND.SOURCES.LIBS             := 
MEDIA.SOUND.SOURCES.COMPILER_CFLAGS  := 
MEDIA.SOUND.SOURCES.COMPILER_DEFINES := 
MEDIA.SOUND.SOURCES.IMPORTS          := compile.static.mak ../shared/compile.static.mak

#Цель №2 - MediaLib tests
MEDIA.SOUND.TESTS.TYPE             := test-suite
MEDIA.SOUND.TESTS.INCLUDE_DIRS     :=
MEDIA.SOUND.TESTS.SOURCE_DIRS      := tests
MEDIA.SOUND.TESTS.LIB_DIRS         :=  
MEDIA.SOUND.TESTS.LIBS             :=
MEDIA.SOUND.TESTS.COMPILER_CFLAGS  :=
MEDIA.SOUND.TESTS.COMPILER_DEFINES :=
MEDIA.SOUND.TESTS.IMPORTS          := compile.static.mak link.static.mak
