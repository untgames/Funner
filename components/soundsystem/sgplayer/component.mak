###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SGPLAYER_SOURCES

#Цель №1 - SGPlayer sources
SGPLAYER_SOURCES.TYPE             := static-lib
SGPLAYER_SOURCES.NAME             := sgplayer
SGPLAYER_SOURCES.INCLUDE_DIRS     := include ../../scenegraph/include ../../mathlib/include ../../xtl/include ../../boundvolumes/include ../manager/include ../../systemlib/include ../../commonlib/include
SGPLAYER_SOURCES.SOURCE_DIRS      := sources
SGPLAYER_SOURCES.LIB_DIRS         :=  
SGPLAYER_SOURCES.LIBS             := 
SGPLAYER_SOURCES.COMPILER_CFLAGS  :=
SGPLAYER_SOURCES.COMPILER_DEFINES := 
