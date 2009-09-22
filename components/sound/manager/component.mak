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
SOUND.MANAGER.SOURCES.IMPORTS          := compile.sound.manager compile.media.sound compile.common

#Цель №2 - Sound system tests
SOUND.MANAGER.TESTS.TYPE             := test-suite
SOUND.MANAGER.TESTS.INCLUDE_DIRS     :=
SOUND.MANAGER.TESTS.SOURCE_DIRS      := tests/manager
SOUND.MANAGER.TESTS.LIBS             :=
SOUND.MANAGER.TESTS.DLLS             :=
SOUND.MANAGER.TESTS.DLL_DIRS         :=
SOUND.MANAGER.TESTS.COMPILER_CFLAGS  :=
SOUND.MANAGER.TESTS.COMPILER_DEFINES :=
SOUND.MANAGER.TESTS.IMPORTS          := compile.sound.manager link.sound.manager link.sound.openal_driver compile.system link.media.sound.snddecl \
																       link.media.sound.default_decoders run.sound.openal_driver
