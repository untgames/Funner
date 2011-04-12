# Compile exports
export.compile.extern.shinylua.INCLUDE_DIRS := include
export.compile.extern.shinylua.IMPORTS      := compile.extern.shiny

# Link exports
export.link.extern.shinylua.LIBS    := funner.extern.shinylua
export.link.extern.shinylua.IMPORTS := link.extern.shiny link.extern.lua
