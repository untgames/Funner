###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.OPEAL_DRIVER.SOURCES SOUND.OPEAL_DRIVER.TESTS

#Цель №1 - OPENALDEVICE sources
SOUND.OPEAL_DRIVER.SOURCES.TYPE             := static-lib
SOUND.OPEAL_DRIVER.SOURCES.NAME             := sound.low_level.openal_driver
SOUND.OPEAL_DRIVER.SOURCES.INCLUDE_DIRS     := ../../../extern/openalsdk/include
SOUND.OPEAL_DRIVER.SOURCES.SOURCE_DIRS      := sources
SOUND.OPEAL_DRIVER.SOURCES.LIB_DIRS         :=  
SOUND.OPEAL_DRIVER.SOURCES.LIBS             := 
SOUND.OPEAL_DRIVER.SOURCES.COMPILER_CFLAGS  :=
SOUND.OPEAL_DRIVER.SOURCES.COMPILER_DEFINES := 
SOUND.OPEAL_DRIVER.SOURCES.IMPORTS          := ../low_level/compile.static.mak ../../media/sound/compile.static.mak ../../system/compile.static.mak 

#Цель №2 - OPENALDEVICE tests
SOUND.OPEAL_DRIVER.TESTS.TYPE             := test-suite
SOUND.OPEAL_DRIVER.TESTS.INCLUDE_DIRS     := ../../xtl/include
SOUND.OPEAL_DRIVER.TESTS.SOURCE_DIRS      := tests
SOUND.OPEAL_DRIVER.TESTS.LIB_DIRS         :=
SOUND.OPEAL_DRIVER.TESTS.LIBS             :=
SOUND.OPEAL_DRIVER.TESTS.DLLS             :=
SOUND.OPEAL_DRIVER.TESTS.DLL_DIRS         :=
SOUND.OPEAL_DRIVER.TESTS.COMPILER_CFLAGS  :=
SOUND.OPEAL_DRIVER.TESTS.COMPILER_DEFINES :=
SOUND.OPEAL_DRIVER.TESTS.IMPORTS          := link.static.mak run.static.mak ../../common/compile.static.mak ../../system/compile.static.mak ../low_level/compile.static.mak ../../media/sound/default_decoders.link.static.mak
