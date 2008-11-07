###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.FONT.SOURCES MEDIA.FONT.XFONT MEDIA.FONT.FONT_CONVERTER MEDIA.FONT.TESTS MEDIA.FONT.UTILS

#Цель - MediaLib sources
MEDIA.FONT.SOURCES.TYPE             := static-lib
MEDIA.FONT.SOURCES.NAME             := funner.media.font
MEDIA.FONT.SOURCES.INCLUDE_DIRS     := 
MEDIA.FONT.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.FONT.SOURCES.LIB_DIRS         :=  
MEDIA.FONT.SOURCES.LIBS             := 
MEDIA.FONT.SOURCES.COMPILER_CFLAGS  := 
MEDIA.FONT.SOURCES.COMPILER_DEFINES := 
MEDIA.FONT.SOURCES.IMPORTS          := compile.static.mak

#Цель - xfont
MEDIA.FONT.XFONT.TYPE             := static-lib
MEDIA.FONT.XFONT.NAME             := funner.media.font.xfont
MEDIA.FONT.XFONT.INCLUDE_DIRS     := 
MEDIA.FONT.XFONT.SOURCE_DIRS      := sources/xfont
MEDIA.FONT.XFONT.LIB_DIRS         :=  
MEDIA.FONT.XFONT.LIBS             := 
MEDIA.FONT.XFONT.COMPILER_CFLAGS  := 
MEDIA.FONT.XFONT.COMPILER_DEFINES := 
MEDIA.FONT.XFONT.IMPORTS          := compile.static.mak

#Цель - xfont
MEDIA.FONT.FONT_CONVERTER.TYPE             := static-lib
MEDIA.FONT.FONT_CONVERTER.NAME             := funner.media.font.font_converter
MEDIA.FONT.FONT_CONVERTER.INCLUDE_DIRS     := ../../../extern/freetype/include
MEDIA.FONT.FONT_CONVERTER.SOURCE_DIRS      := sources/font_converter
MEDIA.FONT.FONT_CONVERTER.LIB_DIRS         :=  
MEDIA.FONT.FONT_CONVERTER.COMPILER_CFLAGS  := 
MEDIA.FONT.FONT_CONVERTER.COMPILER_DEFINES := 
MEDIA.FONT.FONT_CONVERTER.IMPORTS          := compile.static.mak ../../media/image/compile.static.mak ../../media/image/link.static.mak

#Цель - MediaLib tests
MEDIA.FONT.TESTS.TYPE             := test-suite
MEDIA.FONT.TESTS.INCLUDE_DIRS     :=
MEDIA.FONT.TESTS.SOURCE_DIRS      := tests
MEDIA.FONT.TESTS.LIB_DIRS         :=  
MEDIA.FONT.TESTS.LIBS             :=
MEDIA.FONT.TESTS.COMPILER_CFLAGS  :=
MEDIA.FONT.TESTS.COMPILER_DEFINES :=
MEDIA.FONT.TESTS.IMPORTS          := compile.static.mak link.static.mak xfont.link.static.mak font_converter.link.static.mak \
                                     ../../media/image/compile.static.mak ../../media/image/link.static.mak ../../media/image/left_bottom_packer.link.static.mak

#Цель - MediaLib utils
MEDIA.FONT.UTILS.TYPE             := application
MEDIA.FONT.UTILS.NAME             := image2xfont
MEDIA.FONT.UTILS.INCLUDE_DIRS     :=
MEDIA.FONT.UTILS.SOURCE_DIRS      := utils
MEDIA.FONT.UTILS.LIB_DIRS         :=  
MEDIA.FONT.UTILS.LIBS             :=
MEDIA.FONT.UTILS.COMPILER_CFLAGS  :=
MEDIA.FONT.UTILS.COMPILER_DEFINES :=
MEDIA.FONT.UTILS.IMPORTS          := compile.static.mak link.static.mak xfont.link.static.mak ../image/compile.static.mak ../image/link.static.mak
