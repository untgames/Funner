###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SGPLAYER_SOURCES SGPLAYER_TESTS

#Цель №1 - SGPlayer sources
SGPLAYER_SOURCES.TYPE             := static-lib
SGPLAYER_SOURCES.NAME             := sound.scene_graph_player
SGPLAYER_SOURCES.INCLUDE_DIRS     := include
SGPLAYER_SOURCES.SOURCE_DIRS      := sources
SGPLAYER_SOURCES.LIB_DIRS         :=  
SGPLAYER_SOURCES.LIBS             := 
SGPLAYER_SOURCES.COMPILER_CFLAGS  :=
SGPLAYER_SOURCES.COMPILER_DEFINES :=
SGPLAYER_SOURCES.IMPORTS          := ../manager/exports.static.mak ../../scene_graph/exports.static.mak ../../system/exports.static.mak ../low_level/exports.static.mak

#Цель №2 - SGPlayer tests
SGPLAYER_TESTS.TYPE             := test-suite
SGPLAYER_TESTS.INCLUDE_DIRS     :=
SGPLAYER_TESTS.SOURCE_DIRS      := tests
SGPLAYER_TESTS.LIBS             :=
SGPLAYER_TESTS.DLLS             :=
SGPLAYER_TESTS.DLL_DIRS         :=
SGPLAYER_TESTS.COMPILER_CFLAGS  :=
SGPLAYER_TESTS.COMPILER_DEFINES :=
SGPLAYER_TESTS.IMPORTS          := exports.static.mak ../manager/exports.static.mak ../../system/exports.static.mak ../openal_device/exports.static.mak
