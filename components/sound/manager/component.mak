###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND_SYSTEM_SOURCES SOUND_SYSTEM_TESTS

#Цель №1 - Sound system sources
SOUND_SYSTEM_SOURCES.TYPE             := static-lib
SOUND_SYSTEM_SOURCES.NAME             := sound.manager
SOUND_SYSTEM_SOURCES.INCLUDE_DIRS     := include ../../medialib/include
SOUND_SYSTEM_SOURCES.SOURCE_DIRS      := sources/manager
SOUND_SYSTEM_SOURCES.LIB_DIRS         :=  
SOUND_SYSTEM_SOURCES.LIBS             := 
SOUND_SYSTEM_SOURCES.COMPILER_CFLAGS  :=
SOUND_SYSTEM_SOURCES.COMPILER_DEFINES := 
SOUND_SYSTEM_SOURCES.IMPORTS          := ../low_level/exports.static.mak ../../system/exports.static.mak

#Цель №2 - Sound system tests
SOUND_SYSTEM_TESTS.TYPE             := test-suite
SOUND_SYSTEM_TESTS.INCLUDE_DIRS     :=
SOUND_SYSTEM_TESTS.SOURCE_DIRS      := tests/manager
SOUND_SYSTEM_TESTS.LIBS             :=
SOUND_SYSTEM_TESTS.DLLS             :=
SOUND_SYSTEM_TESTS.DLL_DIRS         :=
SOUND_SYSTEM_TESTS.COMPILER_CFLAGS  :=
SOUND_SYSTEM_TESTS.COMPILER_DEFINES :=
SOUND_SYSTEM_TESTS.IMPORTS          := exports.static.mak ../low_level/exports.static.mak ../openal_device/exports.static.mak ../../system/exports.static.mak
