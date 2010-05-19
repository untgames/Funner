###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := RENDER.MID_LEVEL.SOURCES RENDER.MID_LEVEL.TESTS

#���� �1 - RENDER system sources
RENDER.MID_LEVEL.SOURCES.TYPE             := static-lib
RENDER.MID_LEVEL.SOURCES.NAME             := funner.render.mid_level
RENDER.MID_LEVEL.SOURCES.SOURCE_DIRS      := sources
RENDER.MID_LEVEL.SOURCES.IMPORTS          := compile.render.mid_level compile.media.image compile.media.geometry \
                                             compile.media.rfx compile.system compile.render.low_level

#���� �2 - RENDER system tests
RENDER.MID_LEVEL.TESTS.TYPE             := test-suite
RENDER.MID_LEVEL.TESTS.INCLUDE_DIRS     := ../../xtl/include
RENDER.MID_LEVEL.TESTS.SOURCE_DIRS      := tests
RENDER.MID_LEVEL.TESTS.IMPORTS          := compile.render.mid_level link.render.mid_level compile.system link.render.low_level.opengl_driver run.render.low_level.opengl_driver compile.media.image
