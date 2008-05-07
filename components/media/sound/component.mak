###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.SOUND.SOURCES MEDIA.SOUND.DEFAULT_DECODERS MEDIA.SOUND.SNDDECL MEDIA.SOUND.TESTS

#Цель №1 - Sound sources
MEDIA.SOUND.SOURCES.TYPE             := static-lib
MEDIA.SOUND.SOURCES.NAME             := media.sound
MEDIA.SOUND.SOURCES.INCLUDE_DIRS     := 
MEDIA.SOUND.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.SOUND.SOURCES.LIB_DIRS         :=  
MEDIA.SOUND.SOURCES.LIBS             := 
MEDIA.SOUND.SOURCES.COMPILER_CFLAGS  := 
MEDIA.SOUND.SOURCES.COMPILER_DEFINES := 
MEDIA.SOUND.SOURCES.IMPORTS          := compile.static.mak ../shared/compile.static.mak

#Цель №2 - Sound sources
MEDIA.SOUND.DEFAULT_DECODERS.TYPE             := static-lib
MEDIA.SOUND.DEFAULT_DECODERS.NAME             := media.default_decoders
MEDIA.SOUND.DEFAULT_DECODERS.INCLUDE_DIRS     := ../../../extern/vorbisfile/include ../../../extern/ogg/include
MEDIA.SOUND.DEFAULT_DECODERS.SOURCE_DIRS      := sources/default_decoders
MEDIA.SOUND.DEFAULT_DECODERS.LIB_DIRS         :=  
MEDIA.SOUND.DEFAULT_DECODERS.LIBS             := 
MEDIA.SOUND.DEFAULT_DECODERS.COMPILER_CFLAGS  := -wd4244
MEDIA.SOUND.DEFAULT_DECODERS.COMPILER_DEFINES := 
MEDIA.SOUND.DEFAULT_DECODERS.IMPORTS          := compile.static.mak

#Цель №3 - Sound sources
MEDIA.SOUND.SNDDECL.TYPE             := static-lib
MEDIA.SOUND.SNDDECL.NAME             := media.snddecl
MEDIA.SOUND.SNDDECL.INCLUDE_DIRS     := 
MEDIA.SOUND.SNDDECL.SOURCE_DIRS      := sources/snddecl
MEDIA.SOUND.SNDDECL.LIB_DIRS         :=  
MEDIA.SOUND.SNDDECL.LIBS             := 
MEDIA.SOUND.SNDDECL.COMPILER_CFLAGS  := 
MEDIA.SOUND.SNDDECL.COMPILER_DEFINES := 
MEDIA.SOUND.SNDDECL.IMPORTS          := compile.static.mak

#Цель №4 - Sound tests
MEDIA.SOUND.TESTS.TYPE             := test-suite
MEDIA.SOUND.TESTS.INCLUDE_DIRS     :=
MEDIA.SOUND.TESTS.SOURCE_DIRS      := tests
MEDIA.SOUND.TESTS.LIB_DIRS         :=  
MEDIA.SOUND.TESTS.LIBS             :=
MEDIA.SOUND.TESTS.COMPILER_CFLAGS  :=
MEDIA.SOUND.TESTS.COMPILER_DEFINES :=
MEDIA.SOUND.TESTS.IMPORTS          := compile.static.mak link.static.mak default_decoders.link.static.mak snddecl.link.static.mak
