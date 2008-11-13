###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SUBSYSTEMS.SOURCES ENGINE.SUBSYSTEMS.TESTS

#Цель - sources
ENGINE.SUBSYSTEMS.SOURCES.TYPE              := static-lib
ENGINE.SUBSYSTEMS.SOURCES.NAME              := funner.engine.subsystems
ENGINE.SUBSYSTEMS.SOURCES.INCLUDE_DIRS      :=
ENGINE.SUBSYSTEMS.SOURCES.SOURCE_DIRS       := sources/common
ENGINE.SUBSYSTEMS.SOURCES.LIB_DIRS          :=
ENGINE.SUBSYSTEMS.SOURCES.LIBS              := 
ENGINE.SUBSYSTEMS.SOURCES.COMPILER_CFLAGS   := 
ENGINE.SUBSYSTEMS.SOURCES.COMPILER_DEFINES  :=
ENGINE.SUBSYSTEMS.SOURCES.IMPORTS           := ../core/compile.static.mak ../../render/scene_render/compile.static.mak \
                                               ../../input/manager/compile.static.mak ../../input/low_level/compile.static.mak \
                                               ../../sound/scene_player/compile.static.mak ../../system/compile.static.mak \
                                               ../../script/core/compile.static.mak
ENGINE.SUBSYSTEMS.SOURCES.win32.IMPORTS     := ../../input/window_driver/compile.static.mak ../../render/mid_level_window_driver/compile.static.mak
ENGINE.SUBSYSTEMS.SOURCES.win32.SOURCE_DIRS := sources/window_manager

#Цель - tests
ENGINE.SUBSYSTEMS.TESTS.TYPE             := test-suite
ENGINE.SUBSYSTEMS.TESTS.INCLUDE_DIRS     :=
ENGINE.SUBSYSTEMS.TESTS.SOURCE_DIRS      := tests
ENGINE.SUBSYSTEMS.TESTS.LIB_DIRS         :=  
ENGINE.SUBSYSTEMS.TESTS.LIBS             :=
ENGINE.SUBSYSTEMS.TESTS.COMPILER_CFLAGS  :=
ENGINE.SUBSYSTEMS.TESTS.COMPILER_DEFINES :=
ENGINE.SUBSYSTEMS.TESTS.IMPORTS          := ../core/compile.static.mak ../../common/compile.static.mak ../../system/compile.static.mak \
                                            ../../render/scene_render/compile.static.mak ../../input/manager/compile.static.mak \
                                            scene_render.link.static.mak scene_player.link.static.mak \
                                             input_manager.link.static.mak shell.link.static.mak \
                                            ../../input/manager/xkeymap.link.static.mak ../../input/manager/xkeyreg.link.static.mak \
                                            ../../sound/openal_device/link.static.mak ../../sound/openal_device/run.static.mak \
                                            ../../script/lua/link.static.mak ../script_binds/math.link.static.mak ../script_binds/sg.link.static.mak
ENGINE.SUBSYSTEMS.TESTS.win32.IMPORTS := ../../render/opengl_driver/link.static.mak window_input_driver.link.static.mak window_manager.link.static.mak window_renderer.link.static.mak
