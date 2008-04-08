###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := WATER_SLIDES

#Цель №1 - Water slides
WATER_SLIDES.TYPE             := application
WATER_SLIDES.NAME             := water-slides
WATER_SLIDES.INCLUDE_DIRS     := ../../commonlib/include ../../xtl/include ../../mathlib/include ../../medialib/include ../../rendersystem/lowlevel/include ../../rendersystem/opengldriver/include ../../systemlib/include
WATER_SLIDES.SOURCE_DIRS      := sources
WATER_SLIDES.LIB_DIRS         :=  
WATER_SLIDES.LIBS             := medialib devil ilu commonlib zzip zlib opengl_driver system user32 render_ll glew_static gdi32 opengl32
WATER_SLIDES.COMPILER_CFLAGS  := /wd4355
WATER_SLIDES.COMPILER_DEFINES :=
WATER_SLIDES.EXECUTION_DIR    := .
