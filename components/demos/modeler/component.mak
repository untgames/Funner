###################################################################################################
#Определения и константы
###################################################################################################
TARGETS       := DEMOS.MODELER.ENVELOPE DEMOS.MODELER.TRAJECTORY 
TARGETS.win32 := DEMOS.MODELER.VISUALIZER

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
DEMOS.MODELER.VISUALIZER.IMPORTS       := compile.system link.system link.common.default_console_handler link.render.low_level.opengl_driver \
                                          compile.render.scene_render compile.render.mid_level.window_driver compile.render.low_level \
                                          compile.scene_graph.core link.tools.ui.windows_forms compile.tools.ui.core \
                                          link.script.lua compile.script.core compile.engine.core link.engine.subsystems.scene_render \
                                          link.engine.subsystems.window_renderer link.engine.script_binds.common link.engine.script_binds.lua_override \
                                          compile.media.geometry link.media.geometry.xmesh link.media.rms compile.media.rms \
                                          link.media.geometry.binmesh


