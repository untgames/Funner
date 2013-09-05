# Link exports
export.link.social.facebook.LIBS                  := funner.social.facebook
export.link.social.facebook.LINK_INCLUDES         := FacebookSession
export.link.social.facebook.iphone.LINK_FLAGS     := -weak_framework AdSupport
export.link.social.facebook.android.LINK_INCLUDES := AndroidFacebookLoadHandler
export.link.social.facebook.IMPORTS               := link.social.core link.system link.network
