###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPT.BINDS.SOURCES SCRIPT.BINDS.TESTS

#Цель №1 - SCRIPT_BINDS sources
SCRIPT.BINDS.SOURCES.TYPE             := static-lib
SCRIPT.BINDS.SOURCES.NAME             := funner.script.binds
SCRIPT.BINDS.SOURCES.INCLUDE_DIRS     := include
SCRIPT.BINDS.SOURCES.SOURCE_DIRS      := sources
SCRIPT.BINDS.SOURCES.LIB_DIRS         :=  
SCRIPT.BINDS.SOURCES.LIBS             := 
SCRIPT.BINDS.SOURCES.COMPILER_CFLAGS  := 
SCRIPT.BINDS.SOURCES.COMPILER_DEFINES := 
SCRIPT.BINDS.SOURCES.IMPORTS				  := compile.static.mak ../core/compile.static.mak ../../scene_graph/compile.static.mak ../../common/compile.static.mak \
                                         ../../render/scene_render/compile.static.mak

#Цель №2 - SCRIPT_BINDS tests
SCRIPT.BINDS.TESTS.TYPE             := test-suite
SCRIPT.BINDS.TESTS.INCLUDE_DIRS     :=
SCRIPT.BINDS.TESTS.SOURCE_DIRS      := tests/lua
SCRIPT.BINDS.TESTS.LIB_DIRS         :=  
SCRIPT.BINDS.TESTS.LIBS             :=
SCRIPT.BINDS.TESTS.COMPILER_CFLAGS  := 
SCRIPT.BINDS.TESTS.COMPILER_DEFINES :=
SCRIPT.BINDS.TESTS.IMPORTS	  		  := compile.static.mak link.static.mak ../core/compile.static.mak  ../../common/compile.static.mak ../core/lua.link.static.mak
