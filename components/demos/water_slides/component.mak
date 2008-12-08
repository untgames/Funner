###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.WATER_SLIDES

#Цель №1 - Water slides
DEMOS.WATER_SLIDES.TYPE             := application
DEMOS.WATER_SLIDES.NAME             := water-slides
DEMOS.WATER_SLIDES.INCLUDE_DIRS     :=
DEMOS.WATER_SLIDES.SOURCE_DIRS      := sources
DEMOS.WATER_SLIDES.LIB_DIRS         :=
DEMOS.WATER_SLIDES.LIBS             :=
DEMOS.WATER_SLIDES.DLLS             :=
DEMOS.WATER_SLIDES.DLL_DIRS         :=
DEMOS.WATER_SLIDES.COMPILER_DEFINES :=
DEMOS.WATER_SLIDES.EXECUTION_DIR    := .
DEMOS.WATER_SLIDES.LINK_INCLUDES    :=
DEMOS.WATER_SLIDES.IMPORTS          := ../../system/compile.static.mak ../../system/link.static.mak \
                                       ../../media/image/compile.static.mak ../../media/image/link.static.mak \
                                       ../../render/low_level/compile.static.mak ../../render/opengl_driver/link.static.mak  \
                                       ../../scene_graph/core/link.static.mak ../../scene_graph/core/compile.static.mak \
                                       ../../sound/openal_device/link.static.mak ../../sound/openal_device/run.static.mak \
                                       ../../sound/scene_player/link.static.mak ../../media/sound/snddecl.link.static.mak \
                                       ../../media/sound/default_decoders.link.static.mak ../../sound/scene_player/compile.static.mak
DEMOS.WATER_SLIDES.msvc.COMPILER_CFLAGS  := -wd4355
