###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := CLIENT.SUBSYSTEMS.WINDOW_RENDERER CLIENT.SUBSYSTEMS.SCENE_RENDER CLIENT.SUBSYSTEMS.TESTS

#Цель - sources
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.TYPE             := static-lib
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.NAME             := client.window_renderer
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.SOURCE_DIRS      := sources/window_renderer
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.LIBS             := 
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.WINDOW_RENDERER.IMPORTS          := ../window_manager/compile.static.mak ../../render/mid_level_window_driver/compile.static.mak

#Цель - sources
CLIENT.SUBSYSTEMS.SCENE_RENDER.TYPE             := static-lib
CLIENT.SUBSYSTEMS.SCENE_RENDER.NAME             := client.scene_render
CLIENT.SUBSYSTEMS.SCENE_RENDER.INCLUDE_DIRS     :=
CLIENT.SUBSYSTEMS.SCENE_RENDER.SOURCE_DIRS      := sources/scene_render
CLIENT.SUBSYSTEMS.SCENE_RENDER.LIB_DIRS         :=  
CLIENT.SUBSYSTEMS.SCENE_RENDER.LIBS             := 
CLIENT.SUBSYSTEMS.SCENE_RENDER.COMPILER_CFLAGS  := 
CLIENT.SUBSYSTEMS.SCENE_RENDER.COMPILER_DEFINES :=
CLIENT.SUBSYSTEMS.SCENE_RENDER.IMPORTS          := ../../common/compile.static.mak ../core/compile.static.mak ../../render/scene_render/compile.static.mak

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
                                            ../../render/opengl_driver/link.static.mak ../../common/configurator.link.static.mak \
                                            window_renderer.link.static.mak scene_render.link.static.mak
