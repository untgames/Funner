###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SGPLAYER_SOURCES SGPLAYER_TESTS

#Цель №1 - SGPlayer sources
SGPLAYER_SOURCES.TYPE             := static-lib
SGPLAYER_SOURCES.NAME             := sound.scene_graph_player
SGPLAYER_SOURCES.INCLUDE_DIRS     := 
SGPLAYER_SOURCES.SOURCE_DIRS      := sources
SGPLAYER_SOURCES.LIB_DIRS         :=  
SGPLAYER_SOURCES.LIBS             := 
SGPLAYER_SOURCES.COMPILER_CFLAGS  :=
SGPLAYER_SOURCES.COMPILER_DEFINES :=
SGPLAYER_SOURCES.IMPORTS          := compile.static.mak ../low_level/compile.static.mak

#Цель №2 - SGPlayer tests
SGPLAYER_TESTS.TYPE             := test-suite
SGPLAYER_TESTS.INCLUDE_DIRS     :=
SGPLAYER_TESTS.SOURCE_DIRS      := tests
SGPLAYER_TESTS.LIBS             :=
SGPLAYER_TESTS.DLLS             :=
SGPLAYER_TESTS.DLL_DIRS         :=
SGPLAYER_TESTS.COMPILER_CFLAGS  :=
SGPLAYER_TESTS.COMPILER_DEFINES :=
SGPLAYER_TESTS.IMPORTS          := link.static.mak compile.static.mak ../../system/exports.static.mak ../openal_device/link.static.mak ../../media/sound/snddecl.link.static.mak \
														       ../../media/sound/default_decoders.link.static.mak ../openal_device/run.static.mak
