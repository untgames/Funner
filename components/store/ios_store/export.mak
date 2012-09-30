# Link exports
export.link.store.ios_store.LIBS          := funner.store.ios_store
export.link.store.ios_store.LINK_FLAGS    := -framework StoreKit
export.link.store.ios_store.LINK_INCLUDES := IOSStore
export.link.store.ios_store.IMPORTS       := link.store.core
