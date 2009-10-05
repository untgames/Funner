###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := TOOLS.UI.WINDOWS_FORMS.PLUGIN_API TOOLS.UI.WINDOWS_FORMS.SOURCES \
           TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN \
           TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN \
           TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN TOOLS.UI.WINDOWS_FORMS.TESTS \
           TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN

#Цель - Windows forms interface dll
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.TYPE             := dynamic-lib
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.NAME             := funner.tools.ui.windows_forms.PluginApi
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.INCLUDE_DIRS     := include
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.SOURCE_DIRS      := sources/plugin_api
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.COMPILER_DEFINES := 
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.IMPORTS			     :=
TOOLS.UI.WINDOWS_FORMS.PLUGIN_API.msvc.COMPILER_CFLAGS := -clr

#Цель - Windows forms window system unmanaged sources
TOOLS.UI.WINDOWS_FORMS.SOURCES.TYPE             := static-lib
TOOLS.UI.WINDOWS_FORMS.SOURCES.NAME             := funner.tools.ui.windows_forms
TOOLS.UI.WINDOWS_FORMS.SOURCES.INCLUDE_DIRS     := include
TOOLS.UI.WINDOWS_FORMS.SOURCES.SOURCE_DIRS      := sources/core
TOOLS.UI.WINDOWS_FORMS.SOURCES.LIB_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.SOURCES.LIBS             :=
TOOLS.UI.WINDOWS_FORMS.SOURCES.DLL_DIRS         := ../../../../extern/windows_controls/bin
TOOLS.UI.WINDOWS_FORMS.SOURCES.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.SOURCES.COMPILER_DEFINES := 
TOOLS.UI.WINDOWS_FORMS.SOURCES.IMPORTS			    := compile.tools.ui.core compile.script.core compile.system
TOOLS.UI.WINDOWS_FORMS.SOURCES.msvc.COMPILER_CFLAGS := -clr -wd4793 -wd4503

#Цель - C# type converter plugin
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.TYPE             := cs-dynamic-lib
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.NAME             := ui-plugins/forms
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.SOURCE_DIRS      := sources/plugins/forms
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.DLL_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.DLLS             := funner.tools.ui.windows_forms.PluginApi funner.tools.ui.windows_forms.Helpers
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.COMPILER_DEFINES := 
TOOLS.UI.WINDOWS_FORMS.FORMS_PLUGIN.IMPORTS          := 

#Цель - C# helpers lib plugin
TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN.TYPE             := cs-dynamic-lib
TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN.NAME             := funner.tools.ui.windows_forms.Helpers
TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN.SOURCE_DIRS      := sources/plugins/helpers
TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN.DLL_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN.DLLS             := funner.tools.ui.windows_forms.PluginApi
TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.HELPERS_PLUGIN.COMPILER_DEFINES := 

#Цель - C# Console plugin
TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN.TYPE             := cs-dynamic-lib
TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN.NAME             := ui-plugins/console
TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN.SOURCE_DIRS      := sources/plugins/console
TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN.DLL_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN.DLLS             := funner.tools.ui.windows_forms.PluginApi
TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.CONSOLE_PLUGIN.COMPILER_DEFINES := 

#Цель - C# Property grid plugin
TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN.TYPE             := cs-dynamic-lib
TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN.NAME             := ui-plugins/property_grid
TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN.SOURCE_DIRS      := sources/plugins/property_grid
TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN.DLL_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN.DLLS             := funner.tools.ui.windows_forms.PluginApi
TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.PROPERTY_GRID_PLUGIN.COMPILER_DEFINES := 

#Цель - Tool UI core tests
TOOLS.UI.WINDOWS_FORMS.TESTS.TYPE             := test-suite
TOOLS.UI.WINDOWS_FORMS.TESTS.INCLUDE_DIRS     :=
TOOLS.UI.WINDOWS_FORMS.TESTS.SOURCE_DIRS      := tests
TOOLS.UI.WINDOWS_FORMS.TESTS.LINK_INCLUDES    :=
TOOLS.UI.WINDOWS_FORMS.TESTS.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.TESTS.COMPILER_DEFINES :=
TOOLS.UI.WINDOWS_FORMS.TESTS.IMPORTS          := compile.tools.ui.core link.tools.ui.windows_forms compile.system link.system \
                                                 link.script.lua link.engine.script_binds.common link.engine.script_binds.lua_override \
                                                 link.common.default_console_handler compile.script.core
                                                 