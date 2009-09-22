###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.FONT.SOURCES MEDIA.FONT.XFONT MEDIA.FONT.FONT_CONVERTER MEDIA.FONT.TESTS MEDIA.FONT.IMAGE2XFONT MEDIA.FONT.TTF2XFONT

#Цель - MediaLib sources
MEDIA.FONT.SOURCES.TYPE             := static-lib
MEDIA.FONT.SOURCES.NAME             := funner.media.font
MEDIA.FONT.SOURCES.INCLUDE_DIRS     := 
MEDIA.FONT.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.FONT.SOURCES.LIB_DIRS         :=  
MEDIA.FONT.SOURCES.LIBS             := 
MEDIA.FONT.SOURCES.COMPILER_CFLAGS  := 
MEDIA.FONT.SOURCES.COMPILER_DEFINES := 
MEDIA.FONT.SOURCES.IMPORTS          := compile.media.font

#Цель - xfont
MEDIA.FONT.XFONT.TYPE             := static-lib
MEDIA.FONT.XFONT.NAME             := funner.media.font.xfont
MEDIA.FONT.XFONT.INCLUDE_DIRS     := 
MEDIA.FONT.XFONT.SOURCE_DIRS      := sources/xfont
MEDIA.FONT.XFONT.LIB_DIRS         :=  
MEDIA.FONT.XFONT.LIBS             := 
MEDIA.FONT.XFONT.COMPILER_CFLAGS  := 
MEDIA.FONT.XFONT.COMPILER_DEFINES := 
MEDIA.FONT.XFONT.IMPORTS          := compile.media.font

#Цель - xfont
MEDIA.FONT.FONT_CONVERTER.TYPE             := static-lib
MEDIA.FONT.FONT_CONVERTER.NAME             := funner.media.font.font_converter
MEDIA.FONT.FONT_CONVERTER.INCLUDE_DIRS     := ../../../extern/freetype/include
MEDIA.FONT.FONT_CONVERTER.SOURCE_DIRS      := sources/font_converter
MEDIA.FONT.FONT_CONVERTER.LIB_DIRS         :=  
MEDIA.FONT.FONT_CONVERTER.COMPILER_CFLAGS  := 
MEDIA.FONT.FONT_CONVERTER.COMPILER_DEFINES := 
MEDIA.FONT.FONT_CONVERTER.IMPORTS          := compile.media.font compile.media.image link.media.image

#Цель - MediaLib tests
MEDIA.FONT.TESTS.TYPE             := test-suite
MEDIA.FONT.TESTS.INCLUDE_DIRS     :=
MEDIA.FONT.TESTS.SOURCE_DIRS      := tests
MEDIA.FONT.TESTS.LIB_DIRS         :=  
MEDIA.FONT.TESTS.LIBS             :=
MEDIA.FONT.TESTS.COMPILER_CFLAGS  :=
MEDIA.FONT.TESTS.COMPILER_DEFINES :=
MEDIA.FONT.TESTS.IMPORTS          := compile.media.font link.media.font link.media.font.xfont link.media.font.font_converter \
                                     compile.media.image link.media.image link.media.image.left_bottom_packer

#Цель - Imave to xfont converter
MEDIA.FONT.IMAGE2XFONT.TYPE             := application
MEDIA.FONT.IMAGE2XFONT.NAME             := image2xfont
MEDIA.FONT.IMAGE2XFONT.INCLUDE_DIRS     :=
MEDIA.FONT.IMAGE2XFONT.SOURCE_DIRS      := utils/image2xfont
MEDIA.FONT.IMAGE2XFONT.LIB_DIRS         :=  
MEDIA.FONT.IMAGE2XFONT.LIBS             :=
MEDIA.FONT.IMAGE2XFONT.COMPILER_CFLAGS  :=
MEDIA.FONT.IMAGE2XFONT.COMPILER_DEFINES :=
MEDIA.FONT.IMAGE2XFONT.IMPORTS          := compile.media.font link.media.font link.media.font.xfont compile.media.image link.media.image

#Цель - Imave to xfont converter
MEDIA.FONT.TTF2XFONT.TYPE             := application
MEDIA.FONT.TTF2XFONT.NAME             := ttf2xfont
MEDIA.FONT.TTF2XFONT.INCLUDE_DIRS     :=
MEDIA.FONT.TTF2XFONT.SOURCE_DIRS      := utils/ttf2xfont
MEDIA.FONT.TTF2XFONT.LIB_DIRS         :=  
MEDIA.FONT.TTF2XFONT.LIBS             :=
MEDIA.FONT.TTF2XFONT.COMPILER_CFLAGS  :=
MEDIA.FONT.TTF2XFONT.COMPILER_DEFINES :=
MEDIA.FONT.TTF2XFONT.IMPORTS          := compile.media.font link.media.font link.media.font.xfont compile.media.image \
                                         link.media.image link.media.font.font_converter link.media.image.left_bottom_packer
