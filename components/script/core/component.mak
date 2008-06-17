###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPT_CORE_SOURCES SCRIPT_CORE_TESTS

#Цель №1 - SCRIPT_CORE sources
SCRIPT_CORE_SOURCES.TYPE             := static-lib
SCRIPT_CORE_SOURCES.NAME             := script.core
SCRIPT_CORE_SOURCES.INCLUDE_DIRS     := ../../../extern/lua/include
SCRIPT_CORE_SOURCES.SOURCE_DIRS      := sources/core sources/lua
SCRIPT_CORE_SOURCES.LIB_DIRS         :=  
SCRIPT_CORE_SOURCES.LIBS             :=
SCRIPT_CORE_SOURCES.COMPILER_CFLAGS  := 
SCRIPT_CORE_SOURCES.COMPILER_DEFINES := 
SCRIPT_CORE_SOURCES.IMPORTS				   := compile.static.mak ../../common/compile.static.mak

#Цель №2 - SCRIPT_CORE tests
SCRIPT_CORE_TESTS.TYPE             := test-suite
SCRIPT_CORE_TESTS.INCLUDE_DIRS     :=
SCRIPT_CORE_TESTS.SOURCE_DIRS      := tests/core tests/lua
SCRIPT_CORE_TESTS.LIB_DIRS         :=  
SCRIPT_CORE_TESTS.LIBS             :=
SCRIPT_CORE_TESTS.COMPILER_CFLAGS  := 
SCRIPT_CORE_TESTS.COMPILER_DEFINES :=
SCRIPT_CORE_TESTS.IMPORTS				   := compile.static.mak link.static.mak ../../common/compile.static.mak
