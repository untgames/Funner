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
                                            compile.system compile.input.manager \
                                            compile.engine.core compile.media.rms compile.media.players \
                                            compile.math.curves compile.network compile.physics.scene_physics \
                                            compile.social.core compile.input.scene_input compile.store.core \
                                            compile.media.image compile.analytics.flurry \
                                            compile.push_notifications.core

#Цель №2 - SCRIPT_BINDS tests
ENGINE.SCRIPT_BINDS.TESTS.TYPE        := test-suite
ENGINE.SCRIPT_BINDS.TESTS.SOURCE_DIRS := tests/lua
ENGINE.SCRIPT_BINDS.TESTS.IMPORTS	  	:= compile.script.core compile.math.vecmath compile.common compile.system compile.engine.core \
                                         compile.physics.manager link.physics.low_level.bullet_driver link.script.lua \
                                         link.engine.script_binds.bv link.engine.script_binds.sg link.engine.script_binds.math \
                                         link.engine.script_binds.common link.engine.script_binds.system \
                                         link.engine.script_binds.engine link.engine.script_binds.input \
                                         link.engine.script_binds.lua_override link.engine.script_binds.rms \
                                         link.engine.script_binds.media.players link.engine.script_binds.network \
                                         link.engine.script_binds.physics link.engine.script_binds.math.spline_loader \
                                         link.engine.script_binds.social link.scene_graph.xscene link.media.animation.xanim \
                                         link.engine.script_binds.debug link.engine.script_binds.scene_input link.common.aes \
                                         link.engine.script_binds.media.image
