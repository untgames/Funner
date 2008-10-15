###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPT.CORE.SOURCES SCRIPT.CORE.TESTS SCRIPT.LUA.SOURCES SCRIPT.LUA.TESTS

#Цель №1 - SCRIPT_CORE sources
SCRIPT.CORE.SOURCES.TYPE             := static-lib
SCRIPT.CORE.SOURCES.NAME             := funner.script.core
SCRIPT.CORE.SOURCES.INCLUDE_DIRS     := 
SCRIPT.CORE.SOURCES.SOURCE_DIRS      := sources/core
SCRIPT.CORE.SOURCES.LIB_DIRS         :=  
SCRIPT.CORE.SOURCES.LIBS             :=
SCRIPT.CORE.SOURCES.COMPILER_CFLAGS  := 
SCRIPT.CORE.SOURCES.COMPILER_DEFINES := 
SCRIPT.CORE.SOURCES.IMPORTS				   := compile.static.mak ../../common/compile.static.mak

#Цель №2 - SCRIPT_CORE tests
SCRIPT.CORE.TESTS.TYPE             := test-suite
SCRIPT.CORE.TESTS.INCLUDE_DIRS     :=
SCRIPT.CORE.TESTS.SOURCE_DIRS      := tests/core
SCRIPT.CORE.TESTS.LIB_DIRS         :=  
SCRIPT.CORE.TESTS.LIBS             :=
SCRIPT.CORE.TESTS.COMPILER_CFLAGS  := 
SCRIPT.CORE.TESTS.COMPILER_DEFINES :=
SCRIPT.CORE.TESTS.IMPORTS				   := compile.static.mak link.static.mak ../../common/compile.static.mak

#Цель №3 - SCRIPT_LUA sources
SCRIPT.LUA.SOURCES.TYPE             := static-lib
SCRIPT.LUA.SOURCES.NAME             := funner.script.lua
SCRIPT.LUA.SOURCES.INCLUDE_DIRS     := ../../../extern/lua/include
SCRIPT.LUA.SOURCES.SOURCE_DIRS      := sources/lua
SCRIPT.LUA.SOURCES.LIB_DIRS         :=  
SCRIPT.LUA.SOURCES.LIBS             :=
SCRIPT.LUA.SOURCES.COMPILER_CFLAGS  := 
SCRIPT.LUA.SOURCES.COMPILER_DEFINES := 
SCRIPT.LUA.SOURCES.IMPORTS				   := compile.static.mak ../../common/compile.static.mak

#Цель №4 - SCRIPT_LUA tests
SCRIPT.LUA.TESTS.TYPE             := test-suite
SCRIPT.LUA.TESTS.INCLUDE_DIRS     :=
SCRIPT.LUA.TESTS.SOURCE_DIRS      := tests/lua
SCRIPT.LUA.TESTS.LIB_DIRS         :=  
SCRIPT.LUA.TESTS.LIBS             :=
SCRIPT.LUA.TESTS.COMPILER_CFLAGS  := 
SCRIPT.LUA.TESTS.COMPILER_DEFINES :=
SCRIPT.LUA.TESTS.IMPORTS				   := compile.static.mak lua.link.static.mak ../../common/compile.static.mak

