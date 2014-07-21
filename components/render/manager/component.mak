###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.MANAGER.SOURCES RENDER.MANAGER.TESTS

#Цель №1 - RENDER system sources
RENDER.MANAGER.SOURCES.TYPE                 := static-lib
RENDER.MANAGER.SOURCES.NAME                 := funner.render.manager
RENDER.MANAGER.SOURCES.SOURCE_DIRS          := sources
RENDER.MANAGER.SOURCES.IMPORTS              := compile.render.manager compile.media.image compile.media.geometry \
                                               compile.media.rfx compile.system compile.render.low_level compile.math.bound_volumes
RENDER.MANAGER.SOURCES.msvc.COMPILER_CFLAGS := -wd4503                                               

#Цель №2 - RENDER system tests
RENDER.MANAGER.TESTS.TYPE             := test-suite
RENDER.MANAGER.TESTS.INCLUDE_DIRS     := ../../xtl/include sources
RENDER.MANAGER.TESTS.SOURCE_DIRS      := tests/public tests/internal
RENDER.MANAGER.TESTS.IMPORTS          := compile.render.manager link.render.manager compile.system link.render.low_level.opengl_driver \
                                         run.render.low_level.opengl_driver compile.media.image link.media.image.dds compile.media.rfx link.media.rfx link.media.rfx.default_serializers \
                                         link.media.geometry.binmesh link.media.geometry.xmesh compile.media.geometry compile.render.low_level compile.math.bound_volumes
