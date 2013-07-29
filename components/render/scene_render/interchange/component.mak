###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR

#Цель - Scene render serializer generator
RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR.TYPE        := test-suite
RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR.SOURCE_DIRS := utils/serializer_generator
RENDER.SCENE_RENDER.INTERCHANGE.UTILS.SERIALIZER_GENERATOR.IMPORTS     := compile.common link.common
