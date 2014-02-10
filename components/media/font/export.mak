# Compile exports
export.compile.media.font.INCLUDE_DIRS := include
export.compile.media.font.IMPORTS      := compile.media.image

# Link exports
export.link.media.font.LIBS           := funner.media.font
export.link.media.font.LINK_INCLUDES  := 
export.link.media.font.IMPORTS        := link.common

# Freetype link exports
export.link.media.font.freetype.LIBS          := funner.media.font.freetype
export.link.media.font.freetype.LINK_INCLUDES := FreetypeLoader
export.link.media.font.freetype.IMPORTS       := link.media.font link.extern.freetype

# XFont link exports
#export.link.media.font.xfont.LIBS          := funner.media.font.xfont
#export.link.media.font.xfont.LINK_INCLUDES := XFontLoader XFontSaver
#export.link.media.font.xfont.IMPORTS       := link.media.font link.common.xml

# Font converter link exports
#export.link.media.font.font_converter.LIBS := funner.media.font.font_converter 
#export.link.media.font.font_converter.IMPORTS := link.extern.freetype
