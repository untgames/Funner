###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.FONT.SOURCES MEDIA.FONT.TESTS MEDIA.FONT.UTILS

#Цель №1 - MediaLib sources
MEDIA.FONT.SOURCES.TYPE             := static-lib
MEDIA.FONT.SOURCES.NAME             := media.font
MEDIA.FONT.SOURCES.INCLUDE_DIRS     := 
MEDIA.FONT.SOURCES.SOURCE_DIRS      := sources
MEDIA.FONT.SOURCES.LIB_DIRS         :=  
MEDIA.FONT.SOURCES.LIBS             := 
MEDIA.FONT.SOURCES.COMPILER_CFLAGS  := 
MEDIA.FONT.SOURCES.COMPILER_DEFINES := 
MEDIA.FONT.SOURCES.IMPORTS          := compile.static.mak

#Цель №2 - MediaLib tests
MEDIA.FONT.TESTS.TYPE             := test-suite
MEDIA.FONT.TESTS.INCLUDE_DIRS     :=
MEDIA.FONT.TESTS.SOURCE_DIRS      := tests
MEDIA.FONT.TESTS.LIB_DIRS         :=  
MEDIA.FONT.TESTS.LIBS             :=
MEDIA.FONT.TESTS.COMPILER_CFLAGS  :=
MEDIA.FONT.TESTS.COMPILER_DEFINES :=
MEDIA.FONT.TESTS.IMPORTS          := compile.static.mak link.static.mak

#Цель №3 - MediaLib utils
MEDIA.FONT.UTILS.TYPE             := application
MEDIA.FONT.UTILS.NAME             := image2bfs
MEDIA.FONT.UTILS.INCLUDE_DIRS     :=
MEDIA.FONT.UTILS.SOURCE_DIRS      := utils
MEDIA.FONT.UTILS.LIB_DIRS         :=  
MEDIA.FONT.UTILS.LIBS             :=
MEDIA.FONT.UTILS.COMPILER_CFLAGS  :=
MEDIA.FONT.UTILS.COMPILER_DEFINES :=
MEDIA.FONT.UTILS.IMPORTS          := compile.static.mak link.static.mak ../image/compile.static.mak ../image/link.static.mak
