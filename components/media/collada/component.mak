###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.COLLADA.SOURCES MEDIA.COLLADA.DAE.SOURCES MEDIA.COLLADA.CONVERT.SOURCES MEDIA.COLLADA.DAE.TESTS MEDIA.COLLADA.CONVERTER \
  MEDIA.COLLADA.CONVERTER.TESTS

#Цель - sources
MEDIA.COLLADA.SOURCES.TYPE             := static-lib
MEDIA.COLLADA.SOURCES.NAME             := funner.media.collada
MEDIA.COLLADA.SOURCES.SOURCE_DIRS      := sources/model
MEDIA.COLLADA.SOURCES.IMPORTS          := compile.media.collada compile.media.shared

#Цель - DAE loader sources
MEDIA.COLLADA.DAE.SOURCES.TYPE             := static-lib
MEDIA.COLLADA.DAE.SOURCES.NAME             := funner.media.collada.dae
MEDIA.COLLADA.DAE.SOURCES.SOURCE_DIRS      := sources/dae_loader
MEDIA.COLLADA.DAE.SOURCES.IMPORTS          := compile.media.collada link.media.collada

#Цель - collada convert sources
MEDIA.COLLADA.CONVERT.SOURCES.TYPE             := static-lib
MEDIA.COLLADA.CONVERT.SOURCES.NAME             := funner.media.collada.convert
MEDIA.COLLADA.CONVERT.SOURCES.SOURCE_DIRS      := sources/convert
MEDIA.COLLADA.CONVERT.SOURCES.IMPORTS          := compile.media.collada compile.media.geometry

#Цель - DAE loader tests
MEDIA.COLLADA.DAE.TESTS.TYPE             := test-suite
MEDIA.COLLADA.DAE.TESTS.SOURCE_DIRS      := tests/dae_loader
MEDIA.COLLADA.DAE.TESTS.IMPORTS          := compile.media.collada link.media.collada.dae

#Цель - collada convert tests
MEDIA.COLLADA.CONVERT.TESTS.TYPE             := test-suite
MEDIA.COLLADA.CONVERT.TESTS.SOURCE_DIRS      := tests/convert
MEDIA.COLLADA.CONVERT.TESTS.IMPORTS          := compile.media.collada compile.media.geometry \
                                                link.media.collada.dae link.media.collada.convert link.media.geometry \
                                                link.media.geometry.xmesh

#Цель - Collada converter
MEDIA.COLLADA.CONVERTER.TYPE        := application
MEDIA.COLLADA.CONVERTER.NAME        := collada-converter
MEDIA.COLLADA.CONVERTER.SOURCE_DIRS := utils/collada_converter
MEDIA.COLLADA.CONVERTER.IMPORTS     := compile.media.collada compile.media.image compile.media.geometry \
                                       link.media.collada.dae link.media.collada.convert link.media.image link.media.geometry.xmesh

#Цель - collada convert tests
MEDIA.COLLADA.CONVERTER.TESTS.TYPE               := test-suite
MEDIA.COLLADA.CONVERTER.TESTS.SOURCE_DIRS        := tests/collada_converter
MEDIA.COLLADA.CONVERTER.TESTS.EXECUTION_DIR      := tests/collada_converter
MEDIA.COLLADA.CONVERTER.TESTS.USED_APPLICATIONS  := collada-converter
                                                                                       