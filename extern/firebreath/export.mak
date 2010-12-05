export.compile.extern.firebreath.INCLUDE_DIRS          := sources/3rdParty/boost sources/3rdParty/gecko-sdk/includes sources/ScriptingCore sources/PluginCore sources/NpapiPlugin
export.compile.extern.firebreath.SOURCE_DIRS           := sources/PluginCore sources/ScriptingCore sources/ScriptingCore/DOM sources/ScriptingCore/Util sources/NpapiPlugin sources/NPAPIHost
export.compile.extern.firebreath.msvc.INCLUDE_DIRS     := sources/ActiveXPlugin
export.compile.extern.firebreath.msvc.SOURCE_DIRS      := sources/PluginCore/Win sources/NpapiPlugin/Win sources/ActiveXPlugin
export.compile.extern.firebreath.msvc.COMPILER_DEFINES := _ATL_STATIC_REGISTRY XP_WIN=1 _WINDLL _WINDOWS WIN32
export.link.extern.firebreath.msvc.LIBS                := ole32 oleaut32 advapi32 uuid
