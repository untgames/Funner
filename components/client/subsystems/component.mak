###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := CLIENT.SUBSYSTEMS.WINDOW_RENDERER CLIENT.SUBSYSTEMS.SCENE_RENDER CLIENT.SUBSYSTEMS.SCENE_PLAYER CLIENT.SUBSYSTEMS.INPUT \
           CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER CLIENT.SUBSYSTEMS.TESTS

#Цель - sources
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.TYPE             := static-lib
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.NAME             := funner.client.window_renderer
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.SOURCE_DIRS      := sources/window_renderer
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.LIBS             := 
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.IMPORTS          := ../window_manager/compile.static.mak ../../render/mid_level_window_driver/compile.static.mak

#Цель - sources
CLIENT.SUBSYSTEMS.SCENE_RENDER.TYPE             := static-lib
CLIENT.SUBSYSTEMS.SCENE_RENDER.NAME             := funner.client.scene_render
CLIENT.SUBSYSTEMS.SCENE_RENDER.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.SCENE_RENDER.SOURCE_DIRS      := sources/scene_render
CLIENT.SUBSYSTEMS.SCENE_RENDER.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.SCENE_RENDER.LIBS             := 
CLIENT.SUBSYSTEMS.SCENE_RENDER.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.SCENE_RENDER.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.SCENE_RENDER.IMPORTS          := ../../common/compile.static.mak ../core/compile.static.mak ../../render/scene_render/compile.static.mak

#Цель - sources
CLIENT.SUBSYSTEMS.SCENE_PLAYER.TYPE             := static-lib
CLIENT.SUBSYSTEMS.SCENE_PLAYER.NAME             := funner.client.scene_player
CLIENT.SUBSYSTEMS.SCENE_PLAYER.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.SCENE_PLAYER.SOURCE_DIRS      := sources/scene_player
CLIENT.SUBSYSTEMS.SCENE_PLAYER.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.SCENE_PLAYER.LIBS             := 
CLIENT.SUBSYSTEMS.SCENE_PLAYER.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.SCENE_PLAYER.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.SCENE_PLAYER.IMPORTS          := ../../common/compile.static.mak ../core/compile.static.mak ../../sound/scene_player/compile.static.mak

#Цель - sources
CLIENT.SUBSYSTEMS.INPUT.TYPE             := static-lib
CLIENT.SUBSYSTEMS.INPUT.NAME             := funner.client.input_manager
CLIENT.SUBSYSTEMS.INPUT.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.INPUT.SOURCE_DIRS      := sources/input_manager
CLIENT.SUBSYSTEMS.INPUT.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.INPUT.LIBS             := 
CLIENT.SUBSYSTEMS.INPUT.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.INPUT.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.INPUT.IMPORTS          := ../../common/compile.static.mak ../core/compile.static.mak ../../input/low_level/compile.static.mak \
                                            ../../input/manager/compile.static.mak

#Цель - sources
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.TYPE             := static-lib
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.NAME             := funner.client.direct_input_driver
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.SOURCE_DIRS      := sources/direct_input_driver
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.LIBS             := 
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.DIRECT_INPUT_DRIVER.IMPORTS          := ../core/compile.static.mak ../../input/low_level/compile.static.mak

#Цель - sources
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.TYPE             := static-lib
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.NAME             := funner.client.window_input_driver
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.SOURCE_DIRS      := sources/window_input_driver
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.LIBS             := 
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.WINDOW_INPUT_DRIVER.IMPORTS          := ../window_manager/compile.static.mak ../../input/low_level/compile.static.mak \
                                                          ../../input/window_driver/compile.static.mak

#Цель - tests
CLIENT.SUBSYSTEMS.TESTS.TYPE             := test-suite
CLIENT.SUBSYSTEMS.TESTS.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.TESTS.SOURCE_DIRS      := tests
CLIENT.SUBSYSTEMS.TESTS.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.TESTS.LIBS             :=
CLIENT.SUBSYSTEMS.TESTS.COMPILER_CFLAGS  :=
CLIENT.SUBSYSTEMS.TESTS.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.TESTS.IMPORTS          := ../../common/compile.static.mak ../../system/compile.static.mak \
                                            ../window_manager/compile.static.mak ../core/compile.static.mak \
                                            window_renderer.link.static.mak scene_render.link.static.mak scene_player.link.static.mak \
                                            direct_input_driver.link.static.mak window_input_driver.link.static.mak input_manager.link.static.mak \
                                            ../../input/manager/xkeymap.link.static.mak ../../input/manager/xkeyreg.link.static.mak \
                                            ../../render/opengl_driver/link.static.mak ../../common/configurator.link.static.mak \
                                            ../../sound/openal_device/link.static.mak ../../sound/openal_device/run.static.mak
