###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SUBSYSTEMS.SOURCES ENGINE.SUBSYSTEMS.TESTS

#Цель - sources
ENGINE.SUBSYSTEMS.TYPE              := static-lib
ENGINE.SUBSYSTEMS.NAME              := funner.engine.subsystems
ENGINE.SUBSYSTEMS.INCLUDE_DIRS      :=
ENGINE.SUBSYSTEMS.SOURCE_DIRS       := sources/common
ENGINE.SUBSYSTEMS.LIB_DIRS          :=
ENGINE.SUBSYSTEMS.LIBS              := 
ENGINE.SUBSYSTEMS.COMPILER_CFLAGS   := 
ENGINE.SUBSYSTEMS.COMPILER_DEFINES  :=
ENGINE.SUBSYSTEMS.IMPORTS           := ../core/compile.static.mak ../../render/scene_render/compile.static.mak \
                                       ../../input/manager/compile.static.mak ../../input/low_level/compile.static.mak
ENGINE.SUBSYSTEMS.win32.SOURCE_DIRS := sources/window_manager sources/window_manager/win32
ENGINE.SUBSYSTEMS.win32.IMPORTS     := ../../input/window_driver/compile.static.mak ../../render/mid_level_window_driver/compile.static.mak

#Цель - tests
ENGINE.SUBSYSTEMS.TESTS.TYPE             := test-suite
ENGINE.SUBSYSTEMS.TESTS.INCLUDE_DIRS     :=
ENGINE.SUBSYSTEMS.TESTS.SOURCE_DIRS      := tests
ENGINE.SUBSYSTEMS.TESTS.LIB_DIRS         :=  
ENGINE.SUBSYSTEMS.TESTS.LIBS             :=
ENGINE.SUBSYSTEMS.TESTS.COMPILER_CFLAGS  :=
ENGINE.SUBSYSTEMS.TESTS.COMPILER_DEFINES :=
ENGINE.SUBSYSTEMS.TESTS.IMPORTS          := ../../common/compile.static.mak ../../system/compile.static.mak \
                                            scene_render.link.static.mak scene_player.link.static.mak \
                                            direct_input_driver.link.static.mak window_input_driver.link.static.mak input_manager.link.static.mak \
                                            ../../input/manager/xkeymap.link.static.mak ../../input/manager/xkeyreg.link.static.mak \
                                            ../../render/opengl_driver/link.static.mak ../../common/configurator.link.static.mak \
                                            ../../sound/openal_device/link.static.mak ../../sound/openal_device/run.static.mak
