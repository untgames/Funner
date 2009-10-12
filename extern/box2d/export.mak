# Compile exports
export.compile.extern.box2d.INCLUDE_DIRS         := include
export.compile.extern.box2d.iphone.INCLUDE_DIRS  := workarounds
export.compile.extern.box2d.iphone.INCLUDE_FILES := iphone-build.h

# Link exports
export.link.extern.box2d.LIBS := funner.extern.box2d
