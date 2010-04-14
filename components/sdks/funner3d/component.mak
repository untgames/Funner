###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SDKS.FUNNER3D SDKS.FUNNER3D.BUILD
                                       
SDKS.FUNNER3D.TYPE          := sdk
SDKS.FUNNER3D.NAME          := funner3d
SDKS.FUNNER3D.LIB_FILTER    := funner.%
SDKS.FUNNER3D.LIBS          :=
SDKS.FUNNER3D.IMPORTS       :=  compile.system compile.render.low_level compile.media.geometry compile.media.rfx \
                                compile.media.image compile.physics.low_level \
                                compile.scene_graph.core compile.input.low_level compile.math.curves \
                                link.common link.system link.render.low_level.opengl_driver \
                                link.media.geometry.xmesh link.media.geometry.binmesh link.media.geometry link.media.rfx \
                                link.media.rfx.default_serializers link.media.image link.scene_graph.core \
                                link.physics.low_level.bullet_driver link.input.low_level
SDKS.FUNNER3D.win32.IMPORTS  := link.input.direct_input_driver

SDKS.FUNNER3D.BUILD.TYPE           := sdk
SDKS.FUNNER3D.BUILD.OUT_DIR        := funner3d/build
SDKS.FUNNER3D.BUILD.SOURCE_FILES   := ../../../build
