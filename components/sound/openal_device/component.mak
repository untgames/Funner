###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := OPENALDEVICE_COMPONENTS OPENALDEVICE_TESTS

#Цель №1 - OPENALDEVICE sources
OPENALDEVICE_COMPONENTS.TYPE             := static-lib
OPENALDEVICE_COMPONENTS.NAME             := sound.low_level.openal_driver
OPENALDEVICE_COMPONENTS.INCLUDE_DIRS     := ../../../extern/openalsdk/include
OPENALDEVICE_COMPONENTS.SOURCE_DIRS      := sources
OPENALDEVICE_COMPONENTS.LIB_DIRS         :=  
OPENALDEVICE_COMPONENTS.LIBS             := 
OPENALDEVICE_COMPONENTS.COMPILER_CFLAGS  :=
OPENALDEVICE_COMPONENTS.COMPILER_DEFINES := 
OPENALDEVICE_COMPONENTS.IMPORTS          := ../low_level/compile.static.mak ../../media/sound/compile.static.mak ../../system/exports.static.mak 

#Цель №2 - OPENALDEVICE tests
OPENALDEVICE_TESTS.TYPE             := test-suite
OPENALDEVICE_TESTS.INCLUDE_DIRS     := ../../xtl/include
OPENALDEVICE_TESTS.SOURCE_DIRS      := tests
OPENALDEVICE_TESTS.LIB_DIRS         :=
OPENALDEVICE_TESTS.LIBS             :=
OPENALDEVICE_TESTS.DLLS             :=
OPENALDEVICE_TESTS.DLL_DIRS         :=
OPENALDEVICE_TESTS.COMPILER_CFLAGS  :=
OPENALDEVICE_TESTS.COMPILER_DEFINES :=
OPENALDEVICE_TESTS.IMPORTS          := link.static.mak run.static.mak ../../common/compile.static.mak ../../system/exports.static.mak ../low_level/compile.static.mak ../../media/sound/default_decoders.link.static.mak
