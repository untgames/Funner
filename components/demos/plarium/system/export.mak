# Compile exports
export.compile.plarium.system.INCLUDE_DIRS := include

# Link exports
export.link.plarium.system.LIBS             := plarium.system
export.link.plarium.system.win32.LIBS       := wsock32 ws2_32
export.link.plarium.system.cocoa.LINK_FLAGS := -framework CoreFoundation -framework Foundation
export.link.plarium.system.IMPORTS          := link.extern.zlib
