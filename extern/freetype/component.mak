###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.FREETYPE

#Цель №1 - Freetype
EXTERN.FREETYPE.TYPE                              := static-lib
EXTERN.FREETYPE.NAME                              := funner.extern.freetype
EXTERN.FREETYPE.INCLUDE_DIRS                      := include
EXTERN.FREETYPE.SOURCE_DIRS                       := sources/autofit sources/base sources/bdf sources/cache sources/cff sources/cid sources/gzip \
                                                     sources/lzw sources/pcf sources/pfr sources/psaux sources/pshinter sources/psnames sources/raster \
                                                     sources/sfnt sources/smooth sources/truetype sources/type1 sources/type42 sources/winfonts
EXTERN.FREETYPE.COMPILER_DEFINES                  := FT2_BUILD_LIBRARY
EXTERN.FREETYPE.msvc.COMPILER_DEFINES             := WIN32
EXTERN.FREETYPE.msvc.COMPILER_CFLAGS              := -wd4244
EXTERN.FREETYPE.linux.COMPILER_CFLAGS             := -Wno-uninitialized
EXTERN.FREETYPE.macosx.COMPILER_CFLAGS            := -Wno-implicit
EXTERN.FREETYPE.iphone_simulator.COMPILER_DEFINES := DARWIN_NO_CARBON
