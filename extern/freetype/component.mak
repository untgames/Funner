###################################################################################################
#Targets list
###################################################################################################
TARGETS := EXTERN.FREETYPE

#Target Freetype
EXTERN.FREETYPE.TYPE                     := static-lib
EXTERN.FREETYPE.NAME                     := funner.extern.freetype
EXTERN.FREETYPE.INCLUDE_DIRS             := include
EXTERN.FREETYPE.SOURCE_DIRS              := sources/autofit sources/base sources/cache sources/cff sources/cid \
                                            sources/gzip sources/psaux sources/pshinter sources/psnames sources/raster \
                                            sources/sfnt sources/smooth sources/truetype sources/type1 sources/winfonts
EXTERN.FREETYPE.COMPILER_DEFINES         := FT2_BUILD_LIBRARY
EXTERN.FREETYPE.msvc.COMPILER_DEFINES    := WIN32
EXTERN.FREETYPE.msvc.COMPILER_CFLAGS     := -wd4244
EXTERN.FREETYPE.clang.COMPILER_CFLAGS    := -w
EXTERN.FREETYPE.g++.COMPILER_CFLAGS      := --no-warn
EXTERN.FREETYPE.iphone.COMPILER_DEFINES  := DARWIN_NO_CARBON
EXTERN.FREETYPE.linux.COMPILER_CFLAGS    := --no-warn
