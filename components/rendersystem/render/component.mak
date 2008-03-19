###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER_SOURCES

#Цель №1 - Render sources
RENDER_SOURCES.TYPE             := static-lib
RENDER_SOURCES.NAME             := render
RENDER_SOURCES.INCLUDE_DIRS     := include sources ../lowlevel/include ../../commonlib/include ../../xtl/include
RENDER_SOURCES.SOURCE_DIRS      :=
RENDER_SOURCES.LIB_DIRS         :=
RENDER_SOURCES.LIBS             :=
RENDER_SOURCES.COMPILER_CFLAGS  := /wd4355
RENDER_SOURCES.COMPILER_DEFINES :=
