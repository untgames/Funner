###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := GAME.LAUNCHER.SOURCES

#Цель №2 - Game launcher
GAME.LAUNCHER.SOURCES.TYPE          := application
GAME.LAUNCHER.SOURCES.NAME          := game-launcher
GAME.LAUNCHER.SOURCES.INCLUDE_DIRS  :=
GAME.LAUNCHER.SOURCES.SOURCE_DIRS   := sources
GAME.LAUNCHER.SOURCES.EXECUTION_DIR := . 
GAME.LAUNCHER.SOURCES.LIBS          := demos.luasg.shared
GAME.LAUNCHER.SOURCES.LINK_INCLUDES :=
GAME.LAUNCHER.SOURCES.IMPORTS       := ../../system/link.static.mak ../../system/compile.static.mak \
                                       ../../scene_graph/compile.static.mak \
                                       ../../render/low_level/compile.static.mak ../../render/low_level/link.static.mak \
                                       ../../render/scene_render/compile.static.mak ../../render/scene_render/render2d.link.static.mak \
                                       ../../render/low_level_renderer_driver/compile.static.mak ../../render/low_level_renderer_driver/link.static.mak \
                                       ../../render/opengl_driver/link.static.mak \
                                       ../../sound/manager/compile.static.mak ../../sound/manager/compile.static.mak \
                                       ../../sound/scene_player/compile.static.mak ../../sound/scene_player/link.static.mak \
                                       ../../sound/openal_device/link.static.mak ../../sound/openal_device/run.static.mak \
                                       ../../media/rfx/xmtl.link.static.mak ../../media/sound/snddecl.link.static.mak \
                                       ../../media/sound/default_decoders.link.static.mak \
                                       ../../media/font/link.static.mak ../../media/font/xfont.link.static.mak \
                                       ../../common/compile.static.mak \
                                       ../../script/binds/compile.static.mak ../../script/binds/link.static.mak \
                                       ../../script/core/compile.static.mak ../../script/core/link.static.mak ../../script/core/lua.link.static.mak \
                                       ../../input/low_level/compile.static.mak ../../input/low_level/link.static.mak \
                                       ../../input/window_driver/compile.static.mak ../../input/window_driver/link.static.mak \
                                       ../../input/direct_input_driver/compile.static.mak ../../input/direct_input_driver/link.static.mak \
                                       ../../input/manager/compile.static.mak ../../input/manager/link.static.mak ../../input/manager/keymap.link.static.mak
