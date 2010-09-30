# Compile exports
export.compile.system.INCLUDE_DIRS := include
export.compile.system.IMPORTS      := compile.common

# Link exports
export.link.system.LIBS               := funner.system
export.link.system.IMPORTS            := link.common
export.link.system.LINK_INCLUDES      := ThreadPool
export.link.system.win32.LIBS         := user32 gdi32
export.link.system.macosx.LINK_FLAGS  := -framework CoreFoundation -framework AppKit -framework Carbon
export.link.system.iphone.LINK_FLAGS  := -framework CoreFoundation -framework Foundation -framework UIKit -framework QuartzCore -framework CoreGraphics
export.link.system.linux.LIBS         := dl pthread
export.link.system.bada.LIBS          := FApp FBase FUiControls FUi FGraphics
export.link.system.bada.LINK_FLAGS    := -shared 
export.link.system.bada.LINK_INCLUDES := OspMain

export.info.system.CHMS := funner.system
