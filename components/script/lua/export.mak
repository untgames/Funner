# Link exports
export.link.script.lua.LIBS          := funner.script.lua
export.link.script.lua.LINK_INCLUDES := LuaInterpreter
export.link.script.lua.IMPORTS       := link.script.core

#ifneq (,$(filter x86,$(PROFILES)))
#export.link.script.lua.IMPORTS += link.extern.luajit
#else
export.link.script.lua.IMPORTS += link.extern.lua
#endif
