###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += SCENE_GRAPH.CORE.SOURCES
TARGETS += SCENE_GRAPH.XSCENE.SOURCES
TARGETS += SCENE_GRAPH.CORE.TESTS
TARGETS += SCENE_GRAPH.SERIALIZATION.TESTS
TARGETS += SCENE_GRAPH.CORE.INFO

#Цель - SG components sources
SCENE_GRAPH.CORE.SOURCES.TYPE             := static-lib
SCENE_GRAPH.CORE.SOURCES.NAME             := funner.scene_graph.core
SCENE_GRAPH.CORE.SOURCES.INCLUDE_DIRS     := ../../math/bound_volumes/include
SCENE_GRAPH.CORE.SOURCES.SOURCE_DIRS      += sources/core
SCENE_GRAPH.CORE.SOURCES.SOURCE_DIRS      += sources/screen
SCENE_GRAPH.CORE.SOURCES.SOURCE_DIRS      += sources/controllers
SCENE_GRAPH.CORE.SOURCES.SOURCE_DIRS      += sources/controllers/animation
SCENE_GRAPH.CORE.SOURCES.SOURCE_DIRS      += sources/serialization/manager
SCENE_GRAPH.CORE.SOURCES.LIB_DIRS         :=  
SCENE_GRAPH.CORE.SOURCES.LIBS             :=
SCENE_GRAPH.CORE.SOURCES.COMPILER_DEFINES :=  
SCENE_GRAPH.CORE.SOURCES.IMPORTS          := compile.scene_graph.core compile.common compile.media.rms compile.media.animation compile.media.font
SCENE_GRAPH.CORE.SOURCES.msvc.COMPILER_CFLAGS  := -wd4355

#Цель №1 - SG components sources
SCENE_GRAPH.XSCENE.SOURCES.TYPE             := static-lib
SCENE_GRAPH.XSCENE.SOURCES.NAME             := funner.scene_graph.xscene
SCENE_GRAPH.XSCENE.SOURCES.INCLUDE_DIRS     := ../../math/bound_volumes/include
SCENE_GRAPH.XSCENE.SOURCES.SOURCE_DIRS      += sources/serialization/xscene_parser
SCENE_GRAPH.XSCENE.SOURCES.LIB_DIRS         :=  
SCENE_GRAPH.XSCENE.SOURCES.LIBS             :=
SCENE_GRAPH.XSCENE.SOURCES.COMPILER_DEFINES :=
SCENE_GRAPH.XSCENE.SOURCES.IMPORTS          := compile.scene_graph.core compile.common compile.media.rms compile.media.animation compile.media.font

#Цель - SG tests
SCENE_GRAPH.CORE.TESTS.TYPE             := test-suite
SCENE_GRAPH.CORE.TESTS.INCLUDE_DIRS     :=
SCENE_GRAPH.CORE.TESTS.SOURCE_DIRS      += tests/core
SCENE_GRAPH.CORE.TESTS.SOURCE_DIRS      += tests/screen
SCENE_GRAPH.CORE.TESTS.SOURCE_DIRS      += tests/controllers
SCENE_GRAPH.CORE.TESTS.LIB_DIRS         :=
SCENE_GRAPH.CORE.TESTS.LIBS             :=
SCENE_GRAPH.CORE.TESTS.COMPILER_CFLAGS  :=
SCENE_GRAPH.CORE.TESTS.COMPILER_DEFINES := 
SCENE_GRAPH.CORE.TESTS.IMPORTS          := link.scene_graph.core compile.scene_graph.core compile.common link.media.animation.xanim compile.media.font

#Цель - XScene tests
SCENE_GRAPH.SERIALIZATION.TESTS.TYPE             := test-suite
SCENE_GRAPH.SERIALIZATION.TESTS.INCLUDE_DIRS     :=
SCENE_GRAPH.SERIALIZATION.TESTS.SOURCE_DIRS      += tests/serialization
SCENE_GRAPH.SERIALIZATION.TESTS.LIB_DIRS         :=
SCENE_GRAPH.SERIALIZATION.TESTS.LIBS             :=
SCENE_GRAPH.SERIALIZATION.TESTS.COMPILER_CFLAGS  :=
SCENE_GRAPH.SERIALIZATION.TESTS.COMPILER_DEFINES := 
SCENE_GRAPH.SERIALIZATION.TESTS.IMPORTS          := link.scene_graph.core compile.scene_graph.core compile.common link.media.animation.xanim link.scene_graph.xscene compile.media.rms \
                                                    compile.media.font

#Цель - сборка документации
SCENE_GRAPH.CORE.INFO.TYPE        := doxygen-info
SCENE_GRAPH.CORE.INFO.CHM_NAME    := funner.scene_graph.core
SCENE_GRAPH.CORE.INFO.SOURCE_DIRS := include
SCENE_GRAPH.CORE.INFO.IMPORTS     := compile.scene_graph.core
