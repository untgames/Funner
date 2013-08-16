###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR
TARGETS += RENDER.SCENE_RENDER.INTERCHANGE.SOURCES
TARGETS += RENDER.SCENE_RENDER.INTERCHANGE.TESTS

#Цель - Scene render serializer generator
RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR.TYPE        := test-suite
RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR.SOURCE_DIRS := utils/serializer_generator
RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR.IMPORTS     := compile.common link.common

#Цель - scene render interchange sources
RENDER.SCENE_RENDER.INTERCHANGE.SOURCES.TYPE        := static-lib
RENDER.SCENE_RENDER.INTERCHANGE.SOURCES.NAME        := funner.render.scene_render.interchange
RENDER.SCENE_RENDER.INTERCHANGE.SOURCES.SOURCE_DIRS := sources
RENDER.SCENE_RENDER.INTERCHANGE.SOURCES.IMPORTS     := compile.render.scene_render.interchange compile.common compile.system

#Цель - scene render interchange tests
RENDER.SCENE_RENDER.INTERCHANGE.TESTS.TYPE        := test-suite
RENDER.SCENE_RENDER.INTERCHANGE.TESTS.SOURCE_DIRS := tests
RENDER.SCENE_RENDER.INTERCHANGE.TESTS.IMPORTS     := compile.render.scene_render.interchange link.render.scene_render.interchange
