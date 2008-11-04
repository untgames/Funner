###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPT.LUA.SOURCES SCRIPT.LUA.TESTS

#SCRIPT_LUA sources
SCRIPT.LUA.SOURCES.TYPE             := static-lib
SCRIPT.LUA.SOURCES.NAME             := funner.script.lua
SCRIPT.LUA.SOURCES.INCLUDE_DIRS     := ../../../extern/lua/include
SCRIPT.LUA.SOURCES.SOURCE_DIRS      := sources
SCRIPT.LUA.SOURCES.LIB_DIRS         :=  
SCRIPT.LUA.SOURCES.LIBS             :=
SCRIPT.LUA.SOURCES.COMPILER_CFLAGS  := 
SCRIPT.LUA.SOURCES.COMPILER_DEFINES := __STRICT_ANSI__
SCRIPT.LUA.SOURCES.IMPORTS				  := ../core/compile.static.mak ../../common/compile.static.mak

#SCRIPT_LUA tests
SCRIPT.LUA.TESTS.TYPE             := test-suite
SCRIPT.LUA.TESTS.INCLUDE_DIRS     :=
SCRIPT.LUA.TESTS.SOURCE_DIRS      := tests
SCRIPT.LUA.TESTS.LIB_DIRS         :=
SCRIPT.LUA.TESTS.LIBS             :=
SCRIPT.LUA.TESTS.COMPILER_CFLAGS  :=
SCRIPT.LUA.TESTS.COMPILER_DEFINES :=
SCRIPT.LUA.TESTS.IMPORTS				  := ../core/compile.static.mak link.static.mak ../../common/compile.static.mak
