###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.MODELER.ENVELOPE DEMOS.MODELER.TRAJECTORY DEMOS.MODELER.VISUALIZER

#Утилита расчёта огибающей
DEMOS.MODELER.ENVELOPE.TYPE          := application
DEMOS.MODELER.ENVELOPE.NAME          := modeler-envelope
DEMOS.MODELER.ENVELOPE.SOURCE_DIRS   := sources/envelope
DEMOS.MODELER.ENVELOPE.EXECUTION_DIR := .
DEMOS.MODELER.ENVELOPE.IMPORTS       :=

#Утилита расчёта траектории
DEMOS.MODELER.TRAJECTORY.TYPE          := application
DEMOS.MODELER.TRAJECTORY.NAME          := modeler-trajectory
DEMOS.MODELER.TRAJECTORY.SOURCE_DIRS   := sources/trajectory
DEMOS.MODELER.TRAJECTORY.EXECUTION_DIR := .
DEMOS.MODELER.TRAJECTORY.IMPORTS       :=

#Приложение Modeler
DEMOS.MODELER.VISUALIZER.TYPE          := application
DEMOS.MODELER.VISUALIZER.NAME          := modeler-visualizer
DEMOS.MODELER.VISUALIZER.INCLUDE_DIRS  :=
DEMOS.MODELER.VISUALIZER.SOURCE_DIRS   := sources/visualizer
DEMOS.MODELER.VISUALIZER.EXECUTION_DIR := .
DEMOS.MODELER.VISUALIZER.LINK_INCLUDES := ModelerRender
DEMOS.MODELER.VISUALIZER.IMPORTS       := ../../system/compile.static.mak ../../system/link.static.mak ../../render/scene_render/compile.static.mak \
                                          ../../render/mid_level_window_driver/compile.static.mak ../../render/low_level/compile.static.mak \
                                          ../../scene_graph/compile.static.mak  ../../tools/ui/windows_forms/link.static.mak ../../script/binds/link.static.mak \
                                          ../../tools/ui/core/compile.static.mak ../../script/core/lua.link.static.mak \
                                          ../../client/core/compile.static.mak ../../client/window_manager/compile.static.mak \
                                          ../../client/subsystems/scene_render.link.static.mak ../../client/subsystems/window_renderer.link.static.mak \
                                          ../../common/configurator.link.static.mak ../../render/opengl_driver/link.static.mak \
                                          ../../media/geometry/compile.static.mak ../../media/geometry/xmesh.link.static.mak
