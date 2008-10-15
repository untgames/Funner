###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.MANAGER.SOURCES SOUND.MANAGER.TESTS

#Цель №1 - Sound system sources
SOUND.MANAGER.SOURCES.TYPE             := static-lib
SOUND.MANAGER.SOURCES.NAME             := funner.sound.manager
SOUND.MANAGER.SOURCES.INCLUDE_DIRS     := 
SOUND.MANAGER.SOURCES.SOURCE_DIRS      := sources/manager
SOUND.MANAGER.SOURCES.LIB_DIRS         :=  
SOUND.MANAGER.SOURCES.LIBS             := 
SOUND.MANAGER.SOURCES.COMPILER_CFLAGS  :=
SOUND.MANAGER.SOURCES.COMPILER_DEFINES := 
SOUND.MANAGER.SOURCES.IMPORTS          := compile.static.mak ../../media/sound/compile.static.mak

#Цель №2 - Sound system tests
SOUND.MANAGER.TESTS.TYPE             := test-suite
SOUND.MANAGER.TESTS.INCLUDE_DIRS     :=
SOUND.MANAGER.TESTS.SOURCE_DIRS      := tests/manager
SOUND.MANAGER.TESTS.LIBS             :=
SOUND.MANAGER.TESTS.DLLS             :=
SOUND.MANAGER.TESTS.DLL_DIRS         :=
SOUND.MANAGER.TESTS.COMPILER_CFLAGS  :=
SOUND.MANAGER.TESTS.COMPILER_DEFINES :=
SOUND.MANAGER.TESTS.IMPORTS          := compile.static.mak link.static.mak ../openal_device/link.static.mak ../../system/compile.static.mak ../../media/sound/snddecl.link.static.mak \
																       ../../media/sound/default_decoders.link.static.mak ../openal_device/run.static.mak
