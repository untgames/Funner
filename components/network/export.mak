# Compile exports
export.compile.network.INCLUDE_DIRS := include
export.compile.network.IMPORTS      := compile.xtl

# Link exports
export.link.network.LIBS       := funner.network
export.link.network.win32.LIBS := wsock32
export.link.network.IMPORTS    := link.common link.system
