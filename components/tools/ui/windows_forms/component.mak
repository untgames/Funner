###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := TOOLS.UI.WINDOWS_FORMS.PLUGIN_API TOOLS.UI.WINDOWS_FORMS.SOURCES TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN TOOLS.UI.WINDOWS_FORMS.TESTS

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
TOOLS.UI.WINDOWS_FORMS.SOURCES.IMPORTS			    := ../core/compile.static.mak ../../../script/core/compile.static.mak \
                                                   ../../../common/compile.static.mak ../../../system/compile.static.mak
TOOLS.UI.WINDOWS_FORMS.SOURCES.msvc.COMPILER_CFLAGS := -clr -wd4793 -wd4503

#Цель - Windows forms window system unmanaged sources
TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN.TYPE             := cs-dynamic-lib
TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN.NAME             := ui-plugins/test_cs_plugin
TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN.SOURCE_DIRS      := sources/plugins
TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN.DLL_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN.DLLS             := funner.tools.ui.windows_forms.PluginApi
TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.TEST_PLUGIN.COMPILER_DEFINES := 

#Цель - Tool UI core tests
TOOLS.UI.WINDOWS_FORMS.TESTS.TYPE             := test-suite
TOOLS.UI.WINDOWS_FORMS.TESTS.INCLUDE_DIRS     :=
TOOLS.UI.WINDOWS_FORMS.TESTS.SOURCE_DIRS      := tests
TOOLS.UI.WINDOWS_FORMS.TESTS.LIBS             :=
TOOLS.UI.WINDOWS_FORMS.TESTS.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.TESTS.COMPILER_DEFINES :=
TOOLS.UI.WINDOWS_FORMS.TESTS.IMPORTS          := ../core/compile.static.mak link.static.mak ../../../system/compile.static.mak \
                                                 ../../../system/link.static.mak ../../../script/lua/link.static.mak
