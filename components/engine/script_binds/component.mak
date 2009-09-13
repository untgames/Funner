###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SCRIPT_BINDS.SOURCES ENGINE.SCRIPT_BINDS.TESTS

#Цель №1 - SCRIPT_BINDS sources
ENGINE.SCRIPT_BINDS.SOURCES.TYPE                   := static-lib
ENGINE.SCRIPT_BINDS.SOURCES.NAME                   := funner.engine.script_binds
ENGINE.SCRIPT_BINDS.SOURCES.INCLUDE_DIRS           := include
ENGINE.SCRIPT_BINDS.SOURCES.SOURCE_DIRS            := sources
ENGINE.SCRIPT_BINDS.SOURCES.iphone.SOURCE_DIRS     := sources/iphone
ENGINE.SCRIPT_BINDS.SOURCES.iphone.COMPILER_CFLAGS := -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/AVFoundation.framework/Headers/ \
                                                      -I$(IPHONE_SDK_PATH)/System/Library/Frameworks/Foundation.framework/Headers/
ENGINE.SCRIPT_BINDS.SOURCES.IMPORTS                := ../../script/core/compile.static.mak ../../scene_graph/core/compile.static.mak ../../common/compile.static.mak \
                                                      ../../render/scene_render/compile.static.mak ../../system/compile.static.mak ../../input/manager/compile.static.mak \
                                                      ../core/compile.static.mak ../../media/rms/compile.static.mak

#Цель №2 - SCRIPT_BINDS tests
ENGINE.SCRIPT_BINDS.TESTS.TYPE             := test-suite
ENGINE.SCRIPT_BINDS.TESTS.SOURCE_DIRS      := tests/lua
ENGINE.SCRIPT_BINDS.TESTS.IMPORTS	  		   := ../../script/core/compile.static.mak ../../common/compile.static.mak ../../script/lua/link.static.mak \
                                              bv.link.static.mak sg.link.static.mak math.link.static.mak render.link.static.mak \
                                              common.link.static.mak system.link.static.mak engine.link.static.mak input.link.static.mak \
                                              lua_override.link.static.mak rms.link.static.mak \
                                              ../../system/compile.static.mak ../../media/rms/link.static.mak
