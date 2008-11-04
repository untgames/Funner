###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SCRIPT_BINDS.SOURCES ENGINE.SCRIPT_BINDS.TESTS

#Цель №1 - SCRIPT_BINDS sources
ENGINE.SCRIPT_BINDS.SOURCES.TYPE             := static-lib
ENGINE.SCRIPT_BINDS.SOURCES.NAME             := funner.engine.script_binds
ENGINE.SCRIPT_BINDS.SOURCES.INCLUDE_DIRS     := include
ENGINE.SCRIPT_BINDS.SOURCES.SOURCE_DIRS      := sources
ENGINE.SCRIPT_BINDS.SOURCES.LIB_DIRS         :=  
ENGINE.SCRIPT_BINDS.SOURCES.LIBS             := 
ENGINE.SCRIPT_BINDS.SOURCES.COMPILER_CFLAGS  := 
ENGINE.SCRIPT_BINDS.SOURCES.COMPILER_DEFINES := 
ENGINE.SCRIPT_BINDS.SOURCES.IMPORTS				  := ../../script/core/compile.static.mak ../../scene_graph/compile.static.mak ../../common/compile.static.mak \
                                              ../../render/scene_render/compile.static.mak ../../system/compile.static.mak

#Цель №2 - SCRIPT_BINDS tests
ENGINE.SCRIPT_BINDS.TESTS.TYPE             := test-suite
ENGINE.SCRIPT_BINDS.TESTS.INCLUDE_DIRS     :=
ENGINE.SCRIPT_BINDS.TESTS.SOURCE_DIRS      := tests/lua
ENGINE.SCRIPT_BINDS.TESTS.LIB_DIRS         :=  
ENGINE.SCRIPT_BINDS.TESTS.LIBS             :=
ENGINE.SCRIPT_BINDS.TESTS.COMPILER_CFLAGS  := 
ENGINE.SCRIPT_BINDS.TESTS.COMPILER_DEFINES :=
ENGINE.SCRIPT_BINDS.TESTS.IMPORTS	  		   := ../../script/core/compile.static.mak ../../common/compile.static.mak ../../script/lua/link.static.mak \
                                              bv.link.static.mak sg.link.static.mak math.link.static.mak render.link.static.mak \
                                              common.link.static.mak system.link.static.mak ../../system/compile.static.mak
