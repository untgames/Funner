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
DEMOS.WATER_SLIDES.IMPORTS          := compile.system link.system compile.media.image link.media.image compile.render.low_level \
                                       link.render.low_level.opengl_driver compile.scene_graph.core link.scene_graph.core \
                                       link.sound.low_level.openal_driver run.sound.low_level.openal_driver \
                                       compile.sound.scene_player link.sound.scene_player link.media.sound.snddecl \
                                       link.media.sound.default_decoders compile.sound.scene_player
DEMOS.WATER_SLIDES.msvc.COMPILER_CFLAGS  := -wd4355
