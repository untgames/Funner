###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.IMAGE.SOURCES MEDIA.XATLAS.SOURCES MEDIA.LEFT_BOTTOM_PACKER.SOURCES MEDIA.IMAGE.TESTS

#Цель №1 - Image sources
MEDIA.IMAGE.SOURCES.TYPE             := static-lib
MEDIA.IMAGE.SOURCES.NAME             := media.image
MEDIA.IMAGE.SOURCES.INCLUDE_DIRS     := ../../../extern/devil/include
MEDIA.IMAGE.SOURCES.SOURCE_DIRS      := sources/image sources/atlas
MEDIA.IMAGE.SOURCES.LIB_DIRS         :=  
MEDIA.IMAGE.SOURCES.LIBS             := 
MEDIA.IMAGE.SOURCES.COMPILER_CFLAGS  :=
MEDIA.IMAGE.SOURCES.COMPILER_DEFINES := IL_STATIC_LIB
MEDIA.IMAGE.SOURCES.IMPORTS          := compile.static.mak

#Цель №2 - XAtlas sources
MEDIA.XATLAS.SOURCES.TYPE             := static-lib
MEDIA.XATLAS.SOURCES.NAME             := media.image.xatlas
MEDIA.XATLAS.SOURCES.INCLUDE_DIRS     := 
MEDIA.XATLAS.SOURCES.SOURCE_DIRS      := sources/xatlas
MEDIA.XATLAS.SOURCES.LIB_DIRS         :=  
MEDIA.XATLAS.SOURCES.LIBS             := 
MEDIA.XATLAS.SOURCES.COMPILER_CFLAGS  :=
MEDIA.XATLAS.SOURCES.COMPILER_DEFINES := 
MEDIA.XATLAS.SOURCES.IMPORTS          := compile.static.mak

#Цель №3 - XAtlas sources
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.TYPE             := static-lib
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.NAME             := media.image.left_bottom_packer
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.INCLUDE_DIRS     := ../../bound_volumes/include
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.SOURCE_DIRS      := sources/packers/left_bottom
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.LIB_DIRS         :=  
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.LIBS             := 
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.COMPILER_CFLAGS  :=
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.COMPILER_DEFINES := 
MEDIA.LEFT_BOTTOM_PACKER.SOURCES.IMPORTS          := compile.static.mak

#Цель №4 - Image tests
MEDIA.IMAGE.TESTS.TYPE             := test-suite
MEDIA.IMAGE.TESTS.INCLUDE_DIRS     := ../../bound_volumes/include
MEDIA.IMAGE.TESTS.SOURCE_DIRS      := tests/image tests/atlas
MEDIA.IMAGE.TESTS.LIB_DIRS         :=  
MEDIA.IMAGE.TESTS.LIBS             :=
MEDIA.IMAGE.TESTS.COMPILER_CFLAGS  :=
MEDIA.IMAGE.TESTS.COMPILER_DEFINES :=
MEDIA.IMAGE.TESTS.IMPORTS          := compile.static.mak link.static.mak xatlas.link.static.mak left_bottom_packer.link.static.mak
