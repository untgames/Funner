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
WATER_SLIDES.IMPORTS          := ../../common/exports.static.mak ../../system/exports.static.mak ../../media/exports.static.mak \
                                 ../../sound/openal_device/exports.static.mak ../../render/opengl_driver/exports.static.mak \
                                 ../../scene_graph/exports.static.mak ../../sound/sgplayer/exports.static.mak ../../render/low_level/exports.static.mak \
                                 ../../sound/manager/exports.static.mak
