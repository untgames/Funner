###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.IPHONE3D.SOURCES

#Цель - Тест 3D рендера
DEMOS.IPHONE3D.SOURCES.TYPE          := application
DEMOS.IPHONE3D.SOURCES.NAME          := iphone3d-test
DEMOS.IPHONE3D.SOURCES.SOURCE_DIRS   := sources
DEMOS.IPHONE3D.SOURCES.EXECUTION_DIR := sources
DEMOS.IPHONE3D.SOURCES.IMPORTS       := compile.system compile.render.low_level compile.media.geometry compile.media.rfx \
                                        compile.media.image compile.physics.low_level \
                                        compile.scene_graph.core compile.input.low_level\
                                        link.common link.system link.render.low_level.opengl_driver \
                                        link.media.geometry.xmesh link.media.geometry link.media.rfx \
                                        link.media.rfx.default_serializers link.media.image link.scene_graph.core \
                                        link.physics.low_level.bullet_driver link.input.low_level 
DEMOS.IPHONE3D.SOURCES.win32.IMPORTS := link.input.direct_input_driver 
DEMOS.IPHONE3D.SOURCES.iphone.COMPILER_CFLAGS := -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/CoreFoundation.framework/Headers/
DEMOS.IPHONE3D.SOURCES.iphone.LINK_FLAGS      := -framework CoreFoundation
                                        