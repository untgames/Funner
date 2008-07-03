###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := WATER_SLIDES

#Цель №1 - Water slides
WATER_SLIDES.TYPE             := application
WATER_SLIDES.NAME             := water-slides
WATER_SLIDES.INCLUDE_DIRS     :=
WATER_SLIDES.SOURCE_DIRS      := sources
WATER_SLIDES.LIB_DIRS         :=
WATER_SLIDES.LIBS             :=
WATER_SLIDES.DLLS             :=
WATER_SLIDES.DLL_DIRS         :=
WATER_SLIDES.COMPILER_CFLAGS  := -wd4355
WATER_SLIDES.COMPILER_DEFINES :=
WATER_SLIDES.EXECUTION_DIR    := .
WATER_SLIDES.IMPORTS          := ../../system/compile.static.mak ../../system/link.static.mak \
                                 ../../media/image/compile.static.mak ../../media/image/link.static.mak \
                                 ../../render/low_level/compile.static.mak ../../render/opengl_driver/link.static.mak  \
                                 ../../sound/openal_device/link.static.mak ../../sound/openal_device/run.static.mak \
                                  ../../media/sound/default_decoders.link.static.mak \
                                 ../../media/sound/snddecl.link.static.mak  \
                                 ../../sound/scene_player/compile.static.mak ../../sound/scene_player/link.static.mak
