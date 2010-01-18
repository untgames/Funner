###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.RENDER3D.SOURCES

#Цель - Тест 3D рендера
DEMOS.RENDER3D.SOURCES.TYPE          := application
DEMOS.RENDER3D.SOURCES.NAME          := render3d-test
DEMOS.RENDER3D.SOURCES.SOURCE_DIRS   := sources
DEMOS.RENDER3D.SOURCES.EXECUTION_DIR := sources
DEMOS.RENDER3D.SOURCES.IMPORTS       := compile.system compile.render.low_level compile.media.geometry compile.media.rfx \
                                        compile.media.image \
                                        compile.scene_graph.core link.common link.system link.render.low_level.opengl_driver \
                                        link.media.geometry.xmesh link.media.geometry link.media.rfx \
                                        link.media.rfx.default_serializers link.media.image link.scene_graph.core
