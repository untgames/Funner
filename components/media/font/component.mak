###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.FONT.SOURCES #MEDIA.FONT.XFONT MEDIA.FONT.FONT_CONVERTER MEDIA.FONT.TESTS \
           MEDIA.FONT.IMAGE2XFONT MEDIA.FONT.TTF2XFONT MEDIA.FONT.TTF2XFONT.TESTS

#Цель - MediaLib sources
MEDIA.FONT.SOURCES.TYPE             := static-lib
MEDIA.FONT.SOURCES.NAME             := funner.media.font
MEDIA.FONT.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.FONT.SOURCES.IMPORTS          := compile.media.font

#Цель - xfont
MEDIA.FONT.XFONT.TYPE             := static-lib
MEDIA.FONT.XFONT.NAME             := funner.media.font.xfont
MEDIA.FONT.XFONT.SOURCE_DIRS      := sources/xfont
MEDIA.FONT.XFONT.IMPORTS          := compile.media.font

#Цель - xfont
MEDIA.FONT.FONT_CONVERTER.TYPE             := static-lib
MEDIA.FONT.FONT_CONVERTER.NAME             := funner.media.font.font_converter
MEDIA.FONT.FONT_CONVERTER.INCLUDE_DIRS     := 
MEDIA.FONT.FONT_CONVERTER.SOURCE_DIRS      := sources/font_converter
MEDIA.FONT.FONT_CONVERTER.IMPORTS          := compile.media.font compile.media.image link.media.image compile.extern.freetype

#Цель - MediaLib tests
MEDIA.FONT.TESTS.TYPE             := test-suite
MEDIA.FONT.TESTS.SOURCE_DIRS      := tests/font
MEDIA.FONT.TESTS.IMPORTS          := compile.media.font link.media.font link.media.font.xfont link.media.font.font_converter \
                                     compile.media.image link.media.image link.media.image.left_bottom_packer

#Цель - Imave to xfont converter
MEDIA.FONT.IMAGE2XFONT.TYPE             := application
MEDIA.FONT.IMAGE2XFONT.NAME             := image2xfont
MEDIA.FONT.IMAGE2XFONT.SOURCE_DIRS      := utils/image2xfont
MEDIA.FONT.IMAGE2XFONT.IMPORTS          := compile.media.font link.media.font link.media.font.xfont compile.media.image link.media.image

#Цель - Font to xfont converter
MEDIA.FONT.TTF2XFONT.TYPE             := application
MEDIA.FONT.TTF2XFONT.NAME             := ttf2xfont
MEDIA.FONT.TTF2XFONT.SOURCE_DIRS      := utils/ttf2xfont
MEDIA.FONT.TTF2XFONT.IMPORTS          := compile.media.font link.media.font link.media.font.xfont compile.media.image \
                                         link.media.image link.media.font.font_converter link.media.image.left_bottom_packer \
                                         link.common.iconv

#Цель - ttf2xfont tests
MEDIA.FONT.TTF2XFONT.TESTS.TYPE              := test-suite
MEDIA.FONT.TTF2XFONT.TESTS.SOURCE_DIRS       := tests/ttf2xfont
MEDIA.FONT.TTF2XFONT.TESTS.USED_APPLICATIONS := ttf2xfont

                                         