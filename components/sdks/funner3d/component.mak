###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SDKS.FUNNER3D SDKS.FUNNER3D.BUILD
                                       
SDKS.FUNNER3D.TYPE               := sdk
SDKS.FUNNER3D.NAME               := funner3d
SDKS.FUNNER3D.LIB_EXCLUDE_FILTER := gdi32 ole32 uuid user32 shell32 wsock32 ws2_32 wldap32
SDKS.FUNNER3D.macosx.LIB_EXCLUDE_FILTER := funner.extern.devil funner.extern.ilu pthread ldap ssl crypto
SDKS.FUNNER3D.LIBS          :=
SDKS.FUNNER3D.DLLS          :=
SDKS.FUNNER3D.EXECUTABLES   := collada-converter
SDKS.FUNNER3D.IMPORTS       := compile.system compile.render.low_level compile.media.geometry compile.media.rfx \
                               compile.media.animation compile.media.image compile.media.video compile.media.font \
                               compile.physics.manager compile.scene_graph.core compile.input.low_level compile.input.manager \
                               compile.input.window_driver compile.math.curves compile.sound.scene_player \
                               compile.script.core compile.network compile.render.debug_render \
                               link.common link.system link.render.low_level.opengl_driver \
                               link.media.geometry.xmesh link.media.geometry.binmesh link.media.geometry link.media.rfx \
                               link.media.rfx.default_serializers link.media.animation.xanim link.media.animation.binanim \
                               link.media.image link.media.image.dds link.media.sound.default_decoders link.media.sound.snddecl \
                               link.media.video.theora link.media.font.xfont \
                               link.scene_graph.core link.sound.scene_player link.sound.low_level.openal_driver \
                               link.physics.low_level.bullet_driver link.physics.manager link.input.manager link.input.window_driver \
                               link.input.manager.xkeymap link.input.manager.xkeyreg link.script.lua \
                               link.engine.script_binds.sg link.engine.script_binds.lua_override link.engine.script_binds.math \
                               link.engine.script_binds.common link.network link.render.debug_render \
                               run.extern.devil run.extern.zzip run.sound.low_level.openal_driver
#                               info.math.bound_volumes info.common info.input.low_level info.input.manager \
#                               info.math.vecmath info.math.curves info.media.image info.media.animation \
#                               info.media.geometry info.media.sound info.media.video info.media.rfx \
#                               info.physics.low_level info.render.low_level info.scene_graph.core \
#                               info.system info.xtl info.script.core
SDKS.FUNNER3D.win32.IMPORTS   := link.input.direct_input_driver
SDKS.FUNNER3D.EXCLUDE_IMPORTS := link.common.auto_license_generator                                             

SDKS.FUNNER3D.BUILD.TYPE           := sdk
SDKS.FUNNER3D.BUILD.OUT_DIR        := funner3d/build
SDKS.FUNNER3D.BUILD.SOURCE_FILES   := ../../../build
