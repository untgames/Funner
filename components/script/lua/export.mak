# Link exports
export.link.script.lua.LIBS          := funner.script.lua
export.link.script.lua.LINK_INCLUDES := LuaInterpreter
export.link.script.lua.IMPORTS       := link.script.core link.extern.shinylua

#ifneq (,$(filter msvc,$(PROFILES)))
  export.link.script.lua.IMPORTS += link.extern.lua
#else
#  export.link.script.lua.IMPORTS += link.extern.luajit
#endif
