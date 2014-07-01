# Link script_binds exports
export.link.engine.script_binds.LIBS    := funner.engine.script_binds
export.link.engine.script_binds.IMPORTS := link.script.core
export.link.engine.script_binds.MULTI_IMPORTS := 1

# Link bv exports
export.link.engine.script_binds.bv.LINK_INCLUDES := BoundVolumesScriptBind
export.link.engine.script_binds.bv.IMPORTS       := link.engine.script_binds

# Link common exports
export.link.engine.script_binds.common.LINK_INCLUDES := CommonScriptBind
export.link.engine.script_binds.common.IMPORTS       := link.engine.script_binds link.common.xml link.common.json

# Link engine exports
export.link.engine.script_binds.engine.LINK_INCLUDES := EngineScriptBind
export.link.engine.script_binds.engine.IMPORTS       := link.engine.script_binds link.engine.core link.scene_graph.core \
                                                        link.input.manager link.physics.manager

# Link input exports
export.link.engine.script_binds.input.LINK_INCLUDES := InputScriptBind
export.link.engine.script_binds.input.IMPORTS       := link.engine.script_binds link.input.manager

# Link lua override exports
export.link.engine.script_binds.lua_override.LINK_INCLUDES := LuaOverrideScriptBind
export.link.engine.script_binds.lua_override.IMPORTS       := link.engine.script_binds

# Link math exports
export.link.engine.script_binds.math.LINK_INCLUDES := MathScriptBind
export.link.engine.script_binds.math.IMPORTS       := link.engine.script_binds.common

# Link math exports
export.link.engine.script_binds.math.spline_loader.LINK_INCLUDES := MathSplineLoaderScriptBind
export.link.engine.script_binds.math.spline_loader.IMPORTS       := link.engine.script_binds

# Link media players exports
export.link.engine.script_binds.media.players.LINK_INCLUDES := MediaPlayersScriptBind
export.link.engine.script_binds.media.players.IMPORTS       := link.engine.script_binds link.media.players.core

# Link rms exports
export.link.engine.script_binds.rms.LINK_INCLUDES := RmsScriptBind
export.link.engine.script_binds.rms.IMPORTS       := link.engine.script_binds link.media.rms

# Link sg exports
export.link.engine.script_binds.sg.LINK_INCLUDES := SceneGraphScriptBind
export.link.engine.script_binds.sg.IMPORTS       := link.engine.script_binds link.scene_graph.core

# Link system exports
export.link.engine.script_binds.system.LINK_INCLUDES := SystemScriptBind
export.link.engine.script_binds.system.IMPORTS       := link.engine.script_binds link.system

# Link network exports
export.link.engine.script_binds.network.LINK_INCLUDES := NetworkScriptBind
export.link.engine.script_binds.network.IMPORTS       := link.engine.script_binds link.network

# Link physics exports
export.link.engine.script_binds.physics.LINK_INCLUDES := PhysicsScriptBind
export.link.engine.script_binds.physics.IMPORTS       := link.engine.script_binds link.physics.scene_physics

# Link social exports
export.link.engine.script_binds.social.LINK_INCLUDES := SocialScriptBind
export.link.engine.script_binds.social.IMPORTS       := link.engine.script_binds link.social.core

# Link debug exports
export.link.engine.script_binds.debug.LINK_INCLUDES := DebugScriptBind
export.link.engine.script_binds.debug.IMPORTS       := link.engine.script_binds

# Link debug exports
export.link.engine.script_binds.scene_input.LINK_INCLUDES := SceneInputScriptBind
export.link.engine.script_binds.scene_input.IMPORTS       := link.engine.script_binds link.input.scene_input

# Link store exports
export.link.engine.script_binds.store.LINK_INCLUDES := StoreScriptBind
export.link.engine.script_binds.store.IMPORTS       := link.engine.script_binds link.store.core

# Link media image exports
export.link.engine.script_binds.media.image.LINK_INCLUDES := MediaImageScriptBind
export.link.engine.script_binds.media.image.IMPORTS       := link.engine.script_binds link.media.image

# Link media font exports
export.link.engine.script_binds.media.font.LINK_INCLUDES := MediaFontScriptBind
export.link.engine.script_binds.media.font.IMPORTS       := link.engine.script_binds link.media.font

# Link flurry analytics exports
export.link.engine.script_binds.analytics.flurry.LINK_INCLUDES := FlurryAnalyticsScriptBind
export.link.engine.script_binds.analytics.flurry.IMPORTS       := link.engine.script_binds link.analytics.flurry

# Link push notifications exports
export.link.engine.script_binds.push_notifications.LINK_INCLUDES := PushNotificationsScriptBind
export.link.engine.script_binds.push_notifications.IMPORTS       := link.engine.script_binds link.push_notifications.core
