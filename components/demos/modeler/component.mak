###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.MODELER

#Приложение Modeler
DEMOS.MODELER.TYPE          := application
DEMOS.MODELER.NAME          := modeler
DEMOS.MODELER.INCLUDE_DIRS  :=
DEMOS.MODELER.SOURCE_DIRS   := sources
DEMOS.MODELER.EXECUTION_DIR := .
DEMOS.MODELER.LINK_INCLUDES := ModelerRender
DEMOS.MODELER.IMPORTS       := ../../system/compile.static.mak ../../system/link.static.mak ../../render/scene_render/compile.static.mak \
                               ../../render/mid_level_window_driver/compile.static.mak ../../render/low_level/compile.static.mak \
                               ../../scene_graph/compile.static.mak  ../../tools/ui/windows_forms/link.static.mak ../../script/binds/link.static.mak \
                               ../../tools/ui/core/compile.static.mak ../../script/core/lua.link.static.mak \
                               ../../client/core/compile.static.mak ../../client/window_manager/compile.static.mak \
                               ../../client/subsystems/scene_render.link.static.mak ../../client/subsystems/window_renderer.link.static.mak \
                               ../../common/configurator.link.static.mak ../../render/opengl_driver/link.static.mak
