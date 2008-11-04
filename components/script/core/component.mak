###################################################################################################
#?пределени¤ и константы
###################################################################################################
TARGETS := SCRIPT.CORE.SOURCES SCRIPT.CORE.TESTS

#?ель є1 - SCRIPT_CORE sources
SCRIPT.CORE.SOURCES.TYPE             := static-lib
SCRIPT.CORE.SOURCES.NAME             := funner.script.core
SCRIPT.CORE.SOURCES.INCLUDE_DIRS     := 
SCRIPT.CORE.SOURCES.SOURCE_DIRS      := sources
SCRIPT.CORE.SOURCES.LIB_DIRS         :=  
SCRIPT.CORE.SOURCES.LIBS             :=
SCRIPT.CORE.SOURCES.COMPILER_CFLAGS  := 
SCRIPT.CORE.SOURCES.COMPILER_DEFINES := 
SCRIPT.CORE.SOURCES.IMPORTS				   := compile.static.mak ../../common/compile.static.mak

#?ель є2 - SCRIPT_CORE tests
SCRIPT.CORE.TESTS.TYPE             := test-suite
SCRIPT.CORE.TESTS.INCLUDE_DIRS     :=
SCRIPT.CORE.TESTS.SOURCE_DIRS      := tests
SCRIPT.CORE.TESTS.LIB_DIRS         :=  
SCRIPT.CORE.TESTS.LIBS             :=
SCRIPT.CORE.TESTS.COMPILER_CFLAGS  := 
SCRIPT.CORE.TESTS.COMPILER_DEFINES :=
SCRIPT.CORE.TESTS.IMPORTS				   := compile.static.mak link.static.mak ../../common/compile.static.mak
