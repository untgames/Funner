export.compile.extern.flurry.android.JAR_DIRS := dist
export.compile.extern.flurry.android.JARS     := flurryAnalytics_6.6.0

export.compile.extern.flurry.iphone.INCLUDE_DIRS := include

export.link.extern.flurry.iphone.LIB_DIRS   := lib/ios
export.link.extern.flurry.iphone.LIBS       := Flurry_7.8.0
export.link.extern.flurry.iphone.LINK_FLAGS := -framework Security -framework SystemConfiguration -framework StoreKit
