###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPT_CORE_SOURCES SCRIPT_CORE_TESTS LUA_SOURCES LUA_TESTS

#Цель №1 - SCRIPT_CORE sources
SCRIPT_CORE_SOURCES.TYPE             := static-lib
SCRIPT_CORE_SOURCES.NAME             := script.core
SCRIPT_CORE_SOURCES.INCLUDE_DIRS     := 
SCRIPT_CORE_SOURCES.SOURCE_DIRS      := sources/core
SCRIPT_CORE_SOURCES.LIB_DIRS         :=  
SCRIPT_CORE_SOURCES.LIBS             :=
SCRIPT_CORE_SOURCES.COMPILER_CFLAGS  := 
SCRIPT_CORE_SOURCES.COMPILER_DEFINES := 
SCRIPT_CORE_SOURCES.IMPORTS				   := compile.static.mak ../../common/compile.static.mak

#Цель №2 - SCRIPT_CORE tests
SCRIPT_CORE_TESTS.TYPE             := test-suite
SCRIPT_CORE_TESTS.INCLUDE_DIRS     :=
SCRIPT_CORE_TESTS.SOURCE_DIRS      := tests/core
SCRIPT_CORE_TESTS.LIB_DIRS         :=  
SCRIPT_CORE_TESTS.LIBS             :=
SCRIPT_CORE_TESTS.COMPILER_CFLAGS  := 
SCRIPT_CORE_TESTS.COMPILER_DEFINES :=
SCRIPT_CORE_TESTS.IMPORTS				   := compile.static.mak link.static.mak ../../common/compile.static.mak

#Цель №3 - LUA sources
LUA_SOURCES.TYPE             := static-lib
LUA_SOURCES.NAME             := script.lua
LUA_SOURCES.INCLUDE_DIRS     := ../../../extern/lua/include
LUA_SOURCES.SOURCE_DIRS      := sources/lua
LUA_SOURCES.LIB_DIRS         :=  
LUA_SOURCES.LIBS             :=
LUA_SOURCES.COMPILER_CFLAGS  := 
LUA_SOURCES.COMPILER_DEFINES := 
LUA_SOURCES.IMPORTS				   := compile.static.mak ../../common/compile.static.mak

#Цель №4 - LUA tests
LUA_TESTS.TYPE             := test-suite
LUA_TESTS.INCLUDE_DIRS     :=
LUA_TESTS.SOURCE_DIRS      := tests/lua
LUA_TESTS.LIB_DIRS         :=  
LUA_TESTS.LIBS             :=
LUA_TESTS.COMPILER_CFLAGS  := 
LUA_TESTS.COMPILER_DEFINES :=
LUA_TESTS.IMPORTS				   := compile.static.mak lua.link.static.mak ../../common/compile.static.mak

