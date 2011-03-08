###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.MANAGER.SOURCES RENDER.MANAGER.TESTS

#Цель №1 - RENDER system sources
RENDER.MANAGER.SOURCES.TYPE             := static-lib
RENDER.MANAGER.SOURCES.NAME             := funner.render.manager
RENDER.MANAGER.SOURCES.SOURCE_DIRS      := sources
RENDER.MANAGER.SOURCES.IMPORTS          := compile.render.manager compile.media.image compile.media.geometry \
                                             compile.media.rfx compile.system compile.render.low_level

#Цель №2 - RENDER system tests
RENDER.MANAGER.TESTS.TYPE             := test-suite
RENDER.MANAGER.TESTS.INCLUDE_DIRS     := ../../xtl/include
RENDER.MANAGER.TESTS.SOURCE_DIRS      := tests
RENDER.MANAGER.TESTS.IMPORTS          := compile.render.manager link.render.manager compile.system link.render.low_level.opengl_driver run.render.low_level.opengl_driver compile.media.image
