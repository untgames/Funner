###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SCRIPT_BINDS.SOURCES ENGINE.SCRIPT_BINDS.TESTS

#Цель №1 - SCRIPT_BINDS sources
ENGINE.SCRIPT_BINDS.SOURCES.TYPE         := static-lib
ENGINE.SCRIPT_BINDS.SOURCES.NAME         := funner.engine.script_binds
ENGINE.SCRIPT_BINDS.SOURCES.INCLUDE_DIRS := include
ENGINE.SCRIPT_BINDS.SOURCES.SOURCE_DIRS  := sources
ENGINE.SCRIPT_BINDS.SOURCES.IMPORTS      := compile.script.core compile.scene_graph.core compile.common \
                                            compile.render.scene_render compile.system compile.input.manager \
                                            compile.engine.core compile.media.rms

#Цель №2 - SCRIPT_BINDS tests
ENGINE.SCRIPT_BINDS.TESTS.TYPE        := test-suite
ENGINE.SCRIPT_BINDS.TESTS.SOURCE_DIRS := tests/lua
ENGINE.SCRIPT_BINDS.TESTS.IMPORTS	  	:= compile.script.core compile.common compile.system link.script.lua \
                                         link.engine.script_binds.bv link.engine.script_binds.sg link.engine.script_binds.math \
                                         link.engine.script_binds.render link.engine.script_binds.common link.engine.script_binds.system \
                                         link.engine.script_binds.engine link.engine.script_binds.input \
                                         link.engine.script_binds.lua_override link.engine.script_binds.rms
                                              
