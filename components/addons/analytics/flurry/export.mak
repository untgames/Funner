# Compile exports
export.compile.analytics.flurry.INCLUDE_DIRS := include

# Link exports
export.link.analytics.flurry.LIBS           := funner.analytics.flurry
export.link.analytics.flurry.IMPORTS        := link.common 
export.link.analytics.flurry.iphone.IMPORTS := link.system 
