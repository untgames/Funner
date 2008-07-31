###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEMOS.LUASG.SHARED DEMOS.LUASG.TESTS

#Цель №1 - Общий код demo
DEMOS.LUASG.SHARED.TYPE             := static-lib
DEMOS.LUASG.SHARED.NAME             := demos.luasg.shared
DEMOS.LUASG.SHARED.INCLUDE_DIRS     :=
DEMOS.LUASG.SHARED.SOURCE_DIRS      := shared
DEMOS.LUASG.SHARED.IMPORTS          := ../../system/compile.static.mak ../../render/scene_render/compile.static.mak \
                                       ../../render/low_level_renderer_driver/compile.static.mak ../../render/low_level/compile.static.mak \
                                       ../../scene_graph/compile.static.mak ../../script/binds/compile.static.mak ../../script/binds/link.static.mak \
                                       ../../script/core/compile.static.mak ../../script/core/link.static.mak ../../script/core/lua.link.static.mak \
                                       ../../input/low_level/compile.static.mak ../../input/window_driver/compile.static.mak \
                                       ../../input/manager/compile.static.mak

#Цель №2 - Тесты demo
DEMOS.LUASG.TESTS.TYPE             := test-suite
DEMOS.LUASG.TESTS.INCLUDE_DIRS     := shared
DEMOS.LUASG.TESTS.SOURCE_DIRS      := sources
DEMOS.LUASG.TESTS.LIB_DIRS         :=  
DEMOS.LUASG.TESTS.LIBS             := demos.luasg.shared
DEMOS.LUASG.TESTS.LINK_INCLUDES    :=
DEMOS.LUASG.TESTS.IMPORTS          := ../../system/link.static.mak ../../scene_graph/compile.static.mak \
                                      ../../render/scene_render/compile.static.mak ../../render/scene_render/render2d.link.static.mak \
                                      ../../render/low_level_renderer_driver/link.static.mak ../../render/opengl_driver/link.static.mak \
                                      ../../media/rfx/xmtl.link.static.mak ../../render/debug_renderer_driver/link.static.mak \
                                      ../../common/compile.static.mak ../../script/binds/compile.static.mak ../../script/binds/link.static.mak \
                                      ../../script/core/compile.static.mak ../../script/core/link.static.mak ../../script/core/lua.link.static.mak \
                                      ../../input/low_level/compile.static.mak ../../input/low_level/link.static.mak \
                                      ../../input/window_driver/compile.static.mak ../../input/window_driver/link.static.mak \
                                      ../../input/manager/compile.static.mak ../../input/manager/link.static.mak ../../input/manager/keymap.link.static.mak \
                                      ../../system/compile.static.mak
