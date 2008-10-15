###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := TOOLS.UI.CORE.SOURCES TOOLS.UI.CORE.TESTS

#Цель №1 - Tools UI core sources
TOOLS.UI.CORE.SOURCES.TYPE             := static-lib
TOOLS.UI.CORE.SOURCES.NAME             := funner.tools.ui.core
TOOLS.UI.CORE.SOURCES.INCLUDE_DIRS     := 
TOOLS.UI.CORE.SOURCES.SOURCE_DIRS      := sources
TOOLS.UI.CORE.SOURCES.LIB_DIRS         :=  
TOOLS.UI.CORE.SOURCES.LIBS             :=
TOOLS.UI.CORE.SOURCES.COMPILER_CFLAGS  := 
TOOLS.UI.CORE.SOURCES.COMPILER_DEFINES := 
TOOLS.UI.CORE.SOURCES.IMPORTS				   := compile.static.mak ../../../common/compile.static.mak

#Цель №2 - Tool UI core tests
TOOLS.UI.CORE.TESTS.TYPE             := test-suite
TOOLS.UI.CORE.TESTS.INCLUDE_DIRS     :=
TOOLS.UI.CORE.TESTS.SOURCE_DIRS      := tests
TOOLS.UI.CORE.TESTS.LIB_DIRS         :=  
TOOLS.UI.CORE.TESTS.LIBS             :=
TOOLS.UI.CORE.TESTS.COMPILER_CFLAGS  := 
TOOLS.UI.CORE.TESTS.COMPILER_DEFINES :=
TOOLS.UI.CORE.TESTS.IMPORTS				   := compile.static.mak link.static.mak ../../../common/compile.static.mak
