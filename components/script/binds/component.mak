###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SCRIPT_BINDS_COMPONENTS SCRIPT_BINDS_TESTS

#Цель №1 - SCRIPT_BINDS sources
SCRIPT_BINDS_COMPONENTS.TYPE             := static-lib
SCRIPT_BINDS_COMPONENTS.NAME             := script.binds
SCRIPT_BINDS_COMPONENTS.INCLUDE_DIRS     := include
SCRIPT_BINDS_COMPONENTS.SOURCE_DIRS      := sources
SCRIPT_BINDS_COMPONENTS.LIB_DIRS         :=  
SCRIPT_BINDS_COMPONENTS.LIBS             := 
SCRIPT_BINDS_COMPONENTS.COMPILER_CFLAGS  := 
SCRIPT_BINDS_COMPONENTS.COMPILER_DEFINES := 
SCRIPT_BINDS_COMPONENTS.IMPORTS				  := ../core/exports.static.mak ../../scene_graph/exports.static.mak

#Цель №2 - SCRIPT_BINDS tests
SCRIPT_BINDS_TESTS.TYPE             := test-suite
SCRIPT_BINDS_TESTS.INCLUDE_DIRS     := include
SCRIPT_BINDS_TESTS.SOURCE_DIRS      := tests/lua
SCRIPT_BINDS_TESTS.LIB_DIRS         :=  
SCRIPT_BINDS_TESTS.LIBS             :=
SCRIPT_BINDS_TESTS.COMPILER_CFLAGS  := 
SCRIPT_BINDS_TESTS.COMPILER_DEFINES :=
SCRIPT_BINDS_TESTS.IMPORTS	  		   := ../core/exports.static.mak ../../scene_graph/exports.static.mak exports.static.mak
