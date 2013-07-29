# Compile exports
export.compile.system.INCLUDE_DIRS := include
export.compile.system.IMPORTS      := compile.common

# Link exports
export.link.system.LIBS                      := funner.system
export.link.system.IMPORTS                   := link.common
export.link.system.LINK_INCLUDES             := ThreadPool
export.link.system.x86_win32.LIBS            := user32 gdi32 wtsapi32
export.link.system.wince.LIBS                := Coredll
export.link.system.macosx.LINK_FLAGS         := -framework CoreFoundation -framework AppKit -framework Carbon -framework IOKit -framework WebKit
export.link.system.iphone.LINK_FLAGS         := -framework CoreFoundation -framework Foundation -framework UIKit -framework QuartzCore -framework CoreGraphics -framework SystemConfiguration -framework AudioToolbox
export.link.system.macosx.LINK_INCLUDES      := PlatformConfiguration
export.link.system.win32.LINK_INCLUDES       := PlatformConfiguration
export.link.system.wince.IMPORTS             := link.extern.pthreads_wince
export.link.system.iphone.LINK_INCLUDES      := PlatformConfiguration
export.link.system.linux.LIBS                := dl rt pthread
export.link.system.x11.LIBS                  := X11
export.link.system.x11.IMPORTS               := link.media.image
export.link.system.bada.LIBS                 := FApp FBase FUiControls FUi FGraphics
export.link.system.bada.LINK_FLAGS           := -shared
export.link.system.bada.LINK_INCLUDES        := OspMain
export.link.system.bada_device.LINK_FLAGS    := $(BADA_TARGET_LIBS_DIR)/FOsp.so
export.link.system.beagleboard.LIBS          := Xau xcb Xdmcp
export.link.system.beagleboard.IMPORTS       := link.extern.beagleboard
export.link.system.meego_ia32.IMPORTS        := link.extern.meego_ia32
export.link.system.android.LIBS              := dl log
export.link.system.android.LINK_INCLUDES     := JNI_OnLoad
export.link.system.android.LINK_TOOL         := tools.link.shared-lib
export.link.system.android.RUN_TOOL          := tools.run.android_package
export.link.system.tabletos.LIBS             := EGL GLESv1_CM png freetype screen bps pps
#export.link.system.tabletos.LIBS             := screen bps
export.link.system.tabletos.LINK_INCLUDES    := tabletos_startup main
export.link.system.tabletos.LINK_TOOL        := tools.link.shared-lib
export.link.system.tabletos.RUN_TOOL         := tools.run.tabletos_package
export.info.system.CHMS                      := funner.system
#export.link.system.win8.LINK_FLAGS           := -subsystem:windows -entry:mainCRTStartup
export.link.system.win8.IMPORTS              := link.common.iconv
export.link.system.win8.LINK_TOOL            := tools.link.shared-lib
export.link.system.win8.RUN_TOOL             := tools.run.winrt_x86_package
export.link.system.win8.LINK_INCLUDES        := win8_startup main
export.link.system.win8.EXCLUDE_DEFAULT_LIBS := funner.win8_compat.stdio

ifeq (,$(filter iphone,$(PROFILES))$(filter android,$(PROFILES)))
  export.link.system.x86.IMPORTS            := link.extern.geekinfo
endif
