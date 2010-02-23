###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.RENDER3D.SOURCES

#Цель - Тест 3D рендера
DEMOS.RENDER3D.SOURCES.TYPE          := application
DEMOS.RENDER3D.SOURCES.NAME          := capsule3d
DEMOS.RENDER3D.SOURCES.SOURCE_DIRS   := sources
DEMOS.RENDER3D.SOURCES.EXECUTION_DIR := sources
DEMOS.RENDER3D.SOURCES.DISABLE_RUN   := 1 #цель не регистрируется в сборке по команде run
DEMOS.RENDER3D.SOURCES.IMPORTS       := compile.system compile.render.low_level compile.media.geometry compile.media.rfx \
                                        compile.media.image compile.physics.low_level \
                                        compile.scene_graph.core compile.input.low_level compile.math.curves \
                                        link.common link.system link.render.low_level.opengl_driver \
                                        link.media.geometry.xmesh link.media.geometry.binmesh link.media.geometry link.media.rfx \
                                        link.media.rfx.default_serializers link.media.image link.scene_graph.core \
                                        link.physics.low_level.bullet_driver link.input.low_level
DEMOS.RENDER3D.SOURCES.win32.IMPORTS := link.input.direct_input_driver 
                                        