###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := TOOLS.UI.WINDOWS_FORMS.SOURCES TOOLS.UI.WINDOWS_FORMS.TESTS

#Цель №1 - Windows forms window system unmanaged sources
TOOLS.UI.WINDOWS_FORMS.SOURCES.TYPE             := static-lib
TOOLS.UI.WINDOWS_FORMS.SOURCES.NAME             := funner.tools.ui.windows_forms
TOOLS.UI.WINDOWS_FORMS.SOURCES.INCLUDE_DIRS     := 
TOOLS.UI.WINDOWS_FORMS.SOURCES.SOURCE_DIRS      := sources
TOOLS.UI.WINDOWS_FORMS.SOURCES.LIB_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.SOURCES.LIBS             :=
TOOLS.UI.WINDOWS_FORMS.SOURCES.DLL_DIRS         := ../../../../extern/windows_controls/bin
TOOLS.UI.WINDOWS_FORMS.SOURCES.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.SOURCES.COMPILER_DEFINES := 
TOOLS.UI.WINDOWS_FORMS.SOURCES.IMPORTS			    := ../core/compile.static.mak ../../../script/core/compile.static.mak\
                                                   ../../../common/compile.static.mak ../../../system/compile.static.mak
TOOLS.UI.WINDOWS_FORMS.SOURCES.msvc.COMPILER_CFLAGS := -clr -wd4793 -wd4503

#Цель №2 - Tool UI core tests
TOOLS.UI.WINDOWS_FORMS.TESTS.TYPE             := test-suite
TOOLS.UI.WINDOWS_FORMS.TESTS.INCLUDE_DIRS     :=
TOOLS.UI.WINDOWS_FORMS.TESTS.SOURCE_DIRS      := tests
TOOLS.UI.WINDOWS_FORMS.TESTS.LIB_DIRS         :=
TOOLS.UI.WINDOWS_FORMS.TESTS.LIBS             :=
TOOLS.UI.WINDOWS_FORMS.TESTS.COMPILER_CFLAGS  := 
TOOLS.UI.WINDOWS_FORMS.TESTS.COMPILER_DEFINES :=
TOOLS.UI.WINDOWS_FORMS.TESTS.IMPORTS          := ../core/compile.static.mak link.static.mak ../../../system/compile.static.mak \
                                                 ../../../system/link.static.mak ../../../script/core/lua.link.static.mak
