###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SOUND.OPENAL_DRIVER.SOURCES SOUND.OPENAL_DRIVER.TESTS

#Цель №1 - OPENALDEVICE sources
SOUND.OPENAL_DRIVER.SOURCES.TYPE                   := static-lib
SOUND.OPENAL_DRIVER.SOURCES.NAME                   := funner.sound.low_level.openal_driver
SOUND.OPENAL_DRIVER.SOURCES.win32.INCLUDE_DIRS     := ../../../extern/openalsdk/include
SOUND.OPENAL_DRIVER.SOURCES.SOURCE_DIRS            := sources
SOUND.OPENAL_DRIVER.SOURCES.macosx.COMPILER_CFLAGS := -I/System/Library/Frameworks/OpenAL.framework/Versions/A/Headers
SOUND.OPENAL_DRIVER.SOURCES.linux.COMPILER_CFLAGS  := -I/usr/include/AL
SOUND.OPENAL_DRIVER.SOURCES.IMPORTS                := ../low_level/compile.static.mak ../../media/sound/compile.static.mak ../../system/compile.static.mak 

#Цель №2 - OPENALDEVICE tests
SOUND.OPENAL_DRIVER.TESTS.TYPE             := test-suite
SOUND.OPENAL_DRIVER.TESTS.INCLUDE_DIRS     := ../../xtl/include
SOUND.OPENAL_DRIVER.TESTS.SOURCE_DIRS      := tests
SOUND.OPENAL_DRIVER.TESTS.LIB_DIRS         :=
SOUND.OPENAL_DRIVER.TESTS.LIBS             :=
SOUND.OPENAL_DRIVER.TESTS.DLLS             :=
SOUND.OPENAL_DRIVER.TESTS.DLL_DIRS         :=
SOUND.OPENAL_DRIVER.TESTS.COMPILER_CFLAGS  :=
SOUND.OPENAL_DRIVER.TESTS.COMPILER_DEFINES :=
SOUND.OPENAL_DRIVER.TESTS.IMPORTS          := link.static.mak run.static.mak ../../common/compile.static.mak ../../system/compile.static.mak ../low_level/compile.static.mak ../../media/sound/default_decoders.link.static.mak
