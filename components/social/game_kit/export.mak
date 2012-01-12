# Link exports
export.link.social.game_kit.LIBS          := funner.social.game_kit
export.link.social.game_kit.LINK_FLAGS    := -weak_framework GameKit
export.link.social.game_kit.LINK_INCLUDES := GameKitSession
export.link.social.game_kit.IMPORTS       := link.social.core
