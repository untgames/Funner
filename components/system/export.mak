# Compile exports
export.compile.system.INCLUDE_DIRS := include
export.compile.system.IMPORTS      := compile.common

# Link exports
export.link.system.LIBS              := funner.system
export.link.system.IMPORTS           := link.common
export.link.system.LINK_INCLUDES     := ThreadPool
export.link.system.win32.LIBS        := user32
export.link.system.macosx.LINK_FLAGS := -framework CoreFoundation
export.link.system.iphone.LINK_FLAGS := -framework CoreFoundation -framework Foundation -framework UIKit -framework QuartzCore
export.link.system.linux.LIBS        := dl pthread
