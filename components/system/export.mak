# Compile exports
export.compile.system.INCLUDE_DIRS := include
export.compile.system.IMPORTS      := compile.common

# Link exports
export.link.system.LIBS                  := funner.system
export.link.system.IMPORTS               := link.common
export.link.system.LINK_INCLUDES         := ThreadPool
export.link.system.win32.LIBS            := user32 gdi32
export.link.system.macosx.LINK_FLAGS     := -framework CoreFoundation -framework AppKit -framework Carbon
export.link.system.iphone.LINK_FLAGS     := -framework CoreFoundation -framework Foundation -framework UIKit -framework QuartzCore -framework CoreGraphics
export.link.system.linux.LIBS            := dl rt pthread
export.link.system.x11.LIBS              := X11
export.link.system.x11.IMPORTS           := link.media.image
export.link.system.bada.LIBS             := FApp FBase FUiControls FUi FGraphics
export.link.system.bada.LINK_FLAGS       := -shared
export.link.system.bada.LINK_INCLUDES    := OspMain
export.link.system.beagleboard.LIBS      := Xau xcb Xdmcp
export.link.system.beagleboard.IMPORTS   := link.extern.beagleboard
export.link.system.android.LIBS          := dl
export.link.system.android.LINK_INCLUDES := JNI_OnLoad
export.link.system.android.LINK_TOOL     := tools.link.shared-lib
export.link.system.android.RUN_TOOL      := tools.run.android_package
