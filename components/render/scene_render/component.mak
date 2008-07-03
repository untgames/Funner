###################################################################################################
#Определения и константы
###################################################################################################
#TARGETS := RENDER.SCENE_RENDER.CORE.SOURCES RENDER.SCENE_RENDER.CORE.TESTS
TARGETS := RENDER.SCENE_RENDER.CORE.SOURCES RENDER.SCENE_RENDER.RENDER2D.SOURCES RENDER.SCENE_RENDER.RENDER2D.TESTS

#Цель №1 - Scene render sources
RENDER.SCENE_RENDER.CORE.SOURCES.TYPE             := static-lib
RENDER.SCENE_RENDER.CORE.SOURCES.NAME             := render.scene_render
RENDER.SCENE_RENDER.CORE.SOURCES.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.CORE.SOURCES.SOURCE_DIRS      := sources/core
RENDER.SCENE_RENDER.CORE.SOURCES.LIB_DIRS         :=  
RENDER.SCENE_RENDER.CORE.SOURCES.LIBS             := 
RENDER.SCENE_RENDER.CORE.SOURCES.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.CORE.SOURCES.COMPILER_DEFINES :=
RENDER.SCENE_RENDER.CORE.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak ../../scene_graph/compile.static.mak \
                                             ../mid_level/compile.static.mak
                                             
#Цель №2 - Scene render sources
RENDER.SCENE_RENDER.RENDER2D.SOURCES.TYPE             := static-lib
RENDER.SCENE_RENDER.RENDER2D.SOURCES.NAME             := render.scene_render.render2d
RENDER.SCENE_RENDER.RENDER2D.SOURCES.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.RENDER2D.SOURCES.SOURCE_DIRS      := sources/render2d
RENDER.SCENE_RENDER.RENDER2D.SOURCES.LIB_DIRS         :=  
RENDER.SCENE_RENDER.RENDER2D.SOURCES.LIBS             := 
RENDER.SCENE_RENDER.RENDER2D.SOURCES.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.RENDER2D.SOURCES.COMPILER_DEFINES :=
RENDER.SCENE_RENDER.RENDER2D.SOURCES.IMPORTS          := compile.static.mak ../../common/compile.static.mak ../../scene_graph/compile.static.mak \
                                                         ../mid_level/compile.static.mak ../../media/rfx/compile.static.mak ../../media/image/compile.static.mak
RENDER.SCENE_RENDER.RENDER2D.SOURCES.msvc.COMPILER_CFLAGS  := -wd4355

#Цель №3 - Scene render tests
RENDER.SCENE_RENDER.CORE.TESTS.TYPE             := test-suite
RENDER.SCENE_RENDER.CORE.TESTS.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.CORE.TESTS.SOURCE_DIRS      := tests/core
RENDER.SCENE_RENDER.CORE.TESTS.LIB_DIRS         :=
RENDER.SCENE_RENDER.CORE.TESTS.LIBS             :=
RENDER.SCENE_RENDER.CORE.TESTS.DLLS             :=
RENDER.SCENE_RENDER.CORE.TESTS.DLL_DIRS         :=
RENDER.SCENE_RENDER.CORE.TESTS.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.CORE.TESTS.IMPORTS          := compile.static.mak link.static.mak ../mid_level/compile.static.mak

#Цель №4 - Scene render tests
RENDER.SCENE_RENDER.RENDER2D.TESTS.TYPE             := test-suite
RENDER.SCENE_RENDER.RENDER2D.TESTS.INCLUDE_DIRS     :=
RENDER.SCENE_RENDER.RENDER2D.TESTS.SOURCE_DIRS      := tests/render2d
RENDER.SCENE_RENDER.RENDER2D.TESTS.LIB_DIRS         :=
RENDER.SCENE_RENDER.RENDER2D.TESTS.LIBS             :=
RENDER.SCENE_RENDER.RENDER2D.TESTS.DLLS             :=
RENDER.SCENE_RENDER.RENDER2D.TESTS.DLL_DIRS         :=
RENDER.SCENE_RENDER.RENDER2D.TESTS.COMPILER_CFLAGS  :=
RENDER.SCENE_RENDER.RENDER2D.TESTS.IMPORTS          := compile.static.mak render2d.link.static.mak ../debug_renderer_driver/link.static.mak \
                                                       ../../scene_graph/compile.static.mak ../../common/compile.static.mak \
                                                       ../../media/rfx/xmtl.link.static.mak
